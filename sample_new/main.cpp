// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

#include "shadertoy_sandbox.hpp"

using namespace swizzle;
using namespace shadertoy;

// TODO: wrong mouse clicks
// TODO: texture sampling in simd?
// TODO: inout = a.xxxx


static_assert(sizeof(vec2) == sizeof(swizzle::float_type[2]), "Too big");
static_assert(sizeof(vec3) == sizeof(swizzle::float_type[3]), "Too big");
static_assert(sizeof(vec4) == sizeof(swizzle::float_type[4]), "Too big");

// these headers, especially SDL.h & time.h set up names that are in conflict with sandbox'es;
// sandbox should be moved to a separate h/cpp pair, but out of laziness... including them
// *after* sandbox solves it too

#include <iostream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <condition_variable>
#include <future>
#include <filesystem>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_video.h>

#ifdef SAMPLE_SDL2IMAGE_FOUND
#include <SDL_image.h>
#endif

#include <ctime>
#include <memory>
#include <functional>
#if SAMPLE_OMP_ENABLED
#include <omp.h>
#endif

struct sdl_deleter 
{
    void operator()(SDL_Surface* ptr)
    {
        SDL_FreeSurface(ptr);
    }
    void operator()(SDL_Window* ptr)
    {
        SDL_DestroyWindow(ptr);
    }
    void operator()(SDL_Texture* ptr)
    {
        SDL_DestroyTexture(ptr);
    }
    void operator()(SDL_Renderer* ptr)
    {
        SDL_DestroyRenderer(ptr);
    }
};

struct render_stats
{
    std::chrono::microseconds duration;
    int num_pixels;
    int num_threads;
};

enum class sampler_type {
    none,
    texture,
    buffer_a,
    buffer_b, 
    buffer_c,
    buffer_d,
    keyboard,
};

struct sampler_data
{
    const char* path = nullptr;
    
    sampler_type type = sampler_type::none;
    std::unique_ptr<SDL_Surface, sdl_deleter> images[6];
    swizzle::naive_sampler_data faces[6];
    size_t faces_count = 1;

    void init_sampler(sampler2D& sampler, vec3& resolution) const
    {
        sampler.data = &faces[0];
        sampler.face_count = faces_count;
        resolution = vec3(faces[0].width, faces[0].height, 0.0f);
    }
};

struct aligned_render_target_base 
{
    struct void_deleter
    {
        void operator()(void* ptr)
        {
            delete ptr;
        }
    };

    std::unique_ptr<void, void_deleter> memory;
    void* first_row = nullptr;
    size_t pitch = 0;
    int width = 0;
    int height = 0;
    size_t pixels_size;

    aligned_render_target_base() = default;
    aligned_render_target_base(int width, int height, int bpp, int align) : width(width), height(height)
    {
        const int mask = align - 1;

        // each row has to be aligned
        pitch = (width * bpp + mask) & (~mask);
        pixels_size = static_cast<size_t>(pitch) * height;
        size_t alloc_size = pixels_size + align - 1;

        // alloc and align
        memory.reset(::operator new (alloc_size));
        first_row = memory.get();
        first_row = std::align(align, pixels_size, first_row, alloc_size);
        memset(memory.get(), 0, alloc_size);
    }

};

struct render_target_rgba32 : aligned_render_target_base
{
    static const int bytes_per_pixel = 4;

    render_target_rgba32() = default;
    render_target_rgba32(int width, int height) : aligned_render_target_base(width, height, bytes_per_pixel, 32) {}


    void store(uint8_t* ptr, const vec4& color, size_t pitch)
    {
        store_rgba8_aligned(color.r, color.g, color.b, color.a, ptr, pitch);
    }
};

struct render_target_a8 : aligned_render_target_base
{
    static const int bytes_per_pixel = 1;

    render_target_a8() = default;
    render_target_a8(int width, int height) : aligned_render_target_base(width, height, bytes_per_pixel, 32) {}

    ptrdiff_t offset(int x, int y) const
    {
        return (height - 1 - y) * pitch + x * bytes_per_pixel;
    }

    uint8_t get(int x, int y) const
    {
        return *(reinterpret_cast<uint8_t*>(first_row) + offset(x, y));
    }

    void set(int x, int y, uint8_t value)
    {
        *(reinterpret_cast<uint8_t*>(first_row) + offset(x, y)) = value;
    }
};

struct render_target_float : aligned_render_target_base
{
    static const int bytes_per_pixel = 4 * sizeof(float);

    render_target_float() = default;
    render_target_float(int width, int height) : aligned_render_target_base(width, height, bytes_per_pixel, 32) {}

    void store(uint8_t* ptr, const vec4& color, size_t pitch)
    {
        store_rgba32f_aligned(color.r, color.g, color.b, color.a, ptr, pitch);
    }

    void set(int x, int y, float r, float g, float b, float a)
    {
        float* p = reinterpret_cast<float*>(reinterpret_cast<uint8_t*>(first_row) + y * pitch + x * bytes_per_pixel);
        p[0] = r;
        p[1] = g;
        p[2] = b;
        p[3] = a;
    }

    std::tuple<float, float, float, float> get(int x, int y)
    {
        float* p = reinterpret_cast<float*>(reinterpret_cast<uint8_t*>(first_row) + y * pitch + x * bytes_per_pixel);
        return std::make_tuple(p[0], p[1], p[2], p[3]);
    }

};

constexpr auto pixels_per_batch = static_cast<int>(::swizzle::detail::batch_traits<swizzle::float_type>::size);
static_assert(pixels_per_batch == 1 || pixels_per_batch % 2 == 0, "1 or even scalar count");
constexpr auto columns_per_batch = pixels_per_batch > 1 ? pixels_per_batch / 2 : 1;
constexpr auto rows_per_batch = pixels_per_batch > 1 ? 2 : 1;


template <typename PixelFunc, typename RenderTarget>
static render_stats render(PixelFunc func, shader_inputs uniforms, RenderTarget& rt, const std::atomic_bool& cancelled)
{
    assert(rt.width % columns_per_batch == 0);
    assert(rt.height % rows_per_batch == 0);

    using ::swizzle::detail::static_for;
    using namespace ::swizzle;
    using float_traits = ::swizzle::detail::batch_traits<swizzle::float_type>;

    auto render_begin = std::chrono::steady_clock::now();

    // if there are more than 1 scalars in a vector, work on two rows with half width at the same time
    swizzle::float_type x_offsets(0);
    swizzle::float_type y_offsets(0);

    int num_pixels = 0;
    int num_threads = 0;

    {
        float_traits::aligned_storage_type aligned_storage;
        float* aligned = reinterpret_cast<float*>(&aligned_storage);

        static_for<0, float_traits::size>([&](size_t i) { aligned[i] = static_cast<float>(i % columns_per_batch) + 0.5f; });
        load_aligned(x_offsets, aligned);

        static_for<0, float_traits::size>([&](size_t i) { aligned[i] = static_cast<float>(i / columns_per_batch) + 0.5f; });
        load_aligned(y_offsets, aligned);
    }
  
    
    int max_x = rt.width;
    int max_y = rt.height;

#if !SAMPLE_OMP_ENABLED
    {
        num_threads = 1;
#else
    #pragma omp parallel default(none) shared(num_threads, num_pixels)
    {
        #pragma omp master
        num_threads = omp_get_num_threads();

        #pragma omp for
#endif
        for (int y = 0; y < max_y; y += rows_per_batch)
        {
            if (cancelled)
                continue;

            swizzle::float_type frag_coord_y = static_cast<float>(rt.height - y) - y_offsets;

            uint8_t* ptr = reinterpret_cast<uint8_t*>(rt.first_row) + y * rt.pitch;

            int x;
            for (x = 0; x < max_x; x += columns_per_batch)
            {
                swizzle::bool_type discarded;
                swizzle::vec4 color = func(uniforms, vec2(static_cast<float>(x) + x_offsets, frag_coord_y), {}, &discarded);

                if (!discarded) 
                {
                    rt.store(ptr, color, rt.pitch);
                }
                
                ptr += rt.bytes_per_pixel * columns_per_batch;
            }

#if SAMPLE_OMP_ENABLED
            #pragma omp atomic
#endif
            num_pixels += (max_x) * rows_per_batch;
        }
    }

    return render_stats { std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - render_begin), num_pixels, num_threads };
}

void init_samplers(shader_inputs& inputs, const sampler_data ptr[4])
{
    ptr[0].init_sampler(inputs.iChannel0, inputs.iChannelResolution[0]);
    ptr[1].init_sampler(inputs.iChannel1, inputs.iChannelResolution[1]);
    ptr[2].init_sampler(inputs.iChannel2, inputs.iChannelResolution[2]);
    ptr[3].init_sampler(inputs.iChannel3, inputs.iChannelResolution[3]);
    for (auto& f : inputs.iChannelTime) 
    {
        f = 0.0f;
    }
}

#define STR1(x) #x
#define STR2(x) STR1(x)
#define VT100_CLEARLINE "\33[2K"
#define VT100_UP(x)     "\33[" STR2(x) "A"
#define VT100_DOWN(x)   "\33[" STR2(x) "B"
#define STATS_LINES 11

void make_naive_sampler_data(swizzle::naive_sampler_data& sampler, render_target_float& rt)
{
    sampler.bytes = reinterpret_cast<uint8_t*>(rt.first_row);
    sampler.width = rt.width;
    sampler.height = rt.height;
    sampler.pitch_bytes = rt.pitch;
    sampler.bytes_per_pixel = rt.bytes_per_pixel;
    sampler.is_floating_point = true;
}

void make_naive_sampler_data(swizzle::naive_sampler_data& sampler, const SDL_Surface* surface)
{
    sampler.bytes = reinterpret_cast<uint8_t*>(surface->pixels);
    sampler.width = surface->w;
    sampler.height = surface->h;
    sampler.pitch_bytes = surface->pitch;
    sampler.bytes_per_pixel = surface->format->BytesPerPixel;

    sampler.rshift = surface->format->Rshift;
    sampler.gshift = surface->format->Gshift;
    sampler.bshift = surface->format->Bshift;
    sampler.ashift = surface->format->Ashift;

    if (surface->format->format == SDL_PIXELFORMAT_INDEX8)
    {
        sampler.rmask = 0xFF;
        sampler.gmask = 0xFF;
        sampler.bmask = 0xFF;
        sampler.amask = 0;
    }
    else
    {
        sampler.rmask = surface->format->Rmask;
        sampler.gmask = surface->format->Gmask;
        sampler.bmask = surface->format->Bmask;
        sampler.amask = surface->format->Amask;
    }
}
// TODO: sound
bool load_texture(sampler_data& sampler, const char* name)
{
#ifdef SAMPLE_SDL2IMAGE_FOUND
    auto img = IMG_Load(name);
    if (!img)
    {
        fprintf(stderr, "ERROR: Failed to load texture %s (error: %s)\n", name, IMG_GetError());
        return false;
    }
    else
    {
        // check if other faces are available
        namespace fs = std::filesystem;

        fs::path path = name;
        sampler.faces_count = 1;


        for (int i = 1; i <= 5; ++i) 
        {
            fs::path other = path.parent_path() / path.stem();
            other += fs::path("_" + std::to_string(i));
            other += path.extension();

            auto face_img = IMG_Load(other.string().c_str());
            if (face_img) {
                fprintf(stdout, "omg!!! %s", other.string().c_str());
                make_naive_sampler_data(sampler.faces[i], face_img);
                sampler.faces_count = i;
            }
        }
        
        make_naive_sampler_data(sampler.faces[0], img);
        return true;
    }
#else
    fprintf(stderr, "ERROR: Failed to load texture %s, SDL_image was not found", name);
    return false;
#endif
}

template <class Rep, class Period>
constexpr auto duration_to_seconds(const std::chrono::duration<Rep, Period>& d)
{
    return std::chrono::duration<double>(d).count();
}


template <typename T>
bool from_string(const char* str, T& value)
{
    std::stringstream s;
    s << str;
    return !!(s >> value);
}


void print_help()
{
    printf("-r <width> <height>         set initial resolution\n");
    printf("-w <x> <y> <width> <height> set initial viewport\n");
    printf("-t <time>                   set initial time & pause\n");
    printf("-h                          show this message\n");
    printf("-sN <path>                  set texture for sampler N\n");
    printf("-s{a|b|c|d}N <path>         set texture for sampler N for buffer a, b, c or d\n");
}

int print_args_error()
{
    print_help();
    return 1;
}


SDL_Rect fix_rect(SDL_Rect rect)
{
    if (rect.w < 0)
    {
        rect.x += rect.w;
        rect.w = -rect.w;
    }
    if (rect.h < 0)
    {
        rect.y += rect.h;
        rect.h = -rect.h;
    }
    return rect;
}

int SDL_Keysym_to_Ascii(SDL_Keysym keysym) {

    if (keysym.sym >= SDLK_a && keysym.sym <= SDLK_z)
    {
        if (keysym.mod & KMOD_SHIFT)
        {
            return int('A') + keysym.sym - 'a';
        }
        else
        {
            return keysym.sym;
        }
    }

    if (keysym.sym < 256)
    {
        return keysym.sym;
    }

    switch (keysym.sym)
    {
    case SDLK_PAGEUP:  return 33;
    case SDLK_PAGEDOWN:  return 34;
    case SDLK_END:  return 35;
    case SDLK_HOME: return 36;
    case SDLK_LEFT: return 37;
    case SDLK_UP: return 38;
    case SDLK_RIGHT: return 39;
    case SDLK_DOWN: return 40;
    default:
        return -1;
    }
}


int main(int argc, char* argv[])
{
    using namespace std;

#ifdef SDLIMAGE_FOUND
    // initialise SDLImage
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int initted = IMG_Init(flags);
    if ((initted & flags) != flags) 
    {
        fprintf(stderr, "WARNING: failed to initialize required jpg and png support: %s\n", IMG_GetError());
    }
#endif

    swizzle::vector<int, 2> resolution(512, 288);
    float time = 0.0f;
    float time_scale = 1.0f;

    static_assert(::shadertoy::num_samplers >= 4);
    sampler_data textures[::shadertoy::num_samplers * (::shadertoy::num_buffers + 1)];

    textures[ 0 + 0].path = SAMPLE_ICHANNEL_0_PATH;
    textures[ 0 + 1].path = SAMPLE_ICHANNEL_1_PATH;
    textures[ 0 + 2].path = SAMPLE_ICHANNEL_2_PATH;
    textures[ 0 + 3].path = SAMPLE_ICHANNEL_3_PATH;
    textures[ 4 + 0].path = SAMPLE_BUFFER_A_ICHANNEL_0_PATH;
    textures[ 4 + 1].path = SAMPLE_BUFFER_A_ICHANNEL_1_PATH;
    textures[ 4 + 2].path = SAMPLE_BUFFER_A_ICHANNEL_2_PATH;
    textures[ 4 + 3].path = SAMPLE_BUFFER_A_ICHANNEL_3_PATH;
    textures[ 8 + 0].path = SAMPLE_BUFFER_B_ICHANNEL_0_PATH;
    textures[ 8 + 1].path = SAMPLE_BUFFER_B_ICHANNEL_1_PATH;
    textures[ 8 + 2].path = SAMPLE_BUFFER_B_ICHANNEL_2_PATH;
    textures[ 8 + 3].path = SAMPLE_BUFFER_B_ICHANNEL_3_PATH;
    textures[12 + 0].path = SAMPLE_BUFFER_C_ICHANNEL_0_PATH;
    textures[12 + 1].path = SAMPLE_BUFFER_C_ICHANNEL_1_PATH;
    textures[12 + 2].path = SAMPLE_BUFFER_C_ICHANNEL_2_PATH;
    textures[12 + 3].path = SAMPLE_BUFFER_C_ICHANNEL_3_PATH;
    textures[16 + 0].path = SAMPLE_BUFFER_D_ICHANNEL_0_PATH;
    textures[16 + 1].path = SAMPLE_BUFFER_D_ICHANNEL_1_PATH;
    textures[16 + 2].path = SAMPLE_BUFFER_D_ICHANNEL_2_PATH;
    textures[16 + 3].path = SAMPLE_BUFFER_D_ICHANNEL_3_PATH;

    for (int i = 1; i < argc; ++i)
    {
        char* arg = argv[i];
        if (!strcmp(arg, "-r"))
        {
            if (i + 2 < argc && from_string(argv[++i], resolution.x) && from_string(argv[++i], resolution.y))
            {
            }
            else
            {
                return print_args_error();
            }
        }
        else if (!strcmp(arg, "-t"))
        {
            if (i + 1 < argc && from_string(argv[++i], time))
            {
                time_scale = 0.0f;
            }
            else
            {
                return print_args_error();
            }
        }
        else if (!strncmp(arg, "-s", 2)) 
        {
            char channel = arg[2];
            int sampler_index_base = 0;
            int expected_length = 3;

            if (channel == 'a' || channel == 'b' || channel == 'c' || channel == 'd') 
            {
                static_assert(::shadertoy::num_buffers == 4);
                sampler_index_base = (channel - 'a' + 1) * 4;
                channel = arg[3];
                expected_length = 4;
            }

            if (channel >= '0' && channel <= '3' && i + 1 < argc && strlen(arg) == expected_length)
            {
                static_assert(::shadertoy::num_samplers == 4);
                textures[sampler_index_base + channel - '0'].path = argv[++i];
            }
            else
            {
                print_args_error();
            }
        }
        else if (!strcmp(arg, "-s0") || !strcmp(arg, "-s1") || !strcmp(arg, "-s2") || !strcmp(arg, "-s3"))
        {
            static_assert(::shadertoy::num_samplers == 4);
            int sampler_index = arg[2] - '0';
            if (i + 1 < argc)
            {
                textures[sampler_index].path = argv[++i];
            }
            else
            {
                return print_args_error();
            }
        }
        else if (!strcmp(arg, "-h"))
        {
            print_help();
            return 0;
        }
    }

    // load up textures
    for (auto& texture : textures)
    {
        if (!texture.path || strlen(texture.path) == 0)
        {
            texture.type = sampler_type::none;
            texture.path = nullptr;
        }
        else if (!strcmp(texture.path, "buffer_a")) 
        {
            texture.type = sampler_type::buffer_a;
        }
        else if (!strcmp(texture.path, "buffer_b"))
        {
            texture.type = sampler_type::buffer_b;
        }
        else if (!strcmp(texture.path, "buffer_c"))
        {
            texture.type = sampler_type::buffer_c;
        }
        else if (!strcmp(texture.path, "buffer_d"))
        {
            texture.type = sampler_type::buffer_d;
        }
        else if (!strcmp(texture.path, "keyboard"))
        {
            texture.type = sampler_type::keyboard;
        }
        else
        {
            texture.type = sampler_type::texture;
            load_texture(texture, texture.path);
        }
    }

    if (resolution.x <= 0 || resolution.y < 0 )
    {
        fprintf(stderr, "ERROR: invalid resolution: %dx%d\n", resolution.x, resolution.y);
        return 1;
    }

    printf("p           - pause/unpause\n");
    printf("left arrow  - decrease time by 1 s\n");
    printf("right arrow - increase time by 1 s\n");
    printf("space       - blit now! (show incomplete render)\n");
    printf("esc         - quit\n");
    printf("\n");
    printf("--- Textures: ---\n");
    
    {
        const char* channel_prefix[] = 
        {
            "",
            "buffer_a.",
            "buffer_b.",
            "buffer_c.",
            "buffer_d."
        };
        for (int i = 0; i < size(textures); ++i)
        {
            if (textures[i].path)
            {
                printf("%siChannel%d: %s\n", channel_prefix[i / shadertoy::num_samplers], i % shadertoy::num_samplers, textures[i].path);
            }
        }
    }
    printf("\n");

    for (int i = 0; i < STATS_LINES; ++i)
        printf("\n");

    // initial setup
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "ERROR: Unable to init SDL\n");
        return 1;
    }

    try
    {
        std::unique_ptr<SDL_Window, sdl_deleter> window(SDL_CreateWindow("CxxSwizzle sample", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, resolution.x, resolution.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI));
        std::unique_ptr<SDL_Renderer, sdl_deleter> renderer(SDL_CreateRenderer(window.get(), -1, 0));
        std::unique_ptr<SDL_Surface, sdl_deleter> target_surface;
        std::unique_ptr<SDL_Texture, sdl_deleter> target_texture;

        render_target_rgba32 target_surface_data;
        render_target_float buffer_surfaces[4][2];
        render_target_a8 keyboard_surface_data(256, 3);
        render_target_a8 keyboard_surface_data_used(256, 3);
        std::unique_ptr<SDL_Surface, sdl_deleter> keyboard_surface(SDL_CreateRGBSurfaceWithFormatFrom(keyboard_surface_data_used.first_row, keyboard_surface_data_used.width, keyboard_surface_data_used.height, 32, keyboard_surface_data_used.pitch, SDL_PIXELFORMAT_INDEX8));


        // a function used to resize the target_surface
        auto resize_or_create_surfaces = [&](int w, int h) -> void
        {
            // let's make life easier and make sure surfaces are aligned to the batch size
            auto aligned_w = ((w + columns_per_batch - 1) / columns_per_batch) * columns_per_batch;
            auto aligned_h = ((h + rows_per_batch - 1) / rows_per_batch) * rows_per_batch;

            auto create_buffer_surface = [=]() -> auto { return render_target_float(aligned_w, aligned_h); };

#ifdef SAMPLE_HAS_BUFFER_A
            buffer_surfaces[0][0] = create_buffer_surface(); buffer_surfaces[0][1] = create_buffer_surface();
#endif
#ifdef SAMPLE_HAS_BUFFER_B
            buffer_surfaces[1][0] = create_buffer_surface(); buffer_surfaces[1][1] = create_buffer_surface();
#endif
#ifdef SAMPLE_HAS_BUFFER_C
            buffer_surfaces[2][0] = create_buffer_surface(); buffer_surfaces[2][1] = create_buffer_surface();
#endif
#ifdef SAMPLE_HAS_BUFFER_D
            buffer_surfaces[3][0] = create_buffer_surface(); buffer_surfaces[3][1] = create_buffer_surface();
#endif

            target_surface_data = render_target_rgba32(aligned_w, aligned_h);

            target_surface.reset(SDL_CreateRGBSurfaceWithFormatFrom(target_surface_data.first_row, aligned_w, aligned_h, 32, target_surface_data.pitch, SDL_PIXELFORMAT_RGBA32));
            if (!target_surface)
            {
                throw std::runtime_error("Unable to create target_surface");
            }

            target_texture.reset(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, w, h));
            if (!target_texture)
            {
                throw std::runtime_error("Unable to create target_texture");
            }

            resolution.x = w;
            resolution.y = h;
        };

        resize_or_create_surfaces(resolution.x, resolution.y);
        
        int num_frames = 0;
        float current_frame_timestamp = 0.0f;
        int mouse_x = 0, mouse_y = 0;
        int mouse_press_x = 0, mouse_press_y = 0;
        bool pending_resize = false;
        bool mouse_pressed = false;
        bool mouse_clicked = false;

        render_stats last_render_stats = { };
        float last_frame_timestamp = 0.0f;

        std::chrono::microseconds fps_frames_duration = {};
        int fps_frames_num = 0;
        double current_fps = 0;

        std::atomic_bool abort_render_token = false;


        auto render_all = [&target_surface_data, &buffer_surfaces, &textures, &num_frames](shader_inputs inputs, const std::atomic_bool& cancel) -> auto
        {
            int buffer_surface_index = 1 - (num_frames & 1);

            std::chrono::microseconds duration(0);

#ifdef SAMPLE_HAS_BUFFER_A
            init_samplers(inputs, &textures[4]);
            duration += render(shadertoy::buffer_a, inputs, buffer_surfaces[0][buffer_surface_index], cancel).duration;
#endif
#ifdef SAMPLE_HAS_BUFFER_B
            init_samplers(inputs, &textures[8]);
            duration += render(shadertoy::buffer_b, inputs, buffer_surfaces[1][buffer_surface_index], cancel).duration;
#endif
#ifdef SAMPLE_HAS_BUFFER_C
            init_samplers(inputs, &textures[12]);
            duration += render(shadertoy::buffer_c, inputs, buffer_surfaces[2][buffer_surface_index], cancel).duration;
#endif
#ifdef SAMPLE_HAS_BUFFER_D
            init_samplers(inputs, &textures[16]);
            duration += render(shadertoy::buffer_d, inputs, buffer_surfaces[3][buffer_surface_index], cancel).duration;
#endif
            init_samplers(inputs, &textures[0]);
            render_stats result = render(shadertoy::image, inputs, target_surface_data, cancel);
            result.duration += duration;
            return result;
        };


        auto render_async = [&]() -> std::future<render_stats>
        {
            abort_render_token = false;
            
            // bring about keyboard changes
            memcpy(keyboard_surface_data_used.first_row, keyboard_surface_data.first_row, keyboard_surface_data.pixels_size);
            
            // clear down flag
            for (int i = 0; i < 256; ++i) 
            {
                keyboard_surface_data.set(i, 1, 0);
            }


            {
                int buffer_surface_index = num_frames & 1;
                // sort out buffer textures
                for (auto& data : textures)
                {
                    if (data.type >= sampler_type::buffer_a && data.type <= sampler_type::buffer_d)
                    {
                        int buffer_index = static_cast<int>(data.type) - static_cast<int>(sampler_type::buffer_a);
                        data.faces_count = 1;
                        make_naive_sampler_data(data.faces[0], buffer_surfaces[buffer_index][buffer_surface_index]);
                    }
                    else if (data.type == sampler_type::keyboard)
                    {
                        data.faces_count = 1;
                        make_naive_sampler_data(data.faces[0], keyboard_surface.get());
                    }
                }
            }

            auto s = target_surface.get();

            shader_inputs inputs = {};
            inputs.iTime = time;
            inputs.iTimeDelta = static_cast<float>(duration_to_seconds(last_render_stats.duration));
            inputs.iFrameRate = static_cast<int>(current_fps);
            inputs.iFrame = num_frames;
            inputs.iResolution = vec3(static_cast<float>(s->w), static_cast<float>(s->h), 0.0f);
            inputs.iMouse.x = static_cast<float>(mouse_x);
            inputs.iMouse.y = static_cast<float>(s->h - 1 - mouse_y);
            inputs.iMouse.z = (mouse_pressed ? 1.0f : -1.0f) * mouse_press_x;
            inputs.iMouse.w = (mouse_clicked ? 1.0f : -1.0f) * (s->h - 1 - mouse_press_y);

            mouse_clicked = false;


            {
                using sc = std::chrono::system_clock;
                auto now = sc::now();
                auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
                auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now - seconds).count();
                
                auto t = sc::to_time_t(now);
                std::tm* lt = std::localtime(&t);
                inputs.iDate.x = static_cast<float>(1900 + lt->tm_year);
                inputs.iDate.y = static_cast<float>(lt->tm_mon);
                inputs.iDate.z = static_cast<float>(lt->tm_mday);
                inputs.iDate.w = static_cast<float>((lt->tm_hour * 60 + lt->tm_min) * 60 + lt->tm_sec + microseconds / 1000000.0);
            }

            return std::async(render_all, inputs, std::ref(abort_render_token));
        };

        std::future<render_stats> render_task = render_async();

        for (auto update_begin = chrono::steady_clock::now();;)
        {
            bool blit_now = false;
            bool quit = false;

            // process events
            SDL_Event event;
            while (SDL_PollEvent(&event)) 
            {
                switch (event.type)
                {
                case SDL_WINDOWEVENT:
                    switch (event.window.event)
                    {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                    {
                        abort_render_token = pending_resize = true;
                    }
                    break;
                    }
                    break;
                case SDL_QUIT:
                    {
                        abort_render_token = quit = true;
                    }
                    break; 

                case SDL_KEYUP:
                    {
                        int ascii = SDL_Keysym_to_Ascii(event.key.keysym);
                        if (ascii > 0) 
                        {
                            keyboard_surface_data.set(ascii, 0, 0);
                        }
                    }
                    break;

                case SDL_KEYDOWN:
                    switch ( event.key.keysym.sym ) 
                    {
                    case SDLK_SPACE:
                        blit_now = true;
                        break;
                    case SDLK_ESCAPE:
                        {
                            abort_render_token = quit = true;
                        }
                        break;
                    case SDLK_f:
                        break;
                    case SDLK_p:
                        time_scale = (time_scale > 0 ? 0.0f : 1.0f);
                        break;
                    case SDLK_LEFT:
                        time = std::max(0.0f, time - 1.0f);
                        break;
                    case SDLK_RIGHT:
                        time += 1.0f;
                        break;
                    default:
                        break;
                    }

                    {
                        int ascii = SDL_Keysym_to_Ascii(event.key.keysym);
                        if (ascii > 0) 
                        {
                            keyboard_surface_data.set(ascii, 0, 0xFF);
                            keyboard_surface_data.set(ascii, 1, 0xFF);
                            keyboard_surface_data.set(ascii, 2, keyboard_surface_data.get(ascii, 2) ? 0 : 0xFF);
                        }
                    }
                    
                    break;
                case SDL_MOUSEMOTION:
                    if (mouse_pressed)
                    {
                        mouse_x = event.button.x;
                        mouse_y = event.button.y;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mouse_clicked = mouse_pressed = true;
                    mouse_press_x = mouse_x = event.button.x;
                    mouse_press_y = mouse_y = event.button.y;
                    break;
                case SDL_MOUSEBUTTONUP:
                    mouse_pressed = false;
                    break;

                default:
                    break;
                }
            }

            if (quit)
            {
                break;
            }

            bool frameReady = !(bool)render_task.wait_for(chrono::microseconds{ 33 });

            if (pending_resize)
            {
                if (frameReady)
                {
                    int w, h;
                    SDL_GetWindowSize(window.get(), &w, &h);

                    resize_or_create_surfaces(w, h);
                    pending_resize = false;

                    current_frame_timestamp = time;
                    render_task = render_async();
                }
            }
            else
            {
                if (blit_now || frameReady )
                {
                    SDL_Rect rect;
                    rect.x = 0;
                    rect.y = 0;
                    rect.w = resolution.x;
                    rect.h = resolution.y;
                    auto& s = target_surface;
                    SDL_UpdateTexture(target_texture.get(), &rect, s->pixels, s->pitch);
                    SDL_RenderClear(renderer.get());
                    SDL_RenderCopy(renderer.get(), target_texture.get(), NULL, NULL);

                    SDL_RenderPresent(renderer.get());
                }

                if (frameReady)
                {
                    ++num_frames;
                    last_render_stats = render_task.get();
                    last_frame_timestamp = current_frame_timestamp;

                    // update fps
                    ++fps_frames_num;
                    fps_frames_duration += last_render_stats.duration;
                    if (fps_frames_duration >= std::chrono::milliseconds(500))
                    {
                        current_fps = fps_frames_num / duration_to_seconds(fps_frames_duration);
                        fps_frames_num = 0;
                        fps_frames_duration = {};
                    }

                    current_frame_timestamp = time;
                    render_task = render_async();
                }
            }

            // update timers
            auto now = chrono::steady_clock::now();
            auto delta = chrono::duration_cast<chrono::microseconds>(now - update_begin);

            time += static_cast<float>(duration_to_seconds(delta) * time_scale);
            update_begin = now;
            
            printf(VT100_UP(STATS_LINES) "\r");
            printf(VT100_CLEARLINE "--- Last frame stats ---\n");
            printf(VT100_CLEARLINE "timestamp:       %.2f s\n", last_frame_timestamp);
            printf(VT100_CLEARLINE "duration:        %lg s\n", duration_to_seconds(last_render_stats.duration));
            printf(VT100_CLEARLINE " - per pixel:    %lg ms\n", static_cast<double>(last_render_stats.duration.count()) / (last_render_stats.num_pixels));
            printf(VT100_CLEARLINE "threads:         %d\n", last_render_stats.num_threads);
            printf(VT100_CLEARLINE "\n");
            printf(VT100_CLEARLINE "--- Player stats ---\n");
            printf(VT100_CLEARLINE "time:            %.2f s%s\n", time, time_scale > 0 ? "" : " (paused)");
            printf(VT100_CLEARLINE "frames:          %d\n", num_frames);
            printf(VT100_CLEARLINE "resolution:      %dx%d\n", target_surface->w, target_surface->h);
            printf(VT100_CLEARLINE "fps:             %lg\n", current_fps);
        }

        // wait for the render thread to stop


        printf("\nwaiting for the worker thread to finish...");
        render_task.wait();
    } 
    catch ( exception& error ) 
    {
        fprintf(stderr, "ERROR: %s\n", error.what());
    } 
    catch (...) 
    {
        fprintf(stderr, "ERROR: Unknown error\n");
    }

    SDL_Quit();
    return 0; 
}


#if WIN32
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
static int result_ENABLE_VIRTUAL_TERMINAL_PROCESSING = []() -> int {
    // https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#example
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return GetLastError();
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return GetLastError();
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return GetLastError();
    }
    return 0;
}();

static int result_SetProcessDPIAware = []() -> int {
    if (!SetProcessDPIAware()) 
    {
        return GetLastError();
    }
    return 0;
}();


#endif

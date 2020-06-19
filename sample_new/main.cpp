// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

#include "shadertoy_sandbox.hpp"

using namespace swizzle;
using namespace shadertoy;


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

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_video.h>

#ifdef SAMPLE_SDL2IMAGE_FOUND
#include <SDL_image.h>
#endif

#include <time.h>
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

struct sdl_aligned_deleter
{
    void operator()(SDL_Surface* ptr)
    {
        delete ptr->userdata;
        SDL_FreeSurface(ptr);
    }
};


using surface_ptr = std::unique_ptr<SDL_Surface, sdl_deleter>;
using aligned_surface_ptr = std::unique_ptr<SDL_Surface, sdl_aligned_deleter>;
using window_ptr = std::unique_ptr<SDL_Window, sdl_deleter>;
using texture_ptr = std::unique_ptr<SDL_Texture, sdl_deleter>;
using renderer_ptr = std::unique_ptr<SDL_Renderer, sdl_deleter>;


struct render_stats
{
    std::chrono::microseconds duration;
    int num_pixels;
    int num_threads;
};

struct sampler_data : swizzle::naive_sampler_data 
{
    const char* path = nullptr;
    int buffer_index = -1;
};

aligned_surface_ptr create_aligned_rgba32_surface(int width, int height)
{
    const int bytes_per_pixel = 4;
    const int align = 32;

    // each row has to be allocated
    int pitch = (width * bytes_per_pixel + align - 1) & (~align);
    size_t pixels_size = static_cast<size_t>(pitch * height);
    size_t alloc_size = pixels_size + align - 1;
    
    // alloc and align
    void* pixels = ::operator new (alloc_size);
    void* aligned_pixels = pixels;
    aligned_pixels = std::align(align, pixels_size, aligned_pixels, alloc_size);
    assert(aligned_pixels);

    // init surface
    aligned_surface_ptr result(SDL_CreateRGBSurfaceWithFormatFrom(aligned_pixels, width, height, 32, pitch, SDL_PIXELFORMAT_RGBA32));
    if (!result) 
    {
        delete pixels;
    }
    else 
    {
        result->userdata = pixels;
    }
    
    assert(result);
    return std::move(result);
}


using pixel_func = swizzle::vec4(*)(const shader_inputs& input, swizzle::vec2 fragCoord, swizzle::vec4 fragColor, swizzle::bool_type* discarded);

// 
struct sdl_surface_storage {

    //using uint32_traits = detail::batch_traits<swizzle::uint_type>;

    //surface_ptr surface;
    //int bytes_per_pixel;

    //uint32_traits::aligned_storage_type aligned_blob_r;
    //uint32_traits::aligned_storage_type aligned_blob_g;
    //uint32_traits::aligned_storage_type aligned_blob_b;

    //uint8_t* begin_row(int row) {
    //    return reinterpret_cast<uint8_t*>(surface->pitch) + surface->pitch * bytes_per_pixel;
    //}

    //uint8_t* advance(uint8_t* ptr) {
    //    return ptr + columns_per_batch * bytes_per_pixel;
    //}

    //swizzle::vec4 get_color(uint8_t* ptr) {
    //    uint32_t* pr = reinterpret_cast<uint32_t*>(&aligned_blob_r);
    //    uint32_t* pg = reinterpret_cast<uint32_t*>(&aligned_blob_g);
    //    uint32_t* pb = reinterpret_cast<uint32_t*>(&aligned_blob_b);

    //}

    //void set_color(uint8_t* ptr, swizzle::vec4 color)
    //{
    //    // clamp results
    //    color = vec4::call_clamp(color * 255.0f + 0.5f, 0.0f, 255.0f);
    //    swizzle::store_r8g8b8(ptr, static_cast<swizzle::uint_type>(color.r), static_cast<swizzle::uint_type>(color.g), static_cast<swizzle::uint_type>(color.b));
    //}
};

struct sdl_write_only_rt {
    SDL_Surface* surface;
    
    size_t pitch_bytes;

    const size_t texel_bytes = 4;

    uint8_t* get_ptr()
    {
        return reinterpret_cast<uint8_t*>(surface->pixels);
    }

    vec4 get_color_batch(uint8_t* ptr)
    {
        return vec4(0);
    }

    inline void set_color_batch(uint8_t* ptr, const vec4& color)
    {
        store_rgba32_aligned(color.r, color.g, color.b, color.a, ptr, pitch_bytes);
    }
};

using uint32_traits = detail::batch_traits<swizzle::uint_type>;
using float_traits = ::swizzle::detail::batch_traits<swizzle::float_type>;


constexpr auto pixels_per_batch = static_cast<int>(float_traits::size);
static_assert(pixels_per_batch == 1 || pixels_per_batch % 2 == 0, "1 or even scalar count");
constexpr auto columns_per_batch = pixels_per_batch > 1 ? pixels_per_batch / 2 : 1;
constexpr auto rows_per_batch = pixels_per_batch > 1 ? 2 : 1;


static render_stats render(pixel_func func, shader_inputs uniforms, SDL_Surface* bmp, const std::atomic_bool& cancelled)
{
    assert(bmp->w % columns_per_batch == 0);
    assert(bmp->h % rows_per_batch == 0);

    using ::swizzle::detail::static_for;
    using namespace ::swizzle;

    static_assert(float_traits::size == uint32_traits::size);



    auto render_begin = std::chrono::steady_clock::now();

    // if there are more than 1 scalars in a vector, work on two rows with half width at the same time
    swizzle::float_type x_offsets(0);
    swizzle::float_type y_offsets(0);

    std::atomic<int> num_pixels = 0;
    std::atomic<int> num_threads = 0;

    {
        float_traits::aligned_storage_type aligned_storage;
        float* aligned = reinterpret_cast<float*>(&aligned_storage);

        static_for<0, float_traits::size>([&](size_t i) { aligned[i] = static_cast<float>(i % columns_per_batch) + 0.5f; });
        load_aligned(x_offsets, aligned);

        static_for<0, float_traits::size>([&](size_t i) { aligned[i] = static_cast<float>(1 - i / columns_per_batch) + 0.5f; });
        load_aligned(y_offsets, aligned);
    }
  
    
    int min_x = 0;
    int min_y = 0;
    int max_x = bmp->w;
    int max_y = bmp->h;

    sdl_write_only_rt surface;
    surface.surface = bmp;
    surface.pitch_bytes = bmp->pitch;

#if SAMPLE_OMP_ENABLED
#pragma omp parallel 
    {
        if (omp_get_thread_num() == 0)
        {
            num_threads = omp_get_num_threads();
        }
#else
    {
        num_threads = 1;
#endif
        
#if SAMPLE_OMP_ENABLED
#pragma omp for
#endif
        for (int y = min_y; y < max_y; y += rows_per_batch)
        {
            if (cancelled)
                continue;

            swizzle::float_type frag_coord_y = static_cast<float>(bmp->h - 1 - y) + y_offsets;

            uint8_t* ptr = surface.get_ptr() + y * surface.pitch_bytes + min_x * surface.texel_bytes;

            int x;
            for (x = min_x; x < max_x; x += columns_per_batch)
            {
                swizzle::vec4 previousColor = surface.get_color_batch(ptr);

                auto color = func(uniforms, vec2(static_cast<float>(x) + x_offsets, frag_coord_y), {}, nullptr);

                surface.set_color_batch(ptr, color);

                ptr += surface.texel_bytes * columns_per_batch;
            }

            num_pixels += (max_x - min_x) * rows_per_batch;
        }
    }

    return render_stats { std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - render_begin), num_pixels, num_threads };
}

void set_textures(shader_inputs& inputs, const sampler_data ptr[4])
{
    inputs.iChannel0.data = ptr;
    inputs.iChannel1.data = ptr + 1;
    inputs.iChannel2.data = ptr + 2;
    inputs.iChannel3.data = ptr + 3;
    for (int i = 0; i < 4; ++i)
    {
        inputs.iChannelResolution[i] = vec3(ptr[i].width, ptr[i].height, 0.0f);
    }
}

#define STR1(x) #x
#define STR2(x) STR1(x)
#define VT100_CLEARLINE "\33[2K"
#define VT100_UP(x)     "\33[" STR2(x) "A"
#define VT100_DOWN(x)   "\33[" STR2(x) "B"
#define STATS_LINES 11

const double seconds_to_micro = 1000000.0;
const double micro_to_seconds = 1 / seconds_to_micro;

void from_surface(swizzle::naive_sampler_data& sampler, const SDL_Surface* surface)
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
bool load_texture(swizzle::naive_sampler_data& sampler, const char* name)
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
        from_surface(sampler, img);
        return true;
    }
#else
    fprintf(stderr, "ERROR: Failed to load texture %s, SDL_image was not found", name);
    return false;
#endif
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
            texture.path = nullptr;
        }
        else if (!strcmp(texture.path, "buffer_a")) 
        {
            texture.buffer_index = 0;
        }
        else if (!strcmp(texture.path, "buffer_b"))
        {
            texture.buffer_index = 1;
        }
        else if (!strcmp(texture.path, "buffer_c"))
        {
            texture.buffer_index = 2;
        }
        else if (!strcmp(texture.path, "buffer_d"))
        {
            texture.buffer_index = 3;
        }
        else
        {
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
        window_ptr window(SDL_CreateWindow("CxxSwizzle sample", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, resolution.x, resolution.y, SDL_WINDOW_RESIZABLE));
        renderer_ptr renderer(SDL_CreateRenderer(window.get(), -1, 0));
        aligned_surface_ptr target_surface;
        texture_ptr target_texture;
        surface_ptr buffer_surfaces[4][2];

        // a function used to resize the target_surface
        auto resize_or_create_surfaces = [&](int w, int h) -> void
        {
            // let's make life easier and make sure surfaces are aligned to the batch size
            auto aligned_w = ((w + columns_per_batch - 1) / columns_per_batch) * columns_per_batch;
            auto aligned_h = ((h + rows_per_batch - 1) / rows_per_batch) * rows_per_batch;

            auto create_matching_surface = [=]()-> auto { return SDL_CreateRGBSurface(0, aligned_w, aligned_h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000); };

#ifdef SAMPLE_HAS_BUFFER_A
            buffer_surfaces[0][0].reset(create_matching_surface()); buffer_surfaces[0][1].reset(create_matching_surface());
#endif
#ifdef SAMPLE_HAS_BUFFER_B
            buffer_surfaces[1][0].reset(create_matching_surface()); buffer_surfaces[1][1].reset(create_matching_surface());
#endif
#ifdef SAMPLE_HAS_BUFFER_C
            buffer_surfaces[2][0].reset(create_matching_surface()); buffer_surfaces[2][1].reset(create_matching_surface());
#endif
#ifdef SAMPLE_HAS_BUFFER_D
            buffer_surfaces[3][0].reset(create_matching_surface()); buffer_surfaces[3][1].reset(create_matching_surface());
#endif

            target_surface = create_aligned_rgba32_surface(aligned_w, aligned_h);
            if ( !target_surface )
            {
                throw std::runtime_error("Unable to create target_surface");
            }

            target_texture.reset(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, w, h));
            if ( !target_texture )
                throw std::runtime_error("Unable to create target_texture");

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

        render_stats last_render_stats = { };
        float last_frame_timestamp = 0.0f;

        std::chrono::microseconds fps_frames_duration = {};
        int fps_frames_num = 0;
        double current_fps = 0;

        std::atomic_bool abort_render_token = false;

        auto render_all = [&target_surface, &buffer_surfaces, &textures, &num_frames](shader_inputs inputs, const std::atomic_bool& cancel) -> auto
        {
            int buffer_surface_index = 1 - (num_frames & 1);

#ifdef SAMPLE_HAS_BUFFER_A
            set_textures(inputs, &textures[4]);
            render(shadertoy::buffer_a, inputs, buffer_surfaces[0][buffer_surface_index].get(), cancel);
#endif
#ifdef SAMPLE_HAS_BUFFER_B
            set_textures(inputs, &textures[8]);
            render(shadertoy::buffer_b, inputs, buffer_surfaces[1][buffer_surface_index].get(), cancel);
#endif
#ifdef SAMPLE_HAS_BUFFER_C
            set_textures(inputs, &textures[12]);
            render(shadertoy::buffer_c, inputs, buffer_surfaces[2][buffer_surface_index].get(), cancel);
#endif
#ifdef SAMPLE_HAS_BUFFER_D
            set_textures(inputs, &textures[16]);
            render(shadertoy::buffer_d, inputs, buffer_surfaces[3][buffer_surface_index].get(), cancel);
#endif
            set_textures(inputs, &textures[0]);
            return render(shadertoy::image, inputs, target_surface.get(), cancel);
        };


        auto render_async = [&]() -> std::future<render_stats>
        {
            abort_render_token = false;
            
            {
                int buffer_surface_index = num_frames & 1;
                // sort out buffer textures
                for (auto& data : textures)
                {
                    if (data.buffer_index >= 0)
                    {
                        from_surface(data, buffer_surfaces[data.buffer_index][buffer_surface_index].get());
                    }
                }
            }

            auto s = target_surface.get();

            shader_inputs inputs = {};
            inputs.iTime = time;
            inputs.iFrame = num_frames;
            inputs.iResolution = vec3(static_cast<float>(s->w), static_cast<float>(s->h), 0.0f);
            inputs.iMouse.x = mouse_x / static_cast<float>(s->w);
            inputs.iMouse.y = mouse_y / static_cast<float>(s->h);
            inputs.iMouse.z = mouse_pressed ? 1.0f : 0.0f;
            inputs.iMouse.w = 0.0f;

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
                    break;
                case SDL_MOUSEMOTION:
                    if (mouse_pressed)
                    {
                        mouse_x = event.button.x;
                        mouse_y = event.button.y;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mouse_pressed = true;
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
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
                    SDL_UpdateTexture(target_texture.get(), &rect, target_surface->pixels, target_surface->pitch);
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
                    if (fps_frames_duration >= std::chrono::seconds(1))
                    {
                        current_fps = fps_frames_num / (fps_frames_duration.count() * micro_to_seconds);
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
            time += static_cast<float>(delta.count() * micro_to_seconds * time_scale);
            update_begin = now;
            
            printf(VT100_UP(STATS_LINES) "\r");
            printf(VT100_CLEARLINE "--- Last frame stats ---\n");
            printf(VT100_CLEARLINE "timestamp:       %.2f s\n", last_frame_timestamp);
            printf(VT100_CLEARLINE "duration:        %lg s\n", micro_to_seconds * last_render_stats.duration.count());
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

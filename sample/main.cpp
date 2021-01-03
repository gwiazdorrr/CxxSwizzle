// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

#include "shadertoy_sandbox.hpp"

using namespace swizzle;
using namespace shadertoy;

static_assert(sizeof(vec2) == sizeof(swizzle::float_type[2]), "Too big");
static_assert(sizeof(vec3) == sizeof(swizzle::float_type[3]), "Too big");
static_assert(sizeof(vec4) == sizeof(swizzle::float_type[4]), "Too big");

#include <condition_variable>
#include <filesystem>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <fstream>
#include <unordered_map>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_video.h>
#include <SDL_image.h>
#include <nlohmann/json.hpp>

#include <ctime>

#if SAMPLE_OMP_ENABLED
#include <omp.h>
#endif


// some SDL goodies

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

template <typename T>
using sdl_ptr = std::unique_ptr<T, sdl_deleter>;

template <typename TSdlType>
sdl_ptr<TSdlType> make_sdl_ptr(TSdlType* ptr)
{
    return sdl_ptr<TSdlType>(ptr);
}

template <typename TFunc, typename... TArgs>
auto create_sdl_object_or_throw(TFunc&& f, TArgs&&... args)
{
    auto ptr = make_sdl_ptr(f(std::forward<TArgs>(args)...));
    if (!ptr)
    {
        std::stringstream s;
        s << "SDL error: " << SDL_GetError();   
        throw std::runtime_error(s.str());
    }
    return ptr;
}

template <typename TFunc, typename... TArgs>
auto create_sdl_image_object_or_throw(TFunc&& f, TArgs&&... args)
{
    auto ptr = make_sdl_ptr(f(std::forward<TArgs>(args)...));
    if (!ptr)
    {
        std::stringstream s;
        s << "SDL_image error: " << IMG_GetError();
        throw std::runtime_error(s.str());
    }
    return ptr;
}

template <typename TRenderTarget>
sdl_ptr<SDL_Surface> create_matching_sdl_surface(TRenderTarget& target)
{
    return create_sdl_object_or_throw(SDL_CreateRGBSurfaceWithFormatFrom, target.first_row, target.width, target.height, 32, target.pitch, target.sdl_pixelformat);
}

sdl_ptr<SDL_Texture> create_matching_sdl_texture(SDL_Renderer* renderer, int w, int h)
{
    return create_sdl_object_or_throw(SDL_CreateTexture, renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, w, h);
};


constexpr auto pixels_per_batch  = static_cast<int>(::swizzle::detail::batch_traits<swizzle::float_type>::size);
constexpr auto columns_per_batch = pixels_per_batch > 1 ? pixels_per_batch / 2 : 1;
constexpr auto rows_per_batch    = pixels_per_batch > 1 ? 2 : 1;
static_assert(pixels_per_batch == 1 || pixels_per_batch % 2 == 0, "1 or even scalar count");

struct aligned_render_target_base
{
    struct void_deleter
    {
        void operator()(void* ptr)
        {
            ::operator delete(ptr);
        }
    };

    std::unique_ptr<void, void_deleter> memory;
    void* first_row = nullptr;
    int pitch = 0;
    int width = 0;
    int height = 0;
    size_t pixels_size = 0;

    aligned_render_target_base() = default;
    aligned_render_target_base(int width, int height, int bpp, int align) : width(width), height(height)
    {
        const int mask = align - 1;

        // each row has to be aligned
        pitch = (width * bpp + mask) & (~mask);
        pixels_size = static_cast<size_t>(pitch * height);
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
    static inline const int bytes_per_pixel = 4;
    static inline const uint32_t sdl_pixelformat = SDL_PIXELFORMAT_RGBA32;

    render_target_rgba32() = default;
    render_target_rgba32(int width, int height) : aligned_render_target_base(width, height, bytes_per_pixel, 32) {}


    void store(uint8_t* ptr, const vec4& color, size_t pitch)
    {
        store_rgba8_aligned(color.r, color.g, color.b, color.a, ptr, pitch);
    }
};

struct render_target_a8 : aligned_render_target_base
{
    static inline const int bytes_per_pixel = 1;
    static inline const uint32_t sdl_pixelformat = SDL_PIXELFORMAT_INDEX8;

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

using image_map = std::unordered_map<std::string, sdl_ptr<SDL_Surface>>;

using buffer_render_target_list = std::array<render_target_float, shadertoy::num_buffers>;

void ensure_texture_loaded(image_map& textures, const std::string& path)
{
    if (textures.find(path) == textures.end())
    {
        textures[path] = create_sdl_image_object_or_throw(IMG_Load, path.c_str());
    }
}

const SDL_Surface* get_image_or_throw(const image_map& textures, const std::string& path)
{
    auto found = textures.find(path);
    if (found == textures.end())
    {
        throw std::runtime_error(path);
    }
    return found->second.get();
}

swizzle::naive_sampler_data make_sampler_data(const render_target_float& rt)
{
    swizzle::naive_sampler_data sampler;
    sampler.bytes = reinterpret_cast<uint8_t*>(rt.first_row);
    sampler.width = rt.width;
    sampler.height = rt.height;
    sampler.pitch_bytes = static_cast<unsigned>(rt.pitch);
    sampler.bytes_per_pixel = rt.bytes_per_pixel;
    sampler.is_floating_point = true;
    return sampler;
}

swizzle::naive_sampler_data make_sampler_data(const SDL_Surface* surface)
{
    swizzle::naive_sampler_data sampler;

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

    return sampler;
}

enum class sampler_type 
{
    none,
    texture,
    buffer,
    keyboard,
    cubemap,
};

enum class pass_type : int 
{
    image,
    buffer_a,
    buffer_b,
    buffer_c,
    buffer_d
};

NLOHMANN_JSON_SERIALIZE_ENUM(sampler_type, {
    {sampler_type::none, "none"},
    {sampler_type::texture, "texture"},
    {sampler_type::buffer, "buffer"},
    {sampler_type::keyboard, "keyboard"},
    {sampler_type::cubemap, "cubemap"},
});

NLOHMANN_JSON_SERIALIZE_ENUM(pass_type, {
    {pass_type::image, "image"},
    {pass_type::buffer_a, "buffer_a"},
    {pass_type::buffer_b, "buffer_b"},
    {pass_type::buffer_c, "buffer_c"},
    {pass_type::buffer_d, "buffer_d"}
});


struct sampler_config
{
    std::string label;

    sampler_type type = sampler_type::none;
    std::vector<std::string> paths;

    int source_buffer_index;
};

struct pass_config 
{
    sampler_config samplers[shadertoy::num_samplers];
    sampler_config& get_sampler(int index)
    {
        return samplers[index];
    }
};

struct shadertoy_config
{
    pass_config passes[1 + shadertoy::num_buffers];
    pass_config& get_pass(pass_type pass)
    {
        return passes[static_cast<int>(pass)];
    }
};

struct render_stats
{
    std::chrono::microseconds duration;
    int num_pixels;
    int num_threads;
};

template <typename TPixelFunc, typename TRenderTarget>
static render_stats render(TPixelFunc func, shader_inputs uniforms, TRenderTarget& rt, const std::atomic_bool& cancelled)
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
  
    
    const int max_x = rt.width;
    const int max_y = rt.height;

#if !SAMPLE_OMP_ENABLED
    {
        num_threads = 1;
#else
    #pragma omp parallel default(shared)
    {
        #pragma omp master
        num_threads = omp_get_num_threads();

        #pragma omp for
#endif
        for (int y = 0; y < max_y; y += rows_per_batch)
        {
            if (cancelled)
                continue;

            swizzle::float_type frag_coord_y = static_cast<float>(max_y - y) - y_offsets;

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

struct textures_context
{
    const image_map& images;
    const buffer_render_target_list& buffers;
    const SDL_Surface* keyboard;

    textures_context(const image_map& images, const buffer_render_target_list& buffers, const SDL_Surface* keyboard) 
    : images(images),
      buffers(buffers),
      keyboard(keyboard)
    {}
};


void bind_textures(shader_inputs& inputs, naive_sampler_data* data_buffer, const pass_config& pass, const textures_context& context)
{
    std::array<sampler2D*, shadertoy::num_samplers> samplers = 
    {
        &inputs.iChannel0,
        &inputs.iChannel1,
        &inputs.iChannel2,
        &inputs.iChannel3,
    };

    for (int sampler_no = 0; sampler_no < shadertoy::num_samplers; ++sampler_no)
    {
        const sampler_config& data = pass.samplers[sampler_no];

        samplers[sampler_no]->data = data_buffer;
        samplers[sampler_no]->face_count = 1;

        if (data.type == sampler_type::buffer)
        {
            *data_buffer++ = make_sampler_data(context.buffers[data.source_buffer_index]);
        }
        else if (data.type == sampler_type::keyboard)
        {
            *data_buffer++ = make_sampler_data(context.keyboard);
        }
        else if (data.type == sampler_type::cubemap || data.type == sampler_type::texture)
        {
            samplers[sampler_no]->face_count = data.paths.size();
            for (auto& path: data.paths)
            {
                *data_buffer++ = make_sampler_data(get_image_or_throw(context.images, path));
            }
        }

        inputs.iChannelResolution[sampler_no] = { samplers[sampler_no]->data->width, samplers[sampler_no]->data->height, 0 };
        inputs.iChannelTime[sampler_no] = 0.0f;
    }
}


struct shadertoy_render_buffers
{
    int width;
    int height;

    render_target_rgba32 target;

    // buffers are double buffered (sic!)
    buffer_render_target_list buffers[2];

    shadertoy_render_buffers(int width, int height)
        : width(width), height(height)
    {
        // let's make life easier and make sure surfaces are aligned to the batch size
        auto aligned_w = ((width + columns_per_batch - 1) / columns_per_batch) * columns_per_batch;
        auto aligned_h = ((height + rows_per_batch - 1) / rows_per_batch) * rows_per_batch;

        auto create_buffer_surface = [=]() -> auto { return render_target_float(aligned_w, aligned_h); };

#ifdef SAMPLE_HAS_BUFFER_A
        buffers[0][0] = create_buffer_surface(); buffers[1][0] = create_buffer_surface();
#endif
#ifdef SAMPLE_HAS_BUFFER_B
        buffers[0][1] = create_buffer_surface(); buffers[1][1] = create_buffer_surface();
#endif
#ifdef SAMPLE_HAS_BUFFER_C
        buffers[0][2] = create_buffer_surface(); buffers[1][2] = create_buffer_surface();
#endif
#ifdef SAMPLE_HAS_BUFFER_D
        buffers[0][3] = create_buffer_surface(); buffers[1][3] = create_buffer_surface();
#endif

        target = render_target_rgba32(aligned_w, aligned_h);
    }
};


#define STR1(x) #x
#define STR2(x) STR1(x)
#define VT100_CLEARLINE "\33[2K"
#define VT100_UP(x)     "\33[" STR2(x) "A"
#define VT100_DOWN(x)   "\33[" STR2(x) "B"
#define STATS_LINES 11




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
    printf("-h                          show this message\n");
    printf("-o <path>                   render one frame, save to <path> (PNG) and quit.");
    printf("-r <width> <height>         set initial resolution\n");
    printf("-c <path>                   config path (default: %s)\n", SAMPLE_CONFIG_PATH);
    printf("-t <time>                   set initial time & pause\n");
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

shadertoy_config apply_config(const char* path, image_map& textures)
{
    using json = nlohmann::json;

    std::ifstream i(path);
    json j;
    i >> j;

    std::unordered_map<std::string_view, pass_type> string_to_pass_type =
    {
        { "image",    pass_type::image },
        { "buffer_a", pass_type::buffer_a },
        { "buffer_b", pass_type::buffer_b },
        { "buffer_c", pass_type::buffer_c },
        { "buffer_d", pass_type::buffer_d },
    };

    std::array<std::string, shadertoy::num_samplers> channel_keys = {
        "iChannel0",
        "iChannel1",
        "iChannel2",
        "iChannel3",
    };


    std::map<std::string, int> buffer_name_to_index = {
        { "buffer_a", 0 },
        { "buffer_b", 1 },
        { "buffer_c", 2 },
        { "buffer_d", 3 },
    };

    shadertoy_config result;

    for (json::iterator it_pass = j.begin(); it_pass != j.end(); ++it_pass)
    {
        pass_type pass_id = string_to_pass_type[it_pass.key()];
        pass_config& pass = result.passes[static_cast<int>(pass_id)];

        for (size_t channel_index = 0; channel_index < channel_keys.size(); ++channel_index)
        {
            auto it_channel = it_pass->find(channel_keys[channel_index]);
            if (it_channel != it_pass->end())
            {
                auto& channel_node = it_channel.value();

                sampler_config sampler;
                sampler.type = channel_node["type"];

                if (sampler.type == sampler_type::texture)
                {
                    std::string path = channel_node["src"];
                    sampler.paths.push_back(path);
                    ensure_texture_loaded(textures, path);

                    sampler.label = path;
                }
                else if (sampler.type == sampler_type::cubemap)
                {
                    auto& src = channel_node["src"];
                    if (src.size() != 6)
                    {
                        throw std::runtime_error("Cubemap is expected to have 6 faces");
                    }

                    for (size_t i = 0; i < 6; ++i)
                    {
                        std::string path = src[i];
                        sampler.paths.push_back(path);
                        ensure_texture_loaded(textures, path);
                    }

                    sampler.label = sampler.paths[0];
                }
                else if (sampler.type == sampler_type::buffer)
                {
                    sampler.label = channel_node["src"];
                    sampler.source_buffer_index = buffer_name_to_index[sampler.label];
                }
                else if (sampler.type == sampler_type::keyboard)
                {
                    sampler.label = "keyboard";
                }

                pass.samplers[channel_index] = std::move(sampler);
            }
        }
    }

    return std::move(result);
}


#ifndef _MSC_VER
#define __cdecl
#endif

int __cdecl main(int argc, char* argv[])
{
    using namespace std;
    

    // initialise SDLImage
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int initted = IMG_Init(flags);
    if ((initted & flags) != flags) 
    {
        fprintf(stderr, "WARNING: failed to initialize required jpg and png support: %s\n", IMG_GetError());
    }

    swizzle::vector<int, 2> resolution(512, 288);
    float time = 0.0f;
    float time_scale = 1.0f;
    const char* output_path = nullptr;

    static_assert(::shadertoy::num_samplers >= 4);
    
    const char* config_path = SAMPLE_CONFIG_PATH;


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
        else if (!strcmp(arg, "-o"))
        {
            if (i + 1 < argc) 
            {
                output_path = argv[++i];
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
        else if (!strcmp(arg, "-c"))
        {
            if (i + 1 < argc)
            {
                config_path = argv[++i];
            }
            else
            {
                return print_args_error();
            }
        }
    }

    image_map images;
    shadertoy_config config = apply_config(config_path, images);
    
    if (resolution.x <= 0 || resolution.y < 0 )
    {
        fprintf(stderr, "ERROR: invalid resolution: %dx%d\n", resolution.x, resolution.y);
        return 1;
    }

    // initial setup
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "ERROR: Unable to init SDL\n");
        return 1;
    }


    try
    {
        shadertoy_render_buffers render_targets(resolution.x, resolution.y);

        // keyboard texture is a special one: first row contains down state, second whether it was pressed 
        // in this frame and the last one is a toggle; also, keyboard needs to be double buffered
        render_target_a8 keyboard_data = { 256, 3 };
        render_target_a8 keyboard_surface_data = { 256, 3 };
        sdl_ptr<SDL_Surface> keyboard_surface = create_matching_sdl_surface(keyboard_surface_data);
        
        const int keyboard_row_down = 0;
        const int keyboard_row_pressed = 1;
        const int keyboard_row_toggle = 2;


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

        auto render_all = [&config](shader_inputs inputs, render_target_rgba32& target, buffer_render_target_list& buffers, const textures_context& context, const std::atomic_bool& cancel) -> auto
        {
            std::chrono::microseconds duration(0);   
            naive_sampler_data data_buffer[6 * num_samplers];

#ifdef SAMPLE_HAS_BUFFER_A
            bind_textures(inputs, data_buffer, config.get_pass(pass_type::buffer_a), context);
            duration += render(shadertoy::buffer_a, inputs, buffers[0], cancel).duration;
#endif
#ifdef SAMPLE_HAS_BUFFER_B
            bind_textures(inputs, data_buffer, config.get_pass(pass_type::buffer_b), context);
            duration += render(shadertoy::buffer_b, inputs, buffers[1], cancel).duration;
#endif
#ifdef SAMPLE_HAS_BUFFER_C
            bind_textures(inputs, data_buffer, config.get_pass(pass_type::buffer_c), context);
            duration += render(shadertoy::buffer_c, inputs, buffers[2], cancel).duration;
#endif
#ifdef SAMPLE_HAS_BUFFER_D
            bind_textures(inputs, data_buffer, config.get_pass(pass_type::buffer_d), context);
            duration += render(shadertoy::buffer_d, inputs, buffers[3], cancel).duration;
#endif
            bind_textures(inputs, data_buffer, config.get_pass(pass_type::image), context);
            render_stats result = render(shadertoy::image, inputs, target, cancel);
            result.duration += duration;
            return result;
        };


        auto render_async = [&]() -> std::future<render_stats>
        {
            abort_render_token = false;
            
            // bring about keyboard changes
            memcpy(keyboard_surface_data.first_row, keyboard_data.first_row, keyboard_data.pixels_size);

            // clear down flag
            for (int i = 0; i < 256; ++i)
            {
                keyboard_data.set(i, keyboard_row_pressed, 0);
            }

            shader_inputs inputs = {};
            inputs.iTime         = time;
            inputs.iTimeDelta    = static_cast<float>(duration_to_seconds(last_render_stats.duration));
            inputs.iFrameRate    = static_cast<int>(current_fps);
            inputs.iFrame        = num_frames;
            inputs.iResolution   = vec3(static_cast<float>(render_targets.width), static_cast<float>(render_targets.height), 0.0f);
            inputs.iMouse.x      = static_cast<float>(mouse_x);
            inputs.iMouse.y      = static_cast<float>(render_targets.height - 1 - mouse_y);
            inputs.iMouse.z      = (mouse_pressed ? 1.0f : -1.0f) * mouse_press_x;
            inputs.iMouse.w      = (mouse_clicked ? 1.0f : -1.0f) * (render_targets.height - 1 - mouse_press_y);

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

            textures_context context(images, render_targets.buffers[1 - num_frames & 1], keyboard_surface.get());
            return std::async(render_all, inputs, std::ref(render_targets.target), std::ref(render_targets.buffers[num_frames & 1]), context, std::ref(abort_render_token));
        };

        std::future<render_stats> render_task = render_async();

        if (output_path != nullptr)
        {
            render_task.wait();
            sdl_ptr<SDL_Surface> target_surface = create_matching_sdl_surface(render_targets.target);
            IMG_SavePNG(target_surface.get(), output_path);
        }
        else
        {
            sdl_ptr<SDL_Window> window          = create_sdl_object_or_throw(SDL_CreateWindow, "CxxSwizzle sample", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, resolution.x, resolution.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
            sdl_ptr<SDL_Renderer> renderer      = create_sdl_object_or_throw(SDL_CreateRenderer, window.get(), -1, 0);
            sdl_ptr<SDL_Texture> target_texture = create_matching_sdl_texture(renderer.get(), resolution.x, resolution.y);

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
                for (int pass_index = 0; pass_index < (shadertoy::num_buffers + 1); ++pass_index)
                {
                    int sampler_index = 0;
                    for (auto& sampler : config.passes[pass_index].samplers)
                    {
                        ++sampler_index;

                        if (sampler.type == sampler_type::none)
                            continue;

                        printf("%siChannel%d: %s\n", channel_prefix[pass_index], sampler_index, sampler.label.c_str());
                    }
                }
            }
            printf("\n");

            for (int i = 0; i < STATS_LINES; ++i)
                printf("\n");


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
                            keyboard_data.set(ascii, keyboard_row_down, 0);
                        }
                    }
                    break;

                    case SDL_KEYDOWN:
                        switch (event.key.keysym.sym)
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
                                keyboard_data.set(ascii, keyboard_row_down,    0xFF);
                                keyboard_data.set(ascii, keyboard_row_pressed, 0xFF);
                                keyboard_data.set(ascii, keyboard_row_toggle,  keyboard_data.get(ascii, 2) ? 0 : 0xFF);
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

                        render_targets = shadertoy_render_buffers(w, h);
                        target_texture = create_matching_sdl_texture(renderer.get(), w, h);
                        resolution     = { w, h };
                        pending_resize = false;

                        current_frame_timestamp = time;
                        render_task = render_async();
                    }
                }
                else
                {
                    if (blit_now || frameReady)
                    {
                        SDL_Rect rect;
                        rect.x = 0;
                        rect.y = 0;
                        rect.w = resolution.x;
                        rect.h = resolution.y;
                        
                        SDL_UpdateTexture(target_texture.get(), &rect, render_targets.target.first_row, render_targets.target.pitch);
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
                printf(VT100_CLEARLINE "resolution:      %dx%d\n", render_targets.width, render_targets.height);
                printf(VT100_CLEARLINE "fps:             %lg\n", current_fps);
            }

            printf("\nwaiting for the worker thread to finish...");
            render_task.wait();
        }
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

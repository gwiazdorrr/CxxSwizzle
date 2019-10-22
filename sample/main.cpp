// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#define CXXSWIZZLE_inout_wrapper_ENABLED
#if defined(SAMPLE_USE_SIMD_VC)
#include "use_simd.h"
#else 
#include "use_scalar.h"
#endif

#include <swizzle/vector.hpp>
#include <swizzle/matrix.hpp>
#include <swizzle/sampler2D.hpp>
#include <swizzle/detail/simd_mask.h>
#include <swizzle/inout_wrapper.hpp>
#include <conio.h>
#include <swizzle/detail/batch_write_mask.hpp>

using vec2  = swizzle::vector< swizzle::float_type, 2>;
using vec3  = swizzle::vector< swizzle::float_type, 3>;
using vec4  = swizzle::vector< swizzle::float_type, 4>;
                               
using ivec2 = swizzle::vector< swizzle::int_type, 2>;
using ivec3 = swizzle::vector< swizzle::int_type, 3>;
using ivec4 = swizzle::vector< swizzle::int_type, 4>;
                               
using uvec2 = swizzle::vector< swizzle::uint_type, 2>;
using uvec3 = swizzle::vector< swizzle::uint_type, 3>;
using uvec4 = swizzle::vector< swizzle::uint_type, 4>;
                               
using bvec2 = swizzle::vector< swizzle::bool_type, 2>;
using bvec3 = swizzle::vector< swizzle::bool_type, 3>;
using bvec4 = swizzle::vector< swizzle::bool_type, 4>;

using mat2x2 = swizzle::matrix<swizzle::float_type, 2, 2>;
using mat3x3 = swizzle::matrix<swizzle::float_type, 3, 3>;
using mat4x4 = swizzle::matrix<swizzle::float_type, 4, 4>;

using mat2 = mat2x2;
using mat3 = mat3x3;
using mat4 = mat4x4;

using sampler2D = swizzle::naive_sampler2D<batch_float_t, batch_int_t, batch_uint_t>;

static_assert(sizeof(vec2) == sizeof(batch_float_t[2]), "Too big");
static_assert(sizeof(vec3) == sizeof(batch_float_t[3]), "Too big");
static_assert(sizeof(vec4) == sizeof(batch_float_t[4]), "Too big");

//typedef swizzle::glsl::matrix< swizzle::vector, vec4::scalar_type, 2, 2> mat2;
//typedef swizzle::glsl::matrix< swizzle::vector, vec4::scalar_type, 3, 3> mat3;
//typedef swizzle::glsl::matrix< swizzle::vector, vec4::scalar_type, 4, 4> mat4;


//namespace Vc
//{
//    Vector<float> radiands(const Vector<float>& value)
//    {}
//
//}


// todo: YOU CAN' do float cos = cos(rad);

// ! A really, really simplistic sampler using SDLImage
//struct SDL_Surface;
//class naive_sampler2D : public swizzle::glsl::texture_functions::tag
//{
//public:
//
//
//    typedef const vec2::scalar_type& float_type;
//    typedef const vec2& tex_coord_type;
//    typedef const vec3& cube_coord_type;
//    typedef const ivec2& tex_fetch_coord_type;
//
//    naive_sampler2D(vec4 checkers0, vec4 checkers1);
//    
//    naive_sampler2D(const char* path, WrapMode wrapMode);
//    ~naive_sampler2D();
//
//    vec4 texelFetch(const ivec2& coord, int lod) const
//    {
//        return fetch(coord);
//    }
//    vec4 sample(const vec3& coord) const
//    {
//        return sample(coord.xy);
//    }
//    vec4 sample(const vec2& coord) const;
//    vec4 sample(const vec2& coord, float_type bias) const
//    {
//        return sample(coord);
//    }
//
//    vec4 sampleLod(const vec2& coord, float_type lod) const
//    {
//        return sample(coord);
//    }
//
//    vec4 fetch(const ivec2& coord) const;
//
//
//
//    //void setData(std::vector<uint8_t>, int width, int pitch)
//    //{
//    //}
//
//private:
//    SDL_Surface *m_image;
//    WrapMode m_wrapMode;
//    vec4 checkers0 = vec4(1.0f, 0.0f, 0.0f, 1.0f);
//    vec4 checkers1 = vec4(1.0f, 0.0f, 1.0f, 1.0f);
//
//    // do not allow copies to be made
//    naive_sampler2D(const naive_sampler2D&);
//    naive_sampler2D& operator=(const naive_sampler2D&);
//};

// this where the magic happens...


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

#ifdef SDLIMAGE_FOUND
#include <SDL_image.h>
#endif

#include <time.h>
#include <memory>
#include <functional>
#if OMP_ENABLED
#include <omp.h>
#endif



//! A handy way of creating (and checking) unique_ptrs of SDL objects
template <class T>
std::unique_ptr< T, std::function<void (T*)> > make_unique_with_deleter(T* value, std::function<void (T*)> deleter)
{
    if (!value)
    {
        throw std::runtime_error("Null pointer");
    }
    return std::unique_ptr<T, decltype(deleter)>(value, deleter);
}

//! As above, but allows null initialisation
template <class T>
std::unique_ptr< T, std::function<void (T*)> > make_unique_with_deleter(std::function<void (T*)> deleter)
{
    return std::unique_ptr<T, decltype(deleter)>(nullptr, deleter);
}

//void debug_print(batch_float_t vec)
//{
//    using namespace swizzle;
//    using namespace swizzle::detail;
//
//    batch_traits<batch_float_t>::aligned_storage_type storage;
//    float* p = reinterpret_cast<float*>(&storage);
//
//    store_aligned(vec, p);
//
//    for (int i = 0; i < batch_traits<vec4::scalar_type>::size; ++i)
//    {
//        std::cout << p[i] << " ";
//    }
//    std::cout << "\n";
//}

struct fragment_shader_uniforms
{
    batch_float_t iTime;
    batch_int_t iFrame;
    vec2 iResolution;
    vec4 iMouse;
};

struct render_stats
{
    std::chrono::microseconds duration;
    size_t num_pixels;
    int num_threads;
};

vec4 shade(const fragment_shader_uniforms& uniforms, vec2 fragCoord);

static render_stats render(fragment_shader_uniforms uniforms, SDL_Surface* bmp, SDL_Rect viewport, const std::atomic_bool& cancelled)
{
    swizzle::vector<int, 2> p_min(std::max(0, std::min(bmp->w, viewport.x)), std::max(0, std::min(bmp->h, viewport.y)));
    auto p_max = p_min;
    p_max.x = std::min(bmp->w, p_max.x + viewport.w);
    p_max.y = std::min(bmp->h, p_max.y + viewport.h);

    float f = float(0x1000);
    using ::swizzle::detail::static_for;

    constexpr auto pixels_per_batch = float_traits::size;
    static_assert(pixels_per_batch == 1 || pixels_per_batch % 2 == 0, "1 or even scalar count");
    constexpr auto columns_per_batch = pixels_per_batch > 1 ? pixels_per_batch / 2 : 1;
    constexpr auto rows_per_batch = pixels_per_batch > 1 ? 2 : 1;

    auto render_begin = std::chrono::steady_clock::now();

	// if there are more than 1 scalars in a vector, work on two rows with half width at the same time
    batch_float_t x_offsets(0);
	batch_float_t y_offsets(0);

    std::atomic<size_t> num_pixels = 0;
    std::atomic<int> num_threads = 0;

    {
        float_traits::aligned_storage_type aligned_storage;
        float* aligned = reinterpret_cast<float*>(&aligned_storage);

		static_for<0, float_traits::size>([&](size_t i) { aligned[i] = static_cast<float>(i % columns_per_batch) + 0.5f; });
        load_aligned(x_offsets, aligned);

		static_for<0, float_traits::size>([&](size_t i) { aligned[i] = static_cast<float>(1 - i / columns_per_batch) + 0.5f; });
        load_aligned(y_offsets, aligned);
    }
  
    auto p_min_aligned = p_min;
    p_min_aligned.x = (p_min.x / columns_per_batch) * columns_per_batch;
    p_min_aligned.y = (p_min.y / rows_per_batch) * rows_per_batch;

#if !defined(_DEBUG) && OMP_ENABLED
#pragma omp parallel 
    {
        // each thread needs to have at least rows_per_batch rows to process; also, we don't want even number of rows
        // if rows_per_batch > 1
        int threads_count = omp_get_num_threads();
        int thread_num = omp_get_thread_num();

        if (thread_num == 0)
        {
            num_threads = threads_count;
        }

        // ceil
        int height_per_thread = ( (p_max.y - p_min.y) + threads_count - 1) / threads_count;
        height_per_thread = std::max(rows_per_batch, height_per_thread);



        int height_start = p_min_aligned.y + thread_num * height_per_thread;
        int height_end = height_start + height_per_thread;

        height_end = std::min(p_max.y, height_end);

#else
    {
        int height_start = p_min_aligned.y;
        int height_end = p_max.y;
        num_threads = 1;
#endif

        //debug_print(uniforms.iTime);

        uint32_traits::aligned_storage_type aligned_blob_r;
        uint32_traits::aligned_storage_type aligned_blob_g;
        uint32_traits::aligned_storage_type aligned_blob_b;

        // check the comment above for explanation
        uint32_t* pr = reinterpret_cast<uint32_t*>(&aligned_blob_r);
        uint32_t* pg = reinterpret_cast<uint32_t*>(&aligned_blob_g);
        uint32_t* pb = reinterpret_cast<uint32_t*>(&aligned_blob_b);

        //assert(rows_per_batch <= height_end - height_start);

        //debug_print(uniforms.iTime);
        for (int y = height_start; !cancelled && y < height_end; y += rows_per_batch)
        {
            batch_float_t frag_coord_y = static_cast<float>(bmp->h - 1 - y) + y_offsets;

            uint8_t * ptr = reinterpret_cast<uint8_t*>(bmp->pixels) + y * bmp->pitch + p_min_aligned.x * 3;

            int x;
            for (x = p_min_aligned.x; x < p_max.x; x += columns_per_batch)
            {
                auto color =  shade(uniforms, vec2(static_cast<float>(x) + x_offsets, frag_coord_y));
                color *= 255.0f + 0.5f;

                store_aligned(static_cast<batch_uint_t>(color.r), pr);
                store_aligned(static_cast<batch_uint_t>(color.g), pg);
                store_aligned(static_cast<batch_uint_t>(color.b), pb);

                if ( x < p_min.x || x + columns_per_batch > p_max.x || y < p_min.y || y + rows_per_batch > p_max.y )
                {
                    // slow case: partially out of the surface
                    int min_cols = std::max(0, p_min.x - x);
                    int min_rows = std::max(0, p_min.y - y);
                    int max_rows = std::min<int>(p_max.y - y, rows_per_batch);
                    int max_cols = std::min<int>(p_max.x - x, columns_per_batch);

                    for (int row = min_rows; row < max_rows; ++row)
                    {
                        auto p = ptr + row * bmp->pitch + min_cols * 3;
                        for (int col = min_cols; col < max_cols; ++col)
                        {
                            *p++ = static_cast<uint8_t>(pr[row * columns_per_batch + col]);
                            *p++ = static_cast<uint8_t>(pg[row * columns_per_batch + col]);
                            *p++ = static_cast<uint8_t>(pb[row * columns_per_batch + col]);
                        }
                    }
                }
                else
                {
                    {
                        auto p = ptr;
                        static_for<0, columns_per_batch>([&](size_t j)
                        {
                            *p++ = static_cast<uint8_t>(pr[j]);
                            *p++ = static_cast<uint8_t>(pg[j]);
                            *p++ = static_cast<uint8_t>(pb[j]);
                        });
                    }
                    // handle second row (if present)
                    {
                        auto p = ptr + bmp->pitch;
                        static_for<columns_per_batch, pixels_per_batch>([&](size_t j)
                        {
                            *p++ = static_cast<uint8_t>(pr[j]);
                            *p++ = static_cast<uint8_t>(pg[j]);
                            *p++ = static_cast<uint8_t>(pb[j]);
                        });
                    }
                }

                ptr += 3 * columns_per_batch;
            }

            num_pixels += x * rows_per_batch;
        }
    }

    return render_stats { std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - render_begin), num_pixels, num_threads };
}




#define STR1(x) #x
#define STR2(x) STR1(x)
#define VT100_CLEARLINE "\33[2K"
#define VT100_UP(x)     "\33[" STR2(x) "A"
#define VT100_DOWN(x)   "\33[" STR2(x) "B"
#define STATS_LINES 12

const double seconds_to_micro = 1000000.0;
const double micro_to_seconds = 1 / seconds_to_micro;


bool load_texture(sampler2D& sampler, const char* name)
{
#ifdef SDLIMAGE_FOUND
    auto img = IMG_Load(name);
    if (!img)
    {
        fprintf(stderr, "ERROR: Failed to load texture %s (error: %s)\n", name, IMG_GetError());
        return false;
    }
    else
    {
        sampler.bytes = reinterpret_cast<uint8_t*>(img->pixels);
        sampler.width = img->w;
        sampler.height = img->h;
        sampler.pitch = img->w;
        sampler.bytes_per_pixel = img->format->BytesPerPixel;

        sampler.rshift = img->format->Rshift;
        sampler.gshift = img->format->Gshift;
        sampler.bshift = img->format->Bshift;
        sampler.ashift = img->format->Ashift;

        if (img->format->format == SDL_PIXELFORMAT_INDEX8)
        {
            sampler.rmask = 0xFF;
            sampler.gmask = 0xFF;
            sampler.bmask = 0xFF;
            sampler.amask = 0;
        }
        else
        {
            sampler.rmask = img->format->Rmask;
            sampler.gmask = img->format->Gmask;
            sampler.bmask = img->format->Bmask;
            sampler.amask = img->format->Amask;
        }

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

    swizzle::vector<int, 2> resolution(512, 512);
    SDL_Rect viewport = { 0, 0, 0, 0 };
    float time = 0.0f;
    float time_scale = 1.0f;
    

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
        else if (!strcmp(arg, "-w"))
        {
            if (i + 4 < argc && from_string(argv[++i], viewport.x) && from_string(argv[++i], viewport.y) && from_string(argv[++i], viewport.w) && from_string(argv[++i], viewport.h))
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
        else if (!strcmp(arg, "-h"))
        {
            print_help();
            return 0;
        }
    }

    if (viewport.w == 0 || viewport.h == 0)
        viewport = { 0, 0, resolution.x, resolution.y };

    if (resolution.x <= 0 || resolution.y < 0 )
    {
        fprintf(stderr, "ERROR: invalid resolution: %dx%d\n", resolution.x, resolution.y);
        return 1;
    }



    printf("p           - pause/unpause\n");
    printf("left arrow  - decrease time by 1 s\n");
    printf("right arrow - increase time by 1 s\n");
    printf("shift+mouse - select viewport\n");
    printf("space       - blit now! (show incomplete render)\n");
    printf("esc         - quit\n");
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
        printf(VT100_DOWN(STATS_LINES));

        auto window = make_unique_with_deleter<SDL_Window>(SDL_CreateWindow("CxxSwizzle sample", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            resolution.x, resolution.y, SDL_WINDOW_RESIZABLE), SDL_DestroyWindow);
        auto renderer = make_unique_with_deleter<SDL_Renderer>(SDL_CreateRenderer(window.get(), -1, 0), SDL_DestroyRenderer);
        auto target_surface = make_unique_with_deleter<SDL_Surface>(SDL_FreeSurface);
        auto target_texture = make_unique_with_deleter<SDL_Texture>(SDL_DestroyTexture);

        // a function used to resize the target_surface
        auto resize_or_create_surface = [&](int w, int h) -> void
        {
            target_surface.reset( SDL_CreateRGBSurface(0, w, h, 24, 0x000000ff, 0x0000ff00, 0x00ff0000, 0 ) );
            if ( !target_surface )
            {
                throw std::runtime_error("Unable to create target_surface");
            }

            target_texture.reset(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, w, h));
            if ( !target_texture )
                throw std::runtime_error("Unable to create target_texture");

            resolution.x = w;
            resolution.y = h;
        };

        resize_or_create_surface(resolution.x, resolution.y);
        
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

        auto renderAsync = [&]() -> std::future<render_stats>
        {
            abort_render_token = false;
            auto s = target_surface.get();

            fragment_shader_uniforms uniforms = {};
            uniforms.iTime = time;
            uniforms.iFrame = num_frames;
            uniforms.iResolution = vec2(static_cast<float>(s->w), static_cast<float>(s->h));
            uniforms.iMouse.x = mouse_x / static_cast<float>(s->w);
            uniforms.iMouse.y = mouse_y / static_cast<float>(s->h);
            uniforms.iMouse.z = mouse_pressed ? 1.0f : 0.0f;
            uniforms.iMouse.w = 0.0f;

            return std::async(render, uniforms, s, viewport, std::ref(abort_render_token));
        };

        std::future<render_stats> render_task = renderAsync();
        
        SDL_Rect mouse_rect = {};

        bool is_shift_pressed = false;
        bool is_shift_selecting = false;

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
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_LSHIFT:
                        is_shift_pressed = false;
                        break;
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
                    case SDLK_LSHIFT:
                        is_shift_pressed = true;
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
                    else if (is_shift_selecting)
                    {
                        mouse_rect.w = event.button.x - mouse_rect.x;
                        mouse_rect.h = event.button.y - mouse_rect.y;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (is_shift_pressed)
                    {
                        is_shift_selecting = true;
                        mouse_rect.x = event.button.x;
                        mouse_rect.y = event.button.y;
                        mouse_rect.w = mouse_rect.h = 0;
                    }
                    else
                    {
                        mouse_pressed = true;
                        mouse_x = event.button.x;
                        mouse_y = event.button.y;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (is_shift_selecting)
                    {
                        is_shift_selecting = false;
                        if (mouse_rect.h == 0 || mouse_rect.w == 0)
                        {
                            viewport = { 0, 0, resolution.x, resolution.y };
                        }
                        else
                        {
                            viewport = fix_rect(mouse_rect);
                        }
                        
                        abort_render_token = true;
                        mouse_rect = {};
                    }

                    mouse_pressed = false;
                    is_shift_selecting = false;
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

                    resize_or_create_surface(w, h);
                    pending_resize = false;

                    current_frame_timestamp = time;
                    render_task = renderAsync();
                }
            }
            else
            {
                if (blit_now || frameReady || is_shift_selecting)
                {
                    SDL_UpdateTexture(target_texture.get(), NULL, target_surface->pixels, target_surface->pitch);
                    SDL_RenderClear(renderer.get());
                    SDL_RenderCopy(renderer.get(), target_texture.get(), NULL, NULL);

                    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 255, 128);
                    {
                        auto r = viewport;
                        r.x -= 1;
                        r.y -= 1;
                        r.w += 2;
                        r.h += 2;
                        SDL_RenderDrawRect(renderer.get(), &r);
                    }

                    if (mouse_rect.w != 0 && mouse_rect.h != 0)
                    {
                        auto fixed = fix_rect(mouse_rect);
                        SDL_SetRenderDrawColor(renderer.get(), 0, 255, 255, 128);
                        SDL_RenderDrawRect(renderer.get(), &fixed);
                        SDL_RenderPresent(renderer.get());
                    }

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
                    render_task = renderAsync();
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
            printf(VT100_CLEARLINE "viewport:        %d %d %d %d\n", viewport.x, viewport.y, viewport.w, viewport.h);
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

//
//naive_sampler2D::naive_sampler2D( const char* path, WrapMode wrapMode ) 
//    : m_wrapMode(wrapMode)
//    , m_image(nullptr)
//{
//#ifdef SDLIMAGE_FOUND
//    m_image = IMG_Load(path);
//    if (!m_image)
//    {
//        std::cerr << "WARNING: Failed to load target_texture " << path << "\n";
//        std::cerr << "  SDL_Image message: " << IMG_GetError() << "\n";
//    }
//#else
//    std::cerr << "WARNING: target_texture won't be loaded, SDL_image was not found.\n";
//#endif
//
//}
//
//naive_sampler2D::~naive_sampler2D()
//{
//    if ( m_image )
//    {
//        SDL_FreeSurface(m_image);
//        m_image = nullptr;
//    }
//}
//
//
//vec4 naive_sampler2D::fetch(const ivec2& coord) const
//{
//  //  vec2 mask = vec2::call_step(32.0f, vec2(coord));
//
//    //vec2 mask = vec2(ivec2::call_lessThan(coord, ivec2(32, 32)));
//    //float f = 0;
////    vec2::scalar_type f = swizzle::vector<float, 1>::call_abs(mask.x - mask.y);
//    //return vec4::call_mix(checkers0, checkers1, f);
//    return vec4(0);
//}
//
////namespace Vc
////{
////    template <typename T>
////    inline Vector<T> acos(const Vector<T>& x)
////    {
////        // silly acos that does store & load... I'm sorry
////        std::aligned_storage_t<x.Size * sizeof(T), x.MemoryAlignment> storage;
////        T* ptr = reinterpret_cast<T*>(&storage);
////        x.store(ptr, Aligned);
////        ::swizzle::detail::static_for<0, Vector<T>::Size>([&](size_t i) { ptr[i] = ::std::acos(ptr[i]); });
////        x.load(ptr, Aligned);
////        return atan2(y, x);
////    }
////}
//vec4 naive_sampler2D::sample( const vec2& coord ) const
//{
//    return vec4(0);
//    //
//    //using namespace glsl_sandbox;
//    //vec2 uv;
//    //switch (m_wrapMode)
//    //{
//    //case Repeat:
//    //    uv = mod(coord, 1.0f);
//    //    break;
//    //case MirrorRepeat:
//    //    uv = abs(mod(coord - 1.0f, 2.0f) - 1.0f);
//    //    break;
//    //case Clamp:
//    //default:
//    //    uv = clamp(coord, 0.0f, 1.0f);
//    //    break;
//    //}
//
//    //// OGL uses left-bottom corner as origin...
//    //uv.y = 1.0 - uv.y;
//    ////if ( !m_image )
//    ////{
//    //    // checkers
//    //    auto s = step(0.5f, uv);
//    //    auto m2 = abs(s.x - s.y);
//    //    return mix(checkers0, checkers1, m2);
//    //    /*if (uv_x < 0.5 && uv_y < 0.5 || uv_x > 0.5 && uv_y > 0.5)
//    //    {
//    //        return vec4(1, 0, 0, 1);
//    //    }
//    //    else
//    //    {
//    //        return vec4(0, 1, 0, 1);
//    //    }*/
//    ////}
//    ////else
//    ////{
//    ////    raw_batch_uint32_t x = batch_cast<raw_batch_uint32_t>(static_cast<raw_batch_float_t>(uv.x * (m_image->w - 1.0) + 0.5));
//    ////    raw_batch_uint32_t y = batch_cast<raw_batch_uint32_t>(static_cast<raw_batch_float_t>(uv.y * (m_image->h - 1.0) + 0.5));
//
//    ////    auto& format = *m_image->format;
//    ////    raw_batch_uint32_t index = (y * m_image->pitch + x * format.BytesPerPixel);
//
//    ////    // stack-alloc blob for storing indices and color components
//    ////    uint8_t unaligned_blob[5 * (pixels_per_batch * sizeof(unsigned) + batch_uint32_align)];
//    ////    unsigned* pindex = align_ptr<batch_uint32_align>(reinterpret_cast<unsigned*>(unaligned_blob));
//    ////    unsigned* pr = align_ptr<batch_uint32_align>(pindex + pixels_per_batch);
//    ////    unsigned* pg = align_ptr<batch_uint32_align>(pr + pixels_per_batch);
//    ////    unsigned* pb = align_ptr<batch_uint32_align>(pg + pixels_per_batch);
//    ////    unsigned* pa = align_ptr<batch_uint32_align>(pb + pixels_per_batch);
//
//    ////    batch_store_aligned(index, pindex);
//    ////    
//    ////    // fill the buffers
//    ////    swizzle::detail::static_for<0, pixels_per_batch>([&](size_t i)
//    ////    {
//    ////        auto pixelPtr = static_cast<uint8_t*>(m_image->pixels) + pindex[i];
//
//    ////        uint32_t pixel = 0;
//    ////        for (size_t i = 0; i < format.BytesPerPixel; ++i)
//    ////        {
//    ////            pixel |= (pixelPtr[i] << (i * 8));
//    ////        }
//
//    ////        pr[i] = (pixel & format.Rmask) >> format.Rshift;
//    ////        pg[i] = (pixel & format.Gmask) >> format.Gshift;
//    ////        pb[i] = (pixel & format.Bmask) >> format.Bshift;
//    ////        pa[i] = format.Amask ? ((pixel & format.Amask) >> format.Ashift) : 255;
//    ////    });
//
//    ////    // load data
//    ////    raw_batch_uint32_t r, g, b, a;
//    ////    batch_load_aligned(r, pr);
//    ////    batch_load_aligned(g, pg);
//    ////    batch_load_aligned(b, pb);
//    ////    batch_load_aligned(a, pa);
//
//    ////    vec4 result;
//    ////    result.r = batch_cast<raw_batch_float_t>(r);
//    ////    result.g = batch_cast<raw_batch_float_t>(g);
//    ////    result.b = batch_cast<raw_batch_float_t>(b);
//    ////    result.a = batch_cast<raw_batch_float_t>(a);
//
//    ////    return clamp(result / 255.0f, c_zero, c_one);
//    ////}
//}

#include "glsl_sandbox.hpp"

// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

#if defined(SAMPLE_USE_SIMD_VC)
#include "use_simd.h"
#else 
#include "use_scalar.h"
#endif

#include <swizzle/vector.hpp>
#include <swizzle/matrix.hpp>
#include <swizzle/glsl/texture_functions.h>
#include <swizzle/detail/simd_mask.h>

typedef swizzle::vector< batch_float_t, 2 > vec2;
typedef swizzle::vector< batch_float_t, 3 > vec3;
typedef swizzle::vector< batch_float_t, 4 > vec4;

typedef swizzle::vector< batch_int_t, 2 > ivec2;
typedef swizzle::vector< batch_int_t, 3 > ivec3;
typedef swizzle::vector< batch_int_t, 4 > ivec4;

typedef swizzle::vector< batch_uint_t, 2 > uvec2;
typedef swizzle::vector< batch_uint_t, 3 > uvec3;
typedef swizzle::vector< batch_uint_t, 4 > uvec4;

typedef swizzle::vector< batch_bool_t, 2 > bvec2;
typedef swizzle::vector< batch_bool_t, 3 > bvec3;
typedef swizzle::vector< batch_bool_t, 4 > bvec4;

typedef swizzle::matrix< swizzle::vector, batch_float_t, 2, 2 > mat2;
typedef swizzle::matrix< swizzle::vector, batch_float_t, 3, 3 > mat3;
typedef swizzle::matrix< swizzle::vector, batch_float_t, 4, 4 > mat4;

using mat2x2 = mat2;
using mat3x3 = mat3;
using mat4x4 = mat4;

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

//template <typename ValueType, ValueType Value, ValueType... Values>
//std::integer_sequence<ValueType, Values..., Value> append_integer_sequence_resolver(std::integer_sequence<ValueType, Values...>)
//{
//    return {};
//}
//
//template <typename SequenceType, typename ValueType, ValueType Value>
//using append_integer_sequence = decltype(append_integer_sequence_resolver<ValueType, Value>(std::declval<SequenceType>()));
//
//template <typename SequenceType, size_t Value>
//using append_index_sequence = append_integer_sequence<SequenceType, size_t, Value>;
//
//
//
//
//
//
//typedef std::make_index_sequence<10> ta;
//typedef append_index_sequence<ta, 66> tb;
//typedef take_n<5, 1, 2, 3, 4, 5, 6, 7> tc;
//static_assert(tb::size() == 11, "aaa");
//static_assert(tb::size() == 11, "aaa");
//static_assert(tc::size() == 5, "aaa");

// ! A really, really simplistic sampler using SDLImage
struct SDL_Surface;
class naive_sampler2D : public swizzle::glsl::texture_functions::tag
{
public:
    enum WrapMode
    {
        Clamp,
        Repeat,
        MirrorRepeat
    };

    typedef const vec2::scalar_type& float_type;
    typedef const vec2& tex_coord_type;
    typedef const vec3& cube_coord_type;
    typedef const ivec2& tex_fetch_coord_type;

    naive_sampler2D(vec4 checkers0, vec4 checkers1);
    
    naive_sampler2D(const char* path, WrapMode wrapMode);
    ~naive_sampler2D();

    vec4 texelFetch(const ivec2& coord, int lod) const
    {
        return fetch(coord);
    }
    vec4 sample(const vec3& coord) const
    {
        return sample(coord.xy);
    }
    vec4 sample(const vec2& coord) const;
    vec4 sample(const vec2& coord, float_type bias) const
    {
        return sample(coord);
    }

    vec4 sampleLod(const vec2& coord, float_type lod) const
    {
        return sample(coord);
    }

    vec4 fetch(const ivec2& coord) const;



    void setData(std::vector<uint8_t>, int width, int pitch)
    {
    }

private:
    SDL_Surface *m_image;
    WrapMode m_wrapMode;
    vec4 checkers0 = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    vec4 checkers1 = vec4(1.0f, 0.0f, 1.0f, 1.0f);

    // do not allow copies to be made
    naive_sampler2D(const naive_sampler2D&);
    naive_sampler2D& operator=(const naive_sampler2D&);
};

// this where the magic happens...
namespace glsl_sandbox
{
    #include <swizzle/glsl/vector_functions.h>



    naive_sampler2D iChannel0(nullptr, naive_sampler2D::Repeat);
    naive_sampler2D iChannel1(nullptr, naive_sampler2D::Repeat);
    naive_sampler2D iChannel2(nullptr, naive_sampler2D::Repeat);
    naive_sampler2D iChannel3(nullptr, naive_sampler2D::Repeat);
    vec3 iChannelResolution[4] = {
        vec3(128, 128, 1),
        vec3(128, 128, 1),
        vec3(128, 128, 1),
        vec3(128, 128, 1)
    };

    // change meaning of glsl keywords to match sandbox
    #define uniform extern
    #define in
    #define out ref_proxy::
    #define inout ref_proxy::
    #define main operator()
    #define mainImage operator()
    #define float batch_float_t   
    #define int batch_int_t
    #define uint batch_uint_t
    #define bool batch_bool_t
    #define char definitely_not_a_char
#define lowp

#ifdef SIMD_IF
    #define if(x) SIMD_IF(x)
#endif
#ifdef SIMD_ELSE
    #define else SIMD_ELSE
#endif
#ifdef SIMD_WHILE
    #define while(x) SIMD_WHILE(x)
#endif
#ifdef SIMD_CONDITION
    #define condition(x) SIMD_CONDITION(x)
#endif
    
    #pragma warning(push)
    #pragma warning(disable: 4244) // disable return implicit conversion warning
    #pragma warning(disable: 4305) // disable truncation warning
    
    struct fragment_shader_uniforms
    {
        vec2 iResolution;
        batch_float_t iTime;
        vec3 iMouse;
        int iFrame;
    };

    struct fragment_shader : fragment_shader_uniforms
    {
        struct Ray;
        struct ray;

        void aaa()
        {
            int aaaa = int(6666);
            float sss = 0.0f;
            // light visibility across the volume
            float ssha = 0.0f;

            // mix reflecting and refracting contributions
            float dif = 0.0f;

            ::swizzle::detail::get_vector_type<batch_float_t, batch_float_t, double>::type::call_mix(sss, ssha, 0.2);

            auto tmp = mix(sss, ssha, 0.2);
            dif = mix(dif, tmp, 0.5);

            
            /*aaaa.call_mix(sss, ssha, 0.2);
            auto a = mix(sss, ssha, 0.2);
            dif = mix(dif, a, 0.5);*/


            //vec2 a;
            //mix(a, a, 0.0);
//            mix(0.0, 0.0, 0.0);

            //::swizzle::detail::get_vector_type<double, double, double>::type::call_mix(0.0, 0.0, 0.0);
            //mix(a, a, a);
        }

        using sampler2D = const naive_sampler2D&;

        template <template <typename> class mod>
        struct modifier
        {
            using vec2 = typename mod<vec2>::type;
            typedef vec3& vec3;
            typedef vec4& vec4;
        };

        // a nested namespace used when redefining 'inout' and 'out' keywords
        struct ref_proxy
        {
#ifdef CXXSWIZZLE_VECTOR_INOUT_WRAPPER_ENABLED
            typedef swizzle::detail::vector_inout_wrapper<vec2> vec2;
            typedef swizzle::detail::vector_inout_wrapper<vec3> vec3;
            typedef swizzle::detail::vector_inout_wrapper<vec4> vec4;
#else
            typedef vec2& vec2;
            typedef vec3& vec3;
            typedef vec4& vec4;
#endif
            typedef ::batch_float_t& batch_float_t;
            typedef int& int;

            typedef Ray& Ray;
            typedef ray& ray;
        };

        struct in_proxy
        {
            typedef const ::vec2& vec2;
            typedef const ::vec3& vec3;
            typedef const ::vec4& vec4;
            typedef const ::batch_float_t& batch_float_t;
            typedef int& int;
            typedef const Ray& Ray;
            typedef const ray& ray;
        };

        vec2 gl_FragCoord;
        vec4 gl_FragColor;

        //#include "shaders/sampler.frag"
        //#include "shaders/leadlight.frag"
        //#include "shaders/terrain.frag"
        //#include "shaders/complex.frag"
        //#include "shaders/road.frag"
        //#include "shaders/gears.frag"
        //#include "shaders/water_turbulence.frag"
//#include "shaders/elevated_coast.frag"

        #include "shader.frag"

#undef time
    };

    // be a dear a clean up
    #pragma warning(pop)
    #undef bool
    #undef float
    #undef int
    #undef uint
    #undef main
    #undef in
    #undef out
    #undef inout
    #undef uniform
    #undef char

#ifdef if
    #undef if
#endif
#ifdef else
    #undef else
#endif
#ifdef while
    #undef while
#endif
#ifdef condition
    #undef condition
#endif
}

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

const batch_float_t c_one = 1.0f;
const batch_float_t c_zero = 0.0f;

template <size_t Align, typename T>
T* align_ptr(T* ptr)
{
    static_assert((Align & (Align - 1)) == 0, "Align needs to be a power of two");
    auto value = reinterpret_cast<ptrdiff_t>(ptr);
    return reinterpret_cast<T*>((value + Align) & (~(Align - 1)));
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


static void render(glsl_sandbox::fragment_shader_uniforms uniforms, SDL_Surface* bmp, const std::atomic_bool& cancelled)
{
    //debug_print(uniforms.iTime);

    float f = float(0x1000);
    using ::swizzle::detail::static_for;


    //debug_print(uniforms.iTime);

	// if there are more than 1 scalars in a vector, work on two rows with half width at the same time
    batch_float_t x_offsets(0);
	batch_float_t y_offsets(0);

    {
        float_traits::aligned_storage_type aligned_storage;
        float* aligned = reinterpret_cast<float*>(&aligned_storage);

		static_for<0, float_traits::size>([&](size_t i) { aligned[i] = static_cast<float>(i % columns_per_batch); });
        load_aligned(x_offsets, aligned);

		static_for<0, float_traits::size>([&](size_t i) { aligned[i] = static_cast<float>(1 - i / columns_per_batch); });
        load_aligned(y_offsets, aligned);
    }

#if !defined(_DEBUG) && OMP_ENABLED
#pragma omp parallel 
    {
        // each thread needs to have at least rows_per_batch rows to process; also, we don't want even number of rows
        // if rows_per_batch > 1
        int threads_count = omp_get_num_threads();
        int thread_num = omp_get_thread_num();

        // ceil
        int height_per_thread = (bmp->h + threads_count - 1) / threads_count;
        height_per_thread = std::max(rows_per_batch, height_per_thread);
        int height_per_threadRem = height_per_thread % rows_per_batch;

        int height_start = thread_num * height_per_thread;
        int height_end = height_start + height_per_thread;

        height_end = std::min(bmp->h, height_end);

#else
    {
        int height_start = 0;
        int height_end = bmp->h;
#endif

        //debug_print(uniforms.iTime);

        uint32_traits::aligned_storage_type aligned_blob_r;
        uint32_traits::aligned_storage_type aligned_blob_g;
        uint32_traits::aligned_storage_type aligned_blob_b;

        // check the comment above for explanation
        uint32_t* pr = reinterpret_cast<uint32_t*>(&aligned_blob_r);
        uint32_t* pg = reinterpret_cast<uint32_t*>(&aligned_blob_g);
        uint32_t* pb = reinterpret_cast<uint32_t*>(&aligned_blob_b);

        assert(rows_per_batch <= height_end - height_start);

        //debug_print(uniforms.iTime);

        for (int y = height_start; !cancelled && y < height_end; y += rows_per_batch)
        {
            batch_float_t fy = static_cast<float>(bmp->h - 1 - y) + y_offsets;

            uint8_t * ptr = reinterpret_cast<uint8_t*>(bmp->pixels) + y * bmp->pitch;
            
            for (int x = 0; x < bmp->w; x += columns_per_batch)
            {
                glsl_sandbox::fragment_shader shader;
                static_cast<glsl_sandbox::fragment_shader_uniforms&>(shader) = uniforms;

                shader.gl_FragCoord.y = fy;
                shader.gl_FragCoord.x = static_cast<float>(x) + x_offsets;

                // vvvvvvvvvvvvvvvvvvvvvvvvvv
                // THE SHADER IS INVOKED HERE
                
               
                // ^^^^^^^^^^^^^^^^^^^^^^^^^^
                shader(shader.gl_FragColor, shader.gl_FragCoord);
                

                // convert to [0;255]
                auto color = glsl_sandbox::clamp(shader.gl_FragColor, c_zero, c_one);
                color *= 255.0f + 0.5f;
                //debug_print(color.x);
                //debug_print(color.y);

                //vec4 color(255.0f, 0.0f, 0.0f, 1.0f);
                /*::Vc::float_v a;
                a.data()*/


                store_aligned(static_cast<batch_uint_t>(color.r), pr);
                store_aligned(static_cast<batch_uint_t>(color.g), pg);
                store_aligned(static_cast<batch_uint_t>(color.b), pb);

                if (x > bmp->w - columns_per_batch || y > bmp->h - rows_per_batch)
                {
                    // slow case: parially out of the surface
                    int max_rows = std::min<int>(bmp->h - y, rows_per_batch);
                    int max_cols = std::min<int>(bmp->w - x, columns_per_batch);

                    for (int row = 0; row < max_rows; ++row)
                    {
                        auto p = ptr + row * bmp->pitch;
                        for (int col = 0; col < max_cols; ++col)
                        {
                            *p++ = static_cast<uint8_t>(pr[col]);
                            *p++ = static_cast<uint8_t>(pg[col]);
                            *p++ = static_cast<uint8_t>(pb[col]);
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
        }
    }
  }

static std::chrono::microseconds render_timed(glsl_sandbox::fragment_shader_uniforms& uniforms, SDL_Surface* bmp, const std::atomic_bool& cancelled)
{
    //debug_print(uniforms.iTime);
    //printf("\n\n%hd\n\n", uniforms.wtfffff);
    auto begin = std::chrono::steady_clock::now();
    render(uniforms, bmp, cancelled);
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
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
        cerr << "WARNING: failed to initialize required jpg and png support: " << IMG_GetError() << endl;
    }
#endif

    // get initial resolution
    swizzle::vector<int, 2> initial_resolution;
    initial_resolution.x = 512;
    initial_resolution.y = 512;
    if (argc == 2)
    {
        std::stringstream s;
        s << argv[1];
        if ( !(s >> initial_resolution) )
        {
            cerr << "ERROR: unable to parse resolution argument" << endl;
            return 1;
        }
    }

    if ( initial_resolution.x <= 0 || initial_resolution.y < 0 )
    {
        cerr << "ERROR: invalid resolution: " << initial_resolution  << endl;
        return 1;
    }

    cout << "\n";
    cout << "+/-   - increase/decrease time scale\n";
    cout << "lmb   - update glsl_sandbox::mouse\n";
    cout << "space - blit now! (show incomplete render)\n";
    cout << "esc   - quit\n\n";

    // initial setup
	SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cerr << "Unable to init SDL" << endl;
        return 1;
    }

    try 
    {
        auto window = make_unique_with_deleter<SDL_Window>(SDL_CreateWindow("CxxSwizzle sample", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            initial_resolution.x, initial_resolution.y, SDL_WINDOW_RESIZABLE), SDL_DestroyWindow);
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
        };

        resize_or_create_surface(initial_resolution.x, initial_resolution.y);
        
        double time_scale = 1.0;
        int frame = 0;
        float time = 0.0f;
        vec2 mouse_position(0.0f, 0.0f);
        bool pending_resize = false;
        bool mouse_pressed = false;

        std::atomic_bool abort_render_token = false;
        glsl_sandbox::fragment_shader_uniforms uniforms;

        auto renderAsync = [&]() -> std::future<std::chrono::microseconds>
        {
            uniforms = {};

            abort_render_token = false;
            auto s = target_surface.get();
            uniforms.iTime = time;
            
            uniforms.iFrame = frame;
            uniforms.iResolution = vec2(static_cast<batch_float_t>(s->w), static_cast<batch_float_t>(s->h));
            uniforms.iMouse.xy = mouse_position / uniforms.iResolution;
            uniforms.wtfffff = 7777;
            glsl_sandbox::fragment_shader_uniforms copppy = uniforms;

            //debug_print(uniforms.iTime);
            //debug_print(copppy.iTime);


            return std::async(render_timed, std::ref(uniforms), s, std::ref(abort_render_token));
        };

        std::future<std::chrono::microseconds> render_task = renderAsync();
        std::chrono::microseconds last_render_duration;
        auto begin = std::chrono::steady_clock::now();
        auto micro_to_seconds = 1 / 1000000.0;

        for (;;)
        {
            bool blitNow = false;
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
                        blitNow = true;
                        break;
                    case SDLK_ESCAPE:
                        {
                            abort_render_token = quit = true;
                        }
                        break;
                    case SDLK_PLUS:
                    case SDLK_EQUALS:
                        time_scale *= 2.0;
                        break;
                    case SDLK_MINUS:
                        time_scale /= 2.0;
                        break;
                    default:
                        break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (mouse_pressed)
                    {
                        mouse_position.x = static_cast<float>(event.button.x);
                        mouse_position.y = static_cast<float>(target_surface->h - 1 - event.button.y);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mouse_pressed = true;
                    mouse_position.x = static_cast<float>(event.button.x);
                    mouse_position.y = static_cast<float>(target_surface->h - 1 - event.button.y);
                    break;
                case SDL_MOUSEBUTTONUP:
                    mouse_pressed = false;
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

                    render_task = renderAsync();
                }
            }
            else
            {
                if (blitNow || frameReady)
                {
                    SDL_UpdateTexture(target_texture.get(), NULL, target_surface->pixels, target_surface->pitch);
                    SDL_RenderClear(renderer.get());
                    SDL_RenderCopy(renderer.get(), target_texture.get(), NULL, NULL);
                    SDL_RenderPresent(renderer.get());
                }

                if (frameReady)
                {
                    ++frame;
                    last_render_duration = render_task.get();
                    render_task = renderAsync();
                }
            }
            

            // clear line
            cout << "\r" << (char)27 << "[2K";
            cout << "frame: " << frame << "\t time: " << time << "\t time_scale: " << time_scale << "\t time: " << last_render_duration.count() * micro_to_seconds;
            cout.flush();

            auto delta = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - begin);
            time += static_cast<float>(delta.count() * micro_to_seconds * time_scale);
            begin = chrono::steady_clock::now();
        }

        // wait for the render thread to stop
        cout << "\nwaiting for the worker thread to finish...";
        render_task.wait();
    } 
    catch ( exception& error ) 
    {
        cerr << "ERROR: " << error.what() << endl;
    } 
    catch (...) 
    {
        cerr << "ERROR: Unknown error" << endl;
    }

    SDL_Quit();
    return 0; 
}


naive_sampler2D::naive_sampler2D( const char* path, WrapMode wrapMode ) 
    : m_wrapMode(wrapMode)
    , m_image(nullptr)
{
#ifdef SDLIMAGE_FOUND
    m_image = IMG_Load(path);
    if (!m_image)
    {
        std::cerr << "WARNING: Failed to load target_texture " << path << "\n";
        std::cerr << "  SDL_Image message: " << IMG_GetError() << "\n";
    }
#else
    std::cerr << "WARNING: target_texture won't be loaded, SDL_image was not found.\n";
#endif

}

naive_sampler2D::~naive_sampler2D()
{
    if ( m_image )
    {
        SDL_FreeSurface(m_image);
        m_image = nullptr;
    }
}


vec4 naive_sampler2D::fetch(const ivec2& coord) const
{
  //  vec2 mask = vec2::call_step(32.0f, vec2(coord));

    //vec2 mask = vec2(ivec2::call_lessThan(coord, ivec2(32, 32)));
    //float f = 0;
//    vec2::scalar_type f = swizzle::vector<float, 1>::call_abs(mask.x - mask.y);
    //return vec4::call_mix(checkers0, checkers1, f);
    return vec4(0);
}

//namespace Vc
//{
//    template <typename T>
//    inline Vector<T> acos(const Vector<T>& x)
//    {
//        // silly acos that does store & load... I'm sorry
//        std::aligned_storage_t<x.Size * sizeof(T), x.MemoryAlignment> storage;
//        T* ptr = reinterpret_cast<T*>(&storage);
//        x.store(ptr, Aligned);
//        ::swizzle::detail::static_for<0, Vector<T>::Size>([&](size_t i) { ptr[i] = ::std::acos(ptr[i]); });
//        x.load(ptr, Aligned);
//        return atan2(y, x);
//    }
//}

vec4 naive_sampler2D::sample( const vec2& coord ) const
{
    using namespace glsl_sandbox;
    vec2 uv;
    switch (m_wrapMode)
    {
    case Repeat:
        uv = mod(coord, 1.0f);
        break;
    case MirrorRepeat:
        uv = abs(mod(coord - 1.0f, 2.0f) - 1.0f);
        break;
    case Clamp:
    default:
        uv = clamp(coord, 0.0f, 1.0f);
        break;
    }

    // OGL uses left-bottom corner as origin...
    uv.y = 1.0 - uv.y;
    
    //if ( !m_image )
    //{
        // checkers
        auto s = step(0.5f, uv);
        auto m2 = abs(s.x - s.y);
        return mix(checkers0, checkers1, m2);
        /*if (uv_x < 0.5 && uv_y < 0.5 || uv_x > 0.5 && uv_y > 0.5)
        {
            return vec4(1, 0, 0, 1);
        }
        else
        {
            return vec4(0, 1, 0, 1);
        }*/
    //}
    //else
    //{
    //    raw_batch_uint32_t x = batch_cast<raw_batch_uint32_t>(static_cast<raw_batch_float_t>(uv.x * (m_image->w - 1.0) + 0.5));
    //    raw_batch_uint32_t y = batch_cast<raw_batch_uint32_t>(static_cast<raw_batch_float_t>(uv.y * (m_image->h - 1.0) + 0.5));

    //    auto& format = *m_image->format;
    //    raw_batch_uint32_t index = (y * m_image->pitch + x * format.BytesPerPixel);

    //    // stack-alloc blob for storing indices and color components
    //    uint8_t unaligned_blob[5 * (pixels_per_batch * sizeof(unsigned) + batch_uint32_align)];
    //    unsigned* pindex = align_ptr<batch_uint32_align>(reinterpret_cast<unsigned*>(unaligned_blob));
    //    unsigned* pr = align_ptr<batch_uint32_align>(pindex + pixels_per_batch);
    //    unsigned* pg = align_ptr<batch_uint32_align>(pr + pixels_per_batch);
    //    unsigned* pb = align_ptr<batch_uint32_align>(pg + pixels_per_batch);
    //    unsigned* pa = align_ptr<batch_uint32_align>(pb + pixels_per_batch);

    //    batch_store_aligned(index, pindex);
    //    
    //    // fill the buffers
    //    swizzle::detail::static_for<0, pixels_per_batch>([&](size_t i)
    //    {
    //        auto pixelPtr = static_cast<uint8_t*>(m_image->pixels) + pindex[i];

    //        uint32_t pixel = 0;
    //        for (size_t i = 0; i < format.BytesPerPixel; ++i)
    //        {
    //            pixel |= (pixelPtr[i] << (i * 8));
    //        }

    //        pr[i] = (pixel & format.Rmask) >> format.Rshift;
    //        pg[i] = (pixel & format.Gmask) >> format.Gshift;
    //        pb[i] = (pixel & format.Bmask) >> format.Bshift;
    //        pa[i] = format.Amask ? ((pixel & format.Amask) >> format.Ashift) : 255;
    //    });

    //    // load data
    //    raw_batch_uint32_t r, g, b, a;
    //    batch_load_aligned(r, pr);
    //    batch_load_aligned(g, pg);
    //    batch_load_aligned(b, pb);
    //    batch_load_aligned(a, pa);

    //    vec4 result;
    //    result.r = batch_cast<raw_batch_float_t>(r);
    //    result.g = batch_cast<raw_batch_float_t>(g);
    //    result.b = batch_cast<raw_batch_float_t>(b);
    //    result.a = batch_cast<raw_batch_float_t>(a);

    //    return clamp(result / 255.0f, c_zero, c_one);
    //}
}

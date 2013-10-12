// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

#include <swizzle/glsl/naive/vector.h>
#include <swizzle/glsl/naive/matrix.h>
#include <swizzle/glsl/texture_functions.h>

typedef swizzle::glsl::naive::vector< float, 2 > vec2;
typedef swizzle::glsl::naive::vector< float, 3 > vec3;
typedef swizzle::glsl::naive::vector< float, 4 > vec4;

static_assert(sizeof(vec2) == sizeof(float[2]), "Too big");
static_assert(sizeof(vec3) == sizeof(float[3]), "Too big");
static_assert(sizeof(vec4) == sizeof(float[4]), "Too big");

typedef swizzle::glsl::naive::matrix< swizzle::glsl::naive::vector, float, 2, 2> mat2;
typedef swizzle::glsl::naive::matrix< swizzle::glsl::naive::vector, float, 3, 3> mat3;
typedef swizzle::glsl::naive::matrix< swizzle::glsl::naive::vector, float, 4, 4> mat4;

//! A really, really simplistic sampler using SDLImage
struct SDL_Surface;
class sampler2D : public swizzle::glsl::texture_functions::tag
{
public:
    enum WrapMode
    {
        Clamp,
        Repeat,
        MirrorRepeat
    };

    typedef vec2 tex_coord_type;

    sampler2D(const char* path, WrapMode wrapMode);
    ~sampler2D();
    vec4 sample(vec2 coord);

private:
    SDL_Surface *m_image;
    WrapMode m_wrapMode;

    // do not allow copies to be made
    sampler2D(const sampler2D&);
    sampler2D& operator=(const sampler2D&);
};

// this where the magic happens...
namespace glsl_sandbox
{
    // a nested namespace used when redefining 'inout' and 'out' keywords
    namespace ref
    {
        typedef ::vec2& vec2;
        typedef ::vec3& vec3;
        typedef ::vec4& vec4;
    }

    #include <swizzle/glsl/vector_functions.h>

    // constants shaders are using
    float time;
    vec2 mouse;
    vec2 resolution(50.0f, 50.0f);

    // constants some shaders from shader toy are using
    vec2& iResolution = resolution;
    float& iGlobalTime = time;
    vec2& iMouse = mouse;

    sampler2D diffuse("diffuse.png", sampler2D::Repeat);
    sampler2D specular("specular.png", sampler2D::Repeat);

    struct fragment_shader
    {
        vec2 gl_FragCoord;
        vec4 gl_FragColor;
        void operator()(void);
    };

    // change meaning of glsl keywords to match sandbox
    #define uniform extern
    #define in
    #define out ref::
    #define inout ref::
    #define main fragment_shader::operator()

    #pragma warning(push)
    #pragma warning(disable: 4244) // disable return implicit conversion warning
    #pragma warning(disable: 4305) // disable truncation warning
    
    //#include "shaders/sampler.frag"
    //#include "shaders/leadlight.frag"
    //#include "shaders/terrain.frag"
    //#include "shaders/complex.frag"
    //#include "shaders/road.frag"
    #include "shaders/gears.frag"

    // be a dear a clean up
    #pragma warning(pop)
    #undef main
    #undef in
    #undef out
    #undef inout
    #undef uniform
}

// these headers, especially SDL.h & time.h set up names that are in conflict with sandbox'es;
// sandbox should be moved to a separate h/cpp pair, but out of laziness... including them
// *after* sandbox solves it too

#include <iostream>
#include <sstream>
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>
#if OMP_ENABLED
#include <omp.h>
#endif

extern C_LINKAGE int main(int argc, char* argv[])
{
    using namespace std;

    // initialise SDLImage
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int initted = IMG_Init(flags);
    if ((initted & flags) != flags) {
        cerr << "WARNING: failed to initialise required jpg and png support: " << IMG_GetError() << endl;
    }

    SDL_Surface* screen = nullptr;
    SDL_Surface* bmp = nullptr;

    if (argc == 2)
    {
        vec2 resolution;

        std::stringstream s;
        s << argv[1];
        s >> resolution;
        glsl_sandbox::resolution = resolution;
    }

    if ( glsl_sandbox::resolution.x <= 0.0f || glsl_sandbox::resolution.y <= 0.0f )
    {
        cerr << "ERROR: invalid resolution: " << glsl_sandbox::resolution  << endl;
        return 1;
    }

    cout << "\n";
    cout << "+/-  - increase/decrease time scale\n";
    cout << "lmb  - update glsl_sandbox::mouse\n";
    cout << "esc  - quit\n\n";

    try {
        
        SDL_Init( SDL_INIT_VIDEO );
        SDL_EnableKeyRepeat(100, 10);
        SDL_WM_SetCaption("SDL/Swizzle", "SDL/Swizzle");
        SDL_Rect rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = static_cast<Uint16>(glsl_sandbox::resolution.x + 0.5f);
        rect.h = static_cast<Uint16>(glsl_sandbox::resolution.y + 0.5f);
        screen = SDL_SetVideoMode( rect.w, rect.h, 24, SDL_SWSURFACE ); 
        bmp = SDL_CreateRGBSurface(SDL_SWSURFACE, rect.w, rect.h, 24, 0x000000ff, 0x0000ff00, 0x00ff0000, 0 );
        
        float time = 0;
        float timeScale = 1;
        int frame = 0;

        SDL_Event event;
        bool quit = false;

        clock_t begin = clock();
        double bufferedTimeScale = 0.0f;
        

        while (!quit) {

            SDL_LockSurface(bmp);

            glsl_sandbox::time = time;

#if OMP_ENABLED
            #pragma omp parallel 
            {
                int thredsCount = omp_get_num_threads();
                int threadNum = omp_get_thread_num();

                int heightPerThread = bmp->h / thredsCount;
                int heightStart = threadNum * heightPerThread;
                int heightEnd = (threadNum == thredsCount - 1) ? bmp->h : (heightStart + heightPerThread);
#else
            {
                int heightStart = 0;
                int heightEnd = bmp->h;
#endif

                glsl_sandbox::fragment_shader shader;
                for ( int y = heightStart; y < heightEnd; ++y )
                {
                    uint8_t * ptr = reinterpret_cast<uint8_t*>(bmp->pixels) + y * bmp->pitch;
                    for (int x = 0; x < bmp->w; ++x )
                    {
                        shader.gl_FragCoord = vec2(static_cast<float>(x) , bmp->h - 1.0f - y);
                    
                        shader();

                        auto color = glsl_sandbox::clamp(shader.gl_FragColor, 0.0f, 1.0f);

                        *ptr++ = static_cast<uint8_t>(255 * color.x + 0.5f);
                        *ptr++ = static_cast<uint8_t>(255 * color.y + 0.5f);
                        *ptr++ = static_cast<uint8_t>(255 * color.z + 0.5f);
                    }
                }
            }

            SDL_UnlockSurface(bmp);
            SDL_BlitSurface( bmp, NULL, screen, NULL );
            SDL_Flip( screen );

            // process events
            while (SDL_PollEvent(&event)) {
                switch ( event.type ) {
                case SDL_QUIT:
                    quit = true;
                    break; 
                case SDL_KEYDOWN:
                    switch ( event.key.keysym.sym ) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_PLUS:
                    case SDLK_EQUALS:
                        timeScale *= 2.0f;
                        break;
                    case SDLK_MINUS:
                        timeScale /= 2.0f;
                        break;
                    default:
                        break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    glsl_sandbox::mouse.x = static_cast<float>(event.button.x);
                    glsl_sandbox::mouse.y = static_cast<float>(bmp->h - 1 - event.button.y);
                    break;
                default:
                    break;
                }
            }

            cout << "frame: " << frame++ << "\t time: " << time << "\t timescale: " << timeScale << "         \r";
            cout.flush();

            clock_t delta = clock() - begin;
            time += static_cast<float>(delta / double(CLOCKS_PER_SEC) * timeScale);
            begin = clock();
        }
    } catch ( exception& error ) {
        cerr << "ERROR: " << error.what() << endl;
    } catch (...) {
        cerr << "ERROR: Unknown error" << endl;
    }


    if ( bmp ) {
        SDL_FreeSurface(bmp);
    }
    if ( screen ) { 
        SDL_FreeSurface(screen);
    }
    SDL_Quit();
    return 0; 
}

sampler2D::sampler2D( const char* path, WrapMode wrapMode ) : m_wrapMode(wrapMode)
{
    m_image = IMG_Load(path);
    if (!m_image)
    {
        std::cerr << "WARNING: Failed to load texture " << path << "\n";
        std::cerr << "  SDL_Image message: " << IMG_GetError() << "\n";
    }
}

sampler2D::~sampler2D()
{
    if ( m_image )
    {
        SDL_FreeSurface(m_image);
        m_image = nullptr;
    }
}

vec4 sampler2D::sample( vec2 coord )
{
    using namespace glsl_sandbox;

    switch (m_wrapMode)
    {
    case Repeat:
        coord = mod(coord, 1);
        break;
    case MirrorRepeat:
        coord = abs(mod(coord-1, 2)-1);
        break;
    case Clamp:
    default:
        coord = clamp(coord, 0, 1);
        break;
    }

    // OGL uses left-bottom corner as origin...
    coord.y = 1 - coord.y;

    if ( !m_image )
    {
        // checkers
        if ( coord.x < 0.5 && coord.y < 0.5 || coord.x > 0.5 && coord.y > 0.5 )
        {
            return vec4(1, 0, 0, 1);
        }
        else
        {
            return vec4(0, 1, 0, 1);
        }
    }
    else
    {
        int x = static_cast<int>(coord.x * (m_image->w - 1) + 0.5);
        int y = static_cast<int>(coord.y * (m_image->h - 1) + 0.5);

        auto& format = *m_image->format;
        auto pixelPtr = static_cast<uint8_t*>(m_image->pixels) + (y * m_image->pitch + x * format.BytesPerPixel);
        uint32_t pixel = 0;
        for (size_t i = 0; i < format.BytesPerPixel; ++i )
        {
            pixel |= (pixelPtr[i] << (i*8));
        }

        int r = (pixel & format.Rmask) >> format.Rshift;
        int g = (pixel & format.Gmask) >> format.Gshift;
        int b = (pixel & format.Bmask) >> format.Bshift;
        int a = format.Amask ? ((pixel & format.Amask) >> format.Ashift) : 255;
        return clamp(vec4(r, g, b, a) / 255.0f, 0.0f, 1.0f);
    }
}
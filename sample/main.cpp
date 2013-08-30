#include "swizzle/naive/vector_adapter.h"
#include "swizzle/detail/glsl_functions.h"
#include "swizzle/detail/vector_binary_operators.h"

typedef swizzle::detail::binary_operators::tag tag;
typedef swizzle::naive::vector_adapter< float, 2 > vec2;
typedef swizzle::naive::vector_adapter< float, 3 > vec3;
typedef swizzle::naive::vector_adapter< float, 4 > vec4;

namespace glsl_sandbox
{
    using namespace swizzle::detail::glsl_functions;

    // constants shaders are using
    float time;
    vec2 mouse;
    vec2 resolution(100.0f, 100.0f);

    // constants some shaders from shader toy are using
    vec2& iResolution = resolution;
    float& iGlobalTime = time;
    vec2& iMouse = mouse;

    struct fragment_shader
    {
        vec2 gl_FragCoord;
        vec4 gl_FragColor;
        void operator()(void);
    };

    // change meaning of glsl keywords to match sandbox
    #define uniform extern
    #define in
    #define main fragment_shader::operator()

    #pragma warning(push)
    #pragma warning(disable: 4244) // disable return implicit conversion warning
    #pragma warning(disable: 4305) // disable truncation warning

    //#include "shaders/water_turbulence.frag"
    #include "shaders/leadlight.frag"
    //#include "shaders/terrain.frag"
    //#include "shaders/complex.frag"
    //#include "shaders/road.frag"
    //#include "shaders/gears.frag"

    // be a dear a clean up
    #pragma warning(pop)
    #undef main
    #undef in
    #undef uniform
}

// these headers, especially SDL.h & time.h set up names that are in conflict with sandbox'es;
// sandbox should be moved to a separate h/cpp pair, but out of laziness... including them
// *after* sandbox solves it too

#include <iostream>
#include <sstream>
#include <SDL.h>
#include <time.h>
#if OMP_ENABLED
#include <omp.h>
#endif

extern C_LINKAGE int main(int argc, char* argv[])
{
    SDL_Surface* screen = nullptr;
    SDL_Surface* bmp = nullptr;

    using namespace std;

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
                int heightEnd = (threadNum == thredsCount - 1) ? bmp->h : heightPerThread;
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
                    glsl_sandbox::mouse.y = static_cast<float>(event.button.y);
                    break;
                default:
                    break;
                }
            }

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
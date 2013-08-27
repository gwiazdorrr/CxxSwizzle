#include <iostream>
#include <SDL.h>


#include "swizzle/naive/vector_adapter.h"
#include "swizzle/detail/glsl_functions.h"
#include "swizzle/detail/vector_binary_operators.h"

typedef swizzle::detail::binary_operators::tag tag;
typedef swizzle::naive::vector_adapter< swizzle::naive::vector_adapter_traits<float, 2, tag> > vec2;
typedef swizzle::naive::vector_adapter< swizzle::naive::vector_adapter_traits<float, 3, tag> > vec3;
typedef swizzle::naive::vector_adapter< swizzle::naive::vector_adapter_traits<float, 4, tag> > vec4;

namespace glsl_sandbox
{
    using namespace swizzle::detail::glsl_functions;

    vec2 gl_FragCoord;
    vec4 gl_FragColor;

    #define uniform extern
    #define in

    float time;
    vec2 mouse;
    vec2 resolution(320.0f, 240.0f);

    vec2& iResolution = resolution;
    float& iGlobalTime = time;

    #pragma warning(push)
    #pragma warning(disable: 4244) // disable return implicit conversion warning
    #pragma warning(disable: 4305) // disable truncation warning

    //#include "shaders/water_turbulence.frag"
    //#include "shaders/leadlight.frag"
    //#include "shaders/complex.frag"
    //#include "shaders/road.frag"
    #include "shaders/gears.frag"

    #pragma warning(pop)
}

#include <time.h>

extern C_LINKAGE int main(int argc, char* argv[])
{
    SDL_Surface* screen = nullptr;
    SDL_Surface* bmp = nullptr;

    using namespace glsl_sandbox;
    using namespace std;

    try {
        
        SDL_Init( SDL_INIT_VIDEO );
        SDL_EnableKeyRepeat(100, 10);
        SDL_WM_SetCaption("SDL/Swizzle", "SDL/Swizzle");
        SDL_Rect rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = static_cast<Uint16>(iResolution.x);
        rect.h = static_cast<Uint16>(iResolution.y);
        screen = SDL_SetVideoMode( rect.w, rect.h, 24, SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL ); 
        bmp = SDL_CreateRGBSurface(SDL_SWSURFACE, rect.w, rect.h, 24, 0x000000ff, 0x0000ff00, 0x00ff0000, 0 );

        float time = 0;
        float timeScale = 1;

        SDL_Event event;
        bool quit = false;

        clock_t begin = clock();
        double bufferedTimeScale = 0.0f;

        while (!quit) {

            SDL_LockSurface(bmp);

            iGlobalTime = time;

            for ( int y = 0; y < bmp->h; ++y )
            {
                uint8_t * ptr = reinterpret_cast<uint8_t*>(bmp->pixels) + y * bmp->pitch;
                for (int x = 0; x < bmp->w; ++x )
                {
                    gl_FragCoord = vec2(static_cast<float>(x) , bmp->h - 1.0f - y);
                    
                    glsl_sandbox::main();

                    gl_FragColor = clamp(gl_FragColor, 0.0f, 1.0f);

                    *ptr++ = static_cast<uint8_t>(255 * gl_FragColor.x + 0.5f);
                    *ptr++ = static_cast<uint8_t>(255 * gl_FragColor.y + 0.5f);
                    *ptr++ = static_cast<uint8_t>(255 * gl_FragColor.z + 0.5f);
                }
            }

            SDL_UnlockSurface(bmp);
            SDL_BlitSurface( bmp, NULL, screen, NULL );
            SDL_Flip( screen );

            // zajmujemy siê eventami
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
                    mouse.x = static_cast<float>(event.button.x);
                    mouse.y = static_cast<float>(event.button.y);
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
        cerr << "Unknown error" << endl;
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
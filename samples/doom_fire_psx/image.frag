#ifndef CXX_CONST
#define CXX_CONST const
#define CXX_IGNORE(x) x
#define CXX_ARRAY_AUTO(type) type[]
#define CXX_ARRAY(type) type[]
#define CXX_ARRAY_N(type, size) type[size]
#define CXX_MAKE_ARRAY(type) type[]
#endif

#define FIRE_WIDTH 320.0
#define FIRE_HEIGHT 160.0
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord/iResolution.xy;
    uv.x *= FIRE_WIDTH;
    uv.y *= FIRE_HEIGHT;
    int index = int(texelFetch(iChannel0, ivec2(uv), 0).r);
    fragColor = colors[index];
}
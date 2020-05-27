// Fork of https://shadertoy.com/view/Wd2GzV
// Fork of https://shadertoy.com/view/3djGzV

void mainImage( out vec4 O, vec2 U )
{
    O = U.x < iResolution.x/2.
        ? texelFetch(iChannel0,ivec2(U),0)
        : texelFetch(iChannel1,ivec2(U),0);
}
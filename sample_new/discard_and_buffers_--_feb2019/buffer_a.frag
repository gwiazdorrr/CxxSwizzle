void mainImage( out vec4 O, vec2 U )
{
    O = texelFetch(iChannel0,ivec2(U),0);
}
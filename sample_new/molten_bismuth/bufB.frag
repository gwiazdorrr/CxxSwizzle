// created by florian berger (flockaroo) - 2019
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

// just some mouse motion detection

void mainImage( out vec4 fragColor, vec2 fragCoord )
{
    vec4 c=texelFetch(iChannel0,ivec2(0),0);
    vec2 m=iMouse.xy;
    vec2 d=vec2(0);
    if(any(notEqual(iMouse.xy,iMouse.zw))) { d=iMouse.xy-c.xy; }
    fragColor.xyzw = vec4(m,d);
}


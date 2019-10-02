// https://www.shadertoy.com/view/4ltfD7

#define GRADMAG(f) (sqrt(dFdx(f) * dFdx(f) + dFdy(f) * dFdy(f)))

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float cellDensity = 7.0; // sin(iTime) * 1.0 + 5.0;
    float lineWidth   = 2.0;
    
    vec2 ms = iMouse.xy;
    if (ms.x < 10.0 && ms.y < 10.0) {
        ms.x = sin(iTime * 0.31) * 0.5 + 0.5;
        ms.y = cos(iTime * 0.34) * 0.5 + 0.5;
        ms *= iResolution.xy;
    }
    
    vec2 p = (fragCoord - iResolution.xy / 2.0) / iResolution.y;
    
    p *= cellDensity;
    p.y += 0.5;
  //  p += iTime * vec2(1.0, 0.0);
    
    p = (fract(p) - 0.5) * 2.0;
    
    float d = length(fragCoord - ms) / 100.0;
    d = d - length(p);
    
    // d = 1.0 / (0.0 + d);
    
    // because we're dividing by fwidth below,
    // d can be scaled by any value here without effect !
    d *= 10000.0;
        
    float l = smoothstep(0.1, 0.0, abs(d) / GRADMAG(d) / (10.0 * lineWidth));
    
    fragColor = vec4(vec3(l),1.0);
}
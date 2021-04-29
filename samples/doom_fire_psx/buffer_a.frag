#ifndef CXX_CONST
#define CXX_CONST const
#define CXX_IGNORE(x) x
#define CXX_ARRAY_AUTO(type) type[]
#define CXX_ARRAY(type) type[]
#define CXX_ARRAY_N(type, size) type[size]
#define CXX_MAKE_ARRAY(type) type[]
#endif

#define FPS 20.0
#define WIND_STRENGTH 4
#define WIND_START_OFFSET -1
#define DURATION_ON 10.0
#define DURATION_OFF 5.0
#define M_PI 3.1415926535897932384626433832795
// https://www.shadertoy.com/view/tdXXRM
float noise(vec2 p, float t) {
    float ta = t*.654321;
    float tb = t*(ta*.123456);
    return fract(sin(mod(p.x*ta+p.y*tb, M_PI))*5678.);
}
int sampleFire(vec2 coords) {
    return int(texelFetch(iChannel0, ivec2(coords), 0).r);
}
int spreadFire(vec2 coords, float t) {
    // how likely it is that the value stays the same?
    // the tutorial does this from the perspective of a source fire state; source
    // state can affect one of four pixels above, so a chance of a pixel being unaffected
    // is (3/4)^4
    float copyChance = pow(float(WIND_STRENGTH-1)/float(WIND_STRENGTH), float(WIND_STRENGTH));
    
    float rand = noise(coords, t);
    if (rand < copyChance) {
        // value unchanged
        return sampleFire(coords);
    } else {
        // normalize the random
        float r = (rand - copyChance) / (1.0 - copyChance);
        // pick source index
        float offset = r * float(WIND_STRENGTH) + float(WIND_START_OFFSET);
        // sample from the row below with a horizontal offset
        int state = sampleFire(coords + vec2(floor(offset), -1.0));
        // wind down (50% chance)
        state -= fract(offset) > 0.5 ? 1 : 0;
        return state;
    }
}
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    int result;// = sampleFire(fragCoord);
    float t = iTime;
    
#ifdef FPS
    t *= FPS;
    float previousT = (iTime - iTimeDelta) * FPS;
    if (fract(t) > fract(previousT)) {
        // the same frame, don't update
        result = sampleFire(fragCoord);
    } else
#endif    
    if (fragCoord.y < 1.0) {
        // toggle bottom row on and off
        if (mod(iTime, DURATION_ON + DURATION_OFF) < DURATION_ON) {
            result = colors.length() - 1;
        } else {
            result = 0;
        }
    } else {
        result = spreadFire(fragCoord, t);
    }
        
    fragColor.r = float(result);
}
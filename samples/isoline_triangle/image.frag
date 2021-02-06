// White on black, best fullscreen
//#define ALTERNATE_VERSION


// --------------------------------------------------------
// Simplex(ish) Noise
// Shane https://www.shadertoy.com/view/ldscWH
// --------------------------------------------------------

vec3 hash33(vec3 p) { 
    float n = sin(dot(p, vec3(7, 157, 113)));    
    return fract(vec3(2097152, 262144, 32768)*n)*2. - 1.;
}

float tetraNoise(in vec3 p)
{
    vec3 i = floor(p + dot(p, vec3(0.333333)) );  p -= i - dot(i, vec3(0.166666)) ;
    vec3 i1 = step(p.yzx, p), i2 = max(i1, 1.0-i1.zxy); i1 = min(i1, 1.0-i1.zxy);    
    vec3 p1 = p - i1 + 0.166666, p2 = p - i2 + 0.333333, p3 = p - 0.5;
    vec4 v = max(0.5 - vec4(dot(p,p), dot(p1,p1), dot(p2,p2), dot(p3,p3)), 0.0);
    vec4 d = vec4(dot(p, hash33(i)), dot(p1, hash33(i + i1)), dot(p2, hash33(i + i2)), dot(p3, hash33(i + 1.)));
    return clamp(dot(d, v*v*v*8.)*1.732 + .5, 0., 1.); // Not sure if clamping is necessary. Might be overkill.
}

// --------------------------------------------------------
// Triangle distance
// The corners aren't a correct distance, normally they'd
// be rounded, but here they're pointy to keep them sharp
// when drawn with the isolines
// --------------------------------------------------------

float sTri(vec2 p, float radius) {
    radius /= 2.;
    vec2 a = normalize(vec2(1.6,1.));
    return max(
        dot(p, vec2(0,-1)) - radius,
        max(
        	dot(p, a) - radius,
        	dot(p, a * vec2(-1,1)) - radius
        )
    );
}

// --------------------------------------------------------
// Repeat space and blend the edges
//
// Imagine we have the following domain:
// 0 1 2 3 4 5 6 7 8 9 ...
//
// If you repeat with a size of 3, you get hard edges
// between 2 and 0:
// 0 1 2 0 1 2 0 1 2 ...
//
// You could flip each repetition, but you'd see a visible
// mirror effect:
// 0 1 2 2 1 0 0 1 2 ...
// 
// So instead, take two samples out of phase:
// 0 1 2 0 1 2 0 1 2 ...
// 2 0 1 2 0 1 2 0 1 ...
//
// And then blend the samples at these points in such a way
// that the visible joins of one sample are masked by the 
// continuous part of the other sample.
// --------------------------------------------------------

#define PI 3.14159265359

vec2 smoothRepeatStart(float x, float size) {
  return vec2(
      mod(x - size / 2., size),
      mod(x, size)
  );
}

float smoothRepeatEnd(float a, float b, float x, float size) {
  return mix(a, b,
      smoothstep(
          0., 1.,
          sin((x / size) * PI * 2. - PI * .5) * .5 + .5
      )
  );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Square uv centered and sclead to the screen height
    vec2 uv = (-iResolution.xy + 2. * fragCoord.xy) / iResolution.y;
    
    // Zoom in a bit
    uv /= 2.;

    #ifdef ALTERNATE_VERSION
   		uv *= 1.8;
   	#endif
    
    float repeatSize = 4.;
    float x = uv.x - mod(iTime, repeatSize / 2.);
    float y = uv.y;

    vec2 ab; // two sample points on one axis

    float noise;
    float noiseA, noiseB;
    
    // Blend noise at different frequencies, moving in
    // different directions
    
    ab = smoothRepeatStart(x, repeatSize);
    noiseA = tetraNoise(16.+vec3(vec2(ab.x, uv.y) * 1.2, 0)) * .5;
    noiseB = tetraNoise(16.+vec3(vec2(ab.y, uv.y) * 1.2, 0)) * .5;
    noise = smoothRepeatEnd(noiseA, noiseB, x, repeatSize);

    ab = smoothRepeatStart(y, repeatSize / 2.);
    noiseA = tetraNoise(vec3(vec2(uv.x, ab.x) * .5, 0)) * 2.;
    noiseB = tetraNoise(vec3(vec2(uv.x, ab.y) * .5, 0)) * 2.;
    noise *= smoothRepeatEnd(noiseA, noiseB, y, repeatSize / 2.);

    ab = smoothRepeatStart(x, repeatSize);
    noiseA = tetraNoise(9.+vec3(vec2(ab.x, uv.y) * .05, 0)) * 5.;
    noiseB = tetraNoise(9.+vec3(vec2(ab.y, uv.y) * .05, 0)) * 5.;
    noise *= smoothRepeatEnd(noiseA, noiseB, x, repeatSize);

    noise *= .75;

    // Blend with a linear gradient, this gives the isolines a
    // common orientation (try changing .6 to 1.)
    noise = mix(noise, dot(uv, vec2(-.66,1.)*.4), .6);
    
    // Create anti-aliased even weight isolines from the noise...

    // Break the continuous noise into steps
    float spacing = 1./50.;
    float lines = mod(noise, spacing) / spacing;

    // Convert each step into a bump, or, the sawtooth wave
    // into a triangle wave:
    //
    //     /|    /|
    //   /  |  /  |
    // /    |/    |
    //       
    // to:   
    //       
    //   /\    /\
    //  /  \  /  \ 
    // /    \/    \ 

    lines = min(lines * 2., 1.) - max(lines * 2. - 1., 0.);
    
    // Scale it by the amount the noise varies over a pixel,
    // factoring in the spacing scaling that was applied.
    // noise is used because it's continuous, if we use lines we'd
    // see stepping artefacts.
    lines /= fwidth(noise / spacing);
	
    // Double to occupy two pixels and appear smoother
    lines /= 2.;
    
    // Triangle distance
    float d = sTri(uv + vec2(0,.1), .3);
    
    // Create a fuzzy border from 0 - 1 around the triangle, this
    // controls the pointy shape where lines transition from thick
    // to thin
    float weight = smoothstep(.0, .05, d);

    // Adjust to the desired inner and outer weight
    #ifndef ALTERNATE_VERSION
    	weight = mix(4.2, 1.2, weight);
    #else
    	weight = mix(.5, .33, weight);
    #endif
    
    // Scale the weight when fullscreen
    weight *= iResolution.y / 287.;

    // Offset the line by the weight
    lines -= weight - 1.;
    
    #ifdef ALTERNATE_VERSION
    	lines = 1. - lines;
   	#endif
    
	fragColor = vec4(vec3(lines),1.0);
}

//=== original link for citation: https://www.shadertoy.com/view/llySRh
//find many other tricks here: https://shadertoyunofficial.wordpress.com/


// --- printing chars, integers and floats ---------------------------

// --- access to the image of ascii code c

// 2 implementations.
// Use #if 1 for extensive text use (with no superimposition nor color-appearance change)

#define IMMEDIATE_DRAW 0 

#if IMMEDIATE_DRAW //  (allows for superimposition and appearance change).

vec4 char(vec2 p, int c) {
    vec2 dFdx = dFdx(p/16.), dFdy = dFdy(p/16.);
    if (p.x<.0|| p.x>1. || p.y<0.|| p.y>1.) return vec4(0,0,0,1e5);
    //if (p.x<.25|| p.x>.75 || p.y<0.|| p.y>1.) return vec4(0,0,0,1e5); // strange bug with an old driver
	return textureGrad( iChannel0, p/16. + fract( vec2(c, 15-c/16) / 16. ), 
                        dFdx, dFdy );
  // variants:
  //float l = log2(length(fwidth(p/16.*iResolution.xy)));
  //return textureLod( iChannel0, p/16. + fract( vec2(c, 15-c/16) / 16. ), l);
                   // manual MIPmap to avoid border artifact. Should be textureLod, but don't available everywhere
}
#  define draw_char() vec4(0)  // stub the final call function is used

#else // Deferred draw (call draw_char() ). Run and compiles faster.
      //     First only determine the valid char id at cur pixel 
      //     then call the draw char once at the end.

int char_id = -1; vec2 char_pos, dfdx, dfdy; 
vec4 char(vec2 p, int c) {
    vec2 dFdx = dFdx(p/16.), dFdy = dFdy(p/16.);
 // if ( p.x>.25&& p.x<.75 && p.y>.0&& p.y<1. )  // normal char box
    if ( p.x>.25&& p.x<.75 && p.y>.1&& p.y<.85 ) // thighly y-clamped to allow dense text
        char_id = c, char_pos = p, dfdx = dFdx, dfdy = dFdy;
    return vec4(0);
}
vec4 draw_char() {
    int c = char_id; vec2 p = char_pos;
    return c < 0 
        ? vec4(0,0,0,1e5)
        : textureGrad( iChannel0, p/16. + fract( vec2(c, 15-c/16) / 16. ), 
                       dfdx, dfdy );
}
#endif

// --- display int4
#if 0
vec4 pInt(vec2 p, float n) {  // webGL2 variant with dynamic size
    vec4 v = vec4(0);
    for (int i = int(n); i>0; i/=10, p.x += .5 )
        v += char(p, 48+ i%10 );
    return v;
}
#else
vec4 pInt(vec2 p, float n) {
    vec4 v = vec4(0);
    if (n < 0.) 
        v += char(p - vec2(-.5,0), 45 ),
        n = -n;

    for (float i = 3.; i>=0.; i--) 
        n /=  9.999999, // 10., // for windows :-(
        v += char(p - .5*vec2(i,0), 48+ int(fract(n)*10.) );
    return v;
}
#endif

// --- display float4.4
vec4 pFloat(vec2 p, float n) {
    vec4 v = vec4(0);
    if (n < 0.) v += char(p - vec2(-.5,0), 45 ), n = -n;
    v += pInt(p,floor(n)); p.x -= 2.;
    v += char(p, 46);      p.x -= .5;
    v += pInt(p,fract(n)*1e4);
    return v;
}

// --- chars
int CAPS=0;
#define low CAPS=32;
#define caps CAPS=0;
#define spc  U.x-=.5;
#define C(c) spc O+= char(U,64+CAPS+c);
// NB: use either char.x ( pixel mask ) or char.w ( distance field + 0.5 )
 

// --- key toggles -----------------------------------------------------

// FYI: LEFT:37  UP:38  RIGHT:39  DOWN:40   PAGEUP:33  PAGEDOWN:34  END : 35  HOME: 36
// Modifiers: SHIFT: 16 CTRL: 17 ALT: 18
// Advice:  Mode: keyToggle(key)  Action: keydown(key)+keyclick(modifier)
#define keyToggle(ascii)  ( texelFetch(iChannel3,ivec2(ascii,2),0).x > 0.)
#define keyDown(ascii)    ( texelFetch(iChannel3,ivec2(ascii,1),0).x > 0.)
#define keyClick(ascii)   ( texelFetch(iChannel3,ivec2(ascii,0),0).x > 0.)

#define shift             ( texelFetch(iChannel3,ivec2(16,0),0).x  > 0.)
#define ctrl              ( texelFetch(iChannel3,ivec2(17,0),0).x  > 0.)
#define alt               ( texelFetch(iChannel3,ivec2(18,0),0).x  > 0.)
#define modifier          ( int(shift) +2*int(ctrl) + 4*int(alt) )


// --- events ----------------------------------------------------------

// --- mouse side events https://www.shadertoy.com/view/3dcBRS
#define mouseUp      ( iMouse.z < 0. )                  // mouse up even:   mouse button released (well, not just that frame)
#define mouseDown    ( iMouse.z > 0. && iMouse.w > 0. ) // mouse down even: mouse button just clicked
#define mouseClicked ( iMouse.w < 0. )                  // mouse clicked:   mouse button currently clicked

// --- texture loaded
#define textureLoaded(i) ( iChannelResolution[i].x > 0. ) 

// --- (re)init at resolution change or at texture (delayed) load:
// in buffX, store iResolution.x or iChannelResolution[i] somewhere. e.g. (0,0).w
// if ( currentVal != storedVal ) init; storeVal.


// --- short approx hue -------------- https://www.shadertoy.com/view/ll2cDc

#define hue(v)  ( .6 + .6 * cos( 6.3*(v)  + vec4(0,23,21,0)  ) )


// --- antialiased line drawing ------ https://www.shadertoy.com/view/4dcfW8

#define S(d,r,pix) smoothstep( .75, -.75, (d)/(pix)-(r))   // antialiased draw. r >= 1.
// segment with disc ends: seamless distance to segment
float line(vec2 p, vec2 a,vec2 b) { 
    p -= a, b -= a;
    float h = clamp(dot(p, b) / dot(b, b), 0., 1.);   // proj coord on line
    return length(p - b * h);                         // dist to segment
}
// line segment without disc ends ( sometime useful with semi-transparency )
float line0(vec2 p, vec2 a,vec2 b) { 
    p -= a, b -= a;
    float h = dot(p, b) / dot(b, b),                  // proj coord on line
          c = clamp(h, 0., 1.);
    return h==c ? length(p - b * h) : 1e5;            // dist to strict segment
}
    // You might directly return smoothstep( 3./R.y, 0., dist),
    //     but more efficient to factor all lines.
    // Indeed we can even return dot(,) and take sqrt at the end of polyline:
    // p -= b*h; return dot(p,p);


// --- old fashioned float-based hash. Might give user-dependant results --------------------

// nowadays integer noise is safer. see especially ||  : https://www.shadertoy.com/results?query=integer+hash+-
#define hash(p)  fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453)
#define hash2(p) fract(sin((p)*mat2(127.1,311.7, 269.5,183.3)) *43758.5453123)
#define hash3(p) fract(sin((p)*mat3(127.1,311.7, 74.7,  269.5,183.3,246.1,  113.5,271.9,124.6))*43758.5453123)
#define hash2x3(p) fract(sin((p)*mat3x2(127.1,311.7,  269.5,183.3,  113.5,271.9))*43758.5453123)


void mainImage( out vec4 O,  vec2 uv ) // === demos it all ==================================
{
    O -= O;
    vec2 R = iResolution.xy, U;
    uv /= R.y;    
    int lod = int(mod(iTime,10.));
    
    U = ( uv - vec2(.0,.9) ) * 16.;  caps C(18) low C(5)C(19)C(15)C(12) caps C(-6)  // "Resol"
                             U.x-=1.; low C(19)C(3)C(18)C(5)C(5)C(14)               // "screen"
    U = ( uv - vec2(.6,.9) ) * 16.;   low C(20)C(5)C(24)C(20)                       // "text"
    U = ( uv - vec2(.85,.9) ) * 16.;  low C(12)C(15)C(4) spc C(-48+lod)             // "lod"
    U = ( uv - vec2(1.15,.9) ) * 16.;  low C(19)C(15)C(21)C(14)C(4)                 // "sound"

    U = ( uv - vec2(.0,.6) ) * 16.;  caps C(13) low C(15)C(21)C(19)C(5) caps C(-6)  // "mouse"
    U = ( uv - vec2(.5,.6) ) * 16.;  caps C(20) low C(9)C(13)C(5) caps C(-6)        // "Time"
    U = ( uv - vec2(1.45,.55) ) * 16.;  caps C(11) low C(5)C(25) caps C(-6)         // "Key"

    
    U = ( uv - vec2(.1,.8) ) * 8.;        // --- column 1
    O += pInt(U, R.x);  U.y += .8;   // window resolution
    O += pInt(U, R.y);  U.y += .8;
    O += pFloat((U-vec2(-1,.35))*1.5, R.x/R.y);  U.y += .8;
  //O += pInt(U, iResolution.z);  U.y += .8;
    U.y += .8;
    O += pInt(U, iMouse.x);  U.y += .8;        // mouse location
    O += pInt(U, iMouse.y);  U.y += .8;
    U.y += .4;
    O += pInt(U, iMouse.z);  U.y += .8;        // last mouse-click location 
    O += pInt(U, iMouse.w);  U.y += .8;
    
    U = ( uv - vec2(.5,.8) ) * 8.;        // --- column 2

    if ( !textureLoaded(1) )                   // texture not loaded yet
        if (U.x>0. && U.y>-1.5 && U.x<2.5 && U.y<1.5) O.r+= .5;
    O += pInt(U, iChannelResolution[1].x);  U.y += .8; // texture ( video )
    O += pInt(U, iChannelResolution[1].y);  U.y += .8; // see LOD in column 2b
    //O += pInt(U, iChannelResolution[1].z);  U.y += .8;
    U.y += .8;

    O += pFloat(U, iTime);         U.y += .8;  // time
    O += pInt(U, float(iFrame));   U.y += .8;  // iFrame
    O += pFloat(U, 1./iTimeDelta); U.y += .8;  // FPS
    
    U.y += .8;

    O += pInt(U, iDate.w/3600.);          U.x -= 2.5;
    O += pInt(U, mod(iDate.w/60.,60.));   U.x -= 2.5;
    O += pFloat(U, mod(iDate.w,60.));  

    U = ( uv - vec2(.8,.8) ) * 8.;        // --- column 2b

    if (textureSize(iChannel1,0).x==1 &&  iChannelResolution[1].x > 1.) // video/sound lock by stupid new web media policy.
        if (U.x>0. && U.y>-1.5 && U.x<2.5 && U.y<1.5) O.r+= .5; // Colored bg on fonts turned BW later: in immediate mode, should be defered.
                                                                // Or transform char/draw_char for they directly return BW.
    ivec2 S = textureSize(iChannel1,lod);
    O += pInt(U, float(S.x));  U.y += .8; // texture LOD
    O += pInt(U, float(S.y));  U.y += .4;
    U *= 2.; O += pFloat(U, iChannelTime[1]);      // iChannelTime

    U = ( uv - vec2(.6,.2) ) * 16.;  caps C(8) low C(15)C(21)C(18)  // "Hour"
    U = ( uv - vec2(.95,.2) ) * 16.;  caps C(13) low C(9)C(14)      // "Min"
    U = ( uv - vec2(1.25,.2) ) * 16.;  caps C(19) low C(5)C(3)      // "Sec"

    U = ( uv - vec2(1.1,.8) ) * 8.;        // --- column 3

    O += pInt(U, iChannelResolution[2].x);  U.y += .8; // sound texture
    O += pInt(U, iChannelResolution[2].y);  U.y += .8;
    // O += pInt(U, iChannelResolution[2].z);  U.y += .8;

    O += pInt(U, iSampleRate/1e4);          U.x -= 2.; // iSampleRate
    O += pInt(U, mod(iSampleRate,1e4)); 

    U = ( uv - vec2(1.4,.45) ) * 8.;       // --- column 4
    
    bool b = false;
    for (int i=0; i<256; i++)
        if (keyClick(i) )  O += pInt(U, float(i)),  // keypressed ascii 
                           b=true, U.y += .1 *8.;
    if (b==false) O += pInt(U, -1.);
        
    O += draw_char().xxxx;
#if IMMEDIATE_DRAW
    O = O.xxxx;
#endif
   //O*=9.;
    
    // --- non-fonts stuff
    
    U = (uv*R.y/R-.9)/.1; 
    if (min(U.x,U.y)>0.) O = hue(U.x),  // --- hue (already in sRGB final space)
                             O*=O;      // just to fight the final sRGB convertion
    
    U = (uv -vec2(.9*R.x/R.y,.8))*10.;              // --- line drawing
    float pix = 10./R.y;               // pixel size
    O+= S( line( U,vec2(0,0),vec2(1.1,.85)), 3., pix);
    O+= S( line0(U,vec2(0.5,0),vec2(1.6,.85)), 3., pix);

    U = (uv -.8*R/R.y)*10.;                        // --- circle, discs, transp and blend
    O += S( abs(length(U-vec2(.2,1)) -.5), 1., pix); // disc. -.5: relative units. 1: pixel units
    O += S( length(U-vec2(1.1,1)) -.5, 0., pix) * vec4(1,0,0,1)*.5; // vec4(pureCol)*opacity
    O += (1.-O.a)*S( length(U-vec2(1.1,.3)) -.5, 0., pix) * vec4(0,1,0,1); // blend below prevs
    vec4 C = S( length(U-vec2(1.1,-.3)) -.5, 0., pix) * vec4(0,0,1,1)*.5;  // blend above prevs
    O = C + (1.-C.a)*O;
    
    U = uv -vec2(.9*R.x/R.y,.7);        // --- random numbers
    if ( U.x > 0. && U.y > 0. && U.y < .08 ) 
        U.x > .05*R.x/R.y ? O.rgb += hash(U) : O.rgb += hash2x3(U);
    
    // --- color space corrections
    O = pow(O, vec4(1./2.2) ); // shader result must be in sRGB colorspace -> gamma correction
                               // note that it is very close to sqrt(O).
    // similarly, color operations must be done in flat space, while textures are sRGB-encoded: pow(texture(),vec4(2.2)) or ~square.
}
// created by florian berger (flockaroo) - 2019
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

// single pass CFD - with some self consistency fixes

// ...the actual fluid simulation

// this is some "computational flockarooid dynamics" ;)
// the self-advection is done purely rotational on all scales. 
// therefore i dont need any divergence-free velocity field. 
// with stochastic sampling i get the proper "mean values" of rotations 
// over time for higher order scales.
//
// try changing "RotNum" for different accuracies of rotation calculation
// for even RotNum uncomment the line #define SUPPORT_EVEN_ROTNUM

#define RotNum 5
#define SUPPORT_EVEN_ROTNUM

#define keyTex iChannel2
#define KEY_I (texture(keyTex,vec2((105.5-32.0)/256.0,(0.5+0.0)/3.0)).x)

const float ang = PI2/float(RotNum);
mat2 m = mat2(cos(ang),sin(ang),-sin(ang),cos(ang));
mat2 mh = mat2(cos(ang*0.5),sin(ang*0.5),-sin(ang*0.5),cos(ang*0.5));

float getRot(vec2 pos, vec2 b)
{
    float l=log2(dot(b,b))*sqrt(.125)*.0;
    vec2 p = b;
    float rot=0.0;
    for(int i=0;i<RotNum;i++)
    {
        rot+=dot(textureLod(iChannel0,((pos+p)/Res0.xy),l).xy-vec2(0.5),p.yx*vec2(1,-1));
        p = m*p;
    }
    return rot/float(RotNum)/dot(b,b);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 pos = fragCoord;
    vec2 b = cos(float(iFrame)*.3-vec2(0,1.57));  // vary curl-evaluation-points in time
    vec2 v=vec2(0);
    float bbMax=.5*Res0.y; bbMax*=bbMax; // take curls up to half screen size
    for(int l=0;l<20;l++)
    {
        if ( dot(b,b) > bbMax ) break;
        vec2 p = b;
        for(int i=0;i<RotNum;i++)
        {
            v+=p.yx*getRot(pos+p,mh*b);
            p = m*p;
        }
        b*=2.0;
    }
    
    // perform advection
    fragColor=textureLod(iChannel0,fract((pos-v*vec2(-1,1)*5.*sqrt(Res0.x/600.))/Res0.xy),0.);
    
    // feeding some self-consistency into the velocity field
    // (otherwise velocity would be defined only implicitely by the multi-scale rotation sums)
    fragColor.xy=mix(fragColor.xy,v*vec2(-1,1)*sqrt(.125)*.9,.025);
    
    // add a little "motor"
    vec2 c=fract(scuv(iMouse.xy/iResolution.xy))*iResolution.xy;
    vec2 dmouse=texelFetch(iChannel3,ivec2(0),0).zw;
    if (iMouse.x<1.) c=Res0*.5;
    vec2 scr=fract((fragCoord.xy-c)/Res0.x+.5)-.5;
    // slowly rotating current in the center (when mouse not moved yet)
    if (iMouse.x<1.) fragColor.xy += 0.003*cos(iTime*.3-vec2(0,1.57)) / (dot(scr,scr)/0.05+.05);
    // feed mouse motion into flow
    fragColor.xy += .0003*dmouse/(dot(scr,scr)/0.05+.05);

    // add some "crunchy" drops to surface
    fragColor.zw += (texture(iChannel1,fragCoord/Res1*.35).zw-.5)*.002;
    fragColor.zw += (texture(iChannel1,fragCoord/Res1*.7).zw-.5)*.001;
    
    // initialization
    if(iFrame<=4) fragColor=vec4(0);
    if(KEY_I>.5 ) fragColor=(texture(iChannel1,uvSmooth(fragCoord.xy/Res0.xy*.05,Res1))-.5)*.7;
}


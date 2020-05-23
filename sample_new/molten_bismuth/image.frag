// created by florian berger (flockaroo) - 2019
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

// single pass CFD - with some self consistency fixes

// drawing the liquid

// same fluid as in "Spilled" - https://www.shadertoy.com/view/MsGSRd
// ...but with self-consistent-ish velocity field
// the previous method was just defined implicitely by the rotations on multiple scales
// here the calculated velocity field is put back into the stored field

// use mouse to push fluid, press I to init

#define Res  (iResolution.xy)

#define RandTex iChannel1

vec4 myenv(vec3 pos, vec3 dir, float period)
{
    return vec4(dir.xzy, 0);// texture(iChannel2, dir.xzy) + .15;
}

vec4  getCol(vec2 uv) { return texture(iChannel0,scuv(uv)); }
float getVal(vec2 uv) { return length(getCol(uv).xyz); }
    
vec2 getGrad(vec2 uv,float delta)
{
    vec2 d=vec2(delta,0); return vec2( getVal(uv+d.xy)-getVal(uv-d.xy),
                                       getVal(uv+d.yx)-getVal(uv-d.yx) )/delta;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
    
    // calculate normal from gradient (the faster the higher)
    vec3 n = vec3(-getGrad(uv,1.4/iResolution.x)*.02,1.);
    n=normalize(n);

    /*vec3 light = normalize(vec3(-1,1,2));
    float diff=clamp(dot(n,light),0.,1.0);
    float spec=clamp(dot(reflect(light,n),vec3(0,0,-1)),0.0,1.0); spec=exp2(log2(spec)*24.0)*2.5;*/
    
    // evironmental reflection
    vec2 sc=(fragCoord-Res*.5)/iResolution.x;
    vec3 dir=normalize(vec3(sc,-1.));
    vec3 R=reflect(dir,n);
    vec3 refl=myenv(vec3(0),R.xzy,1.).xyz;
    
    // slightly add velocityfield to color - gives it a bit of a 'bismuty' look
    vec4 col=getCol(uv)+.5;
    col=mix(vec4(1),col,.35);
    col.xyz*=.95+-.05*n; // slightly add some normals to color
    
	//fragColor.xyz = col.xyz*(.5+.5*diff)+.1*refl;
	fragColor.xyz = col.xyz*refl;
	fragColor.w=1.;
}


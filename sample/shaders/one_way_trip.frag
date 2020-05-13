// https://www.shadertoy.com/view/WlSSWV
// -------------------------------------------------------------------------------------------

// "ONE WAY TRIP" by PVM

// Shadertoy version of our 4k intro entry for Evoke 2019

// Code: Kali
// Music: Uctumi

// NOTE: Rewind the shader after it starts to correct audio sync

// -------------------------------------------------------------------------------------------

// Original code without optimizations for the 4k intro and including PVM logo

// global variables
float det=.005, fin=0., time; // raymarching threshold, aux variable
const float maxdist=60.; // max distance for raymarching
vec3 ldir=vec3(0.,1.,4.); // light direction (without normalization)
vec3 fcol; // global for coloring
vec3 suncol=vec3(2.,1.,.6); // sun color

// 2D rotation functions
mat2 rot2D(float a) {
    float s=sin(a);
    float c=cos(a);
    return mat2(c,s,-s,c);
}

// A version with degrees used when designing the logo
mat2 rot2Ddeg(float a) {
    a=radians(a);
    float s=sin(a);
    float c=cos(a);
    return mat2(c,s,-s,c);
}


// -------------------------------------------------------------------------------------------

// PVM LOGO


// 2D rectangle with a tilt distortion value
float rect(vec2 p, vec2 b, float inc) {
    p.x+=p.y*inc;
    vec2 d = abs(p)-b;
    return length(max(d,vec2(0))) + min(max(d.x,d.y),0.0);
}

// 2D triangle function by iq (I think)
float tri(vec2 p, vec2 q, float ang) {
    p*=rot2Ddeg(ang);
    p.x = abs(p.x);
    
    vec2 a = p - q*clamp( dot(p,q)/dot(q,q), 0.0, 1.0 );
    vec2 b = p - q*vec2( clamp( p.x/q.x, 0.0, 1.0 ), 1.0 );
    float s = -sign( q.y );
    vec2 d = min( vec2( dot(a,a), s*(p.x*q.y-p.y*q.x) ),
                  vec2( dot(b,b), s*(p.y-q.y)  ));

    return -sqrt(d.x)*sign(d.y);
}


// Here the logo is constructed from distoted rectangles and triangles
vec4 logo(vec2 uv) {
    uv*=1.2;
    uv.x*=1.15;
    uv.y-=.6;
    uv.x-=1.3;
    float d=rect(uv,vec2(.045,.25),-.5);
    uv.x+=.25;
    uv.y+=.01;
    d=min(d,rect(uv,vec2(.045,.24),.5));
    uv.x+=.265;
    uv.y-=.04;
    d=min(d,rect(uv,vec2(.045,.2),-.55));
    uv.x-=.73;
    uv.y-=.06;
    d=min(d,rect(uv,vec2(.045,.16),.4));
    uv.x-=.105;
    uv.y+=.074;
    d=min(d,rect(uv,vec2(.045,.085),-.45));
    uv.x-=.105;
    uv.y+=.045;
    d=min(d,rect(uv,vec2(.045,.13),.45));
    uv.x-=.25;
    uv.y+=.1;
    d=min(d,rect(uv,vec2(.18,.03),.0)); 
    uv.x+=1.32;
    uv.y-=.18;
    d=min(d,rect(uv+vec2(0.0,.03),vec2(.35,.03),.0));   
    uv.x-=.5165;
    uv.y+=.4;
    d=min(d,tri(uv,vec2(.09,.185),0.));
    uv.x-=.492;
    uv.y-=.56;
    d=min(d,tri(uv,vec2(.063,.14),180.));
    uv.x+=.225;
    uv.y-=.17;
    d=min(d,tri(uv,vec2(.063,.145),180.));
    uv.x-=.142;
    uv.y+=.555;
    d=min(d,tri(uv,vec2(.063,.145),0.));
    uv.x+=.985;
    uv.y+=.075;
    vec2 uvd=uv-vec2(uv.y,0.);
    d=min(d,tri(uvd-vec2(0.003,0.022),vec2(.04,.05),0.));
    uv.x-=.16   ;
    uv.y-=.63;
    uvd=uv+vec2(uv.y*.4,0.);
    d=min(d,tri(uvd+vec2(.03,0.),vec2(.07,.23),-145.));    
    uvd=uv+vec2(.465,.33);
    uvd*=rot2Ddeg(27.);
    uvd-=vec2(uvd.y*.5*sign(uvd.x),0.);
    d=min(d,rect(uvd,vec2(.08,.03),.0));    
    uvd=uv+vec2(-1.43,.534);
    uvd*=rot2Ddeg(206.);
    uvd-=vec2(uvd.y*.5*sign(uvd.x),0.);
    d=min(d,rect(uvd,vec2(.08,.03),.0));    
    float s=pow(abs(d)+.9,10.);
    uvd=uv+vec2(-.28,.36);
    uvd*=rot2Ddeg(50.);
    d=max(d,-rect(uvd,vec2(.1,.025),.0));
    // logo coloring, RGBA 
    float o=1.-smoothstep(0.,.01,d);
    float l=1.-smoothstep(0.,.05,d);
    vec3 col=mix(vec3(2.,.15,.1),vec3(1.,2.,.5),min(1.,abs(uv.y+.4)));
    return vec4(col*o+.1,l);
}


// -------------------------------------------------------------------------------------------

// FRACTAL 

// A bunch of sin and cos that defines the curves of the fractal path
// it returns the displacement at a given point. freq was used to explore diferent scales 
vec3 pitpath(float ti) { 
    float freq=.5;
    ti*=freq;
    float x=cos(cos(ti*.35682)+ti*.2823)*cos(ti*.1322)*1.5;
    float y=sin(ti*.166453)*4.+cos(cos(ti*.125465)+ti*.17354)*cos(ti*.05123)*2.;
    vec3  p=vec3(x,y,ti/freq);
    return p;
}

// Distance Estimation function

float de(vec3 pos) {
    float x=1.-smoothstep(5.,8.,abs(pos.x)); // aux variable used for washing the colors away from the path in the fractal
    pos.y+=.9; // offset y position for the fractal object
    pos.xy-=pitpath(pos.z).xy; // distortion of coordinates based on the path function
    mat2 rot=rot2D(.5); // rotation matrix used in the fractal iteration loop
    float organic=smoothstep(.5,1.,-sin(pos.z*.005)); // used for the "organic" part of the fractal
    mat2 rot2=rot2D(organic*.4); // rotation matrix used in the fractal iteration loop, it mutates the fractal to kinda "organic" shapes
    float fold=2.6+sin(pos.z*.01)+organic*1.5; // fold is a parameter for one of the operations inside the fractal loop
    pos.x+=pow(organic,.2)*fold*.75; // x offset for the organic part
    pos.y+=organic*.3; // y offset for the organic part
    pos.z=abs(5.-mod(pos.z,10.)); // tiling for repeating the fractal along the z axis
    pos.x=abs(10.-mod(pos.x+10.,20.)); // tiling for reapeating along x axis
    pos.x-=fold; // x offset to center the fractal
    vec4 p=vec4(pos,1.); // w value will be used for calculating the derivative
    vec3 m=vec3(1000.); // for orbit trap coloring
    int it=int(8.+fin*2.); // gives more iterations to the fractal at the end
    // Amazing Surface fractal formula by Kali
    // Derived from Mandelbox by tglad
    for (int i=0; i<it; i++) {
        p.xz=clamp(p.xz,-vec2(fold,2.),vec2(fold,2.))*2.0-p.xz; // fold transform on xz plane
        p.xyz-=vec3(.5,.8,1.); // translation transform
        p=p*(2.-organic*.2)/clamp(dot(p.xyz,p.xyz),.25,1.)-vec4(2.,.5,-1.,0.)*x; // scale + spheric fold + translation transform
        // rotation transforms
        p.xy*=rot;
        p.xz*=rot2; // rotations on xz and yz give the "organic" feel for the "alien reefs" part
        p.yz*=rot2; 
        m=min(m,abs(p.xyz)); // saves the minimum value of the position during the iteration, used for "orbit traps" coloring
    }
    // fractal coloring (fcol global variable)
    fcol=vec3(1.,.3,.0)*m*x; 
    fcol=max(fcol,length(p)*.0015*vec3(1.,.9,.8)*(1.-fin))*(1.+organic*.5);
    return (max(p.x,p.y)/p.w-.025*(1.-fin))*.85; // returns the distance estimation to the fractal's surface, with some adjustment towards the end
}


// -------------------------------------------------------------------------------------------

// RAYMARCHING

// Returns the perpendicular vector to the surface at a given point
vec3 normal(vec3 p) {
    vec3 d=vec3(0.,det*2.,0.);
    return normalize(vec3(de(p-d.yxx),de(p-d.xyx), de(p-d.xxy))-de(p));
}


// Ambient occlusion and soft shadows, classic iq's methods

float ao(vec3 p, vec3 n) {
    float td=0., ao=0.;
    for(int i=0; i<6; i++) {
        td+=.05;
        float d=de(p-n*td);
        ao+=max(0.,(td-d)/td);
    }
    return clamp(1.-ao*.1,0.,1.);
}


float shadow(vec3 p) {
    float sh=1.,td=.1;
    for (int i=0; i<50; i++) {
        p+=ldir*td;
        float d=de(p);
        td+=d;
        sh=min(sh,10.*d/td);
        if (sh<.05) break;
    }
    return clamp(sh,0.,1.);
}

// Lighting

vec3 shade(vec3 p, vec3 dir, vec3 n, vec3 col) {
    float sha=shadow(p); 
    float aoc=ao(p,n);
    float amb=.25*aoc; // ambient light with ambient occlusion
    float dif=max(0.,dot(ldir,-n))*sha; // diffuse light with shadow
    vec3 ref=reflect(dir,n); // reflection vector
    float spe=pow(max(0.,dot(ldir,ref)),10.)*.7*sha; // specular lights    
    return col*(amb+dif)+spe*suncol; // lighting applied to the surface color
}

// Raymarching

vec4 march(vec3 from, vec3 dir, vec3 camdir) {
    // variable declarations
    vec3 p=from, col=vec3(0.1), backcol=col;
    float totdist=0., d=0.,sdet, glow=0., lhit=1.;
    // the detail value is smaller towards the end as we are closer to the fractal boundary
    det*=1.-fin*.7;
    // raymarching loop to obtain an occlusion value of the sun at the camera direction
    // used for the lens flare
    for (int i=0; i<70; i++) {
        p+=d*ldir; // advance ray from camera pos to light dir
        d=de(p)*2.; // distance estimation, doubled to gain performance as we don't need too much accuracy for this
        lhit=min(lhit,d); // occlusion value based on how close the ray pass from the surfaces and very small if it hits 
        if (d<det) { // ray hits the surface, bye
            break;
        }
    }
    // main raymarching loop
    for (int i=0; i<150; i++) {
        p=from+totdist*dir; // advance ray
        d=de(p); // distance estimation to fractal surface
        sdet=det*(1.+totdist*.1); // makes the detail level lower for far hits 
        if (d<sdet||totdist>maxdist) break; // ray hits the surface or it reached the max distance defined
        totdist+=d; // distance accumulator  
        glow++; // step counting used for glow
    }
    float sun=max(0.,dot(dir,ldir)); // the dot product of the cam direction and the light direction using for drawing the sun
    if (d<.2) { // ray most likely hit a surface
        p-=(sdet-d)*dir; // backstep to correct the ray position
        vec3 c=fcol; // saves the color set by the de function to not get altered by the normal calculation
        vec3 n=normal(p); // calculates the normal at the ray hit point
        col=shade(p,dir,n,c); // sets the color and lighting
    } else { // ray missed any surface, this is the background
        totdist=maxdist; 
        p=from+dir*maxdist; // moves the ray to the max distance defined
        // Kaliset fractal for stars and cosmic dust near the sun. 
        vec3 st = (dir * 3.+ vec3(1.3,2.5,1.25)) * .3;
        for (int i = 0; i < 10; i++) st = abs(st) / dot(st,st) - .8;
        backcol+=length(st)*.015*(1.-pow(sun,3.))*(.5+abs(st.grb)*.5);
        sun-=length(st)*.0017;
        sun=max(0.,sun);
        backcol+=pow(sun,100.)*.5; // adds sun light to the background
    }
    backcol+=pow(sun,20.)*suncol*.8; // sun light
    float normdist=totdist/maxdist; // distance of the ray normalized from 0 to 1
    col=mix(col,backcol,pow(normdist,1.5)); // mix the surface with the background in the far distance (fog)
    col=max(col,col*vec3(sqrt(glow))*.13); // adds a little bit of glow
    // lens flare
    vec2 pflare=dir.xy-ldir.xy;
    float flare=max(0.,1.0-length(pflare))-pow(abs(1.-mod(camdir.x-atan(pflare.y,pflare.x)*5./3.14,2.)),.6);
    float cflare=pow(max(0.,dot(camdir,ldir)),20.)*lhit;
    col+=pow(max(0.,flare),3.)*cflare*suncol;
    col+=pow(sun,30.)*cflare;
    // "only glow" part (at sec. 10)
    col.rgb=mix(col.rgb,glow*suncol*.01+backcol,1.-smoothstep(0.,.8,abs(time-10.5)));
    return vec4(col,normdist); // returns the resulting color and a normalized depth in alpha
}   //(depth was going to be used for a postprocessing shader) 


// -------------------------------------------------------------------------------------------

// Camera and main function

// I learnt this function from eiffie,
// it takes a direction, a reference up vec3
// and returns the rotation matrix to orient a vector
mat3 lookat(vec3 dir, vec3 up){
    dir=normalize(dir);vec3 rt=normalize(cross(dir,normalize(up)));
    return mat3(rt,cross(rt,dir),dir);
}


// the path of the camera at a given point of time
vec3 campath(float ti) {
    float start=pow(max(0.,1.-ti*.02),3.); // interpolation curve for the starting camera
    vec3 p=pitpath(ti); // path displacement of the fractal 
    p*=1.-start; // the camera gradually ends following the fractal when, that happens when start=0
    p+=vec3(start*30.,start*25.,0.); // position offset for starting camera curve   
    return p;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv=fragCoord/iResolution.xy-.5;
    uv.x*=iResolution.x/iResolution.y;
    vec3 dir=normalize(vec3(uv,1.)); // ray direction  
    time=mod(iTime,162.); // time loop
    fin=smoothstep(145.,147.5,time); // aux variable used for the end sequence
    // camera accelerations and slow downs
    float acel1=smoothstep(11.,12.,time)*7.31;
    float acel2=smoothstep(99.,100.,time)*4.;
    float desacel1=smoothstep(77.,78.,time)*5.;
    float desacel2=fin*9.5;
    float tt=time;
    // freeze BW frame
    if (abs(tt-25.5)<.5) tt=25.;
    float acel=acel1+acel2-desacel1-desacel2;
    // time variable
    float t=tt*(3.6+acel)-acel1*11.-acel2*99.+desacel1*77.+desacel2*147.5;
    t+=smoothstep(125.,145.,time)*243.;
    vec3 from=campath(t); // camera position
    from.y-=desacel2*.035; // camera offset on 2nd slow down
    vec3 fw=normalize(campath(t+3.)-from); // camera forward direction
    from.x-=fw.x*.1; // camera x offset based on the forward direction
    dir=dir*lookat(fw*vec3(1.,-1.,1.),vec3(fw.x*.2,1.,0.)); // re-orientation of the ray dir with the camera fwd dir
    ldir=normalize(ldir); // light dir normalization 
    vec4 col=march(from, dir, fw); // get color from raymarching and background
    col.rgb=mix(vec3(length(col.rgb)*.6),col.rgb,.85-step(abs(tt-25.),.1)); // BW freeze frame sequence coloring
    col.rgb*=1.-smoothstep(25.,26.,time)+step(25.1,tt); // BW freeze frame sequence fading
    col.rgb*=1.+step(abs(tt-25.),.1);
    // PVM Logo color mixing
    vec4 pvm=logo(uv*1.5+vec2(.9,.5))*smoothstep(1.,3.,time+uv.x*2.)*(1.-smoothstep(7.5,8.,time+uv.x*2.));
    col.rgb=mix(col.rgb,pvm.rgb,pvm.a);
    // fade in from black
    col.rgb*=smoothstep(0.,4.,time);
    // fade out to black
    col.rgb*=1.-smoothstep(160.,162.,time);
    fragColor = col;
}
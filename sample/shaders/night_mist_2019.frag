/*
----- Night Mist ------
-- by NuSan & Valden --

3rd place at Revision 2019 - PC 4k intro

http://www.pouet.net/prod.php?which=81046
https://youtu.be/m7IFUjf_8gg

NuSan: Concept, visual, code
Valden: Music

Original tools: Leviathan 2.0 by Armak, 4klang by Alcatraz, Shader Minifier by LLB
*/

#define time iTime
#define bpm (time*1.5)
#define section (bpm/16.)
int section2 = 0;
float pi = acos(float(-1));

float box(vec3 p, vec3 s) { p = abs(p) - s; return max(p.x, max(p.y, p.z)); }
float cyl(vec3 p, float s, float h) { return max(length(p.xz) - s, abs(p.y) - h); }
float smin(float a, float b, float h) { float k = clamp((a - b) / h * .5 + .5, 0., 1.); return mix(a, b, k) - k * (1. - k)*h; }

#define rep(a,s) (fract((a)/s+.5)-.5)*s
#define repa(a,s) abs(fract((a)/s+.5)-.5)*s
#define repc(a,s) (smoothstep(0,1,abs(frac//(a)/s+.5)-.5)*2)*s*0.5)
#define c0(a) clamp(a,0.,1.)

mat2 rot(float a) { float ca = cos(a), sa = sin(a); return mat2(ca, sa, -sa, ca); }

float noi2(vec2 p) {
    vec2 ip = floor(p);
    p = smoothstep(0.0, 1.0, fract(p));
    vec4 v = fract(sin(dot(ip, vec2(7, 137)) + vec4(0, 7, 137, 144))*8986.352);
    vec2 v2 = mix(v.xz, v.yw, p.x);
    return mix(v2.x, v2.y, p.y);
}

int scene = 0;
float mont = 0.;
float decal = 0.;
float anim = 0.;

// shift the world on x/y axis according to z axis value
vec3 tunnel(vec3 p) {
    vec3 o = vec3(0);
    if (scene == 1) {
        o.x += sin(p.z*0.05)*6.;
        o.x += sin(p.z*0.15)*3.;
        o.y += sin(p.z*0.07)*7.;
    }
    return o;
}

// morph the world into plane, cylinder or sphere
vec3 morph(vec3 p) {

    p += tunnel(p);
    vec3 rop = p; // plane

    // cylinder
    if (scene == 1) rop = vec3(atan(p.y, p.x)*5., length(p.xy) - 10., p.z*0.5);

    if (scene == 3) {
        // sphere
        rop = vec3(abs(atan(p.z, p.x))*10. - 5., (10. - length(p)), abs(atan(length(p.xz), p.y))*10. - 16.);
    }

    return rop;
}

// kaleidoscopic folding for the city
vec3 kalei(vec3 p, inout vec2 ad) {

    float s = 10. + fract(section*4.)*anim;
    for (int i = 0; i < 5; ++i) {
        p.xz *= rot(float(i) + 12.7);
        float freq = 100.0;
        ad.x += sign(p.x)*sign(p.z)*(float(i)*0.72 + .2);
        ad.y = min(ad.y, min(abs(p.x), abs(p.z)));
        p.xz = repa(p.xz, freq);
        p.xz -= s;
        s *= 0.5;
    }
    // change scene
    p.x += decal;
    return p;
}

float at = 0.;
float mat = 0.;
vec3 localp;
// give signed distance field at position p
float map2(vec3 p) {
    p = morph(p); // morph the world into plane, cylinder or sphere

    vec3 bp = p;
    vec2 uv = p.xz;

    vec2 off = sin(p.xz*vec2(0.2, 0.35)*10. + sin(p.y*10.));

    vec2 ad = vec2(0);
    p = kalei(p, ad); // kaleidoscopic folding for the city

    float d = box(p, vec3(0.4));

    at += 1. / (d + 0.1);

    float city = min(min(box(p - vec3(0, 0, 0.5), vec3(0.55)), box(p - vec3(0.5, 0, -0.3), vec3(0.2, 2, 0.2))), cyl(p - vec3(1.0, 0, 0), 0.3, 3.));

    d = min(d, city);
    localp = p;

    vec3 rp = p;
    rp.xz = rep(rp.xz, 0.5);
    rp.xz += off * 0.05;
    // fields
    float blob = box(p - vec3(3, 0, 0), vec3(1.6));
    d = min(d, min(max(smin(cyl(rp, 0.1, 9.), -5. - p.y, -5.), blob), max(abs(blob - 0.2) - 0.05, -.7 - p.y)));

    vec3 ap = abs(abs(p + vec3(-2, 1.2, 0)) - vec3(4, 0.6, 1.2));
    d = min(d, max(box(ap, vec3(.7)), box(0.7 - max(ap, ap.yzx), vec3(0.2)))); // grid


    // terrain/mountain
    d = smin(min(d, (noi2(uv*0.1)*5. + noi2(uv*0.2)*3. + pow(abs(noi2(uv)*0.2 + noi2(uv*2.)*0.1 + noi2(uv*4.)*0.05 - 0.5), 5.)*10. - 4. - bp.y + mont)*.9), -bp.y, 0.2); // Combine terrain

    mat = abs(d - city) < 0.01 ? 1. : 0.;

    d = max(d, 0.00001); // anti glitch? avoid some nan/inf values

    return d;
}

float rnd1(float t) {
    return fract(sin(t*478.556)*8522.412);
}

float rnd2(vec2 t) {
    return fract(dot(sin(t*741.145 + t.yx*422.988), vec2(7789.532)));
}

// give volumetric amount at position p
float atspeed = 1.;
float gridsize = 5.;
vec3 atmo(vec3 p) {
    vec3 bp = p;
    p = morph(p); // morph the world into plane, cylinder or sphere
    vec2 ad = vec2(0, 10);
    p = kalei(p, ad); // kaleidoscopic folding for the city

    vec3 col = vec3(0);

    vec3 back = vec3(0.5, 0.2, 1.0);
    back.xy *= rot(ad.x);
    back = abs(back);
    float fade = c0((p.y + 8.)*0.03);
    float ff = rnd2(floor(p.xz*gridsize));
    float pulse = 0.7;//fract(bpm*0.25);
    col += ff * back*5.0*smoothstep(0.1, 0.2, ad.y)*pow(smoothstep(0.9*pulse, 1.0*pulse, sin(rnd1(ad.x)*1.7*pi + bpm * pi*atspeed)), 10.)*fade*fade;

    return col;
}

vec3 sky(vec3 r) {
    return mix(vec3(0.2, 0.5, 1.0), vec3(2.0, 1.1, 0.6), smoothstep(0.2, 1., -r.z));
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 uv = vec2(fragCoord.x / iResolution.x, fragCoord.y / iResolution.y);
    uv -= 0.5;
    uv /= vec2(iResolution.y / iResolution.x, 1);

    section2 = int(section);

    float fov = .4;
    float shad = 1.;

    // change scenes according time
    if (section2 > 3 && section < 4.) anim = 30.;
    mont = (section2 > 1 && section2 < 10) ? 100. : 0.;
    if (section2 > 5 && section < 8.) { decal = 4. - floor((section - 5.)*4.)*.5; fov = rnd1(floor(bpm)) + .2; }
    if (section2 > 3 && section2 < 8) scene = 1;
    if (section2 > 9) { scene = 3; atspeed = 2.; }

    float fx = c0((2. - abs(section - 4.1))*4.) + c0((1. - abs(section - 10.))*4.); // night

    vec3 s = vec3(0, -2, -10); // distance cam
    vec3 light = normalize(-vec3(1, 2.5, 3)); // light height

    vec3 t = vec3(0.3, 0.5, 0);

    // camera setting by scenes
    float div = 1.;
    float camoff = section2 > 9 ? 0. : (rnd1(floor(section*div)));

    float mav = time * 0.5 + camoff * 50.;
    float piv = .2;
    float az = 0.7;
    if (scene == 0) {
        s.y = camoff * (section2 > 6 ? 12.5 : 14.);
    }
    if (scene == 1) {
        s.z = 6.;
        az = 2.3*sin(section*5.);
        mav = 10.;
    }
    if (scene == 3) {
        az = 0.2;
        s.z = 20.;
        t = vec3(0, 0, 10);
        mav = min((section - 11.55)*6., (section - 12.15)*3.);
        mav *= c0(-mav);
        piv *= c0(-mav);
        mav += 3.5;
    }
    float muvol = .2;
    if (section2 > 8 && section2 < 10) { muvol = .6; s.z = -30.; anim = 30.; az = 1.3; atspeed = 2.; gridsize = 1.; }

    float rand = rnd2(uv);
    float dither = 0.9 + 0.1*rand;

    s.yz *= rot(az);
    s.xz *= rot(mav);

    if (scene == 0) {
        s.z -= max(20. - time, 0.)*3.;
    }

    float avance = 0.;
    float tar = 10.;
    if (scene == 1) {
        avance = fract(section)*80. - 40.; // traveling
        tar *= float(1 - section2 % 2 * 2)*10.;//+(section>7.25?1:0)*7;
    }
    s.z -= avance;
    t.z -= avance + tar; // cam dir
    s -= tunnel(s);
    t -= tunnel(t);

    vec3 cz = normalize(t - s);
    vec3 cx = normalize(cross(cz, vec3(sin(time)*piv, 1, 0)));
    vec3 cy = cross(cz, cx);

    vec3 r = normalize(cx*uv.x + cy * uv.y + cz * fov);

    // main raymarching loop
    vec3 p = s;
    float dd = 0.;
    for (int i = 0; i < 100; ++i) {
        float d = map2(p)*0.7*dither;
        if (d < 0.001) { break; }
        if (dd > 100.) { dd = 100.; break; }
        p += r * d;
        dd += d;
    }

    float curmat = mat;
    vec3 lop = localp;

    vec2 off = vec2(0.01, 0);
    vec3 n = normalize(vec3(map2(p + off.xyy) - map2(p - off.xyy), map2(p + off.yxy) - map2(p - off.yxy), map2(p + off.yyx) - map2(p - off.yyx)) - .001);

    if (scene == 1) {
        light = normalize(vec3(-p.x, -p.y, 10.*sin(time)));
    }

    float ao = (c0(map2(p + n * 0.2) / 0.2)*.5 + .5) * c0(map2(p + n * 0.05) / 0.05);

    float deg = 1. - c0(dd / 100.);
    float fog = c0(deg*deg);

    // shadow raymarching (cheap)
    vec3 sp = p + n * 0.5 - r * 0.2;
    for (int i = 0; i < 10; ++i) {
        float d = map2(sp);
        shad = min(shad, d);
        sp += d * light*0.6*dither;
    }

    shad = c0((shad - .02)*3.);
    vec2 gg = abs(fract(lop.xy*5.) - 0.5);
    float windows = curmat * smoothstep(0.35, 0.3, max(gg.x, gg.y))*step(abs(n.y), .9)*(1. - fx);

    float spec = max(0., dot(n, normalize(light - r)));

    float fre = pow(1. - abs(dot(n, r)), 5.);
    vec3 col = (max(0., dot(n, light)) * fog*ao * shad * (vec3(2.0, 1.1, 0.6)
        + sky(r)*pow(spec, 5.) + curmat * 10.*pow(spec, 50.))
        * (1. - windows) + (1. - windows * .4)*(fre * ao*2. + 0.3)*fog*(-n.y*.5 + .5)*(curmat*0.7 + .2))
        *mix(vec3(1), vec3(0.2, 0.2, 0.7), fx) + sky(reflect(r, n))*windows*spec*fre *10.
        + atmo(p)*6.*fog*fx + sky(r) * exp((1. - fog)*3.)*0.2*pow(1. - fx, 10.) + vec3(0.2, 0.5, 1.0)
        * pow(at*0.002, 2.)*5.*pow(fx, 0.3);

    /*
    // maximized version
    // diffuse and spec
    col += max(0, dot(n,light)) * fog*ao * shad * (vec3(2.0,1.1,0.6)+sky(r)*pow(spec,5)+curmat*10*pow(spec,50)) * (1-windows);

    // fresnel
    float fre=pow(1-abs(dot(n,r)),5);
    col += (fre * ao*2+0.3)*fog*(-n.y*.5+.5)*(curmat*0.7+.2);

    // night color mood
    col*=mix(vec3(1),vec3(0.2,0.2,0.7),fx);

    // sky reflection inside the windows
    col += sky(reflect(r,n))*windows*spec*fre *10;

    // emissive from the ground during night
    col += atmo(p)*6*fog*fx;

    // sky fog and atmosphere
    col += sky(r) * exp((1-fog)*3)*0.2*pow(1-fx,10);
    col += vec3(0.2,0.5,1.0) * pow(at*0.002,2)*5*pow(fx,0.3);
    */

    // "volumetric" for night scenes
    vec3 rp = r * muvol;
    vec3 vp = s + rp * rand;
    for (int i = 0; i < 100; ++i) {
        if (length(vp - s) > dd) break;
        col += atmo(vp)*fx;
        vp += rp;
    }

    vec2 vig = abs(uv) - vec2(0.5, 0.2);
    col = c0(pow(1. - exp(-col * pow(0.5 - length(max(vec2(0), vig)), 0.5)*2.5), vec3(1.8)))*c0(time / 5.)*c0((12.25 - section)*10.);
    /*
    // maximised version
    // filmic lol
    col = 1-exp(-col*2.5);
    col = c0(pow(col,vec3(2.1)));

    // fade in/out
    col *= c0(time/5);
    col *= c0((11.3-section)*10);
    */

    fragColor = vec4(col, 1);
}
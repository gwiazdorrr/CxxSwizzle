/*--------------------------------------------------------------------------------------
License CC0 - http://creativecommons.org/publicdomain/zero/1.0/
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
----------------------------------------------------------------------------------------
^This means do anything you want with this code. Because we are programmers, not lawyers.

Sorry this code is pretty messy. I had to finish in time for the competition. :)

-Otavio Good
*/

const int MAX_MARCH_REPS = 100;
const float MARCH_DISTANCE_MULTIPLIER = 0.9;
const int MAX_SHADOW_MARCHES = 30;

float localTime = 0.0;
const float earthRad = 6371.0;
const vec3 earthPos = normalize(vec3(-6500,-6400,2400)) * (earthRad + 300.0);

// some noise functions
float Hash(float f)
{
    return fract(cos(f)*7561.0);
}
float Hash2d(vec2 uv)
{
    float f = uv.x + uv.y * 521.0;	// repeats after this value
    float rand = fract(cos(f)*104729.0);
    return rand;
}
vec2 Hash2(vec2 v)
{
    return fract(cos(v*3.333)*vec2(100003.9, 37049.7));
}
float Hash3d(vec3 uv)
{
    float f = uv.x + uv.y * 37.0 + uv.z * 521.0;
    return fract(sin(f)*110003.9);
}

float mixS(float f0, float f1, float a)
{
    if (a < 0.5) return f0;
    return f1;
}

float mixC(float f0, float f1, float a)
{
    return mix(f1, f0, cos(a*3.1415926) *0.5+0.5);
}

float mixP(float f0, float f1, float a)
{
    return mix(f0, f1, a*a*(3.0-2.0*a));
}
vec2 mixP2(vec2 v0, vec2 v1, float a)
{
    return mix(v0, v1, a*a*(3.0-2.0*a));
}

float mixSS(float f0, float f1, float a)
{
    return mix(f0, f1, smoothstep(0.0, 1.0, a));
}

const vec2 zeroOne = vec2(0.0, 1.0);
float noise2dVec(vec2 uv)
{
    vec2 fr = fract(uv);
    vec2 fl = floor(uv);
    vec2 h0 = vec2(Hash2d(fl), Hash2d(fl + zeroOne));
    vec2 h1 = vec2(Hash2d(fl + zeroOne.yx), Hash2d(fl + zeroOne.yy));
    vec2 xMix = mixP2(h0, h1, fr.x);
    return mixC(xMix.x, xMix.y, fr.y);
}
float noise2d(vec2 uv)
{
    vec2 fr = fract(uv);
    vec2 fl = floor(uv);
    float h00 = Hash2d(fl);
    float h10 = Hash2d(fl + zeroOne.yx);
    float h01 = Hash2d(fl + zeroOne);
    float h11 = Hash2d(fl + zeroOne.yy);
    return mixP(mixP(h00, h10, fr.x), mixP(h01, h11, fr.x), fr.y);
}
float noise(vec3 uv)
{
    vec3 fr = fract(uv.xyz);
    vec3 fl = floor(uv.xyz);
    float h000 = Hash3d(fl);
    float h100 = Hash3d(fl + zeroOne.yxx);
    float h010 = Hash3d(fl + zeroOne.xyx);
    float h110 = Hash3d(fl + zeroOne.yyx);
    float h001 = Hash3d(fl + zeroOne.xxy);
    float h101 = Hash3d(fl + zeroOne.yxy);
    float h011 = Hash3d(fl + zeroOne.xyy);
    float h111 = Hash3d(fl + zeroOne.yyy);
    return mixP(
        mixP(mixP(h000, h100, fr.x),
             mixP(h010, h110, fr.x), fr.y),
        mixP(mixP(h001, h101, fr.x),
             mixP(h011, h111, fr.x), fr.y)
        , fr.z);
}

float PI=3.14159265;

vec3 saturate(vec3 a) { return clamp(a, 0.0, 1.0); }
vec2 saturate(vec2 a) { return clamp(a, 0.0, 1.0); }
float saturate(float a) { return clamp(a, 0.0, 1.0); }

vec3 RotateX(vec3 v, float rad)
{
  float cs = cos(rad);
  float sn = sin(rad);
  //if (RIGHT_HANDED_COORD)
  return vec3(v.x, cs * v.y + sn * v.z, -sn * v.y + cs * v.z);
  //else return new float3(x, cos * y - sin * z, sin * y + cos * z);
}
vec3 RotateY(vec3 v, float rad)
{
  float cs = cos(rad);
  float sn = sin(rad);
  //if (RIGHT_HANDED_COORD)
  return vec3(cs * v.x - sn * v.z, v.y, sn * v.x + cs * v.z);
  //else return new float3(cos * x + sin * z, y, -sin * x + cos * z);
}
vec3 RotateZ(vec3 v, float rad)
{
  float cs = cos(rad);
  float sn = sin(rad);
  //if (RIGHT_HANDED_COORD)
  return vec3(cs * v.x + sn * v.y, -sn * v.x + cs * v.y, v.z);
}

vec3 GetStarColor(vec3 rayDir, vec3 sunDir)
{
    rayDir.xyz = rayDir.yzx;
	float dense = 16.0;
	vec3 localRay = normalize(rayDir);
	float sunIntensity = 1.0 - (dot(localRay, sunDir) * 0.5 + 0.5);
	//sunIntensity = (float)Math.Pow(sunIntensity, 14.0);
	sunIntensity = 0.2 / sunIntensity;
	sunIntensity = min(sunIntensity, 40000.0);
	sunIntensity = max(0.0, sunIntensity - 3.0);

	localRay.x = localRay.x + 1.0 - iTime * 0.1;
	//vec3 right = normalize(cross(sunDir, vec3(0.0,1.0,0.0)));
	//vec3 up = normalize(cross(sunDir, right));
	vec2 wrap = fract((localRay.xy)*dense);
	vec4 rand = texture(iChannel2, floor(localRay.xy*dense)/dense).xyzw;
	vec3 starColor = rand.xyz;
	starColor = starColor * 0.75 + 0.25;
	rand.xy = rand.xy * 2.0 - 1.0;
	vec2 center = vec2(0.5, 0.5) + rand.xy * 0.9;// floor(fract((localRay.xy)*8.0)) + 0.5;
	float star = length(wrap - center);
	float test = star;
	star = saturate((1.0 - star));
	float blink = texture(iChannel2, localRay.xy + iTime * 0.03).x;
	float cluster = 0.3;// /*(localRay.x+0.5) */ (localRay.y+0.5) * 2.8 + 0.8;
	star = pow(star, 60.0 + saturate(rand.z - 0.0) * 250.0 * cluster);
	star *= blink;
	
	float milkyMask = saturate(0.25 - abs(localRay.x - 0.65));
	vec3 milkyway = texture(iChannel3, (localRay.yx*1.5 )+vec2(0.65, 0.3)).yxz;
	vec3 milkyLOD = texture(iChannel3, (localRay.yx*1.5 )+vec2(0.65, 0.3), 3.0).yxz;
	vec3 milkyDetail = texture(iChannel3, (-localRay.yx*8.0 )+vec2(0.65, 0.3)).yxz;
	milkyway *= milkyDetail.xxx;
	milkyway *= vec3(1.0, 0.8, 0.91)*1.5;
	milkyway = pow(milkyway, vec3(2.0, 2.0, 2.0)*3.0);
	milkyway += vec3(0.2, 0.0015, 1.001) * milkyLOD * 0.006;
	
	vec3 finalColor = milkyway /* milkyMask*/ * 10.0;
	finalColor += /*environmentSphereColor + sunCol * sunIntensity +*/ starColor * star * 12.0 * cluster;
	return finalColor;
	//return environmentSphereColor + sunCol * sunIntensity + starColor * star * 12000.0 * cluster;
	//return vec3(1.0,1.0,1.0)*cluster*1000.0;
}

// This function basically is a procedural environment map that makes the sun
vec3 sunCol = vec3(258.0, 238.0, 210.0) / 3555.0;//unfortunately, i seem to have 2 different sun colors. :(
vec3 GetSunColorReflection(vec3 rayDir, vec3 sunDir)
{
	vec3 localRay = normalize(rayDir);
	float dist = 1.0 - (dot(localRay, sunDir) * 0.5 + 0.5);
	float sunIntensity = 0.015 / dist;
	sunIntensity = pow(sunIntensity, 0.93)*100.0;

    //sunIntensity += exp(-dist*12.0)*300.0;
	sunIntensity = min(sunIntensity, 40000.0);
    //vec3 skyColor = mix(vec3(1.0, 0.95, 0.85), vec3(0.2,0.3,0.95), pow(saturate(rayDir.y), 0.7))*skyMultiplier*0.95;
	return sunCol * sunIntensity*0.00125;
}
vec3 GetSunColorSmall(vec3 rayDir, vec3 sunDir)
{
	vec3 localRay = normalize(rayDir);
	float dist = 1.0 - (dot(localRay, sunDir) * 0.5 + 0.5);
	float sunIntensity = 0.05 / dist;
    sunIntensity += exp(-dist*12.0)*300.0;
	sunIntensity = min(sunIntensity, 40000.0);
	return sunCol * sunIntensity*0.025;
}

vec4 cXX = vec4(0.0, 3.0, 0.0, 0.0);

vec3 camPos = vec3(0.0), camFacing;
vec3 camLookat=vec3(0,0.0,0);

float cylCone(vec3 p)
{
    float d = length(p.xy) - 1.0;
    d = min(d, length(p.xy - 1.0) - 0.1);
    d = max(0.0, d + max(0.0, abs(p.z) - 1.9)*1.0);
    return d;
}

float sbox(vec3 p, vec3 size)
{
    vec3 a = abs(p) - size;
    float c = max(a.x, a.y);
    c = max(c, a.z);
    return (c);
}

float diamond(vec3 p, vec3 size)
{
    vec3 a = abs(p) - size;
    float c = (a.x*0.5 + a.y*5.0);
    c = (c + a.z*2.0);
    return (c*0.25);
}

float cylinderInner(vec3 p, float len)
{
    float d = length(p.xy) - clamp(abs(p.z), 0.5, 0.8);
    //d = min(d, sbox(abs(p) - vec3(1.0/1.414, 1.0/1.414, 0.0), vec3(0.1, 0.1, len)));
    d = max(d, abs(p.z) - len);
    // bars above and below hangar
    vec3 p2 = vec3(p.x, abs(p.y), abs(p.z));
    d = min(d, sbox(p2 - vec3(0.0, 0.3, 2.0), vec3(0.6, 0.05, 0.025)));
    // little rings around hangar area
    float d3 = length(p.xy) - 0.82;
    float d2 = max(d3, abs(-p.z + 1.9) - 0.03);
    d3 = max(d3, abs(p.z + 1.9) - 0.03);
    return min(d3, min(d, d2));
}

float cylinderV(vec3 p)
{
    float rad = 0.15;
    float len = 4.0;
    float d = length(p.xz) - rad;
    float sp = sin(p.y * 10.0);
    sp = pow(max(0.0, sp - 0.7), 0.4);
    float ridges = length(p.xz) - rad * abs(sp + 0.5);
    d = min(d, ridges);
    //d = min(d, length(p.xy - 1.0) - 0.1);
    //d = min(d, sbox(abs(p) - vec3(0.25, 0.0, 0.25), vec3(0.03, len, 0.01)));
    vec2 p2 = vec2(abs(p.x), p.z);
    d = min(d, length(p2 - vec2(0.25, 0.0)) - 0.02 );
    d = min(d, length(p2 - vec2(0.35, 0.0)) - 0.02 );
    float rep = 0.7;
    vec3 p3 = vec3(abs(p.x), mod(p.y, rep) - rep*0.5, p.z);
    d = min(d, sbox(p3, vec3(0.35, 0.01, 0.02)));
    d = max(d, abs(p.y) - len);
    return d;
}

float outerRadius = 4.0;
float cylinderOuter(vec3 p, float len)
{
    float radial = length(p.xy);
    //float dents = sin(radial * 32.0) * 0.05;
    //dents = pow(abs(dents), 3.0);
    float d = -(radial - outerRadius);
    d = max(d, (radial - (outerRadius + 0.6)));
    float bevel = abs(radial - (outerRadius + 0.2)) * 0.2;
    d = max(d, abs(p.z) - len + bevel);
    return d;
}

float wheel(vec3 p)
{
    vec3 p2 = p;
    if (abs(p2.x) > abs(p2.y)) p2.xy = p2.yx;
    float d = cylinderV(p2);
    d = min(d, cylinderOuter(p, 0.25));
    d = min(d, sbox(vec3(p2.x, abs(p2.y), p2.z) - vec3(0.0, outerRadius, 0.0), vec3(0.4, 0.2, 0.275)));
    return d;
}

// polynomial smooth min (k = 0.1);
float smin( float a, float b, float k )
{
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

float shipPosition;
float SpaceShip(vec3 p)
{
    float d = length(p) - 1.5 + cos(p.z*1.02) * 1.0;
    d *= 1.1;
    d = max(d, -sbox(p + vec3(0.0, -0.6, 0.0), vec3(1.0,0.7,1.5)) );
    d = smin(d, 0.7*diamond(p + vec3(0.0, 0.2, -0.5), vec3(0.1)),0.1);
    d = max(d, sbox(p, vec3(0.9,4.2,2.5)) );
    return d;
}

float ScaleShip(vec3 p, float s)
{
    return SpaceShip(p/s)*s;
}

float rotationSpeed = 0.2;
vec2 DistanceToObject(vec3 p0)
{
    float material = 0.0;
    float h = 0.0;
    vec3 pr = RotateZ(p0, iTime * rotationSpeed);
    float d = cylinderInner(pr, 2.0);
    //p2.x = max(abs(p.x), abs(p.y));
    //d = sbox(p, vec3(1.0, 1.0, 1.0));
    vec3 p2 = vec3(pr.x, pr.y, abs(pr.z)) - vec3(0.0, 0.0, 1.5);
    //p2.z = abs(p2.z);
    d = min(d, wheel(p2));
    d = max(d, -sbox(p2, vec3(0.6, 0.2, 0.7)));	// ship docking bay - mirrored
    d = min(d, ScaleShip(pr - vec3(0.0, 0.1, shipPosition), 0.4));
    material = h;
    float final = d;
    return vec2(final, material);
}

float distFromSphere;
float IntersectSphereAndRay(vec3 pos, float radius, vec3 posA, vec3 posB, out vec3 intersectA2, out vec3 intersectB2)
{
	// Use dot product along line to find closest point on line
	vec3 eyeVec2 = normalize(posB-posA);
	float dp = dot(eyeVec2, pos - posA);
	vec3 pointOnLine = eyeVec2 * dp + posA;
	// Clamp that point to line end points if outside
	//if ((dp - radius) < 0) pointOnLine = posA;
	//if ((dp + radius) > (posB-posA).Length()) pointOnLine = posB;
	// Distance formula from that point to sphere center, compare with radius.
	float distance = length(pointOnLine - pos);
	float ac = radius*radius - distance*distance;
	float rightLen = 0.0;
	if (ac >= 0.0) rightLen = sqrt(ac);
	intersectA2 = pointOnLine - eyeVec2 * rightLen;
	intersectB2 = pointOnLine + eyeVec2 * rightLen;
	distFromSphere = distance - radius;
	if (distance <= radius) return 1.0;
	return 0.0;
}

// dirVec MUST BE NORMALIZED FIRST!!!!
float SphereIntersect(vec3 pos, vec3 dirVecPLZNormalizeMeFirst, vec3 spherePos, float rad)
{
    vec3 radialVec = pos - spherePos;
    float b = dot(radialVec, dirVecPLZNormalizeMeFirst);
    float c = dot(radialVec, radialVec) - rad * rad;
    float h = b * b - c;
    if (h < 0.0) return -1.0;
    return -b - sqrt(h);
}

vec4 cubicTex(vec2 uv)
{
    uv *= iChannelResolution[2].xy;
    vec2 fr = fract(uv);
    vec2 smoothv = fr*fr*(3.0-2.0*fr);
    vec2 fl = floor(uv);
    uv = smoothv + fl;
    return textureLod(iChannel2, (uv + 0.5)/iChannelResolution[2].xy, 0.0);	// use constant here instead?
}

vec4 tex3d(vec3 pos, vec3 normal)
{
	// loook up texture, blended across xyz axis based on normal.
	vec4 texX = texture(iChannel2, pos.yz);
	vec4 texY = texture(iChannel2, pos.xz);
	vec4 texZ = texture(iChannel2, pos.xy);
	//vec4 texX = cubicTex(pos.yz);
	//vec4 texY = cubicTex(pos.xz);
	//vec4 texZ = cubicTex(pos.xy);
	vec4 tex = mix(texX, texZ, abs(normal.z));
	tex = mix(tex, texY, abs(normal.y));//.zxyw;
	return tex;
}

vec4 tex3d3(vec3 pos, vec3 normal)
{
	// loook up texture, blended across xyz axis based on normal.
	vec4 texX = texture(iChannel3, pos.yz);
	vec4 texY = texture(iChannel3, pos.xz);
	vec4 texZ = texture(iChannel3, pos.xy);
	vec4 tex = mix(texX, texZ, abs(normal.z));
	tex = mix(tex, texY, abs(normal.y));//.zxyw;
	return tex;
}

float GrayPanel(vec2 uv)
{
    float grayPanels = texture(iChannel1, uv.xy * 0.25).z;
    float grayPanels2 = texture(iChannel1, uv.xy * 0.333 + vec2(0.1, 0.3)).z;
    grayPanels += grayPanels2;
    float grayPanels3 = texture(iChannel1, uv.xy * 0.1777*vec2(1.0,5.0) + vec2(0.432, 0.765)).z;
    grayPanels += grayPanels3;
    return grayPanels;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    localTime = iTime - 0.0;
	// ---------------- First, set up the camera rays for ray marching ----------------
	vec2 uv = fragCoord.xy/iResolution.xy * 2.0 - 1.0;
    float zoom = 2.0;
    uv /= zoom;

	// Camera up vector.
	vec3 camUp=vec3(0,1,0); // vuv

	// Camera lookat.
	camLookat=vec3(0,0.0,0);	// vrp

    // debugging camera
    float mx=iMouse.x/iResolution.x*PI*2.0-0.7 + localTime*3.1415 * 0.0625;
	float my=0.0;//-iMouse.y/iResolution.y*10.0 - sin(localTime * 0.31)*0.5;//*PI/2.01;
	camPos += vec3(cos(my)*cos(mx),sin(my),cos(my)*sin(mx))*(12.2); 	// prp


    // add randomness to camera for depth-of-field look close up.
    //camPos += vec3(Hash2d(uv)*0.91, Hash2d(uv+37.0), Hash2d(uv+47.0))*0.01;

	// Camera setup.
	vec3 camVec=normalize(camLookat - camPos);//vpn
	vec3 sideNorm=normalize(cross(camUp, camVec));	// u
	vec3 upNorm=cross(camVec, sideNorm);//v
	vec3 worldFacing=(camPos + camVec);//vcv
	vec3 worldPix = worldFacing + uv.x * sideNorm * (iResolution.x/iResolution.y) + uv.y * upNorm;//scrCoord
	vec3 relVec = normalize(worldPix - camPos);//scp

	// ----------------------------------- Animate ------------------------------------
    float repTime = 32.0;	// repeat every x seconds
    float t1 = mod(localTime-5.0, repTime) / repTime;
    shipPosition = 9.5 - min(pow(t1, 0.7), 0.5) * 16.0;
	// --------------------------------------------------------------------------------
	// I put a bounding sphere around the whole object. If the ray is outside
	// of the bounding sphere, I don't bother ray marching. It's just an optimization.
	//vec3 iA, iB;
	//float hit = IntersectSphereAndRay(vec3(0,0,0), 7.6, camPos, camPos+relVec, iA, iB);

	// --------------------------------------------------------------------------------
	vec2 distAndMat = vec2(0.05, 0.0);
	float t = 7.0;
	//float inc = 0.02;
	float maxDepth = 22.0;
	vec3 pos = vec3(0,0,0);
    // start and end the camera ray at the sphere intersections.
    //camPos = 0.5;//iA;
    //maxDepth = distance(iA, iB);
	// ray marching time
	//if (hit > 0.5)	// check if inside bounding sphere before wasting time ray marching.
	{
        for (int i = 0; i < MAX_MARCH_REPS; i++)	// This is the count of the max times the ray actually marches.
        {
            if ((t > maxDepth) || (abs(distAndMat.x) < 0.0075)) break;
            pos = camPos + relVec * t;
            // *******************************************************
            // This is _the_ function that defines the "distance field".
            // It's really what makes the scene geometry.
            // *******************************************************
            distAndMat = DistanceToObject(pos);
            // adjust by constant because deformations mess up distance function.
            t += distAndMat.x * MARCH_DISTANCE_MULTIPLIER;
        }
    }
    /*else
    {
		t = maxDepth + 1.0;
        distAndMat.x = 1.0;
    }*/
	// --------------------------------------------------------------------------------
	// Now that we have done our ray marching, let's put some color on this geometry.

	vec3 sunDir = normalize(vec3(0.93, 0.82, -1.5));
	vec3 finalColor = vec3(0.0);
    float radial = length(pos.xy);

	// If a ray actually hit the object, let's light it.
	if (abs(distAndMat.x) < 0.75)
    //if (t <= maxDepth)
	{
        vec3 posRot = RotateZ(pos, iTime * rotationSpeed);

		float grayPanels = GrayPanel(posRot.xy);

        // calculate the normal from the distance field. The distance field is a volume, so if you
        // sample the current point and neighboring points, you can use the difference to get
        // the normal.
        vec3 smallVec = vec3(0.005, 0, 0);
        vec3 normalU = vec3(distAndMat.x - DistanceToObject(pos - smallVec.xyy).x,
                           distAndMat.x - DistanceToObject(pos - smallVec.yxy).x,
                           distAndMat.x - DistanceToObject(pos - smallVec.yyx).x);

        vec3 normal = normalize(normalU);
        
        // bump map inner stuff
        if (radial < outerRadius - 0.01)
        {
            normal += (texture(iChannel1, posRot.xy * 0.27).x - 0.4) * 0.0625;
            normal.y += (grayPanels - 1.5) * 0.1;
            normal = normalize(normal);
        }
        else
        {
            float ridge = sin(radial*16.0+0.3);
            ridge = pow(abs(ridge), 16.0);
            normal += ridge*normal.z;
            normal = normalize(normal);
        }


        // calculate 2 ambient occlusion values. One for global stuff and one
        // for local stuff - so the green sphere light source can also have ambient.
        float ambientS = 1.0;
        ambientS *= saturate(DistanceToObject(pos + normal * 0.1).x*10.0);
        ambientS *= saturate(DistanceToObject(pos + normal * 0.2).x*5.0);
        ambientS *= saturate(DistanceToObject(pos + normal * 0.4).x*2.5);
        ambientS *= saturate(DistanceToObject(pos + normal * 0.8).x*1.25);
        float ambient = ambientS * saturate(DistanceToObject(pos + normal * 1.6).x*1.25*0.5);
        ambient *= saturate(DistanceToObject(pos + normal * 3.2).x*1.25*0.25);
        //ambient *= saturate(DistanceToObject(pos + normal * 6.4).x*1.25*0.125);
        ambient = max(0.15, pow(ambient, 0.3));	// tone down ambient with a pow and min clamp it.
        ambient = saturate(ambient);

        // Trace a ray toward the sun for sun shadows
        float sunShadow = 1.0;
        float iter = 0.1;
		for (int i = 0; i < MAX_SHADOW_MARCHES; i++)
        {
            float tempDist = DistanceToObject(pos + sunDir * iter).x;
	        sunShadow *= saturate(tempDist*50.0);
            if (tempDist <= 0.0) break;
            //iter *= 1.5;	// constant is more reliable than distance-based
            iter += max(0.00, tempDist)*1.0;
            if (iter > 8.0) break;
        }
        sunShadow = saturate(sunShadow);

        // calculate the reflection vector for highlights
        vec3 ref = reflect(relVec, normal);

        // ------ Calculate texture color ------
        vec3 texColor;
        texColor = vec3(0.35);// * grayPanels;

        // texture outer ring
        if (radial >= outerRadius - 0.01)
        {
            vec2 uvr = vec2(atan(posRot.x, posRot.y) * 8.0, length(posRot.xy) * 2.0);
	        if ((radial >= outerRadius  + 0.3) && (radial < outerRadius + 0.5))
            {
                float checker = texture(iChannel1, uvr).x;
                float checkerL = texture(iChannel0, uvr*0.0625).x;
                checker = pow(checker, 19.0);
                checker *= saturate((checkerL - 0.7) * 94.0);
                //texColor += vec3(15.0, 4.0, 2.0) * 5.0 * saturate(checker);
                //texColor *= vec3(1.0) * saturate(checker);
            }
            float stripes = texture(iChannel1, uvr * vec2(0.0625, 1.0)).x;
            texColor -= stripes * 0.03;
        }
        else
        {
            texColor = vec3(0.3) + grayPanels * 0.03;
        }
        if (length(normal.xy) > 0.95)
        {
            vec2 uvr = vec2(atan(posRot.x, posRot.y) * 1.0, posRot.z * 0.25);
            //float checker = texture(iChannel1, uvr).x;
            float checker = GrayPanel(uvr * vec2(1.0, 4.0));
            texColor += vec3(0.05) * checker - vec3(0.025);
        }

        // ------ Calculate lighting color ------
        // Start with sun color, standard lighting equation, and shadow
        vec3 lightColor = sunCol * saturate(dot(sunDir, normal)) * sunShadow*24.0;
        // sky color, hemisphere light equation approximation, anbient occlusion
        lightColor += vec3(0.1,0.35,0.95) * (dot(normal, normalize(earthPos)) * 0.5 + 0.5) * ambient * 0.4;
        // ground color - another hemisphere light
        lightColor += vec3(1.0) * ((-normal.y) * 0.5 + 0.5) * ambient * 0.01;
        // Apply red glow to hangar
        vec3 lightPos = vec3(0.0, 0.0, 1.75);
        vec3 redGlow = vec3(1.0, 0.1, 0.2) * saturate(0.8 - length(pos - lightPos));
        redGlow *= max(0.0, dot(normalize(-lightPos-pos), normal));
        lightColor += redGlow * 43.0;
        // Apply red glow to hangar (again. too tired to do mirror math.)
        lightPos = vec3(0.0, 0.0, -1.75);
        redGlow = vec3(1.0, 0.1, 0.2) * saturate(0.8 - length(pos - lightPos));
        redGlow *= max(0.0, dot(normalize(-lightPos-pos), normal));
        lightColor += redGlow * 43.0;


        // finally, apply the light to the texture.
        finalColor = texColor * lightColor;

        // glowing windows
        if (radial >= outerRadius - 0.01)
        {
            vec2 uvr = vec2(atan(posRot.x, posRot.y) * 8.0, length(posRot.xy) * 2.0);
	        if ((radial >= outerRadius  + 0.3) && (radial < outerRadius + 0.5))
            {
                float checker = texture(iChannel1, uvr).x;
                float checkerL = texture(iChannel0, uvr*0.0625).x;
                checker = pow(checker, 19.0);
                checker *= saturate((checkerL - 0.7) * 94.0);
                finalColor += vec3(10.0, 6.0, 2.0) * 1.0 * saturate(checker);
                //texColor *= vec3(1.0) * saturate(checker);
            }
        }

        // specular highlights - just a little
        vec3 refColor = GetSunColorReflection(ref, sunDir)*0.68;
        finalColor += refColor * sunCol * sunShadow * 9.0 * texColor.g;

        // fog that fades to sun color so that fog is brightest towards sun
    //    finalColor = mix(vec3(0.98, 0.981, 0.981) + min(vec3(0.25),GetSunColorSmall(relVec, sunDir))*2.0, finalColor, exp(-t*0.007));
        //finalColor = vec3(1.0, 21.0, 26.0) * saturate(length(normalU)-0.01);
        //finalColor = vec3(40.0) * length(normalU);
           // finalColor = normal * 0.5 + 0.5;
	}
    else
    {
        //vec3 iA, iB;
        //float hit = IntersectSphereAndRay(earthPos, earthRad, camPos, camPos+relVec, iA, iB);
        float hit = SphereIntersect(camPos, normalize(relVec), earthPos, earthRad);
        if (hit >= 0.0)
        {
			vec3 intersection = camPos + relVec * hit;
            vec3 normal = normalize(intersection - earthPos);
            finalColor = texture(iChannel2, vec2(normal.z, normal.y)).xyz;
            vec3 surface = vec3(0.01, 0.01, 0.1);
            surface += tex3d(normal * 0.75, normal).yyy;
            float d = dot(normal, normalize(camPos - intersection));

            float atmosphere = 1.0 - d;
            atmosphere = pow(atmosphere, 3.5);
            atmosphere = atmosphere * 0.9 + 0.1;
            surface = mix(surface, vec3(70.0, 130.0, 240.0)/355.0, atmosphere);
			finalColor = surface;
            //finalColor = vec3(1.0) * d;
            //finalColor = normal * 0.5 + 0.5;
        }
        else
        {
			//finalColor += GetStarColor(relVec, sunDir);
            float stars = tex3d3(relVec*1.0, relVec).z;
            stars = pow(stars,63.0);//63
            finalColor += vec3(2.0) * stars;
        }
        // Our ray trace hit nothing, so draw sky.
        // fade the sky color, multiply sunset dimming
        //finalColor *=0.0;// mix(vec3(1.0, 0.95, 0.85), vec3(0.2,0.5,0.95), pow(saturate(relVec.y), 0.7))*0.95;
        // add the sun
    }
    finalColor += GetSunColorReflection(relVec, sunDir);// + vec3(0.1, 0.1, 0.1);

    // vignette?
    finalColor *= vec3(1.0) * saturate(1.0 - length(uv/2.5));
    finalColor *= 1.95;

	// output the final color with sqrt for "gamma correction"
	fragColor = vec4(sqrt(clamp(finalColor, 0.0, 1.0)),1.0);
}



const float MATH_PI = float(3.14159);

const float MENU_SURFACE = 0.;
const float MENU_METAL = 1.;
const float MENU_DIELECTRIC = 2.;
const float MENU_ROUGHNESS = 3.;
const float MENU_BASE_COLOR = 4.;
const float MENU_LIGHTING = 5.;
const float MENU_DIFFUSE = 6.;
const float MENU_SPECULAR = 7.;
const float MENU_DISTR = 8.;
const float MENU_FRESNEL = 9.;
const float MENU_GEOMETRY = 10.;

CXXSWIZZLE_ARRAY(vec3, BASE_COLORS,
	vec3(0.74),
	vec3(0.51, 0.72, 0.81),
	vec3(0.66, .85, .42),
	vec3(0.87, 0.53, 0.66),
	vec3(0.51, 0.46, 0.74),
	vec3(0.78, 0.71, 0.45)
	);

struct AppState
{
	float	menuId;
	float	metal;
	float   roughness;
	float   baseColor;
	float   focus;
	float   focusObjRot;
	float   objRot;
};

vec4 LoadValue(int x, int y)
{
	return texelFetch(iChannel0, ivec2(x, y), 0);
}

void LoadState(out AppState s)
{
	vec4 data;

	data = LoadValue(0, 0);
	s.menuId = data.x;
	s.metal = data.y;
	s.roughness = data.z;
	s.baseColor = data.w;

	data = LoadValue(1, 0);
	s.focus = data.x;
	s.focusObjRot = data.y;
	s.objRot = data.z;
}

void StoreValue(vec2 re, vec4 va, inout vec4 fragColor, vec2 fragCoord)
{
	fragCoord = floor(fragCoord);
	fragColor = (fragCoord.x == re.x && fragCoord.y == re.y) ? va : fragColor;
}

float saturate(float x)
{
	return clamp(x, 0., 1.);
}

vec3 saturate(vec3 x)
{
	return clamp(x, vec3(0.), vec3(1.));
}

float Smooth(float x)
{
	return smoothstep(0., 1., saturate(x));
}

void Repeat(inout float p, float w)
{
	p = mod(p, w) - 0.5f * w;
}

float Circle(vec2 p, float r)
{
	return (length(p / r) - 1.) * r;
}

float Rectangle(vec2 p, vec2 b)
{
	vec2 d = abs(p) - b;
	return min(max(d.x, d.y), 0.) + length(max(d, 0.));
}

void Rotate(inout vec2 p, float a)
{
	p = cos(a) * p + sin(a) * vec2(p.y, -p.x);
}

float Capsule(vec2 p, float r, float c)
{
	return mix(length(p.x) - r, length(vec2(p.x, abs(p.y) - c)) - r, step(c, abs(p.y)));
}

float Arrow(vec2 p, float a, float l, float w)
{
	Rotate(p, a);
	p.y += l;

	float body = Capsule(p, w, l);
	p.y -= w;

	float tip = p.y + l;

	p.y += l + w;
	Rotate(p, +2.);
	tip = max(tip, p.y - 2. * w);
	Rotate(p, -4.);
	tip = max(tip, p.y - 2. * w);

	return min(body, tip);
}

float TextSDF(vec2 p, float glyph)
{
	p = abs(p.x - .5) > .5 || abs(p.y - .5) > .5 ? vec2(0.) : p;
	return 2. * (texture(iChannel3, p / 16. + fract(vec2(glyph, 15. - floor(glyph / 16.)) / 16.)).w - 127. / 255.);
}

void Diagram(inout vec3 color, vec2 p, in AppState s)
{
	vec3 surfColor = vec3(0.9, 0.84, 0.8);
	vec3 lightColor = vec3(0.88, 0.65, 0.2);
	vec3 baseColor = BASE_COLORS[int(s.baseColor)];
	vec3 diffuseColor = s.metal == 1. ? vec3(0.) : baseColor;
	vec3 specularColor = s.metal == 1. ? baseColor : vec3(0.7);

	p -= vec2(84., 44.);

	vec2 t = p - vec2(18., 4.);
	float r = Rectangle(t, vec2(52., 12.));
	color = mix(color, surfColor, Smooth(-r * 2.));

	t.y += s.roughness * sin(t.x);
	r = Rectangle(t - vec2(0., 11.), vec2(52., 1.2));
	color = mix(color, surfColor * 0.6, Smooth(-r * 2.));


	// refraction
	r = 1e4;
	t = p - vec2(18., 15.);
	for (int i = 0; i < 3; ++i)
	{
		r = min(r, Arrow(t - vec2(-15. + float(i) * 15., 0.), -0.4, 7., .7));
	}
	r = min(r, Arrow(t - vec2(9., -15.), 2., 4., .7));
	r = min(r, Arrow(t - vec2(17., -10.), 3.8, 18., .7));
	r = min(r, Arrow(t - vec2(-6., -14.), 0.9, 3., .7));
	r = min(r, Arrow(t - vec2(1., -19.), 2.9, 18., .7));
	r = min(r, Arrow(t - vec2(-22., -15.), 4.5, 2., .7));
	r = min(r, Arrow(t - vec2(-28., -14.), 2.6, 14., .7));
	if (s.metal != 1. && s.menuId < MENU_SPECULAR)
	{
		color = mix(color, diffuseColor, Smooth(-r * 2.));
	}

	// reflection
	r = 1e4;
	t = p - vec2(18., 15.);
	for (int i = 0; i < 3; ++i)
	{
		float off = s.roughness * (1.5 - float(i)) * .45;
		r = min(r, Arrow(t - vec2(-15. + float(i) * 15., 2.), -0.5 * MATH_PI - 0.9 + off, 12., 1.));
	}
	if (s.menuId != MENU_DIFFUSE)
	{
		color = mix(color, specularColor, Smooth(-r * 2.));
	}

	// light in
	r = 1e4;
	t = p - vec2(18., 15.);
	for (int i = 0; i < 3; ++i)
	{
		r = min(r, Arrow(t - vec2(12. + float(i) * 15., 22.), -0.9, 15., 1.));
	}
	color = mix(color, lightColor, Smooth(-r * 2.));
}

float RaySphere(vec3 rayOrigin, vec3 rayDir, vec3 spherePos, float sphereRadius)
{
	vec3 oc = rayOrigin - spherePos;

	float b = dot(oc, rayDir);
	float c = dot(oc, oc) - sphereRadius * sphereRadius;
	float h = b * b - c;

	float t;
	if (h < 0.0)
	{
		t = -1.0;
	}
	else
	{
		t = (-b - sqrt(h));
	}
	return t;
}

float VisibilityTerm(float roughness, float ndotv, float ndotl)
{
	float r2 = roughness * roughness;
	float gv = ndotl * sqrt(ndotv * (ndotv - ndotv * r2) + r2);
	float gl = ndotv * sqrt(ndotl * (ndotl - ndotl * r2) + r2);
	return 0.5 / max(gv + gl, 0.00001);
}

float DistributionTerm(float roughness, float ndoth)
{
	float r2 = roughness * roughness;
	float d = (ndoth * r2 - ndoth) * ndoth + 1.0;
	return r2 / (d * d * MATH_PI);
}

vec3 FresnelTerm(vec3 specularColor, float vdoth)
{
	vec3 fresnel = specularColor + (1. - specularColor) * pow((1. - vdoth), 5.);
	return fresnel;
}

float Cylinder(vec3 p, float r, float height)
{
	float d = length(p.xz) - r;
	d = max(d, abs(p.y) - height);
	return d;
}

float Substract(float a, float b)
{
	return max(a, -b);
}

float SubstractRound(float a, float b, float r)
{
	vec2 u = max(vec2(r + a, r - b), vec2(0.0, 0.0));
	return min(-r, max(a, -b)) + length(u);
}

float Union(float a, float b)
{
	return min(a, b);
}

float Box(vec3 p, vec3 b)
{
	vec3 d = abs(p) - b;
	return min(max(d.x, max(d.y, d.z)), 0.0) + length(max(d, 0.0));
}

float Sphere(vec3 p, float s)
{
	return length(p) - s;
}

float Torus(vec3 p, float sr, float lr)
{
	return length(vec2(length(p.xz) - lr, p.y)) - sr;
}

float Disc(vec3 p, float r, float t)
{
	float l = length(p.xz) - r;
	return l < 0. ? abs(p.y) - t : length(vec2(p.y, l)) - t;
}

float UnionRound(float a, float b, float k)
{
	float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
	return mix(b, a, h) - k * h * (1.0 - h);
}

float Scene(vec3 p, mat3 localToWorld)
{
	p = p * localToWorld;

	// ring
	vec3 t = p;
	t.y -= -.7;
	float r = Substract(Disc(t, 0.9, .1), Cylinder(t, .7, 2.));
	vec3 t2 = t - vec3(0., 0., 1.0);
	Rotate(t2.xz, 0.25 * MATH_PI);
	r = Substract(r, Box(t2, vec3(.5)));
	r = Union(r, Disc(t + vec3(0., 0.05, 0.), 0.85, .05));

	t = p;
	Rotate(t.yz, -.3);

	// body
	float b = Sphere(t, .8);
	b = Substract(b, Sphere(t - vec3(0., 0., .5), .5));
	b = Substract(b, Sphere(t - vec3(0., 0., -.7), .3));
	b = Substract(b, Box(t, vec3(2., .03, 2.)));
	b = Union(b, Sphere(t, .7));

	float ret = Union(r, b);
	return ret;
}

float CastRay(in vec3 ro, in vec3 rd, mat3 localToWorld)
{
	const float maxd = 5.0;

	float h = 0.5;
	float t = 0.0;

	for (int i = 0; i < 50; ++i)
	{
		if (h < 0.001 || t > maxd)
		{
			break;
		}

		h = Scene(ro + rd * t, localToWorld);
		t += h;
	}

	if (t > maxd)
	{
		t = -1.0;
	}

	return t;
}

vec3 SceneNormal(in vec3 pos, mat3 localToWorld)
{
	vec3 eps = vec3(0.001, 0.0, 0.0);
	vec3 nor = vec3(
		Scene(pos + eps.xyy, localToWorld) - Scene(pos - eps.xyy, localToWorld),
		Scene(pos + eps.yxy, localToWorld) - Scene(pos - eps.yxy, localToWorld),
		Scene(pos + eps.yyx, localToWorld) - Scene(pos - eps.yyx, localToWorld));
	return normalize(nor);
}

float SceneAO(vec3 p, vec3 n, mat3 localToWorld)
{
	float ao = 0.0;
	float s = 1.0;
	for (int i = 0; i < 6; ++i)
	{
		float off = 0.001 + 0.2 * float(i) / 5.;
		float t = Scene(n * off + p, localToWorld);
		ao += (off - t) * s;
		s *= 0.4;
	}

	return Smooth(1.0 - 12.0 * ao);
}

// St. Peter's Basilica SH
// https://www.shadertoy.com/view/lt2GRD
struct SHCoefficients
{
	vec3 l00, l1m1, l10, l11, l2m2, l2m1, l20, l21, l22;
};

const SHCoefficients SH_STPETER = SHCoefficients(
	vec3(0.3623915, 0.2624130, 0.2326261),
	vec3(0.1759131, 0.1436266, 0.1260569),
	vec3(-0.0247311, -0.0101254, -0.0010745),
	vec3(0.0346500, 0.0223184, 0.0101350),
	vec3(0.0198140, 0.0144073, 0.0043987),
	vec3(-0.0469596, -0.0254485, -0.0117786),
	vec3(-0.0898667, -0.0760911, -0.0740964),
	vec3(0.0050194, 0.0038841, 0.0001374),
	vec3(-0.0818750, -0.0321501, 0.0033399)
);

vec3 SHIrradiance(vec3 nrm)
{
	const SHCoefficients c = SH_STPETER;
	const float c1 = 0.429043;
	const float c2 = 0.511664;
	const float c3 = 0.743125;
	const float c4 = 0.886227;
	const float c5 = 0.247708;
	return (
		c1 * c.l22 * (nrm.x * nrm.x - nrm.y * nrm.y) +
		c3 * c.l20 * nrm.z * nrm.z +
		c4 * c.l00 -
		c5 * c.l20 +
		2.0 * c1 * c.l2m2 * nrm.x * nrm.y +
		2.0 * c1 * c.l21  * nrm.x * nrm.z +
		2.0 * c1 * c.l2m1 * nrm.y * nrm.z +
		2.0 * c2 * c.l11  * nrm.x +
		2.0 * c2 * c.l1m1 * nrm.y +
		2.0 * c2 * c.l10  * nrm.z
		);
}

// https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile
vec3 EnvBRDFApprox(vec3 specularColor, float roughness, float ndotv)
{
	const vec4 c0 = vec4(-1, -0.0275, -0.572, 0.022);
	const vec4 c1 = vec4(1, 0.0425, 1.04, -0.04);
	vec4 r = roughness * c0 + c1;
	float a004 = min(r.x * r.x, exp2(-9.28 * ndotv)) * r.x + r.y;
	vec2 AB = vec2(-1.04, 1.04) * a004 + r.zw;
	return specularColor * AB.x + AB.y;
}

vec3 EnvRemap(vec3 c)
{
	return pow(2. * c, vec3(2.2));
}

void DrawScene(inout vec3 color, vec2 p, in AppState s)
{
	vec3 lightColor = vec3(2.);
	vec3 lightDir = normalize(vec3(.7, .9, -.2));

	vec3 baseColor = pow(BASE_COLORS[int(s.baseColor)], vec3(2.2));
	vec3 diffuseColor = s.metal == 1. ? vec3(0.) : baseColor;
	vec3 specularColor = s.metal == 1. ? baseColor : vec3(0.02);
	float roughnessE = s.roughness * s.roughness;
	float roughnessL = max(.01, roughnessE);

	float a = -iTime * .5;
	mat3 rot = mat3(
		vec3(cos(a), 0., -sin(a)),
		vec3(0., 1., 0.),
		vec3(sin(a), 0., cos(a))
	);

	p -= vec2(-20., 10.);
	p *= .011;

	float yaw = 2.7 - s.objRot;
	mat3 rotZ = mat3(
		vec3(cos(yaw), 0.0, -sin(yaw)),
		vec3(0.0, 1.0, 0.0),
		vec3(sin(yaw), 0.0, cos(yaw))
	);

	float phi = -0.1;
	mat3 rotY = mat3(
		vec3(1.0, 0.0, 0.0),
		vec3(0.0, cos(phi), sin(phi)),
		vec3(0.0, -sin(phi), cos(phi))
	);

	mat3 localToWorld = rotY * rotZ;

	vec3 rayOrigin = vec3(0.0, .5, -3.5);
	vec3 rayDir = normalize(vec3(p.x, p.y, 2.0));
	float t = CastRay(rayOrigin, rayDir, localToWorld);
	if (t > 0.0)
	{
		vec3 pos = rayOrigin + t * rayDir;
		vec3 normal = SceneNormal(pos, localToWorld);
		vec3 viewDir = -rayDir;
		vec3 refl = reflect(rayDir, normal);

		vec3 diffuse = vec3(0.);
		vec3 specular = vec3(0.);

		vec3 halfVec = normalize(viewDir + lightDir);
		float vdoth = saturate(dot(viewDir, halfVec));
		float ndoth = saturate(dot(normal, halfVec));
		float ndotv = saturate(dot(normal, viewDir));
		float ndotl = saturate(dot(normal, lightDir));
		vec3 envSpecularColor = EnvBRDFApprox(specularColor, roughnessE, ndotv);

		vec3 env1 = EnvRemap(texture(iChannel2, refl).xyz);
		vec3 env2 = EnvRemap(texture(iChannel1, refl).xyz);
		vec3 env3 = EnvRemap(SHIrradiance(refl));
		vec3 env = mix(env1, env2, saturate(roughnessE * 4.));
		env = mix(env, env3, saturate((roughnessE - 0.25) / 0.75));

		diffuse += diffuseColor * EnvRemap(SHIrradiance(normal));
		specular += envSpecularColor * env;

		diffuse += diffuseColor * lightColor * saturate(dot(normal, lightDir));

		vec3 lightF = FresnelTerm(specularColor, vdoth);
		float lightD = DistributionTerm(roughnessL, ndoth);
		float lightV = VisibilityTerm(roughnessL, ndotv, ndotl);
		specular += lightColor * lightF * (lightD * lightV * MATH_PI * ndotl);

		float ao = SceneAO(pos, normal, localToWorld);
		diffuse *= ao;
		specular *= saturate(pow(ndotv + ao, roughnessE) - 1. + ao);

		color = diffuse + specular;
		if (s.menuId == MENU_DIFFUSE)
		{
			color = diffuse;
		}
		if (s.menuId == MENU_SPECULAR)
		{
			color = specular;
		}
		if (s.menuId == MENU_DISTR)
		{
			color = vec3(lightD);
		}
		if (s.menuId == MENU_FRESNEL)
		{
			color = envSpecularColor;
		}
		if (s.menuId == MENU_GEOMETRY)
		{
			color = vec3(lightV) * (4.0f * ndotv * ndotl);
		}
		color = pow(color * .4, vec3(1. / 2.2));
	}
	else
	{
		// shadow
		float planeT = -(rayOrigin.y + 1.2) / rayDir.y;
		if (planeT > 0.0)
		{
			vec3 p = rayOrigin + planeT * rayDir;

			float radius = .7;
			color *= 0.7 + 0.3 * smoothstep(0.0, 1.0, saturate(length(p + vec3(0.0, 1.0, -0.5)) - radius));
		}
	}
}

void InfoText(inout vec3 color, vec2 p, in AppState s)
{
	p -= vec2(52, 12);
	vec2 q = p;
	if (s.menuId == MENU_METAL || s.menuId == MENU_BASE_COLOR || s.menuId == MENU_DISTR)
	{
		p.y -= 6.;
	}
	if (s.menuId == MENU_DIELECTRIC || s.menuId == MENU_FRESNEL)
	{
		p.y += 6.;
	}
	if (s.menuId == MENU_SPECULAR)
	{
		p.y += 6. * 6.;

		if (p.x < 21. && p.y >= 27. && p.y < 30.)
		{
			p.y = 0.;
		}
		else if (s.menuId == MENU_SPECULAR && p.y > 20. && p.y < 28. && p.x < 21.)
		{
			p.y += 3.;
		}
	}

	vec2 scale = vec2(3., 6.);
	vec2 t = floor(p / scale);

	uint v = 0u;
	if (s.menuId == MENU_SURFACE)
	{
		v = t.y == 2. ? (t.x < 4. ? 1702127169u : (t.x < 8. ? 1768431730u : (t.x < 12. ? 1852404852u : (t.x < 16. ? 1752440935u : (t.x < 20. ? 1970479205u : (t.x < 24. ? 1667327602u : (t.x < 28. ? 1768693861u : 7628903u))))))) : v;
		v = t.y == 1. ? (t.x < 4. ? 1937334642u : (t.x < 8. ? 1717924384u : (t.x < 12. ? 1952671084u : (t.x < 16. ? 1684955424u : (t.x < 20. ? 1717924384u : (t.x < 24. ? 1952670066u : (t.x < 28. ? 32u : 0u))))))) : v;
		v = t.y == 0. ? (t.x < 4. ? 1868784481u : (t.x < 8. ? 1852400754u : (t.x < 12. ? 1869881447u : (t.x < 16. ? 1701729056u : (t.x < 20. ? 1931963500u : (t.x < 24. ? 2002873376u : 0u)))))) : v;
		v = t.x >= 0. && t.x < 32. ? v : 0u;
	}
	if (s.menuId == MENU_METAL)
	{
		v = t.y == 1. ? (t.x < 4. ? 1635018061u : (t.x < 8. ? 1852776556u : (t.x < 12. ? 1914730860u : (t.x < 16. ? 1701602917u : (t.x < 20. ? 544437347u : (t.x < 24. ? 1751607660u : (t.x < 28. ? 1914729332u : (t.x < 32. ? 544438625u : 45u)))))))) : v;
		v = t.y == 0. ? (t.x < 4. ? 544432488u : (t.x < 8. ? 2037149295u : (t.x < 12. ? 1701868320u : (t.x < 16. ? 1634497891u : (t.x < 20. ? 114u : 0u))))) : v;
		v = t.x >= 0. && t.x < 36. ? v : 0u;
	}
	if (s.menuId == MENU_DIELECTRIC)
	{
		v = t.y == 3. ? (t.x < 4. ? 1818585412u : (t.x < 8. ? 1920230245u : (t.x < 12. ? 1914725225u : (t.x < 16. ? 1701602917u : (t.x < 20. ? 544437347u : (t.x < 24. ? 1701868328u : (t.x < 28. ? 1634497891u : (t.x < 32. ? 2107762u : 0u)))))))) : v;
		v = t.y == 2. ? (t.x < 4. ? 543452769u : (t.x < 8. ? 1935832435u : (t.x < 12. ? 1634103925u : (t.x < 16. ? 1931502947u : (t.x < 20. ? 1953784163u : (t.x < 24. ? 544436837u : (t.x < 28. ? 1718182952u : (t.x < 32. ? 1702065510u : 41u)))))))) : v;
		v = t.y == 1. ? (t.x < 4. ? 1751607660u : (t.x < 8. ? 1634869364u : (t.x < 12. ? 539915129u : (t.x < 16. ? 1667592275u : (t.x < 20. ? 1918987381u : (t.x < 24. ? 544434464u : (t.x < 28. ? 1936617315u : (t.x < 32. ? 1953390964u : 0u)))))))) : v;
		v = t.y == 0. ? (t.x < 4. ? 808333438u : (t.x < 8. ? 774909234u : (t.x < 12. ? 13360u : 0u))) : v;
		v = t.x >= 0. && t.x < 36. ? v : 0u;
	}
	if (s.menuId == MENU_ROUGHNESS)
	{
		v = t.y == 2. ? (t.x < 4. ? 1735749458u : (t.x < 8. ? 544367976u : (t.x < 12. ? 1718777203u : (t.x < 16. ? 1936024417u : (t.x < 20. ? 1830825248u : (t.x < 24. ? 543519343u : (t.x < 28. ? 1952539507u : (t.x < 32. ? 1701995892u : 100u)))))))) : v;
		v = t.y == 1. ? (t.x < 4. ? 1818649970u : (t.x < 8. ? 1702126437u : (t.x < 12. ? 1768693860u : (t.x < 16. ? 544499815u : (t.x < 20. ? 1937334642u : (t.x < 24. ? 1851858988u : (t.x < 28. ? 1752440932u : (t.x < 32. ? 2126709u : 0u)))))))) : v;
		v = t.y == 0. ? (t.x < 4. ? 1920298082u : (t.x < 8. ? 1919248754u : (t.x < 12. ? 1717924384u : (t.x < 16. ? 1952671084u : (t.x < 20. ? 1936617321u : 0u))))) : v;
		v = t.x >= 0. && t.x < 36. ? v : 0u;
	}
	if (s.menuId == MENU_BASE_COLOR)
	{
		v = t.y == 1. ? (t.x < 4. ? 544370502u : (t.x < 8. ? 1635018093u : (t.x < 12. ? 1679848300u : (t.x < 16. ? 1852401253u : (t.x < 20. ? 1931506533u : (t.x < 24. ? 1969448304u : (t.x < 28. ? 544366956u : (t.x < 32. ? 1869377379u : 114u)))))))) : v;
		v = t.y == 0. ? (t.x < 4. ? 544370502u : (t.x < 8. ? 1818585444u : (t.x < 12. ? 1920230245u : (t.x < 16. ? 544433001u : (t.x < 20. ? 1768169517u : (t.x < 24. ? 1937073766u : (t.x < 28. ? 1868767333u : (t.x < 32. ? 7499628u : 0u)))))))) : v;
		v = t.x >= 0. && t.x < 36. ? v : 0u;
	}
	if (s.menuId == MENU_LIGHTING)
	{
		v = t.y == 2. ? (t.x < 4. ? 1751607628u : (t.x < 8. ? 1735289204u : (t.x < 12. ? 544434464u : (t.x < 16. ? 1869770849u : (t.x < 20. ? 1634560376u : (t.x < 24. ? 543450484u : 2128226u)))))) : v;
		v = t.y == 1. ? (t.x < 4. ? 1634755955u : (t.x < 8. ? 1769234802u : (t.x < 12. ? 1679845230u : (t.x < 16. ? 1969645161u : (t.x < 20. ? 1629513075u : (t.x < 24. ? 2122862u : 0u)))))) : v;
		v = t.y == 0. ? (t.x < 4. ? 1667592307u : (t.x < 8. ? 1918987381u : (t.x < 12. ? 1836016416u : (t.x < 16. ? 1701736304u : (t.x < 20. ? 544437358u : 0u))))) : v;
		v = t.x >= 0. && t.x < 28. ? v : 0u;
	}
	if (s.menuId == MENU_DIFFUSE)
	{
		v = t.y == 2. ? (t.x < 4. ? 1818324307u : (t.x < 8. ? 1668489324u : (t.x < 12. ? 543517793u : (t.x < 16. ? 1935832435u : (t.x < 20. ? 1634103925u : (t.x < 24. ? 1931502947u : (t.x < 28. ? 1953784163u : (t.x < 32. ? 1852404325u : 8295u)))))))) : v;
		v = t.y == 1. ? (t.x < 4. ? 1635087189u : (t.x < 8. ? 981036140u : (t.x < 12. ? 1835093024u : (t.x < 16. ? 1953654114u : (t.x < 20. ? 1146241568u : (t.x < 24. ? 1713388102u : (t.x < 28. ? 824196384u : (t.x < 32. ? 543780911u : 0u)))))))) : v;
		v = t.y == 0. ? (t.x < 4. ? 1702257960u : (t.x < 8. ? 1914730866u : (t.x < 12. ? 1696627041u : (t.x < 16. ? 1937009016u : (t.x < 20. ? 544106784u : (t.x < 24. ? 1634869345u : (t.x < 28. ? 1679844462u : (t.x < 32. ? 2716265u : 0u)))))))) : v;
		v = t.x >= 0. && t.x < 36. ? v : 0u;
	}
	if (s.menuId == MENU_SPECULAR)
	{
		v = t.y == 8. ? (t.x < 4. ? 1818649938u : (t.x < 8. ? 1702126437u : (t.x < 12. ? 1768693860u : (t.x < 16. ? 779380839u : (t.x < 20. ? 1970492704u : (t.x < 24. ? 2037148769u : 8250u)))))) : v;
		v = t.y == 7. ? (t.x < 4. ? 1802465091u : (t.x < 8. ? 1919898669u : (t.x < 12. ? 1668178290u : (t.x < 16. ? 1998597221u : (t.x < 20. ? 1751345512u : (t.x < 24. ? 1685024032u : 7564389u)))))) : v;
		v = t.y == 6. ? (t.x < 4. ? 1919117677u : (t.x < 8. ? 1667327599u : (t.x < 12. ? 544437349u : (t.x < 16. ? 1919250472u : (t.x < 20. ? 1952671078u : (t.x < 24. ? 1919511840u : 544370546u)))))) : v;
		v = t.y == 5. ? (t.x < 4. ? 1734960488u : (t.x < 8. ? 1634563176u : (t.x < 12. ? 3811696u : 0u))) : v;
		v = t.y == 4. ? (t.x < 4. ? 745285734u : (t.x < 8. ? 1178413430u : (t.x < 12. ? 1747744296u : (t.x < 16. ? 1814578985u : (t.x < 20. ? 1747744300u : (t.x < 24. ? 1747469353u : 41u)))))) : v;
		v = t.y == 3. ? (t.x < 4. ? 538976288u : (t.x < 8. ? 538976288u : (t.x < 12. ? 1848128544u : (t.x < 16. ? 673803447u : (t.x < 20. ? 695646062u : 0u))))) : v;
		v = t.y == 2. ? (t.x < 4. ? 539828294u : (t.x < 8. ? 1936028230u : (t.x < 12. ? 7103854u : 0u))) : v;
		v = t.y == 1. ? (t.x < 4. ? 539828295u : (t.x < 8. ? 1836016967u : (t.x < 12. ? 2037544037u : 0u))) : v;
		v = t.y == 0. ? (t.x < 4. ? 539828292u : (t.x < 8. ? 1953720644u : (t.x < 12. ? 1969383794u : (t.x < 16. ? 1852795252u : 0u)))) : v;
		v = t.x >= 0. && t.x < 28. ? v : 0u;
	}
	if (s.menuId == MENU_DISTR)
	{
		v = t.y == 1. ? (t.x < 4. ? 1702109252u : (t.x < 8. ? 1679846770u : (t.x < 12. ? 1852401253u : (t.x < 16. ? 622883685u : (t.x < 20. ? 543584032u : (t.x < 24. ? 1919117677u : (t.x < 28. ? 1667327599u : 544437349u))))))) : v;
		v = t.y == 0. ? (t.x < 4. ? 1818649970u : (t.x < 8. ? 1769235301u : (t.x < 12. ? 1814062958u : (t.x < 16. ? 1952999273u : (t.x < 20. ? 1919903264u : (t.x < 24. ? 1730175264u : (t.x < 28. ? 1852143209u : 1919509536u))))))) : v;
		v = t.x >= 0. && t.x < 32. ? v : 0u;
	}
	if (s.menuId == MENU_FRESNEL)
	{
		v = t.y == 3. ? (t.x < 4. ? 1702109254u : (t.x < 8. ? 1679846770u : (t.x < 12. ? 1852401253u : (t.x < 16. ? 1629516645u : (t.x < 20. ? 1853189997u : (t.x < 24. ? 1718558836u : 32u)))))) : v;
		v = t.y == 2. ? (t.x < 4. ? 1818649970u : (t.x < 8. ? 1702126437u : (t.x < 12. ? 1768693860u : (t.x < 16. ? 544499815u : (t.x < 20. ? 544370534u : (t.x < 24. ? 1768366177u : 544105846u)))))) : v;
		v = t.y == 1. ? (t.x < 4. ? 1935832435u : (t.x < 8. ? 1851880052u : (t.x < 12. ? 539911523u : (t.x < 16. ? 1629516873u : (t.x < 20. ? 1869770864u : (t.x < 24. ? 1701340001u : 3219571u)))))) : v;
		v = t.y == 0. ? (t.x < 4. ? 544370534u : (t.x < 8. ? 2053206631u : (t.x < 12. ? 543649385u : (t.x < 16. ? 1818717793u : (t.x < 20. ? 29541u : 0u))))) : v;
		v = t.x >= 0. && t.x < 28. ? v : 0u;
	}
	if (s.menuId == MENU_GEOMETRY)
	{
		v = t.y == 2. ? (t.x < 4. ? 1702109255u : (t.x < 8. ? 1679846770u : (t.x < 12. ? 1852401253u : (t.x < 16. ? 1931506533u : (t.x < 20. ? 1868849512u : (t.x < 24. ? 1735289207u : (t.x < 28. ? 543584032u : 0u))))))) : v;
		v = t.y == 1. ? (t.x < 4. ? 1919117677u : (t.x < 8. ? 1667327599u : (t.x < 12. ? 544437349u : (t.x < 16. ? 1701864804u : (t.x < 20. ? 1852400750u : (t.x < 24. ? 1852776551u : (t.x < 28. ? 1701344288u : 2126441u))))))) : v;
		v = t.y == 0. ? (t.x < 4. ? 1634890337u : (t.x < 8. ? 1835362158u : (t.x < 12. ? 7630437u : 0u))) : v;
		v = t.x >= 0. && t.x < 32. ? v : 0u;
	}

	float c = float((v >> uint(8. * t.x)) & 255u);

	vec3 textColor = vec3(.3);

	p = (p - t * scale) / scale;
	p.x = (p.x - .5) * .45 + .5;
	float sdf = TextSDF(p, c);
	if (c != 0.)
	{
		color = mix(textColor, color, smoothstep(-.05, +.05, sdf));
	}

	if (s.menuId == MENU_SPECULAR)
	{
		color = mix(color, textColor, smoothstep(.05, -.05, Capsule(q.yx - vec2(-12.3, 48.), .3, 26.)));
	}
}

void MenuText(inout vec3 color, vec2 p, in AppState s)
{
	p -= vec2(-160, -1);

	vec2 scale = vec2(4., 8.);
	vec2 t = floor(p / scale);

	float tab = 1.;
	if (t.y >= 6. && t.y < 10.)
	{
		p.x -= tab * scale.x;
		t.x -= tab;
	}
	if (t.y >= 0. && t.y < 5.)
	{
		p.x -= tab * scale.x;
		t.x -= tab;
	}
	if (t.y >= 0. && t.y < 3.)
	{
		p.x -= tab * scale.x;
		t.x -= tab;
	}

	uint v = 0u;
	v = t.y == 10. ? (t.x < 4. ? 1718777171u : (t.x < 8. ? 6644577u : 0u)) : v;
	v = t.y == 9. ? (t.x < 4. ? 1635018061u : (t.x < 8. ? 108u : 0u)) : v;
	v = t.y == 8. ? (t.x < 4. ? 1818585412u : (t.x < 8. ? 1920230245u : 25449u)) : v;
	v = t.y == 7. ? (t.x < 4. ? 1735749458u : (t.x < 8. ? 1936027240u : 115u)) : v;
	v = t.y == 6. ? (t.x < 4. ? 1702060354u : (t.x < 8. ? 1819231008u : 29295u)) : v;
	v = t.y == 5. ? (t.x < 4. ? 1751607628u : (t.x < 8. ? 1735289204u : 0u)) : v;
	v = t.y == 4. ? (t.x < 4. ? 1717987652u : (t.x < 8. ? 6648693u : 0u)) : v;
	v = t.y == 3. ? (t.x < 4. ? 1667592275u : (t.x < 8. ? 1918987381u : 0u)) : v;
	v = t.y == 2. ? (t.x < 4. ? 1953720644u : (t.x < 8. ? 1969383794u : 1852795252u)) : v;
	v = t.y == 1. ? (t.x < 4. ? 1936028230u : (t.x < 8. ? 7103854u : 0u)) : v;
	v = t.y == 0. ? (t.x < 4. ? 1836016967u : (t.x < 8. ? 2037544037u : 0u)) : v;
	v = t.x >= 0. && t.x < 12. ? v : 0u;

	float c = float((v >> uint(8. * t.x)) & 255u);

	vec3 textColor = vec3(.3);
	if (t.y == 10. - s.menuId)
	{
		textColor = vec3(0.74, 0.5, 0.12);
	}

	p = (p - t * scale) / scale;
	p.x = (p.x - .5) * .45 + .5;
	float sdf = TextSDF(p, c);
	if (c != 0.)
	{
		color = mix(textColor, color, smoothstep(-.05, +.05, sdf));
	}
}

void DrawMenuControls(inout vec3 color, vec2 p, in AppState s)
{
	p -= vec2(-110, 74);

	// radial
	float c2 = Capsule(p - vec2(0., -3.5), 3., 4.);
	float c1 = Circle(p + vec2(0., 7. - 7. * s.metal), 2.5);

	// roughness slider
	p.y += 15.;
	c1 = min(c1, Capsule(p.yx - vec2(0., 20.), 1., 20.));
	c1 = min(c1, Circle(p - vec2(40. * s.roughness, 0.), 2.5));

	p.y += 8.;
	c1 = min(c1, Rectangle(p - vec2(19.5, 0.), vec2(21.4, 4.)));
	color = mix(color, vec3(0.9), Smooth(-c2 * 2.));
	color = mix(color, vec3(0.3), Smooth(-c1 * 2.));

	for (int i = 0; i < 6; ++i)
	{
		vec2 o = vec2(i == int(s.baseColor) ? 2.5 : 3.5);
		color = mix(color, BASE_COLORS[i], Smooth(-2. * Rectangle(p - vec2(2. + float(i) * 7., 0.), o)));
	}
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	vec2 uv = fragCoord.xy / iResolution.xy;
	vec2 q = fragCoord.xy / iResolution.xy;
	vec2 p = -1. + 2. * q;
	p.x *= iResolution.x / iResolution.y;
	p *= 100.;

	AppState s;
	LoadState(s);

	vec3 color = vec3(1., .98, .94) * mix(1.0, 0.4, Smooth(abs(.5 - uv.y)));
	float vignette = q.x * q.y * (1.0 - q.x) * (1.0 - q.y);
	vignette = saturate(pow(32.0 * vignette, 0.05));
	color *= vignette;

	DrawScene(color, p, s);
	Diagram(color, p, s);
	InfoText(color, p, s);
	MenuText(color, p, s);
	DrawMenuControls(color, p, s);

	fragColor = vec4(color, 1.);
}
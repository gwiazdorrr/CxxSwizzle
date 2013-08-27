// http://glsl.heroku.com/e#10632.0

// The other road to hell
// By gleurop

//precision mediump float;

uniform float time;
uniform vec2 resolution;

const int MAX_ITER = 50;

vec2 rotate(in vec2 v, in float a) {
	return vec2(cos(a)*v.x + sin(a)*v.y, -sin(a)*v.x + cos(a)*v.y);
}

float torus(in vec3 p, in vec2 t)
{
	vec2 q = vec2(length(p.xz)-t.x,p.y);
	return length(q)-t.y;
}

float trap(in vec3 p)
{
	//return length(max(abs(p.xy) - vec2(0.05), 0.0));
	//return length(p.xz)-0.05;
	return min(torus(p, vec2(0.5, 0.05)), length(max(abs(p.xz) - vec2(0.05), 0.0)));
	//return max(min(length(p.xz), min(length(p.yz), length(p.xy))) - 0.15, 0.0);
}

float map(in vec3 p)
{
	float cutout = dot(abs(p.yz),vec2(0.5))-0.035;
	float road = max(abs(p.y-0.025), abs(p.z)-0.035);
	
	vec3 z = abs(1.0-mod(p,2.0));
	z.yz = rotate(z.yz, time*0.05);

	float d = 99999.0;
	for (float i = 0.0; i < 3.0; i++) {
		z.xz = rotate(z.xz, radians(i*10.0+time));
		z.zy = rotate(z.yz, radians((i+1.0)*20.0+time*1.1234));
		z = abs(1.0-mod(z+i/3.0,2.0));
		
		z = z*2.0 - 0.3;
		d = min(d, trap(z) * pow(2.0, -(i+1.0)));
	}
	return min(max(d, -cutout), road);
}

vec3 hsv(in float h, in float s, in float v) {
	return mix(vec3(1.0), clamp((abs(fract(h + vec3(3, 2, 1) / 3.0) * 6.0 - 3.0) - 1.0), 0.0 , 1.0), s) * v;
}

vec3 intersect(in vec3 rayOrigin, in vec3 rayDir)
{
	float total_dist = 0.0;
	vec3 p = rayOrigin;
	float d = 1.0;
	float iter = 0.0;
	float mind = 3.14159+sin(time*0.1)*0.2;
	
	for (int i = 0; i < MAX_ITER; i++)
	{		
		if (d < 0.0001) continue;
		
		d = map(p);
		p += d*vec3(rayDir.x, rotate(rayDir.yz, sin(mind)));
		mind = min(mind, d);
		total_dist += d;
		iter++;
	}

	vec3 color = vec3(0.0);
	if (d < 0.0001) {
		float x = (iter/float(MAX_ITER));
		float y = (d-0.001)/0.001/(float(MAX_ITER));
		float z = (0.001-d)/0.001/float(MAX_ITER);
		if (max(abs(p.y-0.025), abs(p.z)-0.035)<0.001) { // Road
			float w = smoothstep(mod(p.x*50.0, 4.0), 2.0, 2.01);
			w -= 1.0-smoothstep(mod(p.x*50.0+2.0, 4.0), 2.0, 1.99);
			w = fract(w+0.0001);
			float a = fract(smoothstep(abs(p.z), 0.0025, 0.0026));
			color = vec3((1.0-x-y*2.)*mix(vec3(0.8, 0.8, 0), vec3(0.1), 1.0-(1.0-w)*(1.0-a)));
		} else {
			float q = 1.0-x-y*2.+z;
			color = hsv(q*0.2+0.85, 1.0-q*0.2, q);
		}
	} else
		color = hsv(d, 1.0, 1.0)*mind*100.0; // Background
	return color;
}

void main()
{
	vec3 upDirection = vec3(0, -1, 0);
	vec3 cameraDir = vec3(1,0,0);
	vec3 cameraOrigin = vec3(time*0.1, 0, 0);
	
	vec3 u = normalize(cross(upDirection, cameraOrigin));
	vec3 v = normalize(cross(cameraDir, u));
	vec2 screenPos = -1.0 + 2.0 * gl_FragCoord.xy / resolution.xy;
	screenPos.x *= resolution.x / resolution.y;
	vec3 rayDir = normalize(u * screenPos.x + v * screenPos.y + cameraDir*(1.0-length(screenPos)*0.5));
	
	gl_FragColor = vec4(intersect(cameraOrigin, rayDir), 1.0);
} 
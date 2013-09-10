// from: http://glsl.heroku.com/e#10623.3
// water turbulence effect by joltz0r 2013-07-04, improved 2013-07-07
// Altered
#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
//varying vec2 surfacePosition;

#define MAX_ITER 16
void main( void ) {
    //vec2 p = surfacePosition*8.0;
	vec2 uv = gl_FragCoord.xy / resolution.xy;
	uv = uv * 4.0 - 2.0;
	vec2 p = uv;
	
	vec2 i = p;
	float c = 2.0;
	float inten = 1.0;

	for (int n = 0; n < MAX_ITER; n++) {
		float t = time * (1.0 - (1.0 / float(n+1)));
		i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
		c += 1.0/length(vec2(p.x / (sin(i.x+t)/inten),p.y / (cos(i.y+t)/inten)));
	}
	c /= float(MAX_ITER);
	float pulse = abs(sin(time*5.));
	float pulse2 = pow(abs(sin(time*3.)),.25);
	float pulse3 = pow(abs(sin(time*5.)),2.);
	gl_FragColor = vec4(vec3(pow(c,1.5+pulse2/2.))*vec3(1.0+pulse2, 2.0-pulse2, 1.5+pulse3)*(1.+pulse2)/2., 1.0);

}
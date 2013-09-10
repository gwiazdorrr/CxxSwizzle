#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

// Leadlight by @hintz 2013-05-02

void main()
{
    vec2 uv = gl_FragCoord / resolution;

	gl_FragColor = texture(diffuse, uv);
}
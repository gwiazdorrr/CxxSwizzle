// horizontal Gaussian blur pass

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 pixelSize = 1./ iChannelResolution[0].xy;
    vec2 uv = fragCoord.xy * pixelSize;
    
    float h = pixelSize.x;
	vec4 sum = vec4(0.0);
	sum += texture(iChannel0, fract(vec2(uv.x - 4.0*h, uv.y)) ) * 0.05;
	sum += texture(iChannel0, fract(vec2(uv.x - 3.0*h, uv.y)) ) * 0.09;
	sum += texture(iChannel0, fract(vec2(uv.x - 2.0*h, uv.y)) ) * 0.12;
	sum += texture(iChannel0, fract(vec2(uv.x - 1.0*h, uv.y)) ) * 0.15;
	sum += texture(iChannel0, fract(vec2(uv.x + 0.0*h, uv.y)) ) * 0.16;
	sum += texture(iChannel0, fract(vec2(uv.x + 1.0*h, uv.y)) ) * 0.15;
	sum += texture(iChannel0, fract(vec2(uv.x + 2.0*h, uv.y)) ) * 0.12;
	sum += texture(iChannel0, fract(vec2(uv.x + 3.0*h, uv.y)) ) * 0.09;
	sum += texture(iChannel0, fract(vec2(uv.x + 4.0*h, uv.y)) ) * 0.05;
    
    fragColor.xyz = sum.xyz/0.98; // normalize
	fragColor.a = 1.;
}
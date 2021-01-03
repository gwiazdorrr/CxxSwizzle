// vertical Gaussian blur pass

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 pixelSize = 1./ iChannelResolution[0].xy;
    vec2 uv = fragCoord.xy * pixelSize;

    float v = pixelSize.y;
	vec4 sum = vec4(0.0);
	sum += texture(iChannel0, fract(vec2(uv.x, uv.y - 4.0*v)) ) * 0.05;
	sum += texture(iChannel0, fract(vec2(uv.x, uv.y - 3.0*v)) ) * 0.09;
	sum += texture(iChannel0, fract(vec2(uv.x, uv.y - 2.0*v)) ) * 0.12;
	sum += texture(iChannel0, fract(vec2(uv.x, uv.y - 1.0*v)) ) * 0.15;
	sum += texture(iChannel0, fract(vec2(uv.x, uv.y + 0.0*v)) ) * 0.16;
	sum += texture(iChannel0, fract(vec2(uv.x, uv.y + 1.0*v)) ) * 0.15;
	sum += texture(iChannel0, fract(vec2(uv.x, uv.y + 2.0*v)) ) * 0.12;
	sum += texture(iChannel0, fract(vec2(uv.x, uv.y + 3.0*v)) ) * 0.09;
	sum += texture(iChannel0, fract(vec2(uv.x, uv.y + 4.0*v)) ) * 0.05;
    
    fragColor.xyz = sum.xyz/0.98; // normalize
	fragColor.a = 1.;
}
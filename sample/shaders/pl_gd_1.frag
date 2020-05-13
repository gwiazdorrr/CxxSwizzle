// https://www.shadertoy.com/view/ttfGDj
void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    // Hint: (x^2 / a^2) + (y^2 / b^2) = 1
    float a = (0.5 * sin(4.0 * iTime) + 0.75) / 3.0;
	float b = (0.25 * -sin(4.0 * iTime) + 0.75) / 3.0;
    
    vec2 ab = vec2(a, b);
    
    vec2 uv = fragCoord / iResolution.xy;
    vec2 xy = vec2(0.5) - uv;
    if (all(greaterThan(xy, -ab)) && all(lessThan(xy, ab)))
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    else
        fragColor = vec4(0.0, 0.0, 1.0, 1.0);
}
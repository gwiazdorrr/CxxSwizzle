// https://www.shadertoy.com/view/tl23zm

const float PI = 3.14159265358979323846;

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 UV = fragCoord / iResolution.xy;
    vec2 Centre = vec2(sin(iTime * 0.5) * 0.5 + 0.5, sin(iTime) * 0.5 + 0.5);
    vec2 FromCentre = UV - Centre;

    float Angle = atan(FromCentre.y, FromCentre.x);
    float Distance = 1.0 / length(FromCentre);

    Distance += iTime * 4.0;
    Angle += iTime;
    // The modulo operator better explains the intent, but may fail to compile on some systems.
    int Segment = int(step(0.5, fract(Distance / 6.0)));//int(Distance / 3.0) % 2;
    
    vec2 TexCoords = vec2(Distance / 3.0, 2.0 * Angle / PI);
    
    fragColor = (Segment == 0)
        ? vec4(fract(TexCoords), 0.0, 1.0)
        : vec4(0.0, fract(TexCoords), 1.0);
}

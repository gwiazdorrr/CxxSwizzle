// https://www.shadertoy.com/view/wd3GWS

const float Pi = 3.14159265358979323846;

// Samples the displacement map for the given screen UV coordinates.
float SampleDisplacement(vec2 UV)
{
    // Tile the texture twice horizontally.
    return texture(iChannel2, UV * vec2(2.0, 1.0)).r;
}

// Scales the displacement from texture data to world space.
float TextureToWorldDisplacement(float Displacement)
{
    // Obtain displacement range of 10 cm.
    return Displacement * 0.1;
}

// Unprojects screen UV coordinates coupled with a displacement map value to a world space position and a world space normal.
vec3 Unproject(vec2 UV, float Displacement, out vec3 Normal)
{
    vec3 WorldPos = vec3(UV, TextureToWorldDisplacement(Displacement));
    // Use screen-space derivatives to obtain a low-resolution normal.
    const float ZStrength = 0.005;
    Normal = normalize(cross(dFdx(WorldPos), dFdy(WorldPos)));
    return WorldPos;
}

// Projects a given world position to screen UV coordinates.
vec2 Project(vec3 World)
{
    return World.xy;
}

// Performs ray marched shadowing, i.e. discrete sampling of geometry to test whether it occludes the light source from the
// point of view of a world position.
float ShadowOcclusion(vec3 MarchOrigin, vec3 MarchEnd)
{
    const int RaymarchStepCount = 512;
    const float RaymarchStep = 1.0 / float(RaymarchStepCount + 1);
    const float TraceOriginEpsilon = 1e-4;
    const float Penumbra = 0.00001;

    // Offset the march origin slightly vertically, so as to avoid false occlusion by direct neighbours.
    // The entire segment we're going to be sampling.
    vec3 MarchSegment = MarchEnd - MarchOrigin;

    float Time = 0.0;
    for (int i = 0; i < RaymarchStepCount; ++i)
    {
        Time += RaymarchStep;
        vec3 SamplePos = MarchOrigin + MarchSegment * Time;
        float LocalDisplacement = TextureToWorldDisplacement(SampleDisplacement(Project(SamplePos)));
        float RayAltitude = mix(MarchOrigin.z, MarchEnd.z, Time);
        float Occlusion = step(LocalDisplacement, RayAltitude);
        //float Occlusion = smoothstep(-Penumbra, Penumbra, RayAltitude - LocalDisplacement);
        if (Occlusion < 1.0)
            return Occlusion;
    }
    return 1.0;
}

// M. Oren and S. K. Nayar. Generalization of Lambert’s Reflectance Model, equation (30).
vec3 OrenNayarDiffuse(vec3 L, vec3 N, vec3 V, float Roughness, vec3 Albedo)
{
    float Sigma2 = Roughness * Roughness;
    float A = 1.0 - 0.5 * Sigma2 / (Sigma2 + 0.33);
    float B = 0.45 * Sigma2 / (Sigma2 + 0.09);
    
    float ThetaI = acos(dot(N, L));
    float ThetaR = acos(dot(N, V));
    
    float Alpha = max(ThetaI, ThetaR);
    float Beta = min(ThetaI, ThetaR);
    
    return Albedo / Pi * max(0.0, dot(N, L)) * (A + (B * max(0.0, cos(ThetaI - ThetaR)) * sin(Alpha) * tan(Beta)));
}

// Made-up ambient lighting. Attempts to re-use the displacement map sample as ambient occlusion.
vec3 Ambient(float Displacement, vec3 Colour)
{
    float AmbientOcclusion = 0.4 + 0.6 * Displacement * Displacement;
    return AmbientOcclusion * Colour;
}

// The sRGB transfer function.
vec3 LinearTosRGB(vec3 Colour)
{
    vec3 Weights = step(vec3(0.0031308), Colour);
    return mix(
        12.92 * Colour,
        1.055 * (pow(Colour, vec3(1.0 / 2.4)) - vec3(0.055)),
        Weights
    );
}

// Reference shading code.
vec3 ShadeReference(vec3 LightPos, float LightIntensity, vec3 Albedo, vec3 PixelWorldPos, vec3 PixelWorldNormal, float DisplacementSample)
{
    vec3 PixelToLight = LightPos - PixelWorldPos;
    vec3 L = PixelToLight;
    float r = length(L);
    L *= 1.0 / r;
    vec3 N = PixelWorldNormal;
    vec3 V = vec3(0., 0., 1.);  // Assume orthographic projection for simplicity.
    
    // Inverse square law.
    float Attenuation = 1.0 / (r * r);
    
    float Occlusion = ShadowOcclusion(PixelWorldPos, LightPos);
    
    vec3 DiffuseLighting = OrenNayarDiffuse(L, N, V, 0.3, Albedo);
    vec3 AmbientLighting = Ambient(DisplacementSample, vec3(0.0, 0.025, 0.0625));
    
    return DiffuseLighting;
}

// You are welcome to implement an optimised version here.
vec3 ShadeOptimised(vec3 LightPos, float LightIntensity, vec3 Albedo, vec3 PixelWorldPos, vec3 PixelWorldNormal, float DisplacementSample)
{
    return ShadeReference(LightPos, LightIntensity, Albedo, PixelWorldPos, PixelWorldNormal, DisplacementSample);
}

void mainImage(out vec4 FragColour, in vec2 FragCoord)
{
    vec2 UV = FragCoord / iResolution.xy;
    
    float DisplacementSample = SampleDisplacement(UV);
    vec3 Albedo = vec3(0.6);
    
    vec2 UnitCircle = vec2(cos(iTime), sin(iTime));
    vec3 LightPos = vec3(UnitCircle + 0.5, 0.3);
    const float Intensity = 2.0;
    
    vec3 PixelWorldNormal;
    vec3 PixelWorldPos = Unproject(UV, 0.0, PixelWorldNormal);
    
    vec3 Colour =
#if 1   // Flip this to 0 to shade using your optimised code.
        ShadeReference
#else
        ShadeOptimised
#endif
        (LightPos, Intensity, Albedo, PixelWorldPos, PixelWorldNormal, DisplacementSample);
    
    // Reference linear to sRGB colour conversion.
    FragColour = vec4(LinearTosRGB(Colour), 1.0);
}
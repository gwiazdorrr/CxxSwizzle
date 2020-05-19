// https://www.shadertoy.com/view/wlj3WD

const float M_PI = 3.14159265358979323846;
const float MAX_FLT = 1e37; // Assume single-precision IEEE 754 float.

const float RayStartEpsilon = 1e-4;

// No enums in GLSL, so just declare constants.
const int   Material_Floor      = 0,
            Material_Ceiling    = 1,
            Material_LeftWall   = 2,
            Material_RightWall  = 3,
            Material_BackWall   = 4,
            Material_Box        = 5,
            Material_Sphere     = 6;

// ==================================================================
// Camera

vec3 Camera_GetLocation()
{
    return vec3(-11.0 + cos(iTime), 2.0 * sin(iTime * 2.0), 2.0 * cos(iTime * 0.5));
}

vec3 Camera_GetLookAtPoint()
{
    return vec3(5, 0, 0);
}

// Identical to reference.
vec3 Camera_CalcViewVector(in vec2 UV)
{
    // Compute the basis vectors of the camera.
    vec3 Forward = normalize(Camera_GetLookAtPoint() - Camera_GetLocation());
    vec3 Right = vec3(0.0, 1.0, 0.0);
    vec3 Up = normalize(cross(Forward, Right));
    Right = normalize(cross(Up, Forward));
    
    const float FOV = (65.0 / 180.0 * M_PI);
    const float ZNear = 0.1;
    const float ZFar = 1024.0;
    
    // Transform from 2D screen space to a 3D view vector.
    vec2 ProjectionScale;
    ProjectionScale.x = ZNear * tan(FOV * 0.5);
    ProjectionScale.y = ProjectionScale.x * iResolution.y / iResolution.x;
    UV *= ProjectionScale;
    return normalize(Forward * ZNear + Right * UV.x + Up * UV.y);
}

// ==================================================================
// Ray

struct Ray
{
    vec3 Start;
    vec3 End;
    vec3 Direction;
    // Ray start and end, as projected down to 1D onto the direction axis.
    float ProjectedStart, ProjectedEnd;
    float Length;
};

// Helper for constructing rays.
Ray Make_Ray(vec3 Start, vec3 End)
{
    Ray Ray;
    Ray.Start = Start;
    Ray.End = End;
    Ray.Direction = normalize(End - Start);
    Ray.ProjectedStart = dot(Ray.Direction, Ray.Start);
    Ray.ProjectedEnd = dot(Ray.Direction, Ray.End);
    Ray.Length = Ray.ProjectedEnd - Ray.ProjectedStart;
    return Ray;
}

// ==================================================================
// Scene

// Plane intersector.
bool Plane_Intersect(const in vec4 self, const in Ray Ray, out vec3 OutPoint, out vec3 OutNormal, out float OutDistance)
{
    float D1, D2;

    D1 = dot(self.xyz, Ray.Start) - self.w;
    D2 = dot(self.xyz, Ray.End)   - self.w;
    if (D1 * D2 > 0.0)
    {
        // Both projections on the same side of the plane - no intersection.
        return false;
    }
    if (D1 < 0.0)
    {
        // Starting point behind plane - no intersection.
        return false;
    }
    float Time = -D1 / (D2 - D1);
    if (Time < RayStartEpsilon)
    {
        return false;
    }
    
    OutNormal = self.xyz;
    OutDistance = Ray.Length * Time;
    OutPoint = Ray.Start + Ray.Direction * OutDistance;
    return true;
}

// Sphere intersector.
// Adapted from "Real-Time Collision Detection" by Christer Ericson: 5.3.2 Intersecting Ray or Segment against Sphere
bool Sphere_Intersect(const vec3 Origin, const float Radius, const in Ray Ray, out vec3 OutPoint, out vec3 OutNormal, out float OutDistance)
{
    vec3 M = Ray.Start - Origin;
    float B = dot(M, Ray.Direction);
    float C = dot(M, M) - Radius * Radius;
    // Exit if ray start is outside the sphere (C > 0) and ray points away from it (B > 0).
    if (C > 0.0 && B > 0.0)
        return false;
    float Discriminant = B * B - C;
    // No solution to the quadratic equation - no intersection.
    if (Discriminant < 0.0)
        return false;
    // Find the closest time to intersection.
    OutDistance = -B - sqrt(Discriminant);
    
    // No hit if collision is out of range.
    if (OutDistance > Ray.Length)
        return false;
    
    // If distance is negative, the ray started inside the sphere, therefore clamp.
    OutDistance = max(0.0, OutDistance);
    OutPoint = Ray.Start + OutDistance * Ray.Direction;
    OutNormal = normalize(OutPoint - Origin);
    return true;
}

// Box intersector.
// Adapted from "Real-Time Collision Detection" by Christer Ericson: 5.3.3 Intersecting Ray or Segment against Box
bool Box_Intersect(const vec3 Origin, const vec3 Extents, const float YawAngle, const in Ray WorldRay, out vec3 OutPoint, out vec3 OutNormal, out float OutDistance)
{   
    // Transform ray to the box's local space.
    mat3 RotationMatrix = mat3(
         cos(YawAngle), sin(YawAngle),  0.0,
        -sin(YawAngle), cos(YawAngle),  0.0,
        0.0,            0.0,            1.0
    );
    vec3 LocalStart = RotationMatrix * (WorldRay.Start - Origin);
    vec3 LocalEnd = RotationMatrix * (WorldRay.End - Origin);
    Ray LocalRay = Make_Ray(LocalStart, LocalEnd);
        
    float MinTime = 0.0, MaxTime = MAX_FLT;
    
    for (int i = 0; i < 3; ++i)
    {
        if (abs(LocalRay.Direction[i]) < RayStartEpsilon)
        {
            // Ray is parallel to slab.
            if (LocalRay.Start[i] < -Extents[i] || LocalRay.Start[i] > Extents[i])
                return false;
        }
        else
        {
            // Find intersection against the slab.
            float RecipDir = 1.0 / LocalRay.Direction[i];
            float T1 = (-Extents[i] - LocalRay.Start[i]) * RecipDir;
            float T2 = ( Extents[i] - LocalRay.Start[i]) * RecipDir;
            vec3 PotentialNormal = vec3(0.0);
            PotentialNormal[i] = -1.0;
            if (T1 > T2)
            {
                float Temp = T1;
                T1 = T2;
                T2 = Temp;
                PotentialNormal = -PotentialNormal;
            }
            if (MinTime < T1)
            {
                MinTime = T1;
                OutNormal = PotentialNormal;
            }
            MaxTime = min(MaxTime, T2);
            // Exit as soon as the intersection is found to be empty.
            if (MinTime > MaxTime)
                return false;
        }
    }
    
    // No hit if collision time is out of range.
    if (MinTime < 0.0 || MinTime > LocalRay.Length)
        return false;
    
    OutDistance = MinTime;
    // Transform the hit location and normal back into world space.
    mat3 InvRotationMatrix = transpose(RotationMatrix);
    OutPoint = LocalRay.Start + LocalRay.Direction * MinTime;
    OutPoint = InvRotationMatrix * OutPoint;
    OutPoint += Origin;
    OutNormal = InvRotationMatrix * OutNormal;
    
    return true;
}

// Generic ray-scene intersection. Tests against all the shapes in the scene and returns the closest hit information.
bool Scene_Intersect(const in Ray Ray, out vec3 OutPoint, out vec3 OutNormal, out float OutDistance, out int OutMaterialId)
{
    OutDistance = MAX_FLT;
    vec3 Point, Normal;
    float Distance;
    
    // Primitive data:
    // - Floor: normal = vec3(0, 0, 1), distance = 3, infinitely thin
    // - Ceiling: normal = vec3(0, 0, -1), distance = 3, infinitely thin
    // - Left wall: normal = vec3(0, 1, 0), distance = 3, infinitely thin
    // - Right wall: normal = vec3(0, -1, 0), distance = 3, infinitely thin
    // - Back wall, normal = vec3(-1, 0, 0), distance = 3, infinitely thin
    // - Tall box: origin = vec3(0.7, -1.1, -(1.0 / 0.9)), extents = vec3(0.9, 0.9, 1.8), yaw = 0.12 * M_PI
    // - Small box: origin = vec3(-0.7, 1.1, -(2.0 / 0.9)), extents = vec3(0.9), yaw = -0.12 * M_PI
    // - Sphere: origin = vec3(-1.4, -1.4, -2), radius = 1
    // Remember the materials!
    
#define COMBINE(Test, MaterialId)   \
        if (Test)   \
        {   \
            if (OutDistance > Distance) \
            {   \
                OutDistance = Distance; \
                OutPoint = Point;   \
                OutNormal = Normal; \
                OutMaterialId = MaterialId; \
            }   \
        }
    
#define BOX(Origin, Extents, YawAngle, MaterialId)  \
        COMBINE(Box_Intersect(Origin, Extents, YawAngle, Ray, Point, Normal, Distance), MaterialId)
    BOX(vec3( 0.0,  0.0, -3.0),         vec3(3.0,  3.0, 0.01), 0.0, Material_Floor);
    BOX(vec3( 0.0,  0.0,  3.0),         vec3(3.0,  3.0, 0.01), 0.0, Material_Ceiling);
    BOX(vec3( 0.0, -3.0,  0.0),         vec3(3.0, 0.01,  3.0), 0.0, Material_LeftWall);
    BOX(vec3( 0.0,  3.0,  0.0),         vec3(3.0, 0.01,  3.0), 0.0, Material_RightWall);
    BOX(vec3(-0.7,  1.1, -(2.0 / 0.9)), vec3(0.9, 0.9, 0.9), -0.12 * M_PI, Material_Box);
#undef BOX
    
#undef COMBINE
    
    return OutDistance != MAX_FLT;
}

// Helper overload for a boolean intersection test.
bool Scene_Intersect(const in vec3 Start, const in vec3 End)
{
    vec3 Dummy1, Dummy2;
    float Dummy3;
    int Dummy4;
    return Scene_Intersect(Make_Ray(Start, End), Dummy1, Dummy2, Dummy3, Dummy4);
}

// ==================================================================
// Material

// Material description - Phong diffuse colour. Identical to reference.
vec3 GetMaterialDiffuseColour(int MaterialId)
{
    return (MaterialId == Material_LeftWall) ? vec3(1.0, 0.0, 0.0)
         : (MaterialId == Material_RightWall) ? vec3(0.0, 1.0, 0.0)
         : vec3(1.0);
}

// Material description - Phong specular colour and exponent. Identical to reference.
vec4 GetMaterialSpecularColour(int MaterialId)
{
    // Only the sphere has geometry with a significant chance of showing a specular highlight.
    return (MaterialId == Material_Sphere) ? vec4(vec3(1.0), 60.0) : vec4(0.0);
}

// ==================================================================
// Shading

// Completely made-up ambient lighting, serving no other purpose than aesthetics. Inspired by Valve's ambient cubes,
// see chapter 8.4.1 of "Shading in Valve’s Source Engine", https://steamcdn-a.akamaihd.net/apps/valve/2006/SIGGRAPH06_Course_ShadingInValvesSourceEngine.pdf).
// Identical to reference.
vec3 GetAmbientLighting(vec3 Position, vec3 Normal)
{
    // Naively bend the normal to acquire some contribution of colour based on position.
    vec3 BentNormal = normalize(mix(clamp(Position / 3.0, -1.0, 1.0), Normal, 0.35));
    vec3 WallWeights = BentNormal * 0.5 + vec3(0.5);
    // Mix the colour contribution based on the bent normal.
    vec3 Colour  = mix(vec3(0.0),   // There is no front wall to contribute lighting.
                       GetMaterialDiffuseColour(Material_BackWall),
                       WallWeights.x);
         Colour += mix(GetMaterialDiffuseColour(Material_LeftWall),
                       GetMaterialDiffuseColour(Material_RightWall),
                       WallWeights.y);
         Colour += mix(GetMaterialDiffuseColour(Material_Floor),
                       GetMaterialDiffuseColour(Material_Ceiling),
                       WallWeights.z);
    return Colour * 0.06;
}

// Sample shading using the Phong model. Identical to reference.
vec3 ShadeSample(in vec3 Position, in vec3 N, in int MaterialId)
{
    const vec3 LightPos = vec3(0.0, 0.0, 2.9);
    const float LightIntensity = 25.0;
    const vec3 LightColour = vec3(1.0, 1.0, 0.6) * LightIntensity;
    
    // Phong shading.
    vec3 L = LightPos - Position;
    float R2 = dot(L, L);
    L = normalize(L);
    vec3 R = reflect(-L, N);
    vec3 V = normalize(Camera_GetLocation() - Position);
    float NDotL = dot(N, L);
    float RDotV = dot(R, V);
    
    // Every sample receives ambient lighting.
    vec3 ReflectedLight = GetAmbientLighting(Position, N);
    
    if (NDotL > 0.0)
    {
        // This sample is at least partially facing the light source. Trace a secondary ray to it for 1-sample shadowing.
        if (!Scene_Intersect(Position + N * RayStartEpsilon, LightPos))
        {
            // We are not shadowed by other geometry.
            float InvR2 = 1.0 / R2; // Inverse square law.
            float Attenuation = NDotL * InvR2;
            // Compute the diffuse component of light scattered at the sample.
            vec3 Diffuse = LightColour * Attenuation * GetMaterialDiffuseColour(MaterialId);
            
            vec3 Specular = vec3(0.0);
            if (RDotV > 0.0)
            {
                // The material has a specular response at this sample. Compute the specular component of light.
                Attenuation = pow(RDotV, GetMaterialSpecularColour(MaterialId).w) * InvR2;
                Specular = LightColour * Attenuation * GetMaterialSpecularColour(MaterialId).rgb;
            }
            
            ReflectedLight += Diffuse;
            ReflectedLight += Specular;
        }
    }
    
    return ReflectedLight;
}

// Entry point for actual rendering. Identical to reference.
vec3 Scene_Render(in vec2 UV, const in vec2 PixelCoords)
{
    vec3 View, Target, P, N;

    View = Camera_CalcViewVector(2.0 * UV - vec2(1.0));
    Target = Camera_GetLocation() + View * 10000.0;
    float Distance;
    int MaterialId;

    // Trace primary rays.
    if (Scene_Intersect(Make_Ray(Camera_GetLocation(), Target), P, N, Distance, MaterialId))
        return ShadeSample(P, N, MaterialId);
    else
        return vec3(0.0);
}


void mainImage(out vec4 FragColor, in vec2 FragCoord)
{
    FragColor = vec4(Scene_Render(FragCoord.xy / iResolution.xy, FragCoord), 1.0);
}

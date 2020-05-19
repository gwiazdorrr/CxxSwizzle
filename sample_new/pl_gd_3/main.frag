// https://www.shadertoy.com/view/3lj3zz

// Under FreeBSD license.

float saturate(float x)
{
    return clamp(x, 0., 1.);
}

vec3 saturate(vec3 x)
{
    return clamp(x, vec3(0.), vec3(1.));
}

mat3 LookAt(vec3 origin, vec3 target, float roll)
{
    vec3 up = vec3(sin(roll), cos(roll), 0.);
    vec3 x = normalize(target - origin);
    vec3 z = normalize(cross(x, up));
    vec3 y = normalize(cross(z, x));
    return mat3(z, y, x);
}

float RaySphere(vec3 rayOrigin, vec3 rayDir, vec3 spherePos, float sphereRadius)
{
    vec3 oc = rayOrigin - spherePos;

    float b = dot(oc, rayDir);
    float c = dot(oc, oc) - sphereRadius * sphereRadius;
    float h = b * b - c;

    float t;
    if (h < 0.)
    {
        t = -1.;
    }
    else
    {
        t = (-b - sqrt(h));
    }
    return t;
}

vec2 Rotate(in vec2 p, float a) 
{
    return cos(a) * p + sin(a) * vec2(p.y, -p.x);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    vec2 screenUV = fragCoord.xy / iResolution.xy;
    vec2 screenPos = -1. + 2. * screenUV;
    screenPos.x *= iResolution.x / iResolution.y;

    vec3 rayOrigin = 1.2 * vec3(0.,2.5,3.5);
    float cameraAngle = 0.9 + 3.14 + sin(iTime) * 0.5;    
    rayOrigin.zx = Rotate(rayOrigin.zx, -cameraAngle);
    
    vec3 cameraTarget = vec3(0.0,0.65,0.0);
    mat3 camMat = LookAt(rayOrigin, cameraTarget, 0.);
    vec3 rayDir = normalize(camMat * vec3(screenPos.xy,4.0));

    vec3 color = vec3(0.);    

    vec3 colorA = vec3(.58, .48, .23) * 1.3;
    vec3 colorB = vec3(.16, .37, .30) * 1.3;
    vec3 colorC = vec3(.3,.4,.7);
    
    vec3 lightDir = -normalize(vec3(0.1, 1., 0.1));
    
    color = colorC * smoothstep(1.0, 0.0, abs(.5 - screenUV.y));
 
    // Spheres
    float t = 1e6;  
    for (float z = -1.; z <= 1.; z += 1.)    
        for (float y = -1.; y <= 1.; y += 1.)
            for (float x = -1.; x <= 1.; x += 1.)
            {
                float sphereRadius = 0.25;                
                vec3 spherePos = 2. * sphereRadius * vec3(x, z, y);
                spherePos.y += 3.0 * sphereRadius;
                float newT = RaySphere(rayOrigin, rayDir, spherePos, sphereRadius);
                if (newT >= 0. && newT < t)
                {
                    t = newT;
                    vec3 hitPos = rayOrigin + t * rayDir;
                    hitPos -= spherePos;
                    hitPos.xy = Rotate(hitPos.xy,.5);
                    vec2 phiTheta = vec2(atan(hitPos.z, hitPos.x), acos(hitPos.y / sphereRadius));
                    
                    float pattern = smoothstep(0.6, 0.4, abs(2.0 * fract(phiTheta.y * 2.0) - 1.));
                    if (x == 0. || y == 0. || z == 0.)
                    {
                        pattern = 1. - pattern;
                    }
                    vec3 baseColor = mix(colorA, colorB, pattern);
                    
                    vec3 viewDir = normalize(hitPos - rayOrigin);
                    vec3 normal = -normalize(hitPos);
                    vec3 halfVector = normalize(lightDir + viewDir);
                    vec3 reflectDir = reflect(-lightDir, normal);
                    float ndotl = saturate(dot(normal, lightDir));
                    float phongAlpha = mix(64., 8.0, pattern);
                    vec3 ambient = baseColor * colorC * 0.15;
                    vec3 diffuse = baseColor * ndotl;
                    float specular = 0.5 * ndotl * pow(saturate(dot(viewDir, reflectDir)), phongAlpha);
                    color = ambient + diffuse + specular;
                }
            }
    
    fragColor = vec4(pow(color, vec3(1./ 2.2)), 1.);
}
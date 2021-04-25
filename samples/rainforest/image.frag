// Created by inigo quilez - iq/2016
// I share this piece (art and code) here in Shadertoy and through its Public API, only for educational purposes. 
// You cannot use, sell, share or host this piece or modifications of it as part of your own commercial or non-commercial product, website or project.
// You can share a link to it or an unmodified screenshot of it provided you attribute "by Inigo Quilez, @iquilezles and iquilezles.org". 
// If you are a techer, lecturer, educator or similar and these conditions are too restrictive for your needs, please contact me and we'll work it out.

// Normals are analytical (true derivatives) for the terrain and for the
// clouds, including the noise, the fbm and the smoothsteps.
//
// See here for more info: http://iquilezles.org/www/articles/morenoise/morenoise.htm
//
// Lighting and art composed for this shot/camera. The trees are really
// cheap (ellipsoids with noise), but they kind of do the job in distance
// and low image resolutions Also I used some cheap reprojection technique
// to smooth out the render.


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 p = fragCoord/iResolution.xy;

    vec3 col = texture( iChannel0, p ).xyz;
    //vec3 col = texelFetch( iChannel0, ivec2(fragCoord-0.5), 0 ).xyz;
    
    col *= 0.5 + 0.5*pow( 16.0*p.x*p.y*(1.0-p.x)*(1.0-p.y), 0.05 );
         
    fragColor = vec4( col, 1.0 );
}

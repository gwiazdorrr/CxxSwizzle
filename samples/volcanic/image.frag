// Created by inigo quilez - iq/2013
// I share this piece (art and code) here in Shadertoy and through its Public API, only for educational purposes. 
// You cannot use, sell, share or host this piece or modifications of it as part of your own commercial or non-commercial product, website or project.
// You can share a link to it or an unmodified screenshot of it provided you attribute "by Inigo Quilez, @iquilezles and iquilezles.org". 
// If you are a teacher, lecturer, educator or similar and these conditions are too restrictive for your needs, please contact me and we'll work it out.

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord/iResolution.xy;
    vec4 data = textureLod( iChannel0, uv, 0.0 );

    vec3 col = vec3(0.0);
    
    if( data.w < 0.0 )
    {
        col = texture( iChannel0, uv ).xyz;
    }
    else
    {
        // decompress velocity vector
        float ss =   mod(data.w,1024.0)/1023.0;
        float st = floor(data.w/1024.0)/1023.0;

        // motion blur (linear blur across velocity vectors)
        vec2 dir = (2.0*vec2(ss,st)-1.0)*0.25;
        float tot = 0.0;
        for( int i=0; i<32; i++ )
        {
            float h = float(i)/31.0;
            vec2  p = uv + dir*h;
            float w = 1.0-h;
            col += w*textureLod( iChannel0, p, 0.0 ).xyz;
            tot += w;
        }
        col /= tot;
    }

    // vignetting	
	col *= 0.5 + 0.5*pow( 16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y), 0.1 );

    fragColor = vec4( col, 1.0 );
}
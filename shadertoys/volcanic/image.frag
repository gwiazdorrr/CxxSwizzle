// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord/iResolution.xy;
    vec4 data = texture( iChannel0, uv );

    vec3 col = vec3(0.0);
    if( data.w < 0.0 )
    {
        col = texture( iChannel0, uv ).xyz;
    }
    else
    {
        // decompress velocity vector
        float ss = mod(data.w,256.0)/255.0;
        float st = floor(data.w/256.0)/255.0;

        // motion blur (linear blur across velocity vectors
        vec2 dir = (-1.0 + 2.0*vec2( ss, st ))*0.25;
        col = vec3(0.0);
        for( int i=0; i<32; i++ )
        {
            float h = float(i)/31.0;
            vec2 pos = uv + dir*h;
            col += texture( iChannel0, pos ).xyz;
        }
        col /= 32.0;
    }

    // vignetting	
	col *= 0.5 + 0.5*pow( 16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y), 0.1 );

    fragColor = vec4( col, 1.0 );
}

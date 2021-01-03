// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.


//#define HIGH_QUALITY_NOISE

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
	f = f*f*(3.0-2.0*f);
#ifndef HIGH_QUALITY_NOISE
	vec2 uv = (p.xy+vec2(37.0,17.0)*p.z) + f.xy;
	vec2 rg = textureLod( iChannel0, (uv+ 0.5)/256.0, 0. ).yx;
#else
	vec2 uv = (p.xy+vec2(37.0,17.0)*p.z);
	vec2 rg1 = textureLod( iChannel0, (uv+ vec2(0.5,0.5))/256.0, 0. ).yx;
	vec2 rg2 = textureLod( iChannel0, (uv+ vec2(1.5,0.5))/256.0, 0. ).yx;
	vec2 rg3 = textureLod( iChannel0, (uv+ vec2(0.5,1.5))/256.0, 0. ).yx;
	vec2 rg4 = textureLod( iChannel0, (uv+ vec2(1.5,1.5))/256.0, 0. ).yx;
	vec2 rg = mix( mix(rg1,rg2,f.x), mix(rg3,rg4,f.x), f.y );
#endif	
	return mix( rg.x, rg.y, f.z );
}



float noise( in vec2 x )
{
    vec2 p = floor(x);
    vec2 f = fract(x);
	vec2 uv = p.xy + f.xy*f.xy*(3.0-2.0*f.xy);
	return textureLod( iChannel0, (uv+118.4)/256.0, 0. ).x;
}

vec4 texcube( sampler2D sam, in vec3 p, in vec3 n )
{
	vec4 x = texture( sam, p.yz );
	vec4 y = texture( sam, p.zx );
	vec4 z = texture( sam, p.xy );
	return x*abs(n.x) + y*abs(n.y) + z*abs(n.z);
}

//=====================================================================

float lava( vec2 p )
{
	p += vec2(2.0,4.0);
    float f;
    f  = 0.5000*noise( p ); p = p*2.02;
    f += 0.2500*noise( p ); p = p*2.03;
    f += 0.1250*noise( p ); p = p*2.01;
    f += 0.0625*noise( p );
    return f;
}

const mat3 m = mat3( 0.00,  0.80,  0.60,
                    -0.80,  0.36, -0.48,
                    -0.60, -0.48,  0.64 );

float displacement( vec3 p )
{
	p += vec3(1.0,0.0,0.8);
	
    float f;
    f  = 0.5000*noise( p ); p = m*p*2.02;
    f += 0.2500*noise( p ); p = m*p*2.03;
    f += 0.1250*noise( p ); p = m*p*2.01;
    f += 0.0625*noise( p ); 
	
	float n = noise( p*3.5 );
    f += 0.03*n*n;
	
    return f;
}

float mapTerrain( in vec3 pos )
{
	return pos.y*0.1 + (displacement(pos*vec3(0.8,1.0,0.8)) - 0.4)*(1.0-smoothstep(1.0,3.0,pos.y));
}

float raymarchTerrain( in vec3 ro, in vec3 rd )
{
	float maxd = 30.0;
    float t = 0.1;
    for( int i=0; i<256; i++ )
    {
	    float h = mapTerrain( ro+rd*t );
        if( h<(0.001*t) || t>maxd ) break;
        t += h*0.8;
    }

    if( t>maxd ) t=-1.0;
    return t;
}

vec3 calcNormal( in vec3 pos, in float t )
{
    vec3 eps = vec3( max(0.02,0.001*t),0.0,0.0);
	return normalize( vec3(
           mapTerrain(pos+eps.xyy) - mapTerrain(pos-eps.xyy),
           mapTerrain(pos+eps.yxy) - mapTerrain(pos-eps.yxy),
           mapTerrain(pos+eps.yyx) - mapTerrain(pos-eps.yyx) ) );

}

vec3 lig = normalize( vec3(-0.3,0.4,0.7) );
	
vec4 mapClouds( in vec3 pos )
{
	vec3 q = pos*0.5 + vec3(0.0,-iTime,0.0);
	
	float d;
    d  = 0.5000*noise( q ); q = q*2.02;
    d += 0.2500*noise( q ); q = q*2.03;
    d += 0.1250*noise( q ); q = q*2.01;
    d += 0.0625*noise( q );
		
	d = d - 0.55;
	d *= smoothstep( 0.5, 0.55, lava(0.1*pos.xz)+0.01 );

	d = clamp( d, 0.0, 1.0 );
	
	vec4 res = vec4( d );

	res.xyz = mix( vec3(1.0,0.8,0.7), 0.2*vec3(0.4,0.4,0.4), res.x );
	res.xyz *= 0.25;
	res.xyz *= 0.5 + 0.5*smoothstep( -2.0, 1.0, pos.y );
	
	return res;
}

vec4 raymarchClouds( in vec3 ro, in vec3 rd, in vec3 bcol, float tmax )
{
	vec4 sum = vec4( 0.0 );

	float sun = pow( clamp( dot(rd,lig), 0.0, 1.0 ),6.0 );
	float t = 0.0;
	for( int i=0; i<60; i++ )
	{
		if( t>tmax || sum.w>0.95 ) break;//continue;
		vec3 pos = ro + t*rd;
		vec4 col = mapClouds( pos );
		
        col.xyz += vec3(1.0,0.7,0.4)*0.4*sun*(1.0-col.w);
		col.xyz = mix( col.xyz, bcol, 1.0-exp(-0.00006*t*t*t) );
		
		col.rgb *= col.a;

		sum = sum + col*(1.0 - sum.a);	

		t += max(0.1,0.05*t);
	}

	sum.xyz /= (0.001+sum.w);

	return clamp( sum, 0.0, 1.0 );
}

float softshadow( in vec3 ro, in vec3 rd, float mint, float k )
{
    float res = 1.0;
    float t = mint;
    for( int i=0; i<64; i++ )
    {
        float h = mapTerrain(ro + rd*t);
		h = max( h, 0.0 );
        res = min( res, k*h/t );
        t += clamp( h, 0.02, 0.5 );
		if( res<0.001 ) break;
    }
    return clamp(res,0.0,1.0);
}

vec3 path( float time )
{
	return vec3( 16.0*cos(0.2+0.5*.1*time*1.5), 1.5, 16.0*sin(0.1+0.5*0.11*time*1.5) );
	
}

mat3 setCamera( in vec3 ro, in vec3 ta, float cr )
{
	vec3 cw = normalize(ta-ro);
	vec3 cp = vec3(sin(cr), cos(cr),0.0);
	vec3 cu = normalize( cross(cw,cp) );
	vec3 cv = normalize( cross(cu,cw) );
    return mat3( cu, cv, cw );
}

void moveCamera( float time, out vec3 oRo, out vec3 oTa, out float oCr, out float oFl )
{
	vec3 ro = path( time+0.0 );
	vec3 ta = path( time+1.6 );
	ta.y *= 0.35 + 0.25*sin(0.09*time);
	float cr = 0.3*sin(1.0+0.07*time);
    oRo = ro;
    oTa = ta;
    oCr = cr;
    oFl = 2.1;
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 q = fragCoord.xy / iResolution.xy;
	vec2 p = -1.0 + 2.0*q;
	p.x *= iResolution.x / iResolution.y;
	
    // camera	
	float off = step( 0.001, iMouse.z )*6.0*iMouse.x/iResolution.x;
    float time = 3.4 + iTime + off;

    vec3 ro, ta; float cr, fl;
    moveCamera( time, ro, ta, cr, fl );
	// camera2world transform
    mat3 cam = setCamera( ro, ta, cr );

    // ray    
    vec3 rd = cam * normalize(vec3(p.xy,fl));

    // sky	 
	vec3 col = vec3(0.32,0.36,0.4) - rd.y*0.4;
    float sun = clamp( dot(rd,lig), 0.0, 1.0 );
	col += vec3(1.0,0.8,0.4)*0.2*pow( sun, 6.0 );
    col *= 0.9;

	vec3 bcol = col;
    
    // terrain	
	float t = raymarchTerrain(ro, rd);
    float st = t;
    if( t>0.0 )
	{
		vec3 pos = ro + t*rd;
		vec3 nor = calcNormal( pos, t );
		vec3 ref = reflect( rd, nor );

		vec3 bn = -1.0 + 2.0*texcube( iChannel0, 3.0*pos/4.0, nor ).xyz;
		nor = normalize( nor + 0.6*bn );
		
		float hh = 1.0 - smoothstep( -2.0, 1.0, pos.y );

        // lighting
		float sun = clamp( dot( nor, lig ), 0.0, 1.0 );
		float sha = 0.0; if( sun>0.01) sha=softshadow(pos,lig,0.01,32.0);
		float bac = clamp( dot( nor, normalize(lig*vec3(-1.0,0.0,-1.0)) ), 0.0, 1.0 );
		float sky = 0.5 + 0.5*nor.y;
        float lav = smoothstep( 0.5, 0.55, lava(0.1*pos.xz) )*hh*clamp(0.5-0.5*nor.y,0.0,1.0);
		float occ = pow( (1.0-displacement(pos*vec3(0.8,1.0,0.8)))*1.6-0.5, 2.0 );

		float amb = 1.0;

		col = vec3(0.8);

		vec3 lin = vec3(0.0);
		lin += 1.4*sun*vec3(1.80,1.27,0.99)*pow(vec3(sha),vec3(1.0,1.2,1.5));
		lin += 0.9*sky*vec3(0.16,0.20,0.40)*occ;
		lin += 0.9*bac*vec3(0.40,0.28,0.20)*occ;
		lin += 0.9*amb*vec3(0.15,0.17,0.20)*occ;
		lin += lav*vec3(3.00,0.61,0.00);

        // surface shading/material		
		col = texcube( iChannel1, 0.5*pos, nor ).xyz;
		col = col*(0.2+0.8*texcube( iChannel2, 4.0*vec3(2.0,8.0,2.0)*pos, nor ).x);
		vec3 verde = vec3(1.0,0.9,0.2);
		verde *= texture( iChannel2, pos.xz ).xyz;
		col = mix( col, 0.8*verde, hh );
		
		float vv = smoothstep( 0.0, 0.8, nor.y )*smoothstep(0.0, 0.1, pos.y-0.8 );
		verde = vec3(0.2,0.45,0.1);
		verde *= texture( iChannel2, 30.0*pos.xz ).xyz;
		verde += 0.2*texture( iChannel2, 1.0*pos.xz ).xyz;
		vv *= smoothstep( 0.0, 0.5, texture( iChannel2, 0.1*pos.xz + 0.01*nor.x ).x );
		col = mix( col, verde*1.1, vv );
		
		col = lin * col;
		
        // sun spec
        vec3 hal = normalize(lig-rd);
        col += 1.0*
               vec3(1.80,1.27,0.99)*
               pow( clamp(dot(nor,hal),0.0,1.0), 16.0 ) * 
               sun*sha*
               (0.04+0.96*pow(clamp(1.0+dot(hal,rd),0.0,1.0),5.0));
            
		// atmospheric
		col = mix( col, (1.0-0.7*hh)*bcol, 1.0-exp(-0.00006*t*t*t) );
    }

    // sun glow
    col += vec3(1.0,0.6,0.2)*0.2*pow( sun, 2.0 )*clamp( (rd.y+0.4)/(0.0+0.4),0.0,1.0);
	
    // smoke	
	{
	if( t<0.0 ) t=600.0;
    vec4 res = raymarchClouds( ro, rd, bcol, t );
	col = mix( col, res.xyz, res.w );
	}

    // gamma	
	col = pow( clamp( col, 0.0, 1.0 ), vec3(0.45) );

    // contrast, desat, tint and vignetting	
	col = col*0.3 + 0.7*col*col*(3.0-2.0*col);
	col = mix( col, vec3(col.x+col.y+col.z)*0.33, 0.2 );
    col *= 1.25*vec3(1.02,1.05,1.0);
      
    //-------------------------------------
	// velocity vectors (through depth reprojection)
    //-------------------------------------
    float vel = -1.0;
    if( st>0.0 )
    {
        // old camera position
        float oldTime = time - 1.0/30.0; // 1/30 of a second blur
        vec3 oldRo, oldTa; float oldCr, oldFl;
        moveCamera( oldTime, oldRo, oldTa, oldCr, oldFl );
        mat3 oldCam = setCamera( oldRo, oldTa, oldCr );

        // world spcae
        vec3 wpos = ro + rd*st;
        // camera space
        vec3 cpos = vec3( dot( wpos - oldRo, oldCam[0] ),
                          dot( wpos - oldRo, oldCam[1] ),
                          dot( wpos - oldRo, oldCam[2] ) );
        // ndc space
        vec2 npos = oldFl * cpos.xy / cpos.z;
        // screen space
        vec2 spos = 0.5 + 0.5*npos*vec2(iResolution.y/iResolution.x,1.0);


        // compress velocity vector in a single float
        vec2 uv = fragCoord/iResolution.xy;
        spos = clamp( 0.5 + 0.5*(spos - uv)/0.25, 0.0, 1.0 );
        vel = floor(spos.x*255.0) + floor(spos.y*255.0)*256.0;
    }

    fragColor = vec4( col, vel );
}
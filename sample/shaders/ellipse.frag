// The MIT License
// Copyright © 2015 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.



// Computing the distance to an ellipse using David Eberly's method instead 
// of mine, but tweaked
//
// David's: http://www.geometrictools.com/Documentation/DistancePointEllipseEllipsoid.pdf
//
// Mine: http://iquilezles.org/www/articles/ellipsedist/ellipsedist.htm and https://www.shadertoy.com/view/4sS3zz


// List of some other 2D distances:
//
// Triangle:             https://www.shadertoy.com/view/XsXSz4
// Isosceles Triangle:   https://www.shadertoy.com/view/MldcD7
// Regular Triangle:     https://www.shadertoy.com/view/Xl2yDW
// Regular Pentagon:     https://www.shadertoy.com/view/llVyWW
// Regular Octogon:      https://www.shadertoy.com/view/llGfDG
// Rounded Rectangle:    https://www.shadertoy.com/view/4llXD7
// Rhombus:              https://www.shadertoy.com/view/XdXcRB
// Trapezoid:            https://www.shadertoy.com/view/MlycD3
// Polygon:              https://www.shadertoy.com/view/wdBXRW
// Hexagram:             https://www.shadertoy.com/view/tt23RR
// Regular Star:         https://www.shadertoy.com/view/3tSGDy
// Ellipse 1:            https://www.shadertoy.com/view/4sS3zz
// Ellipse 2:            https://www.shadertoy.com/view/4lsXDN
// Quadratic Bezier:     https://www.shadertoy.com/view/MlKcDD
// Uneven Capsule:       https://www.shadertoy.com/view/4lcBWn
// Vesica:               https://www.shadertoy.com/view/XtVfRW
// Cross:                https://www.shadertoy.com/view/XtGfzw
// Pie:                  https://www.shadertoy.com/view/3l23RK
// Arc:                  https://www.shadertoy.com/view/wl23RK
// Horseshoe:            https://www.shadertoy.com/view/WlSGW1
//
// and many more here:   http://www.iquilezles.org/www/articles/distfunctions2d/distfunctions2d.htm


float sdEllipse( vec2 p, vec2 e )
{
    p = abs( p );
    
    // could precompute
    //if( e.x<e.y ) { p = p.yx; e = e.yx; }
    
    
    vec2 r = e*e;
    vec2 z = p/e;
    vec2 n = r*z;
    
    float g = dot(z,z) - 1.0;
    float s0 = z.y - 1.0;
    float s1;
	if (g<0.0) 
		s1 = 0.0;
	else
		s1 = length( n )/r.y - 1.0;
    float s = 0.0;
    for( int i=0; i<64; i++ )
    {
        s = 0.5*(s0+s1);
        //if( s==s0 || s==s1 ) break;
        
        vec2 ratio = n / ( r.y*s + r );
        
        g = dot(ratio,ratio) - 1.0;
        
        //if( g>0.0 ) { s0=s; } else  if( g<0.0 ) { s1=s; } else break;
        if( g>0.0 ) s0=s; else s1=s;
    }

    
    vec2 q = p * r / ( r.y*s + r );
    

    return length( p-q ) * sign( p.y - q.y );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = -1.0 + 2.0 * fragCoord.xy/iResolution.xy;
	uv.x *= iResolution.x/iResolution.y;
	
    vec2 m = iMouse.xy/iResolution.xy;
	m.x *= iResolution.x/iResolution.y;
	
	float d = sdEllipse( uv, vec2(0.3,0.3)*m + vec2(1.0,0.5)  );
    vec3 col = vec3(1.0) - sign(d)*vec3(0.1,0.4,0.7);
	col *= 1.0 - exp(-2.0*abs(d));
	col *= 0.8 + 0.2*cos(120.0*d);
	col = mix( col, vec3(1.0), 1.0-smoothstep(0.0,0.02,abs(d)) );

	fragColor = vec4( col, 1.0 );;

//	fragColor.xyz = dFdy(fragColor.xyz);
	//fragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
	//if ( (uv.x > uv.y) ) fragColor.r =1.0;
}
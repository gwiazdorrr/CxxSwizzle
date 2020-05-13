// cartoon face shader (v2016-4-19) / tomaes.32x.de
const float PI = 3.1415926535;

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
    float t = iTime+740.0-10.0+1.54;
    
    uv.y /= sqrt(t+uv.x/uv.y)*(0.04 + sin(t+uv.x)*0.003);
    
    // "skin" color
    vec3 c = vec3(0.3 / sqrt(uv.x),0.5,0.26 - sin(t)*0.15);

    float size = 0.55 + mod(t,0.5)*0.1 + sin(t*2.7)*0.05;
    float bend = 0.3;
    
    float o = mod(sin(t),0.2)*0.09;
    float ny = (0.75-size)*0.2;
    
	// nose: left
    if (distance(vec2(0.63,0.65+ny),vec2(uv.x*1.3,uv.y)) < 0.01)
        c = vec3(1.0-uv.y*0.1,0.9,0.0);
	// nose: right
    if (distance(vec2(0.71,0.65+ny),vec2(uv.x*1.3,uv.y)) < 0.01)
        c = vec3(1.0-uv.y*0.1,0.9,0.0); 
    
    // eyes: left
    if (distance(vec2(0.29,0.8),vec2(uv.x*1.3,uv.y)) < 0.09)
        c = vec3(1.0-uv.y*0.1,0.9,0.0);
    if (distance(vec2(0.29+o,0.8-o),vec2(uv.x*1.3,uv.y)) < 0.03)
        c = vec3(0.0,0.0,0.0);
       
    // eyes: right
    if (distance(vec2(1.0,0.8),vec2(uv.x*1.3,uv.y)) < 0.06)
        c = vec3(1.0-uv.y*0.1,0.9,0.0);
    if (distance(vec2(1.0-o,0.8+o),vec2(uv.x*1.3,uv.y)) < 0.02)
        c = vec3(0.0,0.0,0.0);
       
    // mouth     
    o = min(uv.x*uv.x+0.2,0.3); //sin(uv.x*PI)*bend; // + 0.4*mod(uv.x+mod(uv.y*0.4,0.02),0.02); 

    if (o == 0.3) 
        o+= abs(sin(uv.x*9.9))*0.05;
    
    if (uv.y+o*1.2 > size && uv.y-o*1.1 < 0.8-size)
		c = vec3(1.0-uv.y,1.0-uv.y,0.8-uv.x);
    
    o = min(uv.x*uv.x+0.18,0.3); //sin(uv.x*PI)*bend + mod(uv.x+1.2*mod(uv.y*0.4,0.02),0.02);     
    
    // teeth
    if (uv.y+o > size && uv.y-o < 0.81-size)
    {
        if (mod(uv.x+sin(t*0.5+uv.x*20.0+uv.y*5.0)*0.2,0.1) < 0.04)
            c = vec3( 1.0,1.0-uv.y*0.5,0.7 ); //vec3( 1.0,1.0,0.7 );
        else
            c = vec3( 1.0, 0.7, uv.y);
        
        float q = abs(sin((mod(uv.x,0.1)-0.037)*PI ))*0.1;
        
        if (uv.x > 0.37 && (mod(uv.y,1.0) < (0.35-q) || mod(uv.y,1.0) > (0.55+q)) && mod(uv.x, 0.1) < 0.07)
            c = vec3(1.0, 1.0, 0.4); //vec3(0.4, 0.6, 1.0);
    }
    
    // "mouth balls"
    vec2 d = vec2(uv.x,uv.y*0.8-ny);
    if ((distance(d,vec2(0.07,0.45))   < 0.08) ||
       (distance(d,vec2(1.0-0.07,0.5)) < 0.03) )
        c = vec3(0.0,0.25,0.15);
    
    // borders
    o = sin(uv.y*PI*1.55)*0.03 + ny*0.8;
    
    if (uv.x+o < 0.1 || uv.x-o > 0.9)
        c = vec3(1.0,0.4,0.5+uv.y*0.04 - mod(t+uv.y+o,0.5)*5.5);
       
    fragColor = vec4( c, 1.0 );
}
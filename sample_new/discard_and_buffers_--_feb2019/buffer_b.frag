
void mainImage( out vec4 O, vec2 u )
{
    float t = iTime, l, p;
    vec2 R = iResolution.xy,
         U = ( 2.*u - R ) / R.y,
         P = vec2(cos(t+sin(2.7*t)),sin(t/.97) );
    p = 2./R.y;
    l = length(P-U);
    if (l>.1) discard;
  //if (l>.1) if (U.x<0.) discard; else return;
    
    O[iFrame%2] += smoothstep(0.,p,abs(l-.1+p));
}
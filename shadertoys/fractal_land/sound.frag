vec2 mainSound( in int samp,float time)
{
    time=mod(time-5.,12.);
	return vec2( fract(sin(6.2831*440.0*time)*100.)*exp(-1.0*time))*min(1.,time);
}
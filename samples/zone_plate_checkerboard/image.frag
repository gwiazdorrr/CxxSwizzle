
// Text rendering code copied/pasted from https://www.shadertoy.com/view/4dtGD2 by Hamneggs
#define _f float
const lowp _f CH_A    = _f(0x69f99), CH_B    = _f(0x79797), CH_C    = _f(0xe111e),
       	  	  CH_D    = _f(0x79997), CH_E    = _f(0xf171f), CH_F    = _f(0xf1711),
		  	  CH_G    = _f(0xe1d96), CH_H    = _f(0x99f99), CH_I    = _f(0xf444f),
		  	  CH_J    = _f(0x88996), CH_K    = _f(0x95159), CH_L    = _f(0x1111f),
		  	  CH_M    = _f(0x9f999), CH_N    = _f(0x9bd99), CH_O    = _f(0x69996),
		  	  CH_P    = _f(0x79971), CH_Q    = _f(0x69b5a), CH_R    = _f(0x79759),
		  	  CH_S    = _f(0xe1687), CH_T    = _f(0xf4444), CH_U    = _f(0x99996),
		  	  CH_V    = _f(0x999a4), CH_W    = _f(0x999f9), CH_X    = _f(0x99699),
    	  	  CH_Y    = _f(0x99e8e), CH_Z    = _f(0xf843f), CH_0    = _f(0x6bd96),
		  	  CH_1    = _f(0x46444), CH_2    = _f(0x6942f), CH_3    = _f(0x69496),
		  	  CH_4    = _f(0x99f88), CH_5    = _f(0xf1687), CH_6    = _f(0x61796),
		  	  CH_7    = _f(0xf8421), CH_8    = _f(0x69696), CH_9    = _f(0x69e84),
		  	  CH_APST = _f(0x66400), CH_PI   = _f(0x0faa9), CH_UNDS = _f(0x0000f),
		  	  CH_HYPH = _f(0x00600), CH_TILD = _f(0x0a500), CH_PLUS = _f(0x02720),
		  	  CH_EQUL = _f(0x0f0f0), CH_SLSH = _f(0x08421), CH_EXCL = _f(0x33303),
		  	  CH_QUES = _f(0x69404), CH_COMM = _f(0x00032), CH_FSTP = _f(0x00002),
    	  	  CH_QUOT = _f(0x55000), CH_BLNK = _f(0x00000), CH_COLN = _f(0x00202),
			  CH_LPAR = _f(0x42224), CH_RPAR = _f(0x24442);
const lowp vec2 MAP_SIZE = vec2(4,5);
#undef flt
/*
	returns the status of a bit in a bitmap. This is done value-wise, so
	the exact representation of the float doesn't really matter.
*/
float getBit( in float map, in float index )
{
    // Ooh -index takes out that divide :)
    return mod( floor( map*exp2(-index) ), 2.0 );
}
float drawChar( in float char, in vec2 pos, in vec2 size, in vec2 uv )
{
    // Subtract our position from the current uv so that we can
    // know if we're inside the bounding box or not.
    uv-=pos;
    
    // Divide the screen space by the size, so our bounding box is 1x1.
    uv /= size;    
    
    // Create a place to store the result.
    float res;
    
    // Branchless bounding box check.
    res = step(0.0,min(uv.x,uv.y)) - step(1.0,max(uv.x,uv.y));
    
    // Go ahead and multiply the UV by the bitmap size so we can work in
    // bitmap space coordinates.
    uv *= MAP_SIZE;
    
    // Get the appropriate bit and return it.
    res*=getBit( char, 4.0*floor(uv.y) + floor(uv.x) );
    return clamp(res,0.0,1.0);
}

vec2 movement = vec2(0.0);

float zone_plate(vec2 fragCoord) {
    fragCoord += movement;
    vec2 uv = fragCoord/iResolution.xy * 2.0 - 1.0;
    uv.x /= iResolution.y / iResolution.x;
    
    float dist = sqrt(dot(uv,uv));
    float freq = dist * iResolution.y / 4.0;
	float c = cos(3.1415926 * dist * freq) * 0.5 + 0.5;
    return c;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    movement = vec2(6.0 * sin(iTime), 6.0 * cos(iTime));
    
    bool gt = fragCoord.x < iResolution.x / 2.0 && fragCoord.y < iResolution.y / 2.0;
    bool checker = fragCoord.x > iResolution.x / 2.0 && fragCoord.y > iResolution.y / 2.0;
    bool halfres = fragCoord.x > iResolution.x / 2.0 && fragCoord.y < iResolution.y / 2.0;    
    bool resample = fragCoord.x < iResolution.x / 2.0 && fragCoord.y > iResolution.y / 2.0;
    
    fragCoord = floor(fragCoord);
    vec2 uv = fragCoord/iResolution.xy * 2.0 - 1.0;
    uv.x /= iResolution.y / iResolution.x;
    
    ivec2 fragCoordInt = ivec2(fragCoord);
    
    float col = zone_plate(fragCoord);
    if (checker) {
    	if ((fragCoordInt.x & 1) == (fragCoordInt.y & 1)) {
            float samp1 = zone_plate(fragCoord + vec2(0.0, 1.0));
        	float samp2 = zone_plate(fragCoord + vec2(1.0, 0.0));
            float samp3 = zone_plate(fragCoord + vec2(-1.0, 0.0));
            float samp4 = zone_plate(fragCoord + vec2(0.0, -1.0));
           	col = (samp1 + samp2 + samp3 + samp4) / 4.0;
    	}
    } else_if (halfres) {
        vec2 resampling_factor = vec2(0.5, 1.0);
        vec2 resampled_coord = fragCoord * resampling_factor;
        vec2 interp = fract(resampled_coord);
        vec2 floor_coord = floor(resampled_coord) / resampling_factor;
        vec2 ceil_coord = ceil(resampled_coord) / resampling_factor;
        col = zone_plate(floor_coord) * (1.0 - interp.x)
            + zone_plate(ceil_coord) * (interp.x);
    } else_if (resample) {
        float resampling_factor = 1.0 / sqrt(2.0);
        vec2 resampled_coord = fragCoord * resampling_factor;
        vec2 interp = fract(resampled_coord);
        vec2 floor_coord = floor(resampled_coord) / resampling_factor;
        vec2 ceil_coord = ceil(resampled_coord) / resampling_factor;
        col = zone_plate(floor_coord) * (1.0 - interp.x) * (1.0 - interp.y)
            + zone_plate(ceil_coord) * (interp.x) * (interp.y)
            + zone_plate(vec2(floor_coord.x, ceil_coord.y)) * (1.0 - interp.x) * (interp.y)
            + zone_plate(vec2(ceil_coord.x, floor_coord.y)) * (interp.x) * (1.0 - interp.y);
    }
    
    float dist = sqrt(dot(uv,uv));
    if (dist > 1.0)
        col = 0.0;
    vec3 bg = gt ? vec3(0.0, 1.0, 0.0) : checker ? 
        vec3(0.0, 0.0, 1.0) : halfres ? vec3(1.0, 0.0, 0.0) : vec3(1.0, 1.0, 0.0);
    
    vec3 outputCol = mix(vec3(col,col,col), bg, vec3(abs(uv.x) > 1.0 ? 1.0 : 0.0));
    
    // Set a general character size...
    vec2 charSize = vec2(.03*2.0, .0375*2.0);
    // and a starting position.
    vec2 charPos = vec2(0.9, 0.80);
    // Draw some text
    float chr = 0.0;
    // Bitmap text rendering
    chr += drawChar( CH_C, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_H, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_E, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_C, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_K, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_E, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_R, charPos, charSize, uv); charPos.x += .04 * 2.0;
    charPos = vec2(-1.4, 0.80);
    chr += drawChar( CH_R, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_E, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_S, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_A, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_M, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_P, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_L, charPos, charSize, uv); charPos.x += .04 * 2.0;
    charPos = vec2(-1.4, -0.80);
    chr += drawChar( CH_F, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_U, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_L, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_L, charPos, charSize, uv); charPos.x += .04 * 2.0;
    charPos = vec2(0.9, -0.80);
    chr += drawChar( CH_S, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_T, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_R, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_E, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_T, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_C, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_H, charPos, charSize, uv); charPos.x += .04 * 4.0;
    chr += drawChar( CH_1, charPos, charSize, uv); charPos.x += .04 * 2.0;
    chr += drawChar( CH_D, charPos, charSize, uv); charPos.x += .04 * 2.0;
    // Output to screen
    fragColor = vec4(mix(outputCol, vec3(0.5), chr), 1.0);
}
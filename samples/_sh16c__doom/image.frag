
//   _____ _               _           _               ______           ___  ___
//  /  ___| |             | |         | |              |  _  \          |  \/  |
//  \ `--.| |__   __ _  __| | ___ _ __| |_ ___  _   _  | | | |___   ___ | .  . |
//   `--. \ '_ \ / _` |/ _` |/ _ \ '__| __/ _ \| | | | | | | / _ \ / _ \| |\/| |
//  /\__/ / | | | (_| | (_| |  __/ |  | || (_) | |_| | | |/ / (_) | (_) | |  | |
//  \____/|_| |_|\__,_|\__,_|\___|_|   \__\___/ \__, | |___/ \___/ \___/\_|  |_/
//                                               __/ |                          
//                                              |___/                           

// https://www.shadertoy.com/view/lldGDr

// @P_Malin

// Entry for shadertoy 2016 competetion (Part C)
// YouTube video here: https://www.youtube.com/watch?v=ETDjQ1itiNE

// I worked on this a lot after the week we had for the competition.
// I think the earlier verion I submitted for the competition was like this
// https://www.shadertoy.com/view/ldscz7

// Note - Exit sign texture is disabled - enable it in Buffer D by removing #define LINUX_WORKAROUND

// Comment out for faster rendering:
#define HUD_MESSAGES
#define FULL_HUD

//#define SHOW_SPRITES
//#define SHOW_MAP_DATA

// Enables map on Tab key (slow):
// Also need define in buf c
#define ALLOW_MAP
//#define HIRES_MAP

// Controls: 

// Press Space / Enter to Start!
// Cursors - Turn
// WASD - Move
// Space - Fire
// Enter/F - Open Door
// Shift - Run
// Alt - Strafe
// L - Mouse look
// I - Invert Y

// ( see ReadControls() in  buffer C )


// Note: I've added a bunch of additional stuff to this shader since the competition deadline
//
// * Fixed framerate issue on some machines
// * Fixed some state machine timer logic
// * Enemy sprites for different compass directions
// * Changed aspect ratio
// * Added mouse control + mouselook
// * Sergeants carry shotguns
// * Added sprites for more enemy states
// * Fixed sky rendering
// * Added viemodel
// * Added enemy model textures sprites
// * Added muzzle flash sprite
// * Improved barrel explode sprite
// * Impreove imp fireball sprite
// * Improved HUD and added numbers font
// * Added messages for pickups etc.

// Music : https://soundcloud.com/plasma3/doom-remix-at-dooms-gate-level-1-theme-e1m1

// Thanks to adx and morimea for investigating the linux version.

// HUD / UI / Viewmodel

#define MAP_CHANNEL iChannel0
#define STATE_CHANNEL iChannel1

// ----------------- 8< -------------------------- 8< -------------------------- 8< --------------
// Common code follows

#define NO_UNROLL(X) (X + min(0,iFrame))
#define NO_UNROLLU(X) (X + uint(min(0,iFrame)))

bool Map_PointInSector( sampler2D mapSampler, vec2 vPos, Sector sector )
{
    float fInOutTest = 0.0;
    
    for(int iSideDefIndex=0; iSideDefIndex< NO_UNROLL( sector.iSideDefCount ); iSideDefIndex++)
    {        
        SideDef sideDef = Map_ReadSideDefInfo( mapSampler, sector.iSectorId, iSideDefIndex );
        
        vec2 vPosToA = sideDef.vA - vPos;
        vec2 vPosToB = sideDef.vB - vPos;
        
        if ( vPosToA.y > 0.0 && vPosToB.y > 0.0 )
        	continue;
        if ( vPosToB.y <= 0.0 && vPosToA.y <= 0.0 )
        	continue;               
        
        float fCross = Cross2d( vPosToA, vPosToB );
        if ( vPosToA.y > vPosToB.y )
            fCross = -fCross;
        if ( fCross < 0.0 )
        {
            fInOutTest++;
        }        
    }
    
    return mod( fInOutTest, 2.0 ) >= 1.0;    
}

bool Map_PointInSector( sampler2D mapSampler, MapInfo mapInfo, vec2 vPos, int iSectorId )
{
    if ( !Map_ValidSectorId(mapInfo, iSectorId) )
    {
        return false;
    }

    Sector sector = Map_ReadSector( mapSampler, iSectorId );

    return Map_PointInSector( mapSampler, vPos, sector );
}

int Map_SeekSector( sampler2D mapSampler, MapInfo mapInfo, vec2 vPos )
{    
    for(int iSectorIndex=0; iSectorIndex<NO_UNROLL( mapInfo.iSectorCount ); iSectorIndex++)
    {        
        if ( Map_PointInSector( mapSampler, mapInfo, vPos, iSectorIndex ) )
        {
            return iSectorIndex;
        }        
    }

    return SECTOR_NONE;
}


void Map_UpdateSector( sampler2D mapSampler, MapInfo mapInfo, vec2 vPrev, vec2 vPos, inout int iSectorId )
{    
    if ( vPrev == vPos )
    {
        return;
    }
    
    if ( !Map_PointInSector( mapSampler, mapInfo, vPos, iSectorId ) )
    {
        int iNewSectorId = Map_SeekSector( mapSampler, mapInfo, vPos );
        
        if ( iNewSectorId != SECTOR_NONE )
        {
        	iSectorId = iNewSectorId;
        }                
    }
}

///////////////////////////
// Trace
///////////////////////////

TraceResult Map_Trace( sampler2D mapSampler, MapInfo mapInfo, vec3 vRayOrigin, vec3 vRayDir, int iSectorId, float fMaxDist )
{
    TraceResult result;
    
    result.iTexture = 0u;
    result.vTexture = vec3(0.0);
    result.fDist = 0.0;
    result.fLightLevel = 0.0;
    result.iSector = SECTOR_NONE;
    
    while(true)
    {
        if ( iSectorId == SECTOR_NONE )
        {
            break;
        }
        
        Sector sector = Map_ReadSector( mapSampler, iSectorId );
        
        TraceSectorState sectorState;
        sectorState.iNextSector = SECTOR_NONE;        
        sectorState.fEnterDist = result.fDist;
        sectorState.bNoUpper = false;
        result.fDist = fMaxDist;
        result.iSector = iSectorId;
        
        for(int iSideDefIndex=0; iSideDefIndex<NO_UNROLL( sector.iSideDefCount); iSideDefIndex++)
        {
            SideDef sideDef = Map_ReadSideDefInfo( mapSampler, iSectorId, iSideDefIndex );                        

            TraceSideDef( mapSampler, vRayOrigin, vRayDir, result, sectorState, sector, sideDef );
        }

        // Test floor / ceiling
        if ( sectorState.fExitY < sector.fFloorHeight )
        {
            result.fDist = (sector.fFloorHeight - vRayOrigin.y) / vRayDir.y;
            vec3 vFloorPos = vRayOrigin + vRayDir * result.fDist;
            result.vTexture.st = vec2( vFloorPos.xz );
            result.iTexture = sector.iFloorTexture;
            result.fLightLevel = 0.0;
            sectorState.iNextSector = SECTOR_NONE;
        }
        else
        if ( sectorState.fExitY > sector.fCeilingHeight && !sectorState.bNoUpper )
        {
            result.fDist = (sector.fCeilingHeight - vRayOrigin.y) / vRayDir.y;
            vec3 vCeilingPos = vRayOrigin + vRayDir * result.fDist;
            result.vTexture.st = vec2( vCeilingPos.xz );
            result.iTexture = sector.iCeilingTexture;
            result.fLightLevel = 0.0;
            sectorState.iNextSector = SECTOR_NONE;
        }        

        iSectorId = sectorState.iNextSector;
    }
    
    if ( result.iTexture == 1u )
        result.fDist = FAR_CLIP;
    
    return result;
}


// End of common code
// ----------------- 8< -------------------------- 8< -------------------------- 8< --------------


// ----------------- 8< -------------------------- 8< -------------------------- 8< --------------
// Start of Font code

const ivec3 _SPACE_ = ivec3(0,0,5);
const ivec3 _EXCLAMATION_ = ivec3(49539,384,4);
const ivec3 _QUOTE_ = ivec3(2331,0,7);
const ivec3 _HASH_ = ivec3(167818,1311,7);
const ivec3 _DOLLAR_ = ivec3(508575,69524,7);
const ivec3 _PERCENT_ = ivec3(232984,4487,8);
const ivec3 _AMPERSAND_ = ivec3(249630,8095,8);
const ivec3 _APOSTROPHE_ = ivec3(259,0,4);
const ivec3 _L_PAREN_ = ivec3(115614,3847,7);
const ivec3 _R_PAREN_ = ivec3(462351,1948,7);
const ivec3 _ASTERISK_ = ivec3(509700,526,7);
const ivec3 _PLUS_ = ivec3(114944,2,5);
const ivec3 _COMMA_ = ivec3(0,33152,4);
const ivec3 _MINUS_ = ivec3(245760,0,6);
const ivec3 _PERIOD_ = ivec3(0,384,4);
const ivec3 _SLASH_ = ivec3(232984,391,7);
const ivec3 _0_ = ivec3(843678,3903,8);
const ivec3 _1_ = ivec3(99206,774,5);
const ivec3 _2_ = ivec3(1039935,8067,8);
const ivec3 _3_ = ivec3(1023007,4024,8);
const ivec3 _4_ = ivec3(511387,3096,7);
const ivec3 _5_ = ivec3(508319,3998,7);
const ivec3 _6_ = ivec3(1033150,7987,8);
const ivec3 _7_ = ivec3(793663,6192,8);
const ivec3 _8_ = ivec3(498111,8115,8);
const ivec3 _9_ = ivec3(1038751,4024,8);
const ivec3 _COLON_ = ivec3(3,384,4);
const ivec3 _SEMICOLON_ = ivec3(384,33152,4);
const ivec3 _LESSTHAN_ = ivec3(98816,66307,5);
const ivec3 _EQUALS_ = ivec3(114688,896,5);
const ivec3 _GREATERTHAN_ = ivec3(49280,16774,5);
const ivec3 _QUESTION_MARK_ = ivec3(925568,114751,8);
const ivec3 _AT_ = ivec3(1532350,65237,9);
const ivec3 _A_ = ivec3(1038782,6579,8);
const ivec3 _B_ = ivec3(515135,8123,8);
const ivec3 _C_ = ivec3(115646,7943,8);
const ivec3 _D_ = ivec3(842783,4027,8);
const ivec3 _E_ = ivec3(1034174,7951,8);
const ivec3 _F_ = ivec3(508863,387,8);
const ivec3 _G_ = ivec3(902078,7991,8);
const ivec3 _H_ = ivec3(1038771,6579,8);
const ivec3 _I_ = ivec3(49539,387,4);
const ivec3 _J_ = ivec3(792624,8120,8);
const ivec3 _K_ = ivec3(249267,6555,8);
const ivec3 _L_ = ivec3(49539,8071,8);
const ivec3 _M_ = ivec3(2096099,12779,9);
const ivec3 _N_ = ivec3(1039287,7615,8);
const ivec3 _O_ = ivec3(843678,3903,8);
const ivec3 _P_ = ivec3(1039423,387,8);
const ivec3 _Q_ = ivec3(843678,790335,8);
const ivec3 _R_ = ivec3(515135,6587,8);
const ivec3 _S_ = ivec3(508319,3992,7);
const ivec3 _T_ = ivec3(198207,1548,8);
const ivec3 _U_ = ivec3(842163,3903,8);
const ivec3 _V_ = ivec3(232859,526,7);
const ivec3 _W_ = ivec3(2094563,15359,9);
const ivec3 _X_ = ivec3(466807,15294,9);
const ivec3 _Y_ = ivec3(498099,1548,8);
const ivec3 _Z_ = ivec3(232991,3975,7);
const ivec3 _L_SQUARE_BRACKET_ = ivec3(49543,899,5);
const ivec3 _BACKSLASH_ = ivec3(230275,3100,7);
const ivec3 _R_SQUARE_BRACKET_ = ivec3(99079,902,5);
const ivec3 _CARET_ = ivec3(444164,0,7);
const ivec3 _UNDERSCORE_ = ivec3(0,1032192,8);

float Font_DecodeBitmap( vec2 vCoord, ivec3 vCharacter )
{
    vCoord = floor( vCoord );

    int iRow = int(vCoord.y) - 1;
    int iCol = int(vCoord.x) - 1;
    
    if ( iRow < 0 || iRow >= 6 ) return 0.0;
    if ( iCol < 0 || iCol >= 7 ) return 0.0;
    
    int iRowBits = 0;
        
   	if ( iRow == 0 ) 			iRowBits = vCharacter.x;
    else  if ( iRow == 1 ) 		iRowBits = vCharacter.x / 128;
    else  if ( iRow == 2 ) 		iRowBits = vCharacter.x / 16384;
    else  if ( iRow == 3 ) 		iRowBits = vCharacter.y;
    else  if ( iRow == 4 ) 		iRowBits = vCharacter.y / 128;
    else 						iRowBits = vCharacter.y / 16384;
      
    return (iRowBits & (1 << iCol )) == 0 ? 0.0 : 1.0;
}


struct PrintState
{
    vec2 vTexCoord;
    vec2 vOrigin;
    vec2 vPos;
    
    vec3 vColor;
    vec3 vOutline;
};
    
void Print_MoveTo( inout PrintState printState, vec2 vPos )
{
    printState.vOrigin = printState.vTexCoord - vPos;
    printState.vPos = printState.vOrigin;
}

void Print_Newline( inout PrintState printState )
{
    printState.vPos.x = printState.vOrigin.x;
    printState.vPos.y -= 8.0;
}

void Print_Color( inout PrintState printState, vec3 vColor )
{
    printState.vColor = vColor;
}

void Print_Init( out PrintState printState, vec2 vTexCoord )
{
    printState.vTexCoord = vTexCoord;        
    printState.vOrigin = vTexCoord;
    printState.vPos = vTexCoord;
    printState.vColor = vec3(1,0,0);
    printState.vOutline = vec3(.3,0,0);
    Print_MoveTo( printState, vec2(0) ); 
}

void Print_Space( inout PrintState printState )
{
    printState.vPos.x -= 5.0;
}

bool Print_Test( inout PrintState printState, vec3 vCharacter, float fSpacing )
{
    if ( vCharacter.z == -1.0 )
    {
        Print_MoveTo( printState, vCharacter.xy );
        return false;
    }
    
    if ( printState.vPos.x < vCharacter.z )
        return true;
         
	printState.vPos.x -= vCharacter.z + fSpacing;
    return false;
}

void Print_Char( inout PrintState printState, inout vec3 vResult, ivec3 vCharacter )
{
    float fBitmap = Font_DecodeBitmap( printState.vPos, vCharacter );
    if ( fBitmap > 0.0 ) vResult = printState.vColor;
    
    printState.vPos.x -= float(vCharacter.z - 1);
}

void Print_HudChar( inout PrintState printState, inout vec3 vResult, ivec3 vCharacter )
{
    float fBitmap = Font_DecodeBitmap( printState.vPos, vCharacter );
    float fShadow = Font_DecodeBitmap( printState.vPos - vec2( 1, 1), vCharacter );

    if ( fBitmap > 0.0 ) vResult = printState.vColor * 0.5 + 0.5 * (printState.vPos.y / 8.);
    else if ( fShadow > 0.0 ) vResult *= 0.5;
    
    printState.vPos.x -= float(vCharacter.z);
}

void Print_FancyChar( inout PrintState printState, inout vec3 vResult, ivec3 vCharacter )
{
    float fBitmapTL = Font_DecodeBitmap( printState.vPos - vec2(-1,-1), vCharacter );
    float fBitmapTC = Font_DecodeBitmap( printState.vPos - vec2( 0,-1), vCharacter );
    float fBitmapTR = Font_DecodeBitmap( printState.vPos - vec2( 1,-1), vCharacter );

    float fBitmapCL = Font_DecodeBitmap( printState.vPos - vec2(-1, 0), vCharacter );
    float fBitmapCC = Font_DecodeBitmap( printState.vPos - vec2( 0, 0), vCharacter );
    float fBitmapCR = Font_DecodeBitmap( printState.vPos - vec2( 1, 0), vCharacter );
    
    float fBitmapBL = Font_DecodeBitmap( printState.vPos - vec2(-1, 1), vCharacter );
    float fBitmapBC = Font_DecodeBitmap( printState.vPos - vec2( 0, 1), vCharacter );
    float fBitmapBR = Font_DecodeBitmap( printState.vPos - vec2( 1, 1), vCharacter );
    
    float fOutline 	= min( 1., fBitmapTL + fBitmapTC + fBitmapTR + fBitmapCL + fBitmapCR + fBitmapBL + fBitmapBC + fBitmapBR );    
    float fShade = (fBitmapTL * .5 - fBitmapBR * .5 ) + 0.5;
    
    if ( fBitmapCC > 0.0 ) vResult = printState.vColor * (fShade * 0.25 + 0.75);
    else
    if ( fOutline > 0.0 ) vResult = printState.vOutline;
    
    printState.vPos.x -= float(vCharacter.z);
}

// End of font code
// ----------------- 8< -------------------------- 8< -------------------------- 8< --------------



Entity g_playerEnt;
GameState g_gameState;

float hash2D(vec2 p)    
{
	return Hash( dot( p, vec2(1.0, 41.0) ) );	
}

float noise1D( float p )
{
	float fl = floor(p);
	
	float h0 = Hash( fl );
	float h1 = Hash( fl + 1.0 );
	
	float fr = p - fl;
	float fr2 = fr * fr;
	float fr3 = fr2 * fr;
	
	float t1 = 3.0 * fr2 - 2.0 * fr3;	
	float t0 = 1.0 - t1;
	
	return h0 * t0
		 + h1 * t1;
}

float noise2D( vec2 p, float r )
{
	vec2 fl = floor(p);

	float h00 = hash2D( mod(fl + vec2(0.0, 0.0), r) );
	float h10 = hash2D( mod(fl + vec2(1.0, 0.0), r) );
	float h01 = hash2D( mod(fl + vec2(0.0, 1.0), r) );
	float h11 = hash2D( mod(fl + vec2(1.0, 1.0), r) );
	
	vec2 fr = p - fl;
	
	vec2 fr2 = fr * fr;
	vec2 fr3 = fr2 * fr;
	
	vec2 t1 = 3.0 * fr2 - 2.0 * fr3;	
	vec2 t0 = 1.0 - t1;
	
	return h00 * t0.x * t0.y
		 + h10 * t1.x * t0.y
		 + h01 * t0.x * t1.y
		 + h11 * t1.x * t1.y;
}

float fbm( vec2 p, float per )
{
	float val = 0.0;
	float tot = 0.0;
	float mag = 0.5;

	p += 0.5;
	p = p * (1.0 / 8.0);
	val += noise2D(p, 4.0) * mag; tot+=mag; p=p*2.0 + 1.234; mag*=per;	
	val += noise2D(p, 8.0) * mag; tot+=mag; p=p*2.0 + 2.456; mag*=per;
	val += noise2D(p, 16.0) * mag; tot+=mag; p=p*2.0 + 3.678; mag*=per;
	val += noise2D(p, 32.0) * mag; tot+=mag;

	return val * (1.0 / tot);
}

vec3 Tex( vec2 vTexCoord )
{
    vec2 vSize = vec2(64.0);    
    float fPersistence = 0.8;
	float fNoise2Freq = 0.5;
    
    vTexCoord = floor( vTexCoord );
    
	float fRandom = fbm( vTexCoord, fPersistence );
    
	vec3 col = mix( vec3(63.0, 47.0, 23.0), vec3(147.0, 123.0, 99.0), fRandom) / 255.0;
	
	return col;    
}

void VWipe( inout vec2 vPixel, float fFade, vec2 vSize )
{
    float fEffectTime = max( 0.0, fFade );
    float fEffectOffset = max(fEffectTime - 1.0, 0.0) - Hash(floor(vPixel.x)) * 0.4;

    fEffectOffset = clamp(fEffectOffset, 0.0, 1.0);
    	
    vPixel.y += fEffectOffset * vSize.y;
}

//////////////////////////////////////////////////////////////
// Raymarching


struct SceneResult
{
	float fDist;
	float fObjectId;
    vec3 vUVW;
};

SceneResult Scene_Union( const in SceneResult a, const in SceneResult b )
{
    if ( a.fDist < b.fDist )
    {
        return a;
    }
    return b;
}
    
SceneResult Scene_Subtract( const in SceneResult a, const in SceneResult b )
{
    if ( -a.fDist < b.fDist )
    {
        return a;
    }

    SceneResult result;
    result.fDist = -b.fDist;
    result.fObjectId = b.fObjectId;
    result.vUVW = b.vUVW;
    return result;
}

SceneResult Scene_Intersection( const in SceneResult a, const in SceneResult b )
{
    if ( a.fDist > b.fDist )
    {
        return a;
    }
    return b;
}
    
#ifndef CXXSWIZZLE
SceneResult Scene_GetDistance( const vec3 vPos );    
#endif

vec3 Scene_GetNormal(const in vec3 vPos)
{
    const float fDelta = 0.001;
    vec2 e = vec2( -1, 1 );
    
    vec3 vNormal = 
        Scene_GetDistance( vPos + e.yxx * fDelta ).fDist * e.yxx + 
        Scene_GetDistance( vPos + e.xxy * fDelta ).fDist * e.xxy + 
        Scene_GetDistance( vPos + e.xyx * fDelta ).fDist * e.xyx + 
        Scene_GetDistance( vPos + e.yyy * fDelta ).fDist * e.yyy;
    
    if ( dot( vNormal, vNormal ) < 0.00001 )
    {
        return vec3(0, 1, 0);
    }
    
    return normalize( vNormal );
}    

SceneResult Scene_Trace( const in vec3 vRayOrigin, const in vec3 vRayDir, float maxDist )
{	
    SceneResult result;
    result.fDist = 0.0;
    result.vUVW = vec3(0.0);
    result.fObjectId = 0.0;
    
	float t = 0.1;
	const int kRaymarchMaxIter = 128;
	for(int i=0; i<kRaymarchMaxIter; i++)
	{		
		result = Scene_GetDistance( vRayOrigin + vRayDir * t );		
        t += result.fDist;

        if ( abs(result.fDist) < 0.001 )
		{
			break;
		}		
        if ( t > maxDist )
        {
            result.fObjectId = -1.0;
	        t = maxDist;
            break;
        }
	}
    
    result.fDist = t;

    return result;
}

vec2 Segment_Internal( vec3 vPos, vec3 vP0, vec3 vP1 )
{
	vec3 pa = vPos - vP0;
	vec3 ba = vP1 - vP0;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	
	return vec2( length( pa - ba*h ), h );
}

float Segment( vec3 vPos, vec3 vP0, vec3 vP1, float r0, float r1 )
{
    vec2 vC = Segment_Internal( vPos, vP0, vP1 );
    
    return vC.x - mix(r0, r1, vC.y);
}

#define MAT_BG 0.0
#define MAT_SHOTGUN 1.0
#define MAT_PISTOL 2.0

SceneResult Scene_GetDistance( const vec3 vPos )
{
    vec3 vWeaponPos = vPos;
    
    if ( g_playerEnt.fHealth <= 0.0 )
    {    
        float fDeathFall = 0.0;

        // Death
        if( g_playerEnt.fHealth <= 0.0 )
        {
            fDeathFall = 1.0 - (g_playerEnt.fTimer / 1.5);
        }

    	vWeaponPos.y += fDeathFall * 5.0;
    }        
    
    
    if ( g_playerEnt.fUseWeapon == 2.0 )
    {
	    vWeaponPos += vec3(0.0, 0.0, 1.0) * pow( g_playerEnt.fTimer, 3.0) * 5.0 ;
        
        float fDist = Segment( vWeaponPos, vec3(0,-1.0,2.5), vec3(0, -5, 25), .2, 0.2 );
        float fDist2 =  Segment( vWeaponPos, vec3(0, -4.5, 23), vec3(0, -4.5, 25), .1, 0.1 );
        fDist = min( fDist, fDist2 );
        return SceneResult( fDist, MAT_SHOTGUN, vec3(0.0) );    
    }
    
    // pistol
    vWeaponPos += vec3(0.0, 0.0, 1.0) * pow( g_playerEnt.fTimer, 3.0) * 10.0 ;
    
    float fDist = Segment( vWeaponPos, vec3(0,-0.8,0), vec3(0, -5, 20), .1, 0.1 );
	return SceneResult( fDist, MAT_PISTOL, vec3(0.0) );    
    
    //return SceneResult( length(vPos - vec3(0.0, 1.0, 5.0)) - 0.5, MAT_SHOTGUN, vec3(0.0) );
}

vec4 MuzzleFlash( vec2 vUV, vec2 vPos, vec2 vSize )
{
    vec4 vResult = vec4( 0.0 );
    vec2 vLocalPos = vUV - vPos;
    vLocalPos = vLocalPos / vSize;
    float fDist = length( vLocalPos );
    fDist += abs(vLocalPos.x) + abs(vLocalPos.y);

    float fFactor = fDist * 5.0 + fbm( vLocalPos * 100., 0.8);

    //float fAmount = exp2( * fFactor );

    if ( fFactor < 3.0)
    {
        fFactor = 1.0 - fFactor / 3.0;

        fFactor = pow( 0.5 + fFactor, 5.0 );

        vResult.rgb = vec3( fFactor, pow(fFactor, 2.0) * .4, pow(fFactor, 3.0) * .2) * 5.0;
        vResult.a = 1.0;

    }
    
    return vResult;
}

vec4 ViewmodelSprite( vec2 vUV, float fLightLevel )
{
    vec4 vResult = vec4( 0 );
    
    vec2 vBoundsMin = vec2(0);
    vec2 vBoundsMax = vec2(1);
    
    if ( g_playerEnt.fUseWeapon == 2.0 )
    {
    	vBoundsMin = vec2(0.4, 0.0);
    	vBoundsMax = vec2(0.6,0.25);
    }
    else
    {
    	vBoundsMin = vec2(0.47, 0.0);
    	vBoundsMax = vec2(0.53,0.15);
    }
    
    if ( all( greaterThanEqual( vUV, vBoundsMin ) ) &&
         all( lessThan( vUV, vBoundsMax ) ) )
    {       
        vec3 vCameraPos = vec3(0.0, 0.0, 0.0);    
        vec2 vWindowPos = vec2(vUV.x * 2.0 - 1.0, vUV.y * 2.0 - 1.0);
        vec3 vCameraDir = normalize( vec3(vWindowPos, 3.0) - vCameraPos );

        SceneResult sceneResult = Scene_Trace( vCameraPos, vCameraDir, 1000.0 );

        if ( sceneResult.fDist < 400.0 )
        {

            vec3 vHitPos = vCameraPos + vCameraDir * sceneResult.fDist;

            vec3 vNormal = Scene_GetNormal( vHitPos );

            vec3 vAmbientLight = vec3(0.1);
            vec3 vLightDir = normalize( vec3( 0.2, 0.3, 0.2 ) );
            vec3 vLightColor = vec3(1.0);

            float fShade = max( 0.0, dot( vNormal, vLightDir ) );

            vec3 vDiffuseCol = vec3(.1);
            float fSpecPow = 10.0;
            float fSpecIntensity = 200.0;

            if ( sceneResult.fObjectId == MAT_SHOTGUN )
            {
                vDiffuseCol = vec3(.1);
                fSpecPow = 10.0;
                fSpecIntensity = 200.0;        
            }
            else
            if ( sceneResult.fObjectId == MAT_PISTOL )
            {
                vDiffuseCol = vec3(.2);
                fSpecPow = 50.0;
                fSpecIntensity = 20.0;        

            }

            vec3 vDiffuseLight = vAmbientLight + fShade * vLightColor;
            vResult.rgb = vDiffuseCol * vDiffuseLight;

            vec3 vRefl = reflect( vCameraDir, vNormal );
            float fDot = max(0.0, dot( vRefl, vLightDir )) * fShade;
            float fSpec = pow( fDot, fSpecPow );
            vResult.rgb += fSpec * fSpecIntensity;

            vResult.rgb = 1.0 - exp2( vResult.rgb * -2.0 );
            vResult.rgb = pow( vResult.rgb, vec3(1.0 / 1.5) );
            vResult.rgb = clamp( vResult.rgb, vec3(0.0), vec3(1.0) );
            vResult.rgb *= fLightLevel;
            vResult.rgb = clamp( vResult.rgb, vec3(0.0), vec3(1.0) );

            vResult.a = 1.0;

            return vResult;
        }
        else
        {
            // Bounds debug
			//return vec4(1,0,0,1);            
        }
    }
    
    // muzzle flash sprite
       
    if(  g_playerEnt.fHealth > 0.0 )
    {    
        if ( g_playerEnt.fUseWeapon == 2.0 )
        {
       		if ( g_playerEnt.fTimer > 0.9 )
	    	{
                vec4 vMuzzleFlash = MuzzleFlash( vUV, vec2(0.5, 0.2), vec2(0.5, 0.5) );
                
                if ( vMuzzleFlash.a > 0.0 )
                {
                    return vMuzzleFlash;
                }
            }
        }
        else
        {
       		if ( g_playerEnt.fTimer > 0.45 )
	    	{
                vec4 vMuzzleFlash = MuzzleFlash( vUV, vec2(0.5, 0.12), vec2(0.25, 0.25) );
                
                if ( vMuzzleFlash.a > 0.0 )
                {
                    return vMuzzleFlash;
                }
            }
        }	            
	}
    
    return vResult;    
}

#define FONT_POS 	ivec2(280,0)
#define FONT_CHAR 	ivec2(16,0)


vec4 NumFont_Char( vec2 vCharUV, int iDigit )
{
 	if ( iDigit < 0 )
    	return vec4(0.0);
    
    ivec2 vTexCoord = ivec2(floor(vCharUV * vec2(14.0, 16.0))) + FONT_POS;
    vTexCoord += iDigit * FONT_CHAR;
        
    return texelFetch( iChannel2, vTexCoord, 0 );
}

vec4 PrintHUDPercent(const in vec2 vStringUV, const in float fValue )
{
    float fMaxDigits = 3.0;
    if ((vStringUV.y < 0.0) || (vStringUV.y >= 1.0)) return vec4(0.0);
	float fLog10Value = log2(abs(fValue)) / log2(10.0);
	float fBiggestIndex = max(floor(fLog10Value), 0.0);
	float fDigitIndex = fMaxDigits - floor(vStringUV.x);
	float fCharacter = -1.0;
    
	if(fDigitIndex > (-0.0 - 1.01)) {
		if(fDigitIndex <= fBiggestIndex) {
			if(fDigitIndex == -1.0) {
				fCharacter = 10.0; // Percent
			} else {
				float fDigitValue = (abs(fValue / (pow(10.0, fDigitIndex))));
                float kFix = 0.0001;
                fCharacter = floor(mod(kFix+fDigitValue, 10.0));
			}		
		}
	}
    
    return NumFont_Char( fract(vStringUV), int(fCharacter) );
}


vec4 GetHudText( vec2 vPos, float fHealth, float fArmor )
{    
    vPos = floor( vPos );
	vec4 vHealth = PrintHUDPercent( vec2( (vPos - vec2(33,12)) / vec2(14,16)), fHealth );
    if ( vHealth.a > 0.0 )
    	return vHealth;
    
	vec4 vArmor = PrintHUDPercent( vec2( (vPos - vec2(164,12)) / vec2(14,16)), fArmor );
    if ( vArmor.a > 0.0 )
    	return vArmor;
    
    return vec4(0.0);
}


vec4 SampleScene( vec2 vPixelCoord, vec2 vResolution, float fLightLevel )
{
    vec2 vScenePixelCoord = floor(vPixelCoord) - vec2(0, 32.0);
    vec4 vResult = texture( iChannel3, (vScenePixelCoord + 0.5) / iChannelResolution[2].xy );
    
    vec2 vQuantizedUV = floor(vScenePixelCoord) * (1.0 / vResolution);
    vec4 vViewmodelSprite = ViewmodelSprite( vQuantizedUV, fLightLevel );

    vResult = mix ( vResult, vViewmodelSprite, vViewmodelSprite.a );
    
    if ( vScenePixelCoord.y < 1.0 )
    {
        return vec4((vec3(70./256.)), 1.);
    }   
    
    return vResult;
}

vec3 DrawMap( vec2 vPixelCoord, vec2 vResolution )
{
    vec3 vResult = vec3(0.0);
    vec2 vScenePixelCoord = floor(vPixelCoord) - vec2(0, 31.0);
    
    float fScale = 5.0;
    
    vec2 vPixelWorldPos = (vScenePixelCoord - vec2(160,100)) * fScale + g_playerEnt.vPos.xz;

    MapInfo mapInfo = ReadMapInfo( MAP_CHANNEL );
    
    //if ( Key_IsToggled( KEY_TAB ) )
    {
        for(int iSectorIndex=0; iSectorIndex<mapInfo.iSectorCount; iSectorIndex++)
        {
            Sector sector = Map_ReadSector( MAP_CHANNEL, iSectorIndex );
            
            /*if ( Map_PointInSector( vPixelWorldPos, sector ) )
            {
                vResult = vec3(1,0,0);
            }*/
            

            for(int iSideDefIndex=0; iSideDefIndex<sector.iSideDefCount; iSideDefIndex++)
            {
                SideDef sideDef = Map_ReadSideDefInfo( MAP_CHANNEL, iSectorIndex, iSideDefIndex );                        

                vec2 vSideDir = normalize( sideDef.vB - sideDef.vA );

                float fProj = dot( vSideDir, vPixelWorldPos - sideDef.vA );
                fProj = clamp( fProj, 0.0, sideDef.fLength );
                
                vec2 vClosest = sideDef.vA + vSideDir * fProj;
                float fDist = length( vClosest - vPixelWorldPos );
                
                if (fDist < fScale * .5 )
                {
                    if ( sideDef.iNextSector != SECTOR_NONE )
                    {
	                    vResult = vec3(1,1,0);
                    }
                    else
                    {
	                    vResult = vec3(1,0,0);
                    }
                }
            }			
        }
    }    
    
    return vResult;
}

float Relief( vec2 vPos, vec2 vMin, vec2 vMax )
{
    vPos = floor(vPos);
    if ( (vPos.x == vMin.x ) && (vPos.y <= vMax.y && vPos.y >= vMin.y ) )
    {
	    return 1.3;
    }

    if ( (vPos.y == vMax.y ) && (vPos.x <= vMax.x && vPos.x >= vMin.x ) )
    {
	    return 1.4;
    }
        
    if ( (vPos.x == vMax.x ) && (vPos.y <= vMax.y && vPos.y >= vMin.y ) )
    {
	    return 0.4;
    }

    if ( (vPos.y == vMin.y ) && (vPos.x <= vMax.x && vPos.x >= vMin.x ) )
    {
	    return 0.7;
    }
    
    return 1.0;
}

#ifdef FULL_HUD

ivec3 GetMessageChar( int iMessage, int iChar )
{
	#define MESSAGE_CHAR(X) if ( iChar == 0 ) return X; iChar--
    
    if ( iMessage == EVENT_HEALTH_BONUS ||
       	iMessage == EVENT_ARMOR_BONUS || 
       	iMessage == EVENT_GREENARMOR || 
        iMessage == EVENT_BLUEARMOR ||
       iMessage == EVENT_MEDIKIT)
    {
        MESSAGE_CHAR( _P_ );
        MESSAGE_CHAR( _I_ );
        MESSAGE_CHAR( _C_ );
        MESSAGE_CHAR( _K_ );
        MESSAGE_CHAR( _E_ );
        MESSAGE_CHAR( _D_ );

        MESSAGE_CHAR( _SPACE_ );

        MESSAGE_CHAR( _U_ );
        MESSAGE_CHAR( _P_ );

        MESSAGE_CHAR( _SPACE_ );
    }
    else
    if( iMessage == EVENT_SHOTGUN )
    {
        MESSAGE_CHAR( _Y_ );
        MESSAGE_CHAR( _O_ );
        MESSAGE_CHAR( _U_ );
        
        MESSAGE_CHAR( _SPACE_ );

        MESSAGE_CHAR( _G_ );
        MESSAGE_CHAR( _O_ );
        MESSAGE_CHAR( _T_ );
        
        MESSAGE_CHAR( _SPACE_ );
    }
        
    
    if ( iMessage == EVENT_HEALTH_BONUS || iMessage == EVENT_ARMOR_BONUS || iMessage == EVENT_MEDIKIT)
    {    
        MESSAGE_CHAR( _A_ );
        if ( iMessage == EVENT_ARMOR_BONUS )
        {    
	        MESSAGE_CHAR( _N_ );
        }
        MESSAGE_CHAR( _SPACE_ );
    }
    else if ( iMessage == EVENT_GREENARMOR ||
            iMessage == EVENT_BLUEARMOR || 
            iMessage == EVENT_SHOTGUN )
    {
        MESSAGE_CHAR( _T_ );
        MESSAGE_CHAR( _H_ );
        MESSAGE_CHAR( _E_ );
        
        MESSAGE_CHAR( _SPACE_ );
    }

    if ( iMessage == EVENT_BLUEARMOR )
    {
        MESSAGE_CHAR( _M_ );
        MESSAGE_CHAR( _E_ );
        MESSAGE_CHAR( _G_ );
        MESSAGE_CHAR( _A_ );
    }
    
    if( iMessage == EVENT_SHOTGUN )
    {
        MESSAGE_CHAR( _S_ );
        MESSAGE_CHAR( _H_ );
        MESSAGE_CHAR( _O_ );
        MESSAGE_CHAR( _T_ );
        MESSAGE_CHAR( _G_ );
        MESSAGE_CHAR( _U_ );
        MESSAGE_CHAR( _N_ );
        
        MESSAGE_CHAR( _SPACE_ );
    }
    
    if ( iMessage == EVENT_HEALTH_BONUS )
    {    
        MESSAGE_CHAR( _H_ );
        MESSAGE_CHAR( _E_ );
        MESSAGE_CHAR( _A_ );
        MESSAGE_CHAR( _L_ );
        MESSAGE_CHAR( _T_ );
        MESSAGE_CHAR( _H_ );
    }
    else if ( iMessage == EVENT_ARMOR_BONUS || iMessage == EVENT_GREENARMOR || iMessage == EVENT_BLUEARMOR)
    {    
        MESSAGE_CHAR( _A_ );
        MESSAGE_CHAR( _R_ );
        MESSAGE_CHAR( _M_ );
        MESSAGE_CHAR( _O_ );
        MESSAGE_CHAR( _R_ );
    }
    else if ( iMessage == EVENT_MEDIKIT )
    {    
        MESSAGE_CHAR( _M_ );
        MESSAGE_CHAR( _E_ );
        MESSAGE_CHAR( _D_ );
        MESSAGE_CHAR( _I_ );
        MESSAGE_CHAR( _K_ );
        MESSAGE_CHAR( _I_ );
        MESSAGE_CHAR( _T_ );
    }
    
    if ( iMessage == EVENT_HEALTH_BONUS || iMessage == EVENT_ARMOR_BONUS)
    {    
        MESSAGE_CHAR( _SPACE_ );
        
        MESSAGE_CHAR( _B_ );
        MESSAGE_CHAR( _O_ );
        MESSAGE_CHAR( _N_ );
        MESSAGE_CHAR( _U_ );
        MESSAGE_CHAR( _S_ );
    }

    if ( iMessage == EVENT_BLUEARMOR || iMessage == EVENT_SHOTGUN )
    {
        MESSAGE_CHAR( _EXCLAMATION_ );
    }
    else
    {
        MESSAGE_CHAR( _PERIOD_ );
    }   
    
    return ivec3(0);
}


ivec3 GetHudTextChar( int iChar ) 
{

	#define HUD_TEXT_CHAR(X) if ( iChar == 0 ) return X; iChar--
    
    HUD_TEXT_CHAR( ivec3(6,189, -1) ); // MOVE

    HUD_TEXT_CHAR( _A_ );
    HUD_TEXT_CHAR( _M_ );
    HUD_TEXT_CHAR( _M_ );
    HUD_TEXT_CHAR( _O_ );

    HUD_TEXT_CHAR( ivec3(52,189, -1) ); // MOVE
    
    HUD_TEXT_CHAR( _H_ );
    HUD_TEXT_CHAR( _E_ );
    HUD_TEXT_CHAR( _A_ );
    HUD_TEXT_CHAR( _L_ );
    HUD_TEXT_CHAR( _T_ );
    HUD_TEXT_CHAR( _H_ );
    
    HUD_TEXT_CHAR( ivec3(109,189, -1) ); // MOVE

    HUD_TEXT_CHAR( _A_ );
    HUD_TEXT_CHAR( _R_ );
    HUD_TEXT_CHAR( _M_ );
    HUD_TEXT_CHAR( _S_ );
    
    HUD_TEXT_CHAR( ivec3(187,189, -1) ); // MOVE

    HUD_TEXT_CHAR( _A_ );
    HUD_TEXT_CHAR( _R_ );
    HUD_TEXT_CHAR( _M_ );
    HUD_TEXT_CHAR( _O_ );
    HUD_TEXT_CHAR( _R_ );

    return ivec3(0);
}


void PrintHudMessage( vec2 vTexCoord, int iMessage, inout vec3 vResult )
{
    if ( vTexCoord.y > 8.0 || vTexCoord.y < 0.0 || vTexCoord.x < 0.0 || vTexCoord.x > 240. )
        return;     
    
    vec2 vUV = vec2( vTexCoord.x, vTexCoord.y );
    vUV.y += float(iMessage * 8);
    vUV.y = (iChannelResolution[0].y - 1.0) - vUV.y;
    vUV = floor( vUV ) + 0.5;
    vUV /= iChannelResolution[0].xy;
    vec4 vSample = texture(iChannel0, vUV);
	if( vSample.a > 0.0)
	{
        vResult = (vec3)vSample.rgb;
	}
                    
    
                    /*
    // Message text
    PrintState printState;
    Print_Init( printState, vTexCoord );

    // Fixed size font
    //float fCharIndex = floor( printState.vPos.x / 8. );
    //printState.vPos.x -= fCharIndex * 8.0;
    //vec3 vChar = GetMessageChar( fMessage, fCharIndex );
    
    vec3 vChar = _SPACE_;
    for ( int i=0; i<32; i++)
    {
        vChar = GetMessageChar( fMessage, float(i) );
        if ( Print_Test( printState, vChar, 0.0 ) )
        {
            break;
        }
        if ( vChar.z == 0. )
            break;
    }
        	
    Print_FancyChar( printState, vResult, vChar );
	*/
}
#endif // FULL_HUD

vec4 GameImage( vec2 vUV, vec2 vResolution )
{
    vec4 vResult = vec4(0.0);

    if ( any( lessThan( vUV, vec2(0.0) ) ) || any( greaterThanEqual( vUV, vec2(1,1) ) ) )
    {
        return vResult;
    }
    
    vec2 vHudPixel = vUV * vResolution.xy;
    vec2 vScenePixel = vUV * vResolution.xy;

    g_playerEnt = Entity_Read( STATE_CHANNEL, 0 );
    Sector playerSector = Map_ReadSector( MAP_CHANNEL, g_playerEnt.iSectorId );
    
	g_gameState = GameState_Read( STATE_CHANNEL );
    
    float fHudFade = 0.0;
    float fGameFade = 10.0;
    
    if ( g_gameState.iMainState == MAIN_GAME_STATE_GAME_RUNNING )
    {
        fHudFade = 0.01 + g_gameState.fStateTimer;
        fGameFade = 0.0;
    }
    if ( g_gameState.iMainState == MAIN_GAME_STATE_WIN )
    {
        fGameFade = 0.01 + g_gameState.fStateTimer;
        fHudFade = 0.0;
    }
    
    VWipe( vHudPixel, fHudFade, vResolution );
    VWipe( vScenePixel, fGameFade, vResolution );       
    
    
    vec3 vRenderImage;
    
#ifdef ALLOW_MAP    
    if ( g_gameState.fMap > 0.0 )
    {
#ifdef HIRES_MAP        
        vRenderImage = DrawMap( vScenePixel, vResolution );
#else // HIRES_MAP       
        vec2 vScenePixelCoord = floor(vScenePixel);

        float fScale = 10.0;
        vec2 vPixelWorldPos = (vScenePixelCoord - vec2(160,100)) * fScale + g_playerEnt.vPos.xz;
        
        vec2 vMapUV = (vPixelWorldPos - vec2(1056, -3616)) / 10.0 + vec2(200, 150);
        vMapUV += vec2(0, 32.0);
        
        vRenderImage = texture( iChannel1, (floor(vMapUV) + 0.5) / iChannelResolution[1].xy ).rgb;
#endif // HIRES_MAP
    }        
    else
#endif // ALLOW_MAP        
    {
 		vRenderImage = SampleScene( vScenePixel, vResolution, playerSector.fLightLevel ).rgb;        
    }
    
    if ( vScenePixel.y <= 32.0 )
    {
        vec4 vHudText = GetHudText( vScenePixel, g_playerEnt.fHealth, g_playerEnt.fArmor );

		vec2 vNoiseScale = vec2(500.0, 300.0);        
        float fNoisePer = 0.8;
        if ( vHudText.a > 0.0 )
        {
            vNoiseScale = vec2(600.0); 
            fNoisePer = 0.5;
        }

        float fNoise = fbm( vUV * vNoiseScale, fNoisePer );
        fNoise = fNoise * 0.5 + 0.5;
        
        if ( vHudText.a > 0.0 )
        {
            vRenderImage = vHudText.rgb * fNoise;
        }
    	else
        {
            vRenderImage = vec3(fNoise * fNoise * 0.65 );
        }    
        
#ifdef FULL_HUD
        // Main relief
        vRenderImage *= Relief( vScenePixel, vec2(0, 0), vec2(46, 31));
        vRenderImage *= Relief( vScenePixel, vec2(48, 0), vec2(104, 31));
        vRenderImage *= Relief( vScenePixel, vec2(106, 0), vec2(142, 31));
        vRenderImage *= Relief( vScenePixel, vec2(178, 0), vec2(235, 31));
        vRenderImage *= Relief( vScenePixel, vec2(249, 0), vec2(319, 31));
        
        // weapon avail
        vRenderImage *= Relief( vScenePixel, vec2(107, 200 - 179), vec2(117, 200 - 171));
        vRenderImage *= Relief( vScenePixel, vec2(119, 200 - 179), vec2(129, 200 - 171));
        vRenderImage *= Relief( vScenePixel, vec2(131, 200 - 179), vec2(141, 200 - 171));
        
        vRenderImage *= Relief( vScenePixel, vec2(107, 200 - 189), vec2(117, 200 - 181));
        vRenderImage *= Relief( vScenePixel, vec2(119, 200 - 189), vec2(129, 200 - 181));
        vRenderImage *= Relief( vScenePixel, vec2(131, 200 - 189), vec2(141, 200 - 181));

        // decoration
        vRenderImage *= Relief( vScenePixel, vec2(237, 200 - 179), vec2(247, 200 - 171));
        vRenderImage *= Relief( vScenePixel, vec2(237, 200 - 189), vec2(247, 200 - 181));
        vRenderImage *= Relief( vScenePixel, vec2(237, 200 - 199), vec2(247, 200 - 191));
        
        vRenderImage *= Relief( vScenePixel, vec2(143, 0), vec2(177, 31));
        
        if ( all( greaterThanEqual( vScenePixel, vec2(144,1) ) ) &&
            all( lessThan( vScenePixel, vec2(177,31) ) ) )
        {
            vRenderImage = vec3(0.0);
        }
#endif // FULL_HUD
            
#ifdef FULL_HUD   
        
        PrintHudMessage( vec2(vScenePixel.x, (vResolution.y - 1.) - (vScenePixel.y + 189.)), MESSAGE_HUD_TEXT, vRenderImage );
        
        PrintState printState;
        Print_Init( printState, vec2(vScenePixel.x, (vResolution.y - 1.) - vScenePixel.y) );        

        
        // HUD text AMMO, HEALTH, ARMS, ARMOR
/*
        Print_Color( printState, vec3(.9 ) );
        vec3 vChar = _SPACE_;
        for ( int i=0; i<24; i++)
        {
            vChar = GetHudTextChar( float(i) );
            if ( Print_Test( printState, vChar, 0.0 ) )
            {
                break;
            }
            if ( vChar.z == 0. )
                break;
        }

        Print_HudChar( printState, vRenderImage, vChar );        
*/
        // Arms numbers
        Print_Color( printState, vec3(.8,.8,0 ) );        
        Print_MoveTo( printState, vec2(109,170) );
        Print_Char( printState, vRenderImage, _2_ );

        if( g_playerEnt.fHaveShotgun <= 0.0 )
        {
			Print_Color( printState, vec3(.25 ) );        
        }
        
        Print_MoveTo( printState, vec2(120,170) );
        Print_Char( printState, vRenderImage, _3_ );
        Print_Color( printState, vec3(.25 ) );        
        Print_MoveTo( printState, vec2(132,170) );
        Print_Char( printState, vRenderImage, _4_ );

        Print_MoveTo( printState, vec2(109,179) );
        Print_Char( printState, vRenderImage, _5_ );
        Print_MoveTo( printState, vec2(120,179) );
        Print_Char( printState, vRenderImage, _6_ );
        Print_MoveTo( printState, vec2(132,179) );
        Print_Char( printState, vRenderImage, _7_ );
#endif // FULL_HUD        
    }    
    
	float fEffectAmount = clamp( abs(g_gameState.fHudFx), 0.0, 1.0 );
            
    if (g_gameState.fHudFx > 0.0) 
    {
        vRenderImage.rgb = mix( vRenderImage.rgb, vec3( 0.5, 1, 0.6), fEffectAmount * 0.75 );
    }

    if (g_gameState.fHudFx < 0.0) 
    {
        vRenderImage.rgb = mix( vRenderImage.rgb, vec3( 1, 0, 0), fEffectAmount * 0.75 );
    }
    
#ifdef FULL_HUD    
#ifdef HUD_MESSAGES
    if ( g_gameState.fMessageTimer > 0.0 )
    {
        if (g_gameState.iMessage >= 0 )
        {
        	PrintHudMessage( vec2(vScenePixel.x, (vResolution.y) - vScenePixel.y), g_gameState.iMessage, vRenderImage );
        }
    }
#endif // HUD_MESSAGES    
#endif // FULL_HUD    
    
    
    vec3 vFrontendImage = vec3(0.0);
    
    if ( vHudPixel.y > 0.0 )
    {
        vFrontendImage = Tex( vHudPixel );
        vec2 vHudTextCoord = vec2(vHudPixel.x, (vResolution.y) - vHudPixel.y);

        if ( g_gameState.iMainState == MAIN_GAME_STATE_WIN )
        {
            float fScale = 0.5;
            vec2 vPos = vec2(58,8);

            PrintHudMessage( (vHudTextCoord * fScale - vPos ), MESSAGE_HANGAR, vFrontendImage );        
            vPos.y += 10.0;
            vPos.x = 56.0;
            PrintHudMessage( (vHudTextCoord * fScale - vPos ), MESSAGE_FINISHED, vFrontendImage );        

        }

        /*
        if ( g_gameState.fMainState == MAIN_GAME_STATE_SKILL_SELECT
           || g_gameState.fMainState == MAIN_GAME_STATE_INIT_LEVEL
           || g_gameState.fMainState == MAIN_GAME_STATE_GAME_RUNNING )
        {            
            float fScale = 0.8;
            vec2 vPos = vec2(32,32);

            PrintHudMessage( (vHudTextCoord * fScale - vPos ), MESSAGE_CHOOSE_SKILL, vFrontendImage );        

            vPos.x += 32.0;
            vPos.y += 24.0;

            PrintHudMessage( (vHudTextCoord * fScale - vPos ), MESSAGE_SKILL_1, vFrontendImage );        
            vPos.y += 16.0;
            PrintHudMessage( (vHudTextCoord * fScale - vPos ), MESSAGE_SKILL_2, vFrontendImage );        
            vPos.y += 16.0;
            PrintHudMessage( (vHudTextCoord * fScale - vPos ), MESSAGE_SKILL_3, vFrontendImage );        
            PrintHudMessage( (vHudTextCoord * fScale - vPos + vec2(16.0, 0) ), MESSAGE_SELECT, vFrontendImage );        
            vPos.y += 16.0;
            PrintHudMessage( (vHudTextCoord * fScale - vPos ), MESSAGE_SKILL_4, vFrontendImage );        
            vPos.y += 16.0;
            PrintHudMessage( (vHudTextCoord * fScale - vPos ), MESSAGE_SKILL_5, vFrontendImage );        
            vPos.y += 16.0;       		        
        }
		*/
    }
    
    
    vec2 vHudUV = vHudPixel / vResolution;
    vec2 vSceneUV = vScenePixel / vResolution;
    if ( fHudFade > fGameFade )
    {
        vResult.rgb = vRenderImage;

        if ( vHudUV.y < 1.0 )
        {
            vResult.rgb = vFrontendImage;    
        }
    }
    else
    {
        vResult.rgb = vFrontendImage;

        if ( vSceneUV.y < 1.0 )
        {
	        vResult.rgb = vRenderImage;
        }
    }
    
    if ( g_gameState.iMainState == MAIN_GAME_STATE_BOOT  ) 
    {
        vResult.rgb = vec3( 0, 0, 0 );
    }

    //vResult *= 0.5 + 0.5 * mod(mod(floor(vScenePixel.x), 2.0) + mod(floor(vScenePixel.y), 2.0), 2.0);
    
	return vResult;    
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 vUV = fragCoord / iResolution.xy;
    
    vec2 vResolution = min( iResolution.xy, vec2( 320.0, 200.0 ) );
    //vec2 vAspect = iResolution.xy;
    vec2 vAspect = vec2( 4, 3 );
    
    vec2 vWindowUV = vUV;
    float fXScale = (iResolution.x / iResolution.y) * (vAspect.y / vAspect.x);
    
    vWindowUV.x = vWindowUV.x * fXScale + (1.0 - fXScale) * 0.5;
    
    
// Full screen
#if 0
    vWindowUV = vUV;
#endif  
    
// 1 : 1
#if 0
    vWindowUV = vUV * iResolution.xy / vResolution.xy;
#endif    
    
    
#ifdef SHOW_MAP_DATA
    fragColor = texture( iChannel0, vUV); return;
#endif // SHOW_MAP_DATA    

    //fragColor = texture( iChannel1, vUV * 0.25 ); return;
    
#ifdef SHOW_SPRITES
    fragColor = texture( iChannel2, vUV); return;
#endif // SHOW_SPRITES
        
    //fragColor = texture( iChannel2, vUV); return;
    //fragColor = texture( iChannel2, vUV * vec2(600,400) / iResolution.xy); return;
      
    fragColor = GameImage( vWindowUV, vResolution );
    
    
    #ifdef QUANTIZE_FINAL_IMAGE
    fragColor = Quantize(fragColor.rgb, 32.0);
    #endif
    
    if ( false )
    {
	   vec4 vSample = texture( iChannel2, vUV * 0.5 + vec2(0.0, 0) );
    	fragColor.rgb = mix( fragColor.rgb, vSample.rgb, vSample.a );
    }
}
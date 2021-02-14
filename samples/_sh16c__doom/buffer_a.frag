//#define WRITE_GRID_DATA

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


///////////////////////////////////////

#define FONT_POS 	vec2(280,0)
#define FONT_CHAR 	vec2(16,0)

float NumFont_Rect( vec2 vPos, vec2 bl, vec2 tr )
{
	if ( all( greaterThanEqual( vPos, bl ) ) &&
        all( lessThanEqual( vPos, tr ) ) )
    {
        return 1.0;
    }
        
    return 0.0;
}     


float NumFont_Pixel( vec2 vPos, vec2 vPixel )
{
    return NumFont_Rect( vPos, vPixel, vPixel );
}

float NumFont_Circle( vec2 vTexCoord )
{
    float fResult = 0.0;
    
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(2, 2), vec2(10,12) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(4, 1), vec2(8,13) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(1, 4), vec2(11,10) ));
    
    return fResult;
}

float NumFont_Zero( vec2 vTexCoord )
{
    float fResult = NumFont_Circle( vTexCoord );

    float fHole = NumFont_Rect( vTexCoord, vec2(6, 4), vec2(6,10) );
    fHole = max( fHole, NumFont_Rect( vTexCoord, vec2(5, 5), vec2(7,9) ) );

    fResult = min( fResult, 1.0 - fHole );    

    return fResult;
}

float NumFont_One( vec2 vTexCoord )
{
    float fResult = 0.0;
    
    //fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(4, 1), vec2(8,13), fOutline ));
    //fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(3, 2), vec2(3,4), fOutline ));
    //fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(2, 3), vec2(2,4), fOutline ));
    //fResult = max( fResult, NumFont_Pixel( vTexCoord, vec2(1, 4), fOutline ));
    
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(6, 1), vec2(10,13) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(5, 2), vec2(5,4) ));
    fResult = max( fResult, NumFont_Pixel( vTexCoord, vec2(3, 4) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(4, 3), vec2(4,4.1) ));
    

    return fResult;
}

float NumFont_Two( vec2 vTexCoord )
{
    float fResult = 0.0;
    
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(2, 1), vec2(9,3) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(3, 6), vec2(9,8) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(1, 8), vec2(4,13) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(1, 11), vec2(10,13) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(2, 7), vec2(10,7) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(8, 3), vec2(11,6) ));
    fResult = max( fResult, NumFont_Pixel( vTexCoord, vec2(10, 2) ) );

    return fResult;
}

float NumFont_Three( vec2 vTexCoord )
{
    float fResult = NumFont_Circle( vTexCoord );
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(1, 1), vec2(8,13) ));

    float fHole = NumFont_Rect( vTexCoord, vec2(-1, 4), vec2(7,5) );
    fHole = max( fHole, NumFont_Rect( vTexCoord, vec2(-1, 9), vec2(7,10) ));
    fHole = max( fHole, NumFont_Rect( vTexCoord, vec2(-1, 6), vec2(3,8) ));
    
    fResult = min( fResult, 1.0 - fHole );    
    
    return fResult;
}

float NumFont_Four( vec2 vTexCoord )
{
    float fResult = 0.0;
    
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(1, 1), vec2(4,8) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(8, 1), vec2(11,13) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(1, 6), vec2(11,8) ));

    return fResult;
}

float NumFont_Five( vec2 vTexCoord )
{
    float fResult = 0.0;

    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(1, 1), vec2(10,3) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(1, 1), vec2(3,8) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(1, 6), vec2(9,8) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(8, 7), vec2(10,12) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(8, 8), vec2(11,11) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(1, 11), vec2(9,13) ));
    
    return fResult;
}

float NumFont_Six( vec2 vTexCoord )
{
    float fResult = NumFont_Circle( vTexCoord );
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(4, 1), vec2(11,13) ));

    float fHole = NumFont_Rect( vTexCoord, vec2(5, 9), vec2(8,10) );
    fHole = max( fHole, NumFont_Rect( vTexCoord, vec2(5, 4), vec2(17,5) ));
    fHole = max( fHole, NumFont_Rect( vTexCoord, vec2(10, 6), vec2(17,6) ));
    fHole = max( fHole, NumFont_Rect( vTexCoord, vec2(10, 13), vec2(17,13) ));

    fResult = min( fResult, 1.0 - fHole );    
    
    return fResult;
}

float NumFont_Seven( vec2 vTexCoord )
{
    float fResult = 0.0;
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(1, 1), vec2(11,3) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(8, 4), vec2(11,13) ));

    float fHole = NumFont_Rect( vTexCoord, vec2(9, -1), vec2(17,1) );
    fHole = max( fHole, NumFont_Rect( vTexCoord, vec2(11, -1), vec2(17,3) ));
    fResult = min( fResult, 1.0 - fHole );    
    
    return fResult;
}

float NumFont_Eight( vec2 vTexCoord )
{
    float fResult = 0.0;
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(2, 1), vec2(10,13) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(1, 2), vec2(11,12) ));
    
    float fHole = NumFont_Rect( vTexCoord, vec2(5, 4), vec2(7,5) );
    fHole = max( fHole, NumFont_Rect( vTexCoord, vec2(5, 9), vec2(7,10) ));
    fHole = max( fHole, NumFont_Rect( vTexCoord, vec2(-1, 6), vec2(1,8) ));
    fHole = max( fHole, NumFont_Rect( vTexCoord, vec2(11, 6), vec2(17,8) ));
    fHole = max( fHole, NumFont_Pixel( vTexCoord, vec2(2, 7) ));
    fHole = max( fHole, NumFont_Pixel( vTexCoord, vec2(10, 7) ));

    fResult = min( fResult, 1.0 - fHole );    
    
    return fResult;
}

float NumFont_Nine( vec2 vTexCoord )
{
    float fResult = NumFont_Circle( vTexCoord );
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(1, 3), vec2(9,13) ));

    float fHole = NumFont_Rect( vTexCoord, vec2(5, 4), vec2(7,5) );
    fHole = max( fHole, NumFont_Rect( vTexCoord, vec2(-1, 9), vec2(7,10) ));
    fHole = max( fHole, NumFont_Rect( vTexCoord, vec2(-1, 8), vec2(3,8) ));
    fHole = max( fHole, NumFont_Pixel( vTexCoord, vec2(-1, 7) ));

    fResult = min( fResult, 1.0 - fHole );    
    
    return fResult;
}

float NumFont_Percent( vec2 vTexCoord )
{
    float fResult = 0.0;
    
    vec2 vClosestRectMin;
    vClosestRectMin.x = clamp( vTexCoord.x, 1.0, 11.0 );
    vClosestRectMin.y = 12.0 - vClosestRectMin.x;
    
    vec2 vClosestRectMax = vClosestRectMin + vec2(0,3); 
    
    vClosestRectMax.y = min( vClosestRectMax.y, 13.0 );
    
    fResult = max( fResult, NumFont_Rect( vTexCoord, vClosestRectMin, vClosestRectMax ));
    
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(1, 1), vec2(3,3) ));
    fResult = max( fResult, NumFont_Rect( vTexCoord, vec2(9, 11), vec2(11,13) ));
    
    return fResult;
}

float NumFont_BinChar( vec2 vTexCoord, float fDigit )
{
    vTexCoord.y = 15. - vTexCoord.y;
    vTexCoord = floor(vTexCoord);
    if ( fDigit == 0.0 )
    {
 		return NumFont_Zero( vTexCoord );
    }
    else
    if ( fDigit == 1.0 )
    {
		return NumFont_One( vTexCoord );
    }
    else
    if ( fDigit == 2.0 )
    {
		return NumFont_Two( vTexCoord );
    }
    else
    if ( fDigit == 3.0 )
    {
		return NumFont_Three( vTexCoord );
    }
    else
    if ( fDigit == 4.0 )
    {
		return NumFont_Four( vTexCoord );
    }
    else
    if ( fDigit == 5.0 )
    {
		return NumFont_Five( vTexCoord );
    }
    else
    if ( fDigit == 6.0 )
    {
		return NumFont_Six( vTexCoord );
    }
    else
    if ( fDigit == 7.0 )
    {
		return NumFont_Seven( vTexCoord );
    }
    else
    if ( fDigit == 8.0 )
    {
		return NumFont_Eight( vTexCoord );
    }
    else
    if ( fDigit == 9.0 )
    {
		return NumFont_Nine( vTexCoord );
    }
    else
    if ( fDigit == 10.0 )
    {
		return NumFont_Percent( vTexCoord );
    }
        
    return 0.0;
}


vec4 NumFont_Char( vec2 vTexCoord, float fDigit )
{
    float fOutline = 0.0;
    float f00 = NumFont_BinChar( vTexCoord + vec2(-1,-1), fDigit );
    float f10 = NumFont_BinChar( vTexCoord + vec2( 0,-1), fDigit );
    float f20 = NumFont_BinChar( vTexCoord + vec2( 1,-1), fDigit );
        
    float f01 = NumFont_BinChar( vTexCoord + vec2(-1, 0), fDigit );
    float f11 = NumFont_BinChar( vTexCoord + vec2( 0, 0), fDigit );
    float f21 = NumFont_BinChar( vTexCoord + vec2( 1, 0), fDigit );
        
    float f02 = NumFont_BinChar( vTexCoord + vec2(-1, 1), fDigit );
    float f12 = NumFont_BinChar( vTexCoord + vec2( 0, 1), fDigit );
    float f22 = NumFont_BinChar( vTexCoord + vec2( 1, 1), fDigit );
        
    float fn1 = NumFont_BinChar( vTexCoord + vec2(-2, 0), fDigit );
    float fn2 = NumFont_BinChar( vTexCoord + vec2(-2, 1), fDigit );
    
    float fn3 = NumFont_BinChar( vTexCoord + vec2(-2, 2), fDigit );
    float f03 = NumFont_BinChar( vTexCoord + vec2(-1, 2), fDigit );
    float f13 = NumFont_BinChar( vTexCoord + vec2( 0, 2), fDigit );
        
    float fOutlineI = min( 1.0, f00 + f10 + f20 + f01 + f11 + f21 + f02 + f12 + f22 );
    float fShadow = min( 1.0, fn1 + f01 + f21 + fn2 + f02 + f12 + fn3 + f03 + f13 );

    float nx = f00 * -1.0 + f20 * 1.0
             + f01 * -2.0 + f21 * 2.0
         	 + f02 * -1.0 + f22 * 1.0;
        
    float ny = f00 * -1.0 + f02 * 1.0
             + f10 * -2.0 + f12 * 2.0
         	 + f20 * -1.0 + f22 * 1.0;
    
    vec3 n = normalize( vec3( nx, ny, 0.1 ) );
    
    vec3 vLight = normalize( vec3( 0.5, -1.0, 0.5 ) );
    
    float NdotL = dot( n, vLight ) * 0.25 + 0.75;
    NdotL = sqrt(NdotL);
    
    if ( (fOutlineI + fShadow) <= 0.0 )
    {
        return vec4(0.0);
    }

    vec4 vResult = vec4(1.0);
    
    if ( fShadow > 0.0 )
    {
        vResult.xyz = vec3(0.2);
    }

    if ( fOutlineI > 0.0 )
    {
	    vec3 vDiff = vec3(0.5,0,0);
        
        if ( f11 > 0.0 )
        {
            vDiff = vec3(1,0,0) * NdotL;
        }
        vResult.rgb = vDiff;
    }
    
    return vResult;
}

///////////////////////////////////////


// ----------------- 8< -------------------------- 8< -------------------------- 8< --------------
// Start of Font code

CXXSWIZZLE_CONST ivec3 _SPACE_ = ivec3(0,0,5);
CXXSWIZZLE_CONST ivec3 _EXCLAMATION_ = ivec3(49539,384,4);
CXXSWIZZLE_CONST ivec3 _QUOTE_ = ivec3(2331,0,7);
CXXSWIZZLE_CONST ivec3 _HASH_ = ivec3(167818,1311,7);
CXXSWIZZLE_CONST ivec3 _DOLLAR_ = ivec3(508575,69524,7);
CXXSWIZZLE_CONST ivec3 _PERCENT_ = ivec3(232984,4487,8);
CXXSWIZZLE_CONST ivec3 _AMPERSAND_ = ivec3(249630,8095,8);
CXXSWIZZLE_CONST ivec3 _APOSTROPHE_ = ivec3(259,0,4);
CXXSWIZZLE_CONST ivec3 _L_PAREN_ = ivec3(115614,3847,7);
CXXSWIZZLE_CONST ivec3 _R_PAREN_ = ivec3(462351,1948,7);
CXXSWIZZLE_CONST ivec3 _ASTERISK_ = ivec3(509700,526,7);
CXXSWIZZLE_CONST ivec3 _PLUS_ = ivec3(114944,2,5);
CXXSWIZZLE_CONST ivec3 _COMMA_ = ivec3(0,33152,4);
CXXSWIZZLE_CONST ivec3 _MINUS_ = ivec3(245760,0,6);
CXXSWIZZLE_CONST ivec3 _PERIOD_ = ivec3(0,384,4);
CXXSWIZZLE_CONST ivec3 _SLASH_ = ivec3(232984,391,7);
CXXSWIZZLE_CONST ivec3 _0_ = ivec3(843678,3903,8);
CXXSWIZZLE_CONST ivec3 _1_ = ivec3(99206,774,5);
CXXSWIZZLE_CONST ivec3 _2_ = ivec3(1039935,8067,8);
CXXSWIZZLE_CONST ivec3 _3_ = ivec3(1023007,4024,8);
CXXSWIZZLE_CONST ivec3 _4_ = ivec3(511387,3096,7);
CXXSWIZZLE_CONST ivec3 _5_ = ivec3(508319,3998,7);
CXXSWIZZLE_CONST ivec3 _6_ = ivec3(1033150,7987,8);
CXXSWIZZLE_CONST ivec3 _7_ = ivec3(793663,6192,8);
CXXSWIZZLE_CONST ivec3 _8_ = ivec3(498111,8115,8);
CXXSWIZZLE_CONST ivec3 _9_ = ivec3(1038751,4024,8);
CXXSWIZZLE_CONST ivec3 _COLON_ = ivec3(3,384,4);
CXXSWIZZLE_CONST ivec3 _SEMICOLON_ = ivec3(384,33152,4);
CXXSWIZZLE_CONST ivec3 _LESSTHAN_ = ivec3(98816,66307,5);
CXXSWIZZLE_CONST ivec3 _EQUALS_ = ivec3(114688,896,5);
CXXSWIZZLE_CONST ivec3 _GREATERTHAN_ = ivec3(49280,16774,5);
CXXSWIZZLE_CONST ivec3 _QUESTION_MARK_ = ivec3(925568,114751,8);
CXXSWIZZLE_CONST ivec3 _AT_ = ivec3(1532350,65237,9);
CXXSWIZZLE_CONST ivec3 _A_ = ivec3(1038782,6579,8);
CXXSWIZZLE_CONST ivec3 _B_ = ivec3(515135,8123,8);
CXXSWIZZLE_CONST ivec3 _C_ = ivec3(115646,7943,8);
CXXSWIZZLE_CONST ivec3 _D_ = ivec3(842783,4027,8);
CXXSWIZZLE_CONST ivec3 _E_ = ivec3(1034174,7951,8);
CXXSWIZZLE_CONST ivec3 _F_ = ivec3(508863,387,8);
CXXSWIZZLE_CONST ivec3 _G_ = ivec3(902078,7991,8);
CXXSWIZZLE_CONST ivec3 _H_ = ivec3(1038771,6579,8);
CXXSWIZZLE_CONST ivec3 _I_ = ivec3(49539,387,4);
CXXSWIZZLE_CONST ivec3 _J_ = ivec3(792624,8120,8);
CXXSWIZZLE_CONST ivec3 _K_ = ivec3(249267,6555,8);
CXXSWIZZLE_CONST ivec3 _L_ = ivec3(49539,8071,8);
CXXSWIZZLE_CONST ivec3 _M_ = ivec3(2096099,12779,9);
CXXSWIZZLE_CONST ivec3 _N_ = ivec3(1039287,7615,8);
CXXSWIZZLE_CONST ivec3 _O_ = ivec3(843678,3903,8);
CXXSWIZZLE_CONST ivec3 _P_ = ivec3(1039423,387,8);
CXXSWIZZLE_CONST ivec3 _Q_ = ivec3(843678,790335,8);
CXXSWIZZLE_CONST ivec3 _R_ = ivec3(515135,6587,8);
CXXSWIZZLE_CONST ivec3 _S_ = ivec3(508319,3992,7);
CXXSWIZZLE_CONST ivec3 _T_ = ivec3(198207,1548,8);
CXXSWIZZLE_CONST ivec3 _U_ = ivec3(842163,3903,8);
CXXSWIZZLE_CONST ivec3 _V_ = ivec3(232859,526,7);
CXXSWIZZLE_CONST ivec3 _W_ = ivec3(2094563,15359,9);
CXXSWIZZLE_CONST ivec3 _X_ = ivec3(466807,15294,9);
CXXSWIZZLE_CONST ivec3 _Y_ = ivec3(498099,1548,8);
CXXSWIZZLE_CONST ivec3 _Z_ = ivec3(232991,3975,7);
CXXSWIZZLE_CONST ivec3 _L_SQUARE_BRACKET_ = ivec3(49543,899,5);
CXXSWIZZLE_CONST ivec3 _BACKSLASH_ = ivec3(230275,3100,7);
CXXSWIZZLE_CONST ivec3 _R_SQUARE_BRACKET_ = ivec3(99079,902,5);
CXXSWIZZLE_CONST ivec3 _CARET_ = ivec3(444164,0,7);
CXXSWIZZLE_CONST ivec3 _UNDERSCORE_ = ivec3(0,1032192,8);

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

bool Print_Test( inout PrintState printState, ivec3 vCharacter, float fSpacing )
{
    if ( vCharacter.z == -1 )
    {
        Print_MoveTo( printState, vec2(vCharacter.xy) );
        return false;
    }
    
    if ( printState.vPos.x < float(vCharacter.z) )
        return true;
         
	printState.vPos.x -= float(vCharacter.z) + fSpacing;
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


ivec3 GetMessageChar( int iMessage, int iChar )
{
	#define MESSAGE_CHAR(X) if ( iChar == 0 ) return X; iChar--
    
    if (iMessage == MESSAGE_HUD_TEXT)
    {
        MESSAGE_CHAR( ivec3(6,0, -1) ); // MOVE

        MESSAGE_CHAR( _A_ );
        MESSAGE_CHAR( _M_ );
        MESSAGE_CHAR( _M_ );
        MESSAGE_CHAR( _O_ );

        MESSAGE_CHAR( ivec3(52,0, -1) ); // MOVE

        MESSAGE_CHAR( _H_ );
        MESSAGE_CHAR( _E_ );
        MESSAGE_CHAR( _A_ );
        MESSAGE_CHAR( _L_ );
        MESSAGE_CHAR( _T_ );
        MESSAGE_CHAR( _H_ );

        MESSAGE_CHAR( ivec3(109,0, -1) ); // MOVE

        MESSAGE_CHAR( _A_ );
        MESSAGE_CHAR( _R_ );
        MESSAGE_CHAR( _M_ );
        MESSAGE_CHAR( _S_ );

        MESSAGE_CHAR( ivec3(187,0, -1) ); // MOVE

        MESSAGE_CHAR( _A_ );
        MESSAGE_CHAR( _R_ );
        MESSAGE_CHAR( _M_ );
        MESSAGE_CHAR( _O_ );
        MESSAGE_CHAR( _R_ );        
    }
    else
    if (iMessage == MESSAGE_CHOOSE_SKILL )
    {
        MESSAGE_CHAR( _C_ ); MESSAGE_CHAR( _H_ );MESSAGE_CHAR( _O_ ); MESSAGE_CHAR( _O_ ); MESSAGE_CHAR( _S_ ); MESSAGE_CHAR( _E_ );
        MESSAGE_CHAR( _SPACE_ );
        MESSAGE_CHAR( _S_ ); MESSAGE_CHAR( _K_ );MESSAGE_CHAR( _I_ ); MESSAGE_CHAR( _L_ ); MESSAGE_CHAR( _L_ );
        MESSAGE_CHAR( _SPACE_ );
        MESSAGE_CHAR( _L_ ); MESSAGE_CHAR( _E_ );MESSAGE_CHAR( _V_ ); MESSAGE_CHAR( _E_ ); MESSAGE_CHAR( _L_ );
        MESSAGE_CHAR( _COLON_ );
    }
    else
    if (iMessage == MESSAGE_SKILL_1 )
    {
        MESSAGE_CHAR( _I_ ); MESSAGE_CHAR( _APOSTROPHE_ );MESSAGE_CHAR( _M_ );
        MESSAGE_CHAR( _SPACE_ );
        MESSAGE_CHAR( _T_ ); MESSAGE_CHAR( _O_ );MESSAGE_CHAR( _O_ );
        MESSAGE_CHAR( _SPACE_ );
        MESSAGE_CHAR( _Y_ ); MESSAGE_CHAR( _O_ );MESSAGE_CHAR( _U_ ); MESSAGE_CHAR( _N_ ); MESSAGE_CHAR( _G_ );
        MESSAGE_CHAR( _SPACE_ );
        MESSAGE_CHAR( _T_ ); MESSAGE_CHAR( _O_ );
        MESSAGE_CHAR( _SPACE_ );
        MESSAGE_CHAR( _D_ ); MESSAGE_CHAR( _I_ );MESSAGE_CHAR( _E_ );
        MESSAGE_CHAR( _PERIOD_ );
    }
    else
    if (iMessage == MESSAGE_SKILL_2 )
    {
        MESSAGE_CHAR( _H_ ); MESSAGE_CHAR( _E_ );MESSAGE_CHAR( _Y_ );
        MESSAGE_CHAR( _COMMA_ );MESSAGE_CHAR( _SPACE_ );
        MESSAGE_CHAR( _N_ ); MESSAGE_CHAR( _O_ );MESSAGE_CHAR( _T_ );
        MESSAGE_CHAR( _SPACE_ );
        MESSAGE_CHAR( _T_ ); MESSAGE_CHAR( _O_ );MESSAGE_CHAR( _O_ );
        MESSAGE_CHAR( _SPACE_ );
        MESSAGE_CHAR( _R_ ); MESSAGE_CHAR( _O_ );MESSAGE_CHAR( _U_ ); MESSAGE_CHAR( _G_ ); MESSAGE_CHAR( _H_ );
        MESSAGE_CHAR( _PERIOD_ );
    }
    else
    if (iMessage == MESSAGE_SKILL_3 )
    {
        MESSAGE_CHAR( _H_ ); MESSAGE_CHAR( _U_ );MESSAGE_CHAR( _R_ );MESSAGE_CHAR( _T_ );
        MESSAGE_CHAR( _SPACE_ );
        MESSAGE_CHAR( _M_ );MESSAGE_CHAR( _E_ );
        MESSAGE_CHAR( _SPACE_ );
        MESSAGE_CHAR( _P_ ); MESSAGE_CHAR( _L_ );MESSAGE_CHAR( _E_ ); MESSAGE_CHAR( _N_ ); MESSAGE_CHAR( _T_ ); MESSAGE_CHAR( _Y_ );
        MESSAGE_CHAR( _PERIOD_ );
    }
    else
    if (iMessage == MESSAGE_SKILL_4 )
    {
        MESSAGE_CHAR( _U_ ); MESSAGE_CHAR( _L_ );MESSAGE_CHAR( _T_ );MESSAGE_CHAR( _R_ );MESSAGE_CHAR( _A_ );
        MESSAGE_CHAR( _MINUS_ );
        MESSAGE_CHAR( _V_ ); MESSAGE_CHAR( _I_ );MESSAGE_CHAR( _O_ );MESSAGE_CHAR( _L_ );MESSAGE_CHAR( _E_ );MESSAGE_CHAR( _N_ );MESSAGE_CHAR( _C_ );MESSAGE_CHAR( _E_ );
        MESSAGE_CHAR( _PERIOD_ );
    }
    else
    if (iMessage == MESSAGE_SKILL_5 )
    {
        MESSAGE_CHAR( _N_ ); MESSAGE_CHAR( _I_ );MESSAGE_CHAR( _G_ );MESSAGE_CHAR( _H_ );MESSAGE_CHAR( _T_ );MESSAGE_CHAR( _M_ );MESSAGE_CHAR( _A_ );MESSAGE_CHAR( _R_ );MESSAGE_CHAR( _E_ );
        MESSAGE_CHAR( _EXCLAMATION_ );
    }
    else
    if (iMessage == MESSAGE_HANGAR )
    {
        MESSAGE_CHAR( _H_ ); MESSAGE_CHAR( _A_ );MESSAGE_CHAR( _N_ );MESSAGE_CHAR( _G_ );MESSAGE_CHAR( _A_ );MESSAGE_CHAR( _R_ );
    }
    else
    if (iMessage == MESSAGE_FINISHED )
    {
        MESSAGE_CHAR( _F_ ); MESSAGE_CHAR( _I_ );MESSAGE_CHAR( _N_ );MESSAGE_CHAR( _I_ );MESSAGE_CHAR( _S_ );MESSAGE_CHAR( _H_ );MESSAGE_CHAR( _E_ );MESSAGE_CHAR( _D_ );
    }
    else
    if (iMessage == MESSAGE_KILLS )
    {
        MESSAGE_CHAR( _K_ ); MESSAGE_CHAR( _I_ );MESSAGE_CHAR( _L_ );MESSAGE_CHAR( _L_ );MESSAGE_CHAR( _S_ );
    }
    else
    if (iMessage == MESSAGE_ITEMS )
    {
        MESSAGE_CHAR( _I_ ); MESSAGE_CHAR( _T_ );MESSAGE_CHAR( _E_ );MESSAGE_CHAR( _M_ );MESSAGE_CHAR( _S_ );
    }
    else
    if (iMessage == MESSAGE_SECRET )
    {
        MESSAGE_CHAR( _S_ ); MESSAGE_CHAR( _E_ );MESSAGE_CHAR( _C_ );MESSAGE_CHAR( _R_ );MESSAGE_CHAR( _E_ );MESSAGE_CHAR( _T_ );
    }
    else
    if (iMessage == MESSAGE_TIME )
    {
        MESSAGE_CHAR( _T_ ); MESSAGE_CHAR( _I_ );MESSAGE_CHAR( _M_ );MESSAGE_CHAR( _E_ );
    }
    else
    if (iMessage == MESSAGE_SELECT )
    {
        MESSAGE_CHAR( _ASTERISK_ );
    }
    else
    {    
        if ( iMessage == MESSAGE_HEALTH_BONUS ||
            iMessage == MESSAGE_ARMOR_BONUS || 
            iMessage == MESSAGE_GREENARMOR || 
            iMessage == MESSAGE_BLUEARMOR ||
            iMessage == MESSAGE_STIMPACK ||
           iMessage == MESSAGE_MEDIKIT)
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
        if( iMessage == MESSAGE_SHOTGUN )
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


        if ( iMessage == MESSAGE_HEALTH_BONUS || iMessage == MESSAGE_ARMOR_BONUS || iMessage == MESSAGE_STIMPACK || iMessage == MESSAGE_MEDIKIT)
        {    
            MESSAGE_CHAR( _A_ );
            if ( iMessage == MESSAGE_ARMOR_BONUS )
            {    
                MESSAGE_CHAR( _N_ );
            }
            MESSAGE_CHAR( _SPACE_ );
        }
        else if ( iMessage == MESSAGE_GREENARMOR ||
                iMessage == MESSAGE_BLUEARMOR || 
                iMessage == MESSAGE_SHOTGUN )
        {
            MESSAGE_CHAR( _T_ );
            MESSAGE_CHAR( _H_ );
            MESSAGE_CHAR( _E_ );

            MESSAGE_CHAR( _SPACE_ );
        }

        if ( iMessage == MESSAGE_BLUEARMOR )
        {
            MESSAGE_CHAR( _M_ );
            MESSAGE_CHAR( _E_ );
            MESSAGE_CHAR( _G_ );
            MESSAGE_CHAR( _A_ );
        }

        if( iMessage == MESSAGE_SHOTGUN )
        {
            MESSAGE_CHAR( _S_ );
            MESSAGE_CHAR( _H_ );
            MESSAGE_CHAR( _O_ );
            MESSAGE_CHAR( _T_ );
            MESSAGE_CHAR( _G_ );
            MESSAGE_CHAR( _U_ );
            MESSAGE_CHAR( _N_ );
        }

        if ( iMessage == MESSAGE_HEALTH_BONUS )
        {    
            MESSAGE_CHAR( _H_ );
            MESSAGE_CHAR( _E_ );
            MESSAGE_CHAR( _A_ );
            MESSAGE_CHAR( _L_ );
            MESSAGE_CHAR( _T_ );
            MESSAGE_CHAR( _H_ );
        }
        else if ( iMessage == MESSAGE_ARMOR_BONUS || iMessage == MESSAGE_GREENARMOR || iMessage == MESSAGE_BLUEARMOR)
        {    
            MESSAGE_CHAR( _A_ );
            MESSAGE_CHAR( _R_ );
            MESSAGE_CHAR( _M_ );
            MESSAGE_CHAR( _O_ );
            MESSAGE_CHAR( _R_ );
        }
        else if ( iMessage == MESSAGE_MEDIKIT )
        {    
            MESSAGE_CHAR( _M_ );
            MESSAGE_CHAR( _E_ );
            MESSAGE_CHAR( _D_ );
            MESSAGE_CHAR( _I_ );
            MESSAGE_CHAR( _K_ );
            MESSAGE_CHAR( _I_ );
            MESSAGE_CHAR( _T_ );
        }
        else if ( iMessage == MESSAGE_STIMPACK )
        {
            MESSAGE_CHAR( _S_ );
            MESSAGE_CHAR( _T_ );
            MESSAGE_CHAR( _I_ );
            MESSAGE_CHAR( _M_ );
            MESSAGE_CHAR( _P_ );
            MESSAGE_CHAR( _A_ );
            MESSAGE_CHAR( _C_ );
            MESSAGE_CHAR( _K_ );
        }

        if ( iMessage == MESSAGE_HEALTH_BONUS || iMessage == MESSAGE_ARMOR_BONUS)
        {    
            MESSAGE_CHAR( _SPACE_ );

            MESSAGE_CHAR( _B_ );
            MESSAGE_CHAR( _O_ );
            MESSAGE_CHAR( _N_ );
            MESSAGE_CHAR( _U_ );
            MESSAGE_CHAR( _S_ );
        }

        if ( iMessage == MESSAGE_BLUEARMOR || iMessage == MESSAGE_SHOTGUN )
        {
            MESSAGE_CHAR( _EXCLAMATION_ );
        }
        else
        {
            MESSAGE_CHAR( _PERIOD_ );
        }   
    }
    
    return ivec3(0.0);
}

void PrintHudMessage( vec2 vTexCoord, int iMessage, inout vec3 vResult )
{
    if ( vTexCoord.y > 8.0 )
        return;

    if ( iMessage >= MESSAGE_COUNT )
        return;
    
    // Message text
    PrintState printState;
    Print_Init( printState, vTexCoord );

    // Fixed size font
    //float fCharIndex = floor( printState.vPos.x / 8. );
    //printState.vPos.x -= fCharIndex * 8.0;
    //vec3 vChar = GetMessageChar( fMessage, fCharIndex );
    
    ivec3 vChar = _SPACE_;
    for ( int i=0; i<NO_UNROLL( 26 ); i++)
    {
        vChar = GetMessageChar( iMessage, i );
        if ( Print_Test( printState, vChar, 0.0 ) )
        {
            break;
        }
        if ( vChar.z == 0 )
            break;
    }
        
    if ( iMessage == MESSAGE_HUD_TEXT || iMessage == MESSAGE_HANGAR )
    {
		Print_Color( printState, vec3(1. ) );        
    	Print_HudChar( printState, vResult, vChar );
    }
    else
    {
    	Print_FancyChar( printState, vResult, vChar );
    }
}

// Constant Data

///////////////////////////////////

float hash(float p)
{
	vec2 p2 = fract(vec2(p * 5.3983, p * 5.4427));
    p2 += dot(p2.yx, p2.xy + vec2(21.5351, 14.3137));
	return fract(p2.x * p2.y * 95.4337);
}

#define GRID_POS 		vec2(244,32)
#define GRID_SIZE 		vec2(256,256)
#define GRID_WORLD_MIN	vec2(-780,-4900)
#define GRID_WORLD_MAX	vec2(3820,-2060)

bool LineInGridCell( vec2 vA, vec2 vB, vec2 vGridCellMin, vec2 vGridCellMax )
{
    vec2 vAB = vB - vA;
    
    // Check if box crosses line

    float fCross0 = Cross2d( vAB, vec2(vGridCellMin.x, vGridCellMin.y) - vA );
    float fCross1 = Cross2d( vAB, vec2(vGridCellMax.x, vGridCellMin.y) - vA );
    float fCross2 = Cross2d( vAB, vec2(vGridCellMin.x, vGridCellMax.y) - vA );
    float fCross3 = Cross2d( vAB, vec2(vGridCellMax.x, vGridCellMax.y) - vA );
       
    if ( fCross0 < 0.0 && fCross1 < 0.0 && fCross2 < 0.0 && fCross3 < 0.0 )
        return false;

    if ( fCross0 > 0.0 && fCross1 > 0.0 && fCross2 > 0.0 && fCross3 > 0.0 )
        return false;
           
    // Check if line projection crosses box

    vec2 vLineMin = min( vA, vB );
    vec2 vLineMax = max( vA, vB );

    if ( vLineMin.x > vGridCellMax.x ) return false;
    if ( vLineMin.y > vGridCellMax.y ) return false;
    if ( vLineMax.x < vGridCellMin.x ) return false;
    if ( vLineMax.y < vGridCellMin.y ) return false;
    
    return true;
}

bool Map_SectorInGridCell( sampler2D mapSampler, MapInfo mapInfo, vec2 vGridCellMin, vec2 vGridCellMax, int iSectorId )
{
    Sector sector;
    
    sector = Map_ReadSector( mapSampler, iSectorId );
    
    float fInOutTest = 0.0;
    
    for(int iSideDefIndex=0; iSideDefIndex<NO_UNROLL( MAX_SIDEDEF_COUNT ); iSideDefIndex++)
    {        
        SideDef sideDef = Map_ReadSideDefInfo( mapSampler, sector.iSectorId, iSideDefIndex );                        
        
        if ( sideDef.fLength <= 0.0)
        {
            break;
        }

        if ( LineInGridCell( sideDef.vA, sideDef.vB, vGridCellMin, vGridCellMax ) )
        {
            return true;
        }
    }
    
    return false;
}

vec4 GetGridData( sampler2D mapSampler, vec2 vGridCellWorldMin, vec2 vGridCellWorldMax )
{
    MapInfo mapInfo = ReadMapInfo( mapSampler );
    
    int sectorCount = 0;
    int gridSectors[8];
        
	gridSectors[0] = -1;
    gridSectors[1] = -1;
    gridSectors[2] = -1;
    gridSectors[3] = -1;
    gridSectors[4] = -1;
    gridSectors[5] = -1;
    gridSectors[6] = -1;
    gridSectors[7] = -1;
    
    //gridSectors[0] = Map_SeekSector( mapInfo, vGridCellWorldMin ); 
    
	int foundIndex = 0;
    for(int iSectorIndex=0; iSectorIndex<NO_UNROLL( MAX_SECTOR_COUNT ); iSectorIndex++)
    {
        if ( !Map_ValidSectorId( mapInfo, iSectorIndex ) )
        {
            break;
        }

        if ( foundIndex > 7 )
        {
            break;
        }
        
        if ( Map_SectorInGridCell( mapSampler, mapInfo, vGridCellWorldMin, vGridCellWorldMax, iSectorIndex ) )
        {
            if ( foundIndex == 0 ) gridSectors[0] = iSectorIndex;
            if ( foundIndex == 1 ) gridSectors[1] = iSectorIndex;
            if ( foundIndex == 2 ) gridSectors[2] = iSectorIndex;
            if ( foundIndex == 3 ) gridSectors[3] = iSectorIndex;
            if ( foundIndex == 4 ) gridSectors[4] = iSectorIndex;
            if ( foundIndex == 5 ) gridSectors[5] = iSectorIndex;
            if ( foundIndex == 6 ) gridSectors[6] = iSectorIndex;
            if ( foundIndex == 7 ) gridSectors[7] = iSectorIndex;
            foundIndex++;
        }        
    }
    

    vec4 gridDataPacked;
    gridDataPacked.x = BytePack2( vec2( gridSectors[0], gridSectors[1] ) + 1. );
    gridDataPacked.y = BytePack2( vec2( gridSectors[2], gridSectors[3] ) + 1. );
    gridDataPacked.z = BytePack2( vec2( gridSectors[4], gridSectors[5] ) + 1. );
    gridDataPacked.w = BytePack2( vec2( gridSectors[6], gridSectors[7] ) + 1. );
    
    return gridDataPacked;
}

#define PACK_DATA(A,B,C,D) ( A + (B<<8) + (C<<16) + (D<<24) )
#define UNPACK_DATA(X) uvec4( X & 0xffu, (X>>8u)&0xffu, (X>>16u)&0xffu, (X>>24u)&0xffu )

		// Textures
		#define TEX_X 0u
		#define TEX_F_SKY1 1u
		#define TEX_NUKAGE3 2u
		#define TEX_FLOOR7_1 3u
		#define TEX_FLOOR4_8 4u
		#define TEX_CEIL5_1 5u
		#define TEX_FLAT5_5 6u
		#define TEX_FLAT20 7u
		#define TEX_FLOOR5_1 8u
		#define TEX_CEIL5_2 9u
		#define TEX_CEIL3_5 10u
		#define TEX_TLITE6_5 11u
		#define TEX_FLAT14 12u
		#define TEX_FLAT18 13u
		#define TEX_TLITE6_4 14u
		#define TEX_FLOOR7_2 15u
		#define TEX_STEP2 16u
		#define TEX_TLITE6_1 17u
		#define TEX_FLOOR1_1 18u
		#define TEX_FLOOR5_2 19u
		#define TEX_FLOOR6_2 20u
		#define TEX_FLAT23 21u
		#define TEX_TLITE6_6 22u
		#define TEX_DOOR3 23u
		#define TEX_LITE3 24u
		#define TEX_STARTAN3 25u
		#define TEX_BROWN1 26u
		#define TEX_DOORSTOP 27u
		#define TEX_COMPUTE2 28u
		#define TEX_STEP6 29u
		#define TEX_BROWN144 30u
		#define TEX_SUPPORT2 31u
		#define TEX_STARG3 32u
		#define TEX_DOORTRAK 33u
		#define TEX_SLADWALL 34u
		#define TEX_TEKWALL4 35u
		#define TEX_SW1COMP 36u
		#define TEX_BIGDOOR2 37u
		#define TEX_STARGR1 38u
		#define TEX_BROWNGRN 39u
		#define TEX_NUKE24 40u
		#define TEX_BROWN96 41u
		#define TEX_BRNBIGR 42u
		#define TEX_BRNBIGL 43u
		#define TEX_BRNBIGC 44u
		#define TEX_STARTAN1 45u
		#define TEX_EXITDOOR 46u
		#define TEX_SW1STRTN 47u
		#define TEX_BIGDOOR4 48u
		#define TEX_EXITSIGN 49u
		#define TEX_TEKWALL1 50u
		#define TEX_COMPTALL 51u
		#define TEX_COMPSPAN 52u
		#define TEX_PLANET1 53u
		#define TEX_COMPTILE 54u
		#define TEX_STEP1 55u
		#define TEX_COMPUTE3 56u
        
		struct DataSideDefPacked
        {
            uint A;
            uint B;
            uint packed0;
            uint packed1;
        };

		struct DataSideDef
		{
			ivec2 A;
			ivec2 B;
			int len;
			int otherSector;
			float lightLevel;
			uint bytePacked;
		};
            
		#define DATA_SIDE_DEF(Ax, Ay, Bx, By, len, otherSector, light, packed)		\
            DataSideDefPacked														\
            (																		\
                uint((Ax+8192)|((Ay+8192)<<16)),									\
                uint((Bx+8192)|((By+8192)<<16)),									\
                uint(len<<16)|(uint(sign(light)+1.)<<14)|uint(otherSector+1),		\
                packed																\
            )
            
		DataSideDef UnpackSideDef( const DataSideDefPacked p)
        {
            DataSideDef sideDef;
            sideDef.A.x = int(p.A & 0xffffu) - 8192;
            sideDef.A.y = int(p.A >> 16) - 8192;
            sideDef.B.x = int(p.B & 0xffffu) - 8192;
            sideDef.B.y = int(p.B >> 16) - 8192;
            sideDef.len = int(p.packed0 >> 16) - 1;
            sideDef.otherSector = int(p.packed0 & 0x3fffu) - 1;
            sideDef.lightLevel = .063 * (float((p.packed0 >> 14) & 3u) - 1.);
            sideDef.bytePacked = p.packed1;
            return sideDef;
        }            

		struct DataSector
		{
			float lightLevel;
			int floorHeight;
			int ceilingHeight;
			uint floorTex;
			uint ceilingTex;
			int firstSideDef;
			int sideDefCount;
		};
            
		struct DataEntity
		{
			ivec2 origin;
			int type;
			int subType;
			int angle;
			int flags;
			ivec2 triggerPos;
		};

		struct DataMap
		{
			int sectorCount;
			DataSector sectors[88];
			DataSideDefPacked sideDefs[666];
			DataEntity entities[102];
		};

		CXXSWIZZLE_CONST DataMap map = DataMap(
			88,
			CXXSWIZZLE_ARRAY_VAL(DataSector, 88, 
				// Sector Data 0
				DataSector( 1.000, -80, 216, TEX_NUKAGE3, TEX_F_SKY1, 0, 8 ),
				// Sector Data 1
				DataSector( 1.000, -56, 216, TEX_FLOOR7_1, TEX_F_SKY1, 8, 29 ),
				// Sector Data 2
				DataSector( 1.000, 0, 0, TEX_FLOOR4_8, TEX_CEIL5_1, 37, 6 ),
				// Sector Data 3
				DataSector( 1.000, 8, 192, TEX_FLAT5_5, TEX_FLAT5_5, 43, 4 ),
				// Sector Data 4
				DataSector( 0.565, 0, 0, TEX_FLOOR4_8, TEX_FLAT20, 47, 4 ),
				// Sector Data 5
				DataSector( 1.000, 8, 192, TEX_FLAT5_5, TEX_FLAT5_5, 51, 4 ),
				// Sector Data 6
				DataSector( 0.502, -136, -40, TEX_FLOOR5_1, TEX_CEIL5_2, 55, 7 ),
				// Sector Data 7
				DataSector( 0.502, -120, 16, TEX_FLOOR5_1, TEX_CEIL5_2, 62, 4 ),
				// Sector Data 8
				DataSector( 0.502, -104, 16, TEX_FLOOR5_1, TEX_CEIL5_2, 66, 4 ),
				// Sector Data 9
				DataSector( 0.502, -88, 16, TEX_FLOOR5_1, TEX_CEIL5_2, 70, 4 ),
				// Sector Data 10
				DataSector( 0.502, -72, 16, TEX_FLOOR5_1, TEX_CEIL5_2, 74, 4 ),
				// Sector Data 11
				DataSector( 1.000, -56, 24, TEX_FLOOR7_1, TEX_F_SKY1, 78, 5 ),
				// Sector Data 12
				DataSector( 1.000, -56, 64, TEX_FLOOR7_1, TEX_F_SKY1, 83, 8 ),
				// Sector Data 13
				DataSector( 0.690, -56, 16, TEX_FLOOR7_1, TEX_CEIL5_2, 91, 6 ),
				// Sector Data 14
				DataSector( 1.000, 32, 88, TEX_FLOOR4_8, TEX_CEIL5_1, 97, 4 ),
				// Sector Data 15
				DataSector( 0.502, 0, 224, TEX_FLOOR4_8, TEX_CEIL3_5, 101, 32 ),
				// Sector Data 16
				DataSector( 1.000, 0, 224, TEX_FLOOR4_8, TEX_CEIL3_5, 133, 8 ),
				// Sector Data 17
				DataSector( 1.000, 0, 96, TEX_FLOOR4_8, TEX_TLITE6_5, 141, 4 ),
				// Sector Data 18
				DataSector( 1.000, 0, 224, TEX_FLOOR4_8, TEX_CEIL3_5, 145, 8 ),
				// Sector Data 19
				DataSector( 1.000, 0, 96, TEX_FLOOR4_8, TEX_TLITE6_5, 153, 4 ),
				// Sector Data 20
				DataSector( 1.000, 8, 152, TEX_FLAT14, TEX_TLITE6_5, 157, 4 ),
				// Sector Data 21
				DataSector( 1.000, 32, 88, TEX_FLAT18, TEX_CEIL5_1, 161, 12 ),
				// Sector Data 22
				DataSector( 0.627, 0, 72, TEX_FLOOR4_8, TEX_CEIL3_5, 173, 4 ),
				// Sector Data 23
				DataSector( 1.000, 32, 88, TEX_FLAT18, TEX_CEIL5_1, 177, 6 ),
				// Sector Data 24
				DataSector( 0.565, 0, 144, TEX_FLOOR4_8, TEX_CEIL3_5, 183, 6 ),
				// Sector Data 25
				DataSector( 1.000, 0, 88, TEX_FLOOR4_8, TEX_TLITE6_4, 189, 4 ),
				// Sector Data 26
				DataSector( 0.816, 0, 0, TEX_FLOOR4_8, TEX_FLAT20, 193, 4 ),
				// Sector Data 27
				DataSector( 0.878, -16, 200, TEX_FLAT14, TEX_CEIL3_5, 197, 12 ),
				// Sector Data 28
				DataSector( 0.753, -8, 120, TEX_FLAT14, TEX_CEIL3_5, 209, 12 ),
				// Sector Data 29
				DataSector( 0.565, 0, 72, TEX_FLOOR4_8, TEX_CEIL3_5, 221, 29 ),
				// Sector Data 30
				DataSector( 1.000, 0, 72, TEX_FLOOR4_8, TEX_CEIL3_5, 250, 4 ),
				// Sector Data 31
				DataSector( 0.502, -8, 120, TEX_FLOOR4_8, TEX_CEIL3_5, 254, 10 ),
				// Sector Data 32
				DataSector( 0.502, -8, 224, TEX_FLOOR4_8, TEX_FLOOR7_2, 264, 32 ),
				// Sector Data 33
				DataSector( 0.565, 8, 224, TEX_FLOOR4_8, TEX_FLOOR7_2, 296, 4 ),
				// Sector Data 34
				DataSector( 0.565, 24, 224, TEX_FLOOR4_8, TEX_FLOOR7_2, 300, 4 ),
				// Sector Data 35
				DataSector( 1.000, 40, 184, TEX_STEP2, TEX_TLITE6_1, 304, 4 ),
				// Sector Data 36
				DataSector( 1.000, 40, 184, TEX_STEP2, TEX_TLITE6_1, 308, 4 ),
				// Sector Data 37
				DataSector( 0.565, 40, 224, TEX_FLOOR4_8, TEX_FLOOR7_2, 312, 4 ),
				// Sector Data 38
				DataSector( 0.565, 56, 224, TEX_FLOOR4_8, TEX_FLOOR7_2, 316, 4 ),
				// Sector Data 39
				DataSector( 0.565, 72, 224, TEX_FLOOR4_8, TEX_FLOOR7_2, 320, 4 ),
				// Sector Data 40
				DataSector( 0.878, 0, 128, TEX_FLOOR7_1, TEX_F_SKY1, 324, 8 ),
				// Sector Data 41
				DataSector( 0.753, 104, 264, TEX_FLOOR4_8, TEX_FLOOR7_2, 332, 18 ),
				// Sector Data 42
				DataSector( 1.000, 0, 264, TEX_FLOOR7_1, TEX_F_SKY1, 350, 16 ),
				// Sector Data 43
				DataSector( 1.000, 136, 240, TEX_FLAT20, TEX_FLAT20, 366, 4 ),
				// Sector Data 44
				DataSector( 0.753, 128, 264, TEX_FLOOR1_1, TEX_FLOOR7_2, 370, 8 ),
				// Sector Data 45
				DataSector( 1.000, 136, 240, TEX_FLAT20, TEX_FLAT20, 378, 4 ),
				// Sector Data 46
				DataSector( 1.000, 136, 240, TEX_FLAT20, TEX_FLAT20, 382, 4 ),
				// Sector Data 47
				DataSector( 0.690, 104, 192, TEX_FLOOR4_8, TEX_FLOOR7_2, 386, 8 ),
				// Sector Data 48
				DataSector( 0.565, 88, 224, TEX_FLOOR4_8, TEX_FLOOR7_2, 394, 4 ),
				// Sector Data 49
				DataSector( 0.565, -24, 176, TEX_FLOOR5_2, TEX_CEIL3_5, 398, 9 ),
				// Sector Data 50
				DataSector( 0.627, -16, 72, TEX_FLOOR4_8, TEX_CEIL3_5, 407, 4 ),
				// Sector Data 51
				DataSector( 0.565, -48, 176, TEX_NUKAGE3, TEX_CEIL3_5, 411, 3 ),
				// Sector Data 52
				DataSector( 0.565, -24, 176, TEX_FLOOR5_2, TEX_CEIL3_5, 414, 8 ),
				// Sector Data 53
				DataSector( 0.753, -48, 176, TEX_NUKAGE3, TEX_CEIL3_5, 422, 11 ),
				// Sector Data 54
				DataSector( 0.502, -48, 104, TEX_FLOOR4_8, TEX_FLOOR6_2, 433, 10 ),
				// Sector Data 55
				DataSector( 0.627, 96, 176, TEX_FLOOR4_8, TEX_CEIL3_5, 443, 5 ),
				// Sector Data 56
				DataSector( 0.753, -24, 176, TEX_FLOOR5_2, TEX_CEIL3_5, 448, 14 ),
				// Sector Data 57
				DataSector( 0.753, -48, 176, TEX_NUKAGE3, TEX_CEIL3_5, 462, 7 ),
				// Sector Data 58
				DataSector( 0.502, 104, 184, TEX_FLOOR4_8, TEX_FLOOR6_2, 469, 19 ),
				// Sector Data 59
				DataSector( 1.000, 16, 152, TEX_FLAT14, TEX_TLITE6_5, 488, 4 ),
				// Sector Data 60
				DataSector( 1.000, 24, 152, TEX_FLAT14, TEX_TLITE6_5, 492, 4 ),
				// Sector Data 61
				DataSector( 0.627, -8, 72, TEX_FLOOR4_8, TEX_CEIL3_5, 496, 6 ),
				// Sector Data 62
				DataSector( 0.753, 0, 136, TEX_FLAT20, TEX_FLAT20, 502, 4 ),
				// Sector Data 63
				DataSector( 1.000, -56, 24, TEX_FLOOR7_1, TEX_F_SKY1, 506, 4 ),
				// Sector Data 64
				DataSector( 1.000, -24, 104, TEX_FLOOR5_2, TEX_TLITE6_5, 510, 24 ),
				// Sector Data 65
				DataSector( 0.502, -24, 48, TEX_FLOOR5_2, TEX_CEIL3_5, 534, 18 ),
				// Sector Data 66
				DataSector( 1.000, -24, 72, TEX_FLOOR5_2, TEX_CEIL3_5, 552, 10 ),
				// Sector Data 67
				DataSector( 1.000, -24, 56, TEX_FLOOR5_2, TEX_FLAT23, 562, 4 ),
				// Sector Data 68
				DataSector( 1.000, -24, 48, TEX_FLOOR5_2, TEX_CEIL3_5, 566, 4 ),
				// Sector Data 69
				DataSector( 1.000, -24, -24, TEX_FLOOR5_2, TEX_FLAT20, 570, 4 ),
				// Sector Data 70
				DataSector( 1.000, -24, 88, TEX_FLOOR5_2, TEX_TLITE6_5, 574, 12 ),
				// Sector Data 71
				DataSector( 1.000, -24, 72, TEX_FLOOR5_2, TEX_FLAT23, 586, 4 ),
				// Sector Data 72
				DataSector( 1.000, -24, 48, TEX_FLOOR5_2, TEX_CEIL3_5, 590, 4 ),
				// Sector Data 73
				DataSector( 0.502, -104, 48, TEX_FLOOR5_1, TEX_CEIL5_2, 594, 4 ),
				// Sector Data 74
				DataSector( 0.502, -88, 48, TEX_FLOOR5_1, TEX_CEIL5_2, 598, 4 ),
				// Sector Data 75
				DataSector( 0.502, -72, 48, TEX_FLOOR5_1, TEX_CEIL5_2, 602, 4 ),
				// Sector Data 76
				DataSector( 0.502, -56, 48, TEX_FLOOR5_1, TEX_CEIL5_2, 606, 4 ),
				// Sector Data 77
				DataSector( 0.502, -40, 48, TEX_FLOOR5_1, TEX_CEIL5_2, 610, 4 ),
				// Sector Data 78
				DataSector( 0.502, -24, 48, TEX_FLOOR5_2, TEX_CEIL5_2, 614, 4 ),
				// Sector Data 79
				DataSector( 1.000, -136, -24, TEX_FLOOR5_1, TEX_TLITE6_6, 618, 8 ),
				// Sector Data 80
				DataSector( 0.502, -136, -40, TEX_FLOOR5_1, TEX_CEIL5_2, 626, 6 ),
				// Sector Data 81
				DataSector( 0.502, -120, 48, TEX_FLOOR5_1, TEX_CEIL5_2, 632, 4 ),
				// Sector Data 82
				DataSector( 0.502, -24, 48, TEX_FLOOR5_2, TEX_CEIL3_5, 636, 4 ),
				// Sector Data 83
				DataSector( 0.565, -24, -24, TEX_FLOOR5_2, TEX_FLAT5_5, 640, 4 ),
				// Sector Data 84
				DataSector( 0.502, -24, 48, TEX_FLOOR5_2, TEX_CEIL3_5, 644, 6 ),
				// Sector Data 85
				DataSector( 0.502, -24, -24, TEX_FLOOR5_2, TEX_FLAT20, 650, 4 ),
				// Sector Data 86
				DataSector( 1.000, -48, 32, TEX_FLOOR4_8, TEX_TLITE6_6, 654, 6 ),
				// Sector Data 87
				DataSector( 0.502, 104, 184, TEX_FLOOR4_8, TEX_FLOOR6_2, 660, 6 )
			),
			CXXSWIZZLE_ARRAY_VAL(DataSideDefPacked, 666,
				DATA_SIDE_DEF( 1520, -3168, 1672, -3104, 164, 1, 0.000, PACK_DATA(TEX_BROWN144, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 1672, -3104, 1896, -3104, 224, 1, -0.063, PACK_DATA(TEX_BROWN144, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 1896, -3104, 2040, -3144, 149, 1, 0.000, PACK_DATA(TEX_BROWN144, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 2040, -3144, 2128, -3272, 155, 1, 0.000, PACK_DATA(TEX_BROWN144, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 2128, -3272, 2064, -3408, 150, 1, 0.000, PACK_DATA(TEX_BROWN144, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 2064, -3408, 1784, -3448, 282, 1, 0.000, PACK_DATA(TEX_BROWN144, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 1784, -3448, 1544, -3384, 248, 1, 0.000, PACK_DATA(TEX_BROWN144, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 1544, -3384, 1520, -3168, 217, 1, 0.000, PACK_DATA(TEX_BROWN144, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 1376, -3200, 1376, -3104, 96, 5, 0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF( 1376, -3360, 1376, -3264, 96, 3, 0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF( 1376, -3264, 1376, -3200, 64, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1376, -3104, 1376, -2944, 160, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1376, -2944, 1472, -2880, 115, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1856, -2880, 1920, -2920, 75, 2, 0.000, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF( 1672, -3104, 1520, -3168, 164, 0, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 1896, -3104, 1672, -3104, 224, 0, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 2040, -3144, 1896, -3104, 149, 0, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 2128, -3272, 2040, -3144, 155, 0, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 2064, -3408, 2128, -3272, 150, 0, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 1784, -3448, 2064, -3408, 282, 0, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 1544, -3384, 1784, -3448, 248, 0, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 1520, -3168, 1544, -3384, 217, 0, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 2736, -3360, 2736, -3648, 288, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2736, -3648, 2240, -3648, 496, 63, -0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 2240, -3648, 1984, -3648, 256, 12, -0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 1984, -3648, 1376, -3648, 608, 11, -0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 2240, -2920, 2272, -3008, 93, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2272, -3008, 2432, -3112, 190, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2432, -3112, 2736, -3112, 304, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2736, -3112, 2736, -3360, 248, 62, 0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 2u) ),
				DATA_SIDE_DEF( 1376, -3648, 1376, -3520, 128, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1376, -3392, 1376, -3360, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1376, -3520, 1376, -3392, 128, 4, 0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF( 1472, -2880, 1664, -2880, 192, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1664, -2880, 1856, -2880, 192, 2, -0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF( 1920, -2920, 2176, -2920, 256, 2, -0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF( 2176, -2920, 2240, -2920, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1920, -2920, 1856, -2880, 75, 1, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1664, -2816, 2176, -2816, 512, 14, -0.063, PACK_DATA(TEX_COMPUTE3, TEX_X, TEX_COMPUTE3, 0u) ),
				DATA_SIDE_DEF( 1856, -2880, 1664, -2880, 192, 1, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2176, -2920, 1920, -2920, 256, 1, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2176, -2816, 2176, -2920, 104, -1, 0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1664, -2880, 1664, -2816, 64, -1, 0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1344, -3360, 1344, -3264, 96, 27, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1376, -3264, 1376, -3360, 96, 1, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1344, -3264, 1376, -3264, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1376, -3360, 1344, -3360, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1344, -3520, 1344, -3392, 128, 29, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1344, -3392, 1376, -3392, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1376, -3520, 1344, -3520, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1376, -3392, 1376, -3520, 128, 1, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1344, -3200, 1344, -3104, 96, 27, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1376, -3104, 1376, -3200, 96, 1, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1376, -3200, 1344, -3200, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1344, -3104, 1376, -3104, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2240, -4096, 2112, -4032, 143, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2048, -3904, 2176, -3904, 128, 7, -0.063, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2240, -3968, 2240, -4096, 128, 79, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2176, -3904, 2176, -3920, 16, -1, 0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2176, -3920, 2240, -3968, 80, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2112, -4032, 2048, -3920, 128, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2048, -3920, 2048, -3904, 16, -1, 0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2048, -3904, 2048, -3872, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2176, -3872, 2176, -3904, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2048, -3872, 2176, -3872, 128, 8, -0.063, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2176, -3904, 2048, -3904, 128, 6, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2048, -3872, 2048, -3840, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2176, -3840, 2176, -3872, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2048, -3840, 2176, -3840, 128, 9, -0.063, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2176, -3872, 2048, -3872, 128, 7, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2176, -3808, 2176, -3840, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2048, -3840, 2048, -3808, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2048, -3808, 2176, -3808, 128, 10, -0.063, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2176, -3840, 2048, -3840, 128, 8, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2176, -3776, 2176, -3808, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2048, -3808, 2048, -3776, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2048, -3776, 2176, -3776, 128, 13, -0.063, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2176, -3808, 2048, -3808, 128, 9, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1528, -3680, 1376, -3648, 155, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN144, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1672, -3744, 1528, -3680, 157, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN144, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1984, -3776, 1672, -3744, 313, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN144, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1376, -3648, 1984, -3648, 608, 1, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 1984, -3648, 1984, -3776, 128, 12, 0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 2240, -3776, 2208, -3680, 101, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2208, -3680, 2176, -3680, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2016, -3680, 1984, -3776, 101, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2048, -3680, 2016, -3680, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2176, -3680, 2048, -3680, 128, 13, -0.063, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 1984, -3648, 2240, -3648, 256, 1, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 1984, -3776, 1984, -3648, 128, 11, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 2240, -3648, 2240, -3776, 128, 63, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 2048, -3680, 2176, -3680, 128, 12, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2176, -3776, 2048, -3776, 128, 10, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2176, -3680, 2176, -3704, 24, -1, 0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2176, -3704, 2176, -3776, 72, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2048, -3776, 2048, -3704, 72, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2048, -3704, 2048, -3680, 24, -1, 0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2176, -2752, 2176, -2816, 64, -1, 0.063, PACK_DATA(TEX_X, TEX_LITE3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1664, -2752, 2176, -2752, 512, 15, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2176, -2816, 1664, -2816, 512, 2, -0.063, PACK_DATA(TEX_COMPUTE3, TEX_X, TEX_COMPUTE3, 0u) ),
				DATA_SIDE_DEF( 1664, -2816, 1664, -2752, 64, 23, 0.063, PACK_DATA(TEX_COMPSPAN, TEX_X, TEX_COMPTALL, 0u) ),
				DATA_SIDE_DEF( 1664, -2368, 1664, -2112, 256, 21, 0.063, PACK_DATA(TEX_COMPSPAN, TEX_X, TEX_COMPTALL, 0u) ),
				DATA_SIDE_DEF( 1664, -2112, 2496, -2112, 832, 21, -0.063, PACK_DATA(TEX_COMPSPAN, TEX_X, TEX_COMPTALL, 0u) ),
				DATA_SIDE_DEF( 2496, -2112, 2496, -2496, 384, 21, 0.063, PACK_DATA(TEX_COMPSPAN, TEX_X, TEX_COMPTALL, 0u) ),
				DATA_SIDE_DEF( 2176, -2752, 1664, -2752, 512, 14, -0.063, PACK_DATA(TEX_COMPSPAN, TEX_X, TEX_COMPTALL, 0u) ),
				DATA_SIDE_DEF( 2496, -2688, 2496, -2752, 64, -1, 0.063, PACK_DATA(TEX_X, TEX_STARGR1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1664, -2560, 1664, -2432, 128, 22, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_STARGR1, 0u) ),
				DATA_SIDE_DEF( 2496, -2560, 2496, -2688, 128, 61, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_STARGR1, 0u) ),
				DATA_SIDE_DEF( 1664, -2624, 1664, -2600, 24, -1, 0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1664, -2600, 1664, -2560, 40, -1, 0.063, PACK_DATA(TEX_X, TEX_STARGR1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1664, -2432, 1664, -2392, 40, -1, 0.063, PACK_DATA(TEX_X, TEX_STARGR1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1664, -2392, 1664, -2368, 24, -1, 0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2496, -2496, 2496, -2520, 24, -1, 0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2496, -2520, 2496, -2560, 40, -1, 0.063, PACK_DATA(TEX_X, TEX_STARGR1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2496, -2752, 2200, -2752, 296, -1, -0.063, PACK_DATA(TEX_X, TEX_STARGR1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2200, -2752, 2176, -2752, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2112, -2592, 2336, -2592, 224, -1, -0.063, PACK_DATA(TEX_X, TEX_COMPTILE, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2336, -2592, 2336, -2272, 320, -1, 0.063, PACK_DATA(TEX_X, TEX_COMPTILE, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2336, -2272, 2112, -2272, 224, -1, -0.063, PACK_DATA(TEX_X, TEX_COMPTILE, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2112, -2272, 2112, -2304, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_COMPTILE, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2112, -2304, 2144, -2304, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2144, -2560, 2112, -2560, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2112, -2560, 2112, -2592, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_COMPTILE, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2144, -2304, 2144, -2560, 256, 20, 0.063, PACK_DATA(TEX_STEP1, TEX_X, TEX_COMPTILE, 0u) ),
				DATA_SIDE_DEF( 1992, -2552, 1784, -2552, 208, 18, -0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1784, -2552, 1784, -2632, 80, 18, 0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1784, -2632, 1992, -2632, 208, 18, -0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1992, -2632, 1992, -2552, 80, 18, 0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1784, -2312, 1992, -2312, 208, 16, -0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1992, -2312, 1992, -2232, 80, 16, 0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1992, -2232, 1784, -2232, 208, 16, -0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1784, -2232, 1784, -2312, 80, 16, 0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1664, -2752, 1664, -2624, 128, 23, 0.063, PACK_DATA(TEX_COMPSPAN, TEX_X, TEX_COMPTALL, 0u) ),
				DATA_SIDE_DEF( 1984, -2304, 1984, -2240, 64, 17, 0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1984, -2240, 1792, -2240, 192, 17, -0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1792, -2240, 1792, -2304, 64, 17, 0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1792, -2304, 1984, -2304, 192, 17, -0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1992, -2312, 1784, -2312, 208, 15, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1992, -2232, 1992, -2312, 80, 15, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1784, -2232, 1992, -2232, 208, 15, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1784, -2312, 1784, -2232, 80, 15, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1984, -2240, 1984, -2304, 64, 16, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1792, -2240, 1984, -2240, 192, 16, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1792, -2304, 1792, -2240, 64, 16, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1984, -2304, 1792, -2304, 192, 16, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1984, -2624, 1984, -2560, 64, 19, 0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1984, -2560, 1792, -2560, 192, 19, -0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1792, -2560, 1792, -2624, 64, 19, 0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1792, -2624, 1984, -2624, 192, 19, -0.063, PACK_DATA(TEX_STARGR1, TEX_X, TEX_PLANET1, 0u) ),
				DATA_SIDE_DEF( 1784, -2552, 1992, -2552, 208, 15, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1784, -2632, 1784, -2552, 80, 15, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1992, -2632, 1784, -2632, 208, 15, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1992, -2552, 1992, -2632, 80, 15, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1984, -2560, 1984, -2624, 64, 18, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1792, -2560, 1984, -2560, 192, 18, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1792, -2624, 1792, -2560, 64, 18, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1984, -2624, 1792, -2624, 192, 18, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2144, -2304, 2176, -2304, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_COMPTILE, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2176, -2560, 2144, -2560, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_COMPTILE, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2144, -2560, 2144, -2304, 256, 15, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2176, -2304, 2176, -2560, 256, 59, 0.063, PACK_DATA(TEX_STEP1, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1664, -2368, 1600, -2368, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_LITE3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1600, -2368, 1600, -2112, 256, -1, 0.063, PACK_DATA(TEX_X, TEX_COMPUTE2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2560, -2112, 2560, -2496, 384, -1, 0.063, PACK_DATA(TEX_X, TEX_COMPUTE2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2560, -2496, 2496, -2496, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_LITE3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1600, -2048, 1664, -2048, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_LITE3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1664, -2048, 2496, -2048, 832, -1, -0.063, PACK_DATA(TEX_X, TEX_COMPUTE2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2496, -2048, 2560, -2048, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_LITE3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2560, -2048, 2560, -2112, 64, -1, 0.063, PACK_DATA(TEX_X, TEX_LITE3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1600, -2112, 1600, -2048, 64, -1, 0.063, PACK_DATA(TEX_X, TEX_LITE3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1664, -2112, 1664, -2368, 256, 15, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2496, -2112, 1664, -2112, 832, 15, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2496, -2496, 2496, -2112, 384, 15, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1552, -2560, 1552, -2432, 128, 26, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_BIGDOOR2, 0u) ),
				DATA_SIDE_DEF( 1664, -2560, 1552, -2560, 112, -1, -0.063, PACK_DATA(TEX_X, TEX_STARGR1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1552, -2432, 1664, -2432, 112, -1, -0.063, PACK_DATA(TEX_X, TEX_STARGR1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1664, -2432, 1664, -2560, 128, 15, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1600, -2624, 1664, -2624, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_LITE3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1664, -2816, 1600, -2816, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_LITE3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1600, -2816, 1600, -2752, 64, -1, 0.063, PACK_DATA(TEX_X, TEX_LITE3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1600, -2752, 1600, -2624, 128, -1, 0.063, PACK_DATA(TEX_X, TEX_COMPUTE2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1664, -2752, 1664, -2816, 64, 14, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1664, -2624, 1664, -2752, 128, 15, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1216, -2880, 1248, -2528, 353, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1384, -2592, 1344, -2880, 290, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1472, -2560, 1384, -2592, 93, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1248, -2528, 1472, -2432, 243, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1344, -2880, 1216, -2880, 128, 29, -0.063, PACK_DATA(TEX_STEP6, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF( 1472, -2432, 1472, -2560, 128, 25, 0.063, PACK_DATA(TEX_STEP6, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF( 1472, -2560, 1472, -2432, 128, 24, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1536, -2432, 1536, -2560, 128, 26, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_BIGDOOR2, 0u) ),
				DATA_SIDE_DEF( 1536, -2560, 1472, -2560, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1472, -2432, 1536, -2432, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1536, -2560, 1536, -2432, 128, 25, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1552, -2432, 1552, -2560, 128, 22, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1536, -2432, 1552, -2432, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORTRAK, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1552, -2560, 1536, -2560, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORTRAK, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1216, -3392, 1216, -3360, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1216, -3360, 1184, -3360, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1184, -3104, 1216, -3104, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1216, -3104, 1216, -3072, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1344, -3264, 1344, -3360, 96, 3, 0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 2u) ),
				DATA_SIDE_DEF( 1344, -3200, 1344, -3264, 64, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1344, -3104, 1344, -3200, 96, 5, 0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 2u) ),
				DATA_SIDE_DEF( 1344, -3360, 1216, -3392, 131, 29, 0.000, PACK_DATA(TEX_STEP6, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF( 1216, -3072, 1344, -3104, 131, 29, 0.000, PACK_DATA(TEX_STEP6, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  928, -3104, 1184, -3104, 256, 28, -0.063, PACK_DATA(TEX_STEP6, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF( 1184, -3360,  928, -3360, 256, 28, -0.063, PACK_DATA(TEX_STEP6, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  928, -3360,  928, -3104, 256, 28, 0.063, PACK_DATA(TEX_STEP6, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  928, -3392,  928, -3360, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF(  928, -3360,  896, -3360, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1184, -3360, 1184, -3392, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF(  896, -3104,  928, -3104, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF(  928, -3104,  928, -3072, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1184, -3072, 1184, -3104, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1184, -3392,  928, -3392, 256, 29, -0.063, PACK_DATA(TEX_STEP6, TEX_X, TEX_COMPUTE2, 0u) ),
				DATA_SIDE_DEF( 1184, -3104,  928, -3104, 256, 27, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  928, -3360, 1184, -3360, 256, 27, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  928, -3104,  928, -3360, 256, 27, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  896, -3360,  896, -3104, 256, 29, 0.063, PACK_DATA(TEX_STEP6, TEX_X, TEX_COMPUTE2, 0u) ),
				DATA_SIDE_DEF(  928, -3072, 1184, -3072, 256, 29, -0.063, PACK_DATA(TEX_STEP6, TEX_X, TEX_COMPUTE2, 0u) ),
				DATA_SIDE_DEF( 1152, -3648, 1088, -3648, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1024, -3648,  960, -3648, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1280, -3552, 1152, -3648, 160, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  960, -3648,  832, -3552, 160, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1344, -3552, 1280, -3552, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  832, -3552,  704, -3552, 128, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  896, -3392,  928, -3392, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF(  896, -3360,  896, -3392, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1184, -3392, 1216, -3392, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF(  896, -3072,  896, -3104, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF(  928, -3072,  896, -3072, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1216, -3072, 1184, -3072, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1344, -2880, 1344, -3104, 224, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  928, -3392, 1184, -3392, 256, 28, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1216, -3392, 1344, -3360, 131, 27, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1344, -3104, 1216, -3072, 131, 27, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  704, -2944,  832, -2944, 128, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  832, -2944,  968, -2880, 150, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  968, -2880, 1216, -2880, 248, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1088, -3648, 1024, -3648, 64, 30, -0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  896, -3104,  896, -3360, 256, 28, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1184, -3072,  928, -3072, 256, 28, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  704, -3552,  704, -3360, 192, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  704, -3104,  704, -2944, 160, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  704, -3360,  704, -3104, 256, 31, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1216, -2880, 1344, -2880, 128, 24, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 1344, -3360, 1344, -3392, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1344, -3520, 1344, -3552, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1344, -3392, 1344, -3520, 128, 4, 0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF( 1088, -3680, 1024, -3680, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_DOOR3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1024, -3680, 1024, -3648, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_LITE3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1088, -3648, 1088, -3680, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_LITE3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 1024, -3648, 1088, -3648, 64, 29, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  704, -3104,  704, -3360, 256, 29, 0.063, PACK_DATA(TEX_STEP6, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  512, -3328,  512, -3304, 24, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  512, -3160,  512, -3136, 24, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  512, -3136,  680, -3104, 171, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  680, -3104,  704, -3104, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF(  704, -3360,  680, -3360, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF(  680, -3360,  512, -3328, 171, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  496, -3304,  496, -3160, 144, 32, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  512, -3304,  496, -3304, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORTRAK, TEX_X, 2u) ),
				DATA_SIDE_DEF(  496, -3160,  512, -3160, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORTRAK, TEX_X, 2u) ),
				DATA_SIDE_DEF(  496, -3160,  496, -3304, 144, 31, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_STARG3, 0u) ),
				DATA_SIDE_DEF(  496, -3304,  496, -3328, 24, -1, 0.063, PACK_DATA(TEX_X, TEX_STARG3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  496, -3328,  448, -3456, 136, -1, 0.000, PACK_DATA(TEX_X, TEX_STARG3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  448, -3456,  128, -3456, 320, -1, -0.063, PACK_DATA(TEX_X, TEX_STARG3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  128, -3008,  448, -3008, 320, -1, -0.063, PACK_DATA(TEX_X, TEX_STARG3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  496, -3136,  496, -3160, 24, -1, 0.063, PACK_DATA(TEX_X, TEX_STARG3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  448, -3008,  496, -3136, 136, -1, 0.000, PACK_DATA(TEX_X, TEX_STARG3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  128, -3264,  160, -3264, 32, 48, -0.063, PACK_DATA(TEX_SLADWALL, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  160, -3264,  192, -3264, 32, 39, -0.063, PACK_DATA(TEX_SLADWALL, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  192, -3264,  224, -3264, 32, 38, -0.063, PACK_DATA(TEX_SLADWALL, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  224, -3264,  256, -3264, 32, 37, -0.063, PACK_DATA(TEX_SLADWALL, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  256, -3264,  288, -3264, 32, 34, -0.063, PACK_DATA(TEX_SLADWALL, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  288, -3264,  320, -3264, 32, 33, -0.063, PACK_DATA(TEX_SLADWALL, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  320, -3264,  320, -3200, 64, 33, 0.063, PACK_DATA(TEX_STEP6, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  320, -3200,  288, -3200, 32, 33, -0.063, PACK_DATA(TEX_SLADWALL, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  288, -3200,  256, -3200, 32, 34, -0.063, PACK_DATA(TEX_SLADWALL, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  256, -3200,  224, -3200, 32, 37, -0.063, PACK_DATA(TEX_SLADWALL, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  224, -3200,  192, -3200, 32, 38, -0.063, PACK_DATA(TEX_SLADWALL, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  192, -3200,  160, -3200, 32, 39, -0.063, PACK_DATA(TEX_SLADWALL, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  160, -3200,  128, -3200, 32, 48, -0.063, PACK_DATA(TEX_SLADWALL, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  128, -3200,   64, -3072, 143, 47, 0.000, PACK_DATA(TEX_STARG3, TEX_X, TEX_STARG3, 0u) ),
				DATA_SIDE_DEF(   64, -3072,  128, -3008, 90, -1, 0.000, PACK_DATA(TEX_X, TEX_STARG3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  128, -3456,   64, -3392, 90, -1, 0.000, PACK_DATA(TEX_X, TEX_STARG3, TEX_X, 2u) ),
				DATA_SIDE_DEF(   64, -3392,  128, -3264, 143, 47, 0.000, PACK_DATA(TEX_STARG3, TEX_X, TEX_STARG3, 0u) ),
				DATA_SIDE_DEF(  256, -3136,  320, -3136, 64, 36, -0.063, PACK_DATA(TEX_TEKWALL4, TEX_X, TEX_TEKWALL4, 0u) ),
				DATA_SIDE_DEF(  320, -3136,  320, -3072, 64, 36, 0.063, PACK_DATA(TEX_TEKWALL4, TEX_X, TEX_TEKWALL4, 0u) ),
				DATA_SIDE_DEF(  320, -3072,  256, -3072, 64, 36, -0.063, PACK_DATA(TEX_SW1COMP, TEX_X, TEX_TEKWALL4, 0u) ),
				DATA_SIDE_DEF(  256, -3072,  256, -3136, 64, 36, 0.063, PACK_DATA(TEX_TEKWALL4, TEX_X, TEX_TEKWALL4, 0u) ),
				DATA_SIDE_DEF(  256, -3392,  320, -3392, 64, 35, -0.063, PACK_DATA(TEX_TEKWALL4, TEX_X, TEX_TEKWALL4, 0u) ),
				DATA_SIDE_DEF(  320, -3392,  320, -3328, 64, 35, 0.063, PACK_DATA(TEX_TEKWALL4, TEX_X, TEX_TEKWALL4, 0u) ),
				DATA_SIDE_DEF(  320, -3328,  256, -3328, 64, 35, -0.063, PACK_DATA(TEX_TEKWALL4, TEX_X, TEX_TEKWALL4, 0u) ),
				DATA_SIDE_DEF(  256, -3328,  256, -3392, 64, 35, 0.063, PACK_DATA(TEX_TEKWALL4, TEX_X, TEX_TEKWALL4, 0u) ),
				DATA_SIDE_DEF(  320, -3264,  288, -3264, 32, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  320, -3200,  320, -3264, 64, 32, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  288, -3200,  320, -3200, 32, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  288, -3264,  288, -3200, 64, 34, 0.063, PACK_DATA(TEX_STEP6, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  288, -3264,  256, -3264, 32, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  256, -3200,  288, -3200, 32, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  256, -3264,  256, -3200, 64, 37, 0.063, PACK_DATA(TEX_STEP6, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  288, -3200,  288, -3264, 64, 33, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  320, -3392,  256, -3392, 64, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  320, -3328,  320, -3392, 64, 32, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  256, -3328,  320, -3328, 64, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  256, -3392,  256, -3328, 64, 32, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  320, -3136,  256, -3136, 64, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  320, -3072,  320, -3136, 64, 32, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  256, -3072,  320, -3072, 64, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  256, -3136,  256, -3072, 64, 32, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  256, -3264,  224, -3264, 32, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  224, -3200,  256, -3200, 32, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  224, -3264,  224, -3200, 64, 38, 0.063, PACK_DATA(TEX_STEP6, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  256, -3200,  256, -3264, 64, 34, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  224, -3264,  192, -3264, 32, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  192, -3200,  224, -3200, 32, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  192, -3264,  192, -3200, 64, 39, 0.063, PACK_DATA(TEX_STEP6, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  224, -3200,  224, -3264, 64, 37, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  192, -3264,  160, -3264, 32, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  160, -3200,  192, -3200, 32, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  160, -3264,  160, -3200, 64, 48, 0.063, PACK_DATA(TEX_STEP6, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(  192, -3200,  192, -3264, 64, 38, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(   64, -3648, -640, -3648, 704, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN144, TEX_X, 2u) ),
				DATA_SIDE_DEF( -640, -3648, -768, -3520, 181, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN144, TEX_X, 2u) ),
				DATA_SIDE_DEF( -768, -3520, -768, -2944, 576, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWN144, TEX_X, 2u) ),
				DATA_SIDE_DEF( -768, -2944, -640, -2816, 181, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN144, TEX_X, 2u) ),
				DATA_SIDE_DEF( -640, -2816,   64, -2816, 704, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN144, TEX_X, 2u) ),
				DATA_SIDE_DEF( -640, -3520,   64, -3648, 715, 42, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( -640, -2944, -640, -3520, 576, 42, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF(   64, -2816, -640, -2944, 715, 42, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( -256, -3328, -320, -3296, 71, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( -320, -3168, -256, -3136, 71, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  -64, -3136,  -64, -3328, 192, 47, 0.063, PACK_DATA(TEX_STARG3, TEX_X, TEX_STARG3, 0u) ),
				DATA_SIDE_DEF( -128, -3328, -256, -3328, 128, 46, -0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 2u) ),
				DATA_SIDE_DEF( -256, -3136, -128, -3136, 128, 43, -0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 2u) ),
				DATA_SIDE_DEF( -320, -3296, -320, -3168, 128, 45, 0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 2u) ),
				DATA_SIDE_DEF( -128, -3136,  -88, -3136, 40, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  -88, -3136,  -64, -3136, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF(  -64, -3328,  -88, -3328, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF(  -88, -3328, -128, -3328, 40, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( -240, -3264, -208, -3264, 32, 44, -0.063, PACK_DATA(TEX_TEKWALL1, TEX_X, TEX_TEKWALL1, 0u) ),
				DATA_SIDE_DEF( -208, -3264, -192, -3248, 22, 44, 0.000, PACK_DATA(TEX_TEKWALL1, TEX_X, TEX_TEKWALL1, 0u) ),
				DATA_SIDE_DEF( -192, -3248, -192, -3216, 32, 44, 0.063, PACK_DATA(TEX_TEKWALL1, TEX_X, TEX_TEKWALL1, 0u) ),
				DATA_SIDE_DEF( -192, -3216, -208, -3200, 22, 44, 0.000, PACK_DATA(TEX_TEKWALL1, TEX_X, TEX_TEKWALL1, 0u) ),
				DATA_SIDE_DEF( -208, -3200, -240, -3200, 32, 44, -0.063, PACK_DATA(TEX_TEKWALL1, TEX_X, TEX_TEKWALL1, 0u) ),
				DATA_SIDE_DEF( -240, -3200, -256, -3216, 22, 44, 0.000, PACK_DATA(TEX_TEKWALL1, TEX_X, TEX_TEKWALL1, 0u) ),
				DATA_SIDE_DEF( -256, -3216, -256, -3248, 32, 44, 0.063, PACK_DATA(TEX_TEKWALL1, TEX_X, TEX_TEKWALL1, 0u) ),
				DATA_SIDE_DEF( -256, -3248, -240, -3264, 22, 44, 0.000, PACK_DATA(TEX_TEKWALL1, TEX_X, TEX_TEKWALL1, 0u) ),
				DATA_SIDE_DEF( -256, -3120, -336, -3120, 80, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( -336, -3120, -336, -3168, 48, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( -336, -3296, -336, -3344, 48, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( -336, -3344, -256, -3344, 80, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( -128, -3344,  -96, -3344, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  -96, -3344,   64, -3520, 237, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  -96, -3120, -128, -3120, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(   64, -2944,  -96, -3120, 237, -1, 0.000, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF( -256, -3344, -128, -3344, 128, 46, -0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF( -128, -3120, -256, -3120, 128, 43, -0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF( -336, -3168, -336, -3296, 128, 45, 0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_STARTAN3, 0u) ),
				DATA_SIDE_DEF(   64, -2816,   64, -2944, 128, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(   64, -3520,   64, -3648, 128, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN3, TEX_X, 2u) ),
				DATA_SIDE_DEF(   64, -3648, -640, -3520, 715, 40, 0.000, PACK_DATA(TEX_STARG3, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( -640, -3520, -640, -2944, 576, 40, 0.063, PACK_DATA(TEX_STARG3, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( -640, -2944,   64, -2816, 715, 40, 0.000, PACK_DATA(TEX_STARG3, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( -128, -3120, -128, -3136, 16, -1, 0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( -256, -3136, -256, -3120, 16, -1, 0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( -128, -3136, -256, -3136, 128, 41, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 2u) ),
				DATA_SIDE_DEF( -256, -3120, -128, -3120, 128, 42, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( -208, -3264, -240, -3264, 32, 41, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( -192, -3248, -208, -3264, 22, 41, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( -192, -3216, -192, -3248, 32, 41, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( -208, -3200, -192, -3216, 22, 41, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( -240, -3200, -208, -3200, 32, 41, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( -256, -3216, -240, -3200, 22, 41, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( -256, -3248, -256, -3216, 32, 41, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( -240, -3264, -256, -3248, 22, 41, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( -320, -3296, -336, -3296, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( -336, -3168, -320, -3168, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( -320, -3168, -320, -3296, 128, 41, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 2u) ),
				DATA_SIDE_DEF( -336, -3296, -336, -3168, 128, 42, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( -256, -3344, -256, -3328, 16, -1, 0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( -128, -3328, -128, -3344, 16, -1, 0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( -256, -3328, -128, -3328, 128, 41, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 2u) ),
				DATA_SIDE_DEF( -128, -3344, -256, -3344, 128, 42, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  128, -3200,  128, -3264, 64, 48, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(   64, -3072,  128, -3200, 143, 32, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  128, -3264,   64, -3392, 143, 32, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(   64, -3392,   48, -3392, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF(   48, -3392,  -64, -3328, 128, -1, 0.000, PACK_DATA(TEX_X, TEX_STARG3, TEX_X, 2u) ),
				DATA_SIDE_DEF(  -64, -3136,   48, -3072, 128, -1, 0.000, PACK_DATA(TEX_X, TEX_STARG3, TEX_X, 2u) ),
				DATA_SIDE_DEF(   48, -3072,   64, -3072, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF(  -64, -3328,  -64, -3136, 192, 41, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  160, -3264,  128, -3264, 32, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  128, -3200,  160, -3200, 32, 32, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  160, -3200,  160, -3264, 64, 39, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF(  128, -3264,  128, -3200, 64, 47, 0.063, PACK_DATA(TEX_STARG3, TEX_X, TEX_STARG3, 0u) ),
				DATA_SIDE_DEF( 2880, -2912, 2880, -2880, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2752, -3048, 2752, -2912, 136, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2752, -2784, 2944, -2656, 230, 50, 0.000, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 3048, -2880, 2752, -3048, 340, 56, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2880, -2880, 2752, -2800, 150, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2752, -2800, 2752, -2784, 16, -1, 0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2944, -2656, 2960, -2656, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2960, -2656, 3048, -2880, 240, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2752, -2912, 2880, -2912, 128, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2752, -2784, 2624, -2784, 128, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2752, -2560, 2944, -2656, 214, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2944, -2656, 2752, -2784, 230, 49, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2624, -2784, 2752, -2560, 257, 61, 0.000, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2752, -3584, 2752, -3360, 224, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2752, -3360, 2944, -3536, 260, 57, 0.000, PACK_DATA(TEX_NUKE24, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2944, -3536, 2752, -3584, 197, 52, 0.000, PACK_DATA(TEX_NUKE24, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2944, -3648, 2752, -3584, 202, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3072, -3648, 3072, -4000, 352, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2944, -3776, 2944, -3648, 128, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2944, -3536, 3072, -3648, 170, 56, 0.000, PACK_DATA(TEX_NUKE24, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2752, -3584, 2944, -3536, 197, 51, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2944, -3904, 2944, -3776, 128, 83, 0.063, PACK_DATA(TEX_BROWN96, TEX_X, TEX_BROWN96, 0u) ),
				DATA_SIDE_DEF( 2944, -4000, 2944, -3904, 96, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3072, -4000, 2944, -4000, 128, 82, -0.063, PACK_DATA(TEX_BROWNGRN, TEX_X, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 3136, -3072, 3304, -3040, 171, 56, 0.000, PACK_DATA(TEX_NUKE24, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3280, -3320, 2984, -3200, 319, 56, 0.000, PACK_DATA(TEX_NUKE24, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2984, -3200, 3136, -3072, 198, 56, 0.000, PACK_DATA(TEX_NUKE24, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3104, -3552, 3280, -3320, 291, 56, 0.000, PACK_DATA(TEX_NUKE24, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3264, -3616, 3104, -3552, 172, 56, 0.000, PACK_DATA(TEX_NUKE24, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3352, -3568, 3264, -3616, 100, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3472, -3432, 3408, -3432, 64, 55, -0.063, PACK_DATA(TEX_BROWNGRN, TEX_X, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 3408, -3432, 3312, -3496, 115, 55, 0.000, PACK_DATA(TEX_BROWNGRN, TEX_X, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 3312, -3496, 3352, -3568, 82, 55, 0.000, PACK_DATA(TEX_BROWNGRN, TEX_X, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 3400, -3152, 3472, -3432, 289, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3304, -3040, 3400, -3152, 147, 58, 0.000, PACK_DATA(TEX_BROWNGRN, TEX_BROWNGRN, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 3352, -3568, 3448, -3520, 107, 55, 0.000, PACK_DATA(TEX_BROWNGRN, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3520, -3904, 3328, -3968, 202, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3328, -3744, 3360, -3648, 101, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3328, -3968, 3328, -3744, 224, 86, 0.063, PACK_DATA(TEX_BROWNGRN, TEX_X, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 3448, -3520, 3472, -3520, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3472, -3520, 3520, -3584, 80, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3360, -3648, 3352, -3592, 56, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3352, -3592, 3352, -3568, 24, -1, 0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3520, -3584, 3520, -3840, 256, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3520, -3840, 3520, -3904, 64, 87, 0.063, PACK_DATA(TEX_BROWNGRN, TEX_X, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 3408, -3432, 3472, -3432, 64, 53, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3312, -3496, 3408, -3432, 115, 53, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3352, -3568, 3312, -3496, 82, 53, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3472, -3432, 3448, -3520, 91, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3448, -3520, 3352, -3568, 107, 54, 0.000, PACK_DATA(TEX_BROWNGRN, TEX_X, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 3048, -2880, 3048, -2944, 64, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3048, -2944, 3304, -3040, 273, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3304, -3040, 3136, -3072, 171, 53, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2944, -3536, 3112, -3360, 243, 57, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3112, -3360, 2816, -3232, 322, 57, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2984, -3200, 3280, -3320, 319, 53, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2816, -3232, 2976, -3072, 226, 57, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3136, -3072, 2984, -3200, 198, 53, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3264, -3616, 3072, -3648, 194, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3072, -3648, 2944, -3536, 170, 52, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2752, -3048, 3048, -2880, 340, 49, 0.000, PACK_DATA(TEX_NUKE24, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3280, -3320, 3104, -3552, 291, 53, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3104, -3552, 3264, -3616, 172, 53, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2976, -3072, 2752, -3048, 225, 57, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3112, -3360, 2944, -3536, 243, 56, 0.000, PACK_DATA(TEX_NUKE24, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2816, -3232, 3112, -3360, 322, 56, 0.000, PACK_DATA(TEX_NUKE24, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2976, -3072, 2816, -3232, 226, 56, 0.000, PACK_DATA(TEX_NUKE24, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2944, -3536, 2752, -3360, 260, 51, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2752, -3048, 2976, -3072, 225, 56, 0.000, PACK_DATA(TEX_NUKE24, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2752, -3360, 2752, -3112, 248, 62, 0.063, PACK_DATA(TEX_BROWNGRN, TEX_X, TEX_BROWNGRN, 2u) ),
				DATA_SIDE_DEF( 2752, -3112, 2752, -3048, 64, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3680, -3904, 3584, -3904, 96, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3744, -3808, 3680, -3904, 115, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3584, -3840, 3616, -3776, 71, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3616, -3776, 3552, -3552, 232, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3552, -3552, 3552, -3392, 160, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3552, -3392, 3648, -3264, 160, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3680, -3552, 3744, -3808, 263, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3680, -3392, 3680, -3552, 160, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3808, -3264, 3680, -3392, 181, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3648, -3264, 3496, -3032, 277, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3584, -2880, 3808, -3264, 444, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3496, -3032, 3456, -3032, 40, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3360, -2880, 3584, -2880, 224, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3400, -3152, 3304, -3040, 147, 53, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 3584, -3904, 3584, -3840, 64, 87, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3304, -3040, 3320, -3040, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3320, -3040, 3360, -2880, 164, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3456, -3032, 3416, -3152, 126, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3416, -3152, 3400, -3152, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2176, -2304, 2208, -2304, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_COMPTILE, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2208, -2560, 2176, -2560, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_COMPTILE, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2176, -2560, 2176, -2304, 256, 20, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2208, -2304, 2208, -2560, 256, 60, 0.063, PACK_DATA(TEX_STEP1, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2208, -2304, 2304, -2304, 96, -1, -0.063, PACK_DATA(TEX_X, TEX_COMPTILE, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2304, -2304, 2304, -2560, 256, -1, 0.063, PACK_DATA(TEX_X, TEX_COMPTILE, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2304, -2560, 2208, -2560, 96, -1, -0.063, PACK_DATA(TEX_X, TEX_COMPTILE, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2208, -2560, 2208, -2304, 256, 59, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2520, -2560, 2752, -2560, 232, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2624, -2784, 2520, -2688, 141, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2496, -2560, 2520, -2560, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2520, -2688, 2496, -2688, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2496, -2688, 2496, -2560, 128, 15, 0.063, PACK_DATA(TEX_BROWN1, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2752, -2560, 2624, -2784, 257, 50, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2752, -3360, 2736, -3360, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2736, -3112, 2752, -3112, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2752, -3112, 2752, -3360, 248, 57, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2736, -3360, 2736, -3112, 248, 1, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2736, -3648, 2488, -3744, 265, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN144, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2488, -3744, 2240, -3776, 250, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN144, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2240, -3648, 2736, -3648, 496, 1, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 2240, -3776, 2240, -3648, 128, 12, 0.063, PACK_DATA(TEX_STARTAN3, TEX_X, TEX_X, 1u) ),
				DATA_SIDE_DEF( 2856, -4160, 2888, -4160, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2888, -4160, 2912, -4160, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3160, -4352, 3128, -4352, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3128, -4352, 3104, -4352, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3104, -4160, 3128, -4160, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3128, -4160, 3160, -4160, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2912, -4352, 2888, -4352, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2888, -4352, 2856, -4352, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2888, -4320, 2888, -4352, 32, 64, 0.063, PACK_DATA(TEX_X, TEX_BRNBIGR, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2888, -4352, 2888, -4320, 32, 64, 0.063, PACK_DATA(TEX_X, TEX_BRNBIGL, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2888, -4192, 2888, -4320, 128, 64, 0.063, PACK_DATA(TEX_X, TEX_BRNBIGC, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2888, -4320, 2888, -4192, 128, 64, 0.063, PACK_DATA(TEX_X, TEX_BRNBIGC, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2888, -4160, 2888, -4192, 32, 64, 0.063, PACK_DATA(TEX_X, TEX_BRNBIGL, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2888, -4192, 2888, -4160, 32, 64, 0.063, PACK_DATA(TEX_X, TEX_BRNBIGR, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3128, -4352, 3128, -4320, 32, 64, 0.063, PACK_DATA(TEX_X, TEX_BRNBIGL, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3128, -4320, 3128, -4352, 32, 64, 0.063, PACK_DATA(TEX_X, TEX_BRNBIGR, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3128, -4192, 3128, -4160, 32, 64, 0.063, PACK_DATA(TEX_X, TEX_BRNBIGR, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3128, -4160, 3128, -4192, 32, 64, 0.063, PACK_DATA(TEX_X, TEX_BRNBIGL, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3128, -4320, 3128, -4192, 128, 64, 0.063, PACK_DATA(TEX_X, TEX_BRNBIGC, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3128, -4192, 3128, -4320, 128, 64, 0.063, PACK_DATA(TEX_X, TEX_BRNBIGC, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2912, -4160, 3104, -4160, 192, 84, -0.063, PACK_DATA(TEX_BROWNGRN, TEX_X, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 3104, -4352, 2912, -4352, 192, 65, -0.063, PACK_DATA(TEX_BROWNGRN, TEX_X, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 2856, -4352, 2856, -4160, 192, 65, 0.063, PACK_DATA(TEX_BROWNGRN, TEX_X, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 3160, -4160, 3160, -4352, 192, 65, 0.063, PACK_DATA(TEX_BROWNGRN, TEX_X, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 3200, -4128, 3328, -4128, 128, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2688, -4128, 2816, -4128, 128, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2816, -4128, 2856, -4160, 51, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3160, -4160, 3200, -4128, 51, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3104, -4352, 3104, -4384, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3104, -4384, 3160, -4384, 56, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3160, -4384, 3160, -4352, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2856, -4352, 2856, -4384, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2856, -4384, 2912, -4384, 56, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2912, -4384, 2912, -4352, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3328, -4544, 3072, -4544, 256, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2944, -4544, 2688, -4544, 256, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2912, -4352, 3104, -4352, 192, 64, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2688, -4544, 2688, -4128, 416, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3328, -4128, 3328, -4544, 416, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2856, -4160, 2856, -4352, 192, 64, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3160, -4352, 3160, -4160, 192, 64, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3072, -4544, 2944, -4544, 128, 66, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3072, -4544, 3072, -4608, 64, -1, 0.063, PACK_DATA(TEX_X, TEX_LITE3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2944, -4608, 2944, -4544, 64, -1, 0.063, PACK_DATA(TEX_X, TEX_LITE3, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2944, -4544, 3072, -4544, 128, 65, -0.063, PACK_DATA(TEX_BROWNGRN, TEX_X, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 3072, -4608, 3040, -4608, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2976, -4608, 2944, -4608, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3024, -4592, 2992, -4592, 32, 67, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_EXITSIGN, 0u) ),
				DATA_SIDE_DEF( 2992, -4600, 3024, -4600, 32, 67, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_EXITSIGN, 0u) ),
				DATA_SIDE_DEF( 3024, -4600, 3024, -4592, 8, 67, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_EXITSIGN, 0u) ),
				DATA_SIDE_DEF( 2992, -4592, 2992, -4600, 8, 67, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_EXITSIGN, 0u) ),
				DATA_SIDE_DEF( 3040, -4608, 2976, -4608, 64, 68, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_STARTAN1, 0u) ),
				DATA_SIDE_DEF( 2992, -4592, 3024, -4592, 32, 66, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3024, -4600, 2992, -4600, 32, 66, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3024, -4592, 3024, -4600, 8, 66, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2992, -4600, 2992, -4592, 8, 66, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2976, -4632, 2976, -4608, 24, -1, 0.063, PACK_DATA(TEX_X, TEX_EXITDOOR, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3040, -4608, 3040, -4632, 24, -1, 0.063, PACK_DATA(TEX_X, TEX_EXITDOOR, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3040, -4632, 2976, -4632, 64, 69, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_EXITDOOR, 0u) ),
				DATA_SIDE_DEF( 2976, -4608, 3040, -4608, 64, 66, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3040, -4632, 3040, -4648, 16, -1, 0.063, PACK_DATA(TEX_X, TEX_DOORTRAK, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2976, -4648, 2976, -4632, 16, -1, 0.063, PACK_DATA(TEX_X, TEX_DOORTRAK, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3040, -4648, 2976, -4648, 64, 72, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2976, -4632, 3040, -4632, 64, 68, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3040, -4672, 3104, -4672, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2912, -4672, 2976, -4672, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3104, -4672, 3104, -4864, 192, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2912, -4864, 2912, -4800, 64, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2912, -4800, 2912, -4736, 64, -1, 0.063, PACK_DATA(TEX_X, TEX_SW1STRTN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2912, -4736, 2912, -4672, 64, -1, 0.063, PACK_DATA(TEX_X, TEX_STARTAN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3104, -4864, 2912, -4864, 192, -1, -0.063, PACK_DATA(TEX_X, TEX_STARTAN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2976, -4672, 3040, -4672, 64, 72, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_STARTAN1, 0u) ),
				DATA_SIDE_DEF( 3024, -4840, 2992, -4840, 32, 71, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_EXITSIGN, 0u) ),
				DATA_SIDE_DEF( 2992, -4848, 3024, -4848, 32, 71, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_EXITSIGN, 0u) ),
				DATA_SIDE_DEF( 3024, -4848, 3024, -4840, 8, 71, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_EXITSIGN, 0u) ),
				DATA_SIDE_DEF( 2992, -4840, 2992, -4848, 8, 71, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_EXITSIGN, 0u) ),
				DATA_SIDE_DEF( 2992, -4840, 3024, -4840, 32, 70, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3024, -4848, 2992, -4848, 32, 70, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3024, -4840, 3024, -4848, 8, 70, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2992, -4848, 2992, -4840, 8, 70, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3040, -4648, 3040, -4672, 24, -1, 0.063, PACK_DATA(TEX_X, TEX_EXITDOOR, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2976, -4672, 2976, -4648, 24, -1, 0.063, PACK_DATA(TEX_X, TEX_EXITDOOR, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2976, -4648, 3040, -4648, 64, 69, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_EXITDOOR, 0u) ),
				DATA_SIDE_DEF( 3040, -4672, 2976, -4672, 64, 70, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2752, -3776, 2752, -3904, 128, 74, 0.063, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2720, -3904, 2688, -3776, 131, 81, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2688, -3776, 2752, -3776, 64, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2752, -3904, 2720, -3904, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2784, -3776, 2784, -3904, 128, 75, 0.063, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2752, -3904, 2752, -3776, 128, 73, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2752, -3776, 2784, -3776, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2784, -3904, 2752, -3904, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2816, -3776, 2816, -3904, 128, 76, 0.063, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2784, -3904, 2784, -3776, 128, 74, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2784, -3776, 2816, -3776, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2816, -3904, 2784, -3904, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2848, -3776, 2848, -3904, 128, 77, 0.063, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2816, -3904, 2816, -3776, 128, 75, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2816, -3776, 2848, -3776, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2848, -3904, 2816, -3904, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2880, -3776, 2880, -3904, 128, 78, 0.063, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2848, -3904, 2848, -3776, 128, 76, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2848, -3776, 2880, -3776, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2880, -3904, 2848, -3904, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2880, -3904, 2880, -3776, 128, 77, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2880, -3776, 2912, -3776, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2912, -3904, 2880, -3904, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2912, -3776, 2912, -3904, 128, 83, 0.063, PACK_DATA(TEX_BROWN96, TEX_X, TEX_BROWN96, 0u) ),
				DATA_SIDE_DEF( 2240, -4096, 2240, -3968, 128, 6, 0.063, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2368, -3968, 2368, -4096, 128, 80, 0.063, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2368, -4096, 2344, -4096, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2344, -4096, 2264, -4096, 80, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2264, -4096, 2240, -4096, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2240, -3968, 2264, -3968, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2264, -3968, 2344, -3968, 80, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2344, -3968, 2368, -3968, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2368, -4096, 2368, -3968, 128, 79, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2632, -3792, 2688, -3920, 139, 81, 0.000, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2688, -3920, 2672, -3920, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2672, -3920, 2368, -4096, 351, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2368, -3968, 2616, -3792, 304, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2616, -3792, 2632, -3792, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2688, -3776, 2720, -3904, 131, 73, 0.000, PACK_DATA(TEX_BROWN1, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2688, -3920, 2632, -3792, 139, 80, 0.000, PACK_DATA(TEX_X, TEX_X, TEX_BROWN1, 0u) ),
				DATA_SIDE_DEF( 2632, -3792, 2688, -3776, 58, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2720, -3904, 2688, -3920, 35, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWN1, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2944, -4000, 3072, -4000, 128, 52, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2944, -4016, 2944, -4000, 16, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3072, -4000, 3072, -4016, 16, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3072, -4016, 2944, -4016, 128, 85, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_BIGDOOR4, 0u) ),
				DATA_SIDE_DEF( 2944, -3904, 2912, -3904, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2912, -3776, 2944, -3776, 32, -1, -0.063, PACK_DATA(TEX_X, TEX_DOORSTOP, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2944, -3776, 2944, -3904, 128, 52, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2912, -3904, 2912, -3776, 128, 78, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2912, -4160, 2912, -4128, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3104, -4128, 3104, -4160, 32, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3104, -4160, 2912, -4160, 192, 64, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 2912, -4128, 2944, -4032, 101, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3072, -4032, 3104, -4128, 101, -1, 0.000, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2944, -4032, 3072, -4032, 128, 85, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_BIGDOOR4, 0u) ),
				DATA_SIDE_DEF( 2944, -4032, 2944, -4016, 16, -1, 0.063, PACK_DATA(TEX_X, TEX_DOORTRAK, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3072, -4016, 3072, -4032, 16, -1, 0.063, PACK_DATA(TEX_X, TEX_DOORTRAK, TEX_X, 2u) ),
				DATA_SIDE_DEF( 2944, -4016, 3072, -4016, 128, 82, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3072, -4032, 2944, -4032, 128, 84, -0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3200, -3968, 3200, -3744, 224, -1, 0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3328, -3744, 3328, -3968, 224, 54, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_X, 0u) ),
				DATA_SIDE_DEF( 3328, -3968, 3304, -3968, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3304, -3968, 3200, -3968, 104, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3200, -3744, 3304, -3744, 104, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3304, -3744, 3328, -3744, 24, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3520, -3904, 3520, -3840, 64, 54, 0.063, PACK_DATA(TEX_X, TEX_X, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 3584, -3840, 3584, -3904, 64, 58, 0.063, PACK_DATA(TEX_BROWNGRN, TEX_X, TEX_BROWNGRN, 0u) ),
				DATA_SIDE_DEF( 3520, -3840, 3536, -3840, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3536, -3840, 3584, -3840, 48, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3584, -3904, 3536, -3904, 48, -1, -0.063, PACK_DATA(TEX_X, TEX_BROWNGRN, TEX_X, 2u) ),
				DATA_SIDE_DEF( 3536, -3904, 3520, -3904, 16, -1, -0.063, PACK_DATA(TEX_X, TEX_SUPPORT2, TEX_X, 2u) )
			),
			CXXSWIZZLE_ARRAY_VAL(DataEntity, 102,
				DataEntity( ivec2( 1056, -3616 ), ENTITY_TYPE_PLAYER, 0, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 1545, -2483 ), ENTITY_TYPE_DOOR, 0, 0, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2926, -3830 ), ENTITY_TYPE_DOOR, 0, 0, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3000, -4019 ), ENTITY_TYPE_DOOR, 0, 0, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3000, -4644 ), ENTITY_TYPE_DOOR, 0, 0, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3410, -3489 ), ENTITY_TYPE_PLATFORM, 0, -10, 7, ivec2( 2996, -4070 ) ),
				DataEntity( ivec2( 3533, -3874 ), ENTITY_TYPE_PLATFORM, 1, 200, 7, ivec2( 2945, -2918 ) ),
				DataEntity( ivec2( 288, -3104 ), ENTITY_TYPE_DECORATION, ENTITY_SUB_TYPE_DECORATION_TALL_TECHNO_PILLAR, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 288, -3360 ), ENTITY_TYPE_DECORATION, ENTITY_SUB_TYPE_DECORATION_TALL_TECHNO_PILLAR, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 528, -3312 ), ENTITY_TYPE_DECORATION, ENTITY_SUB_TYPE_DECORATION_FLOOR_LAMP, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 528, -3152 ), ENTITY_TYPE_DECORATION, ENTITY_SUB_TYPE_DECORATION_FLOOR_LAMP, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3440, -3472 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_IMP, 135, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3360, -3504 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_IMP, 135, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2912, -2816 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_TROOPER, 90, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3056, -3584 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_TROOPER, 90, 14, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3136, -3408 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_TROOPER, 90, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 1824, -3280 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_BLUEARMOR, 180, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2736, -4256 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_TROOPER, 0, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3280, -4256 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_TROOPER, 180, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3008, -4416 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_IMP, 90, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3280, -4304 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_TROOPER, 180, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2736, -4304 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_TROOPER, 0, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2880, -4416 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_MEDIKIT, 0, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 144, -3136 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 144, -3328 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 96, -3392 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 96, -3072 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 432, -3040 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 432, -3424 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 32, -3232 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( -32, -3232 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 1312, -3520 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 736, -3520 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 752, -2992 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 976, -2912 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 1312, -3264 ), ENTITY_TYPE_BARREL, 0, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 1152, -2912 ), ENTITY_TYPE_BARREL, 0, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 864, -3328 ), ENTITY_TYPE_BARREL, 0, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2672, -2752 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2752, -2640 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3232, -3040 ), ENTITY_TYPE_DECORATION, ENTITY_SUB_TYPE_DECORATION_FLOOR_LAMP, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2800, -3040 ), ENTITY_TYPE_DECORATION, ENTITY_SUB_TYPE_DECORATION_FLOOR_LAMP, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2912, -4176 ), ENTITY_TYPE_DECORATION, ENTITY_SUB_TYPE_DECORATION_FLOOR_LAMP, 180, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3104, -4176 ), ENTITY_TYPE_DECORATION, ENTITY_SUB_TYPE_DECORATION_FLOOR_LAMP, 180, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2720, -4352 ), ENTITY_TYPE_DECORATION, ENTITY_SUB_TYPE_DECORATION_FLOOR_LAMP, 180, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3296, -4352 ), ENTITY_TYPE_DECORATION, ENTITY_SUB_TYPE_DECORATION_FLOOR_LAMP, 180, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2944, -4320 ), ENTITY_TYPE_BARREL, 0, 180, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3184, -4224 ), ENTITY_TYPE_BARREL, 0, 180, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2832, -4240 ), ENTITY_TYPE_BARREL, 0, 180, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3184, -4176 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS, 180, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2832, -4176 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS, 180, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2736, -4192 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS, 180, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3280, -4192 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS, 180, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3280, -4448 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS, 180, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2736, -4448 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 180, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3072, -4832 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 180, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2944, -4832 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 180, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3072, -4704 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 180, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2944, -4704 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 180, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3008, -4816 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_IMP, 90, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3072, -4768 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_STIMPACK, 90, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2304, -4032 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_MEDIKIT, 90, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( -224, -3232 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_GREENARMOR, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( -288, -3232 ), ENTITY_TYPE_DECORATION, ENTITY_SUB_TYPE_DECORATION_BLOODY_MESS, 0, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 1056, -3552 ), ENTITY_TYPE_DECORATION, ENTITY_SUB_TYPE_DECORATION_BLOODY_MESS, 90, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 1792, -2944 ), ENTITY_TYPE_DECORATION, ENTITY_SUB_TYPE_DECORATION_BLOODY_MESS, 270, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3408, -3504 ), ENTITY_TYPE_DECORATION, ENTITY_SUB_TYPE_DECORATION_BLOODY_MESS, 135, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2784, -4160 ), ENTITY_TYPE_DECORATION, ENTITY_SUB_TYPE_DECORATION_BLOODY_MESS, 270, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2272, -2432 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_TROOPER, 180, 15, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2272, -2352 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_TROOPER, 180, 14, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2272, -2512 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 180, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2464, -2432 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 180, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2224, -2320 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 180, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2224, -2544 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 180, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2288, -2544 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 180, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2288, -2320 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 180, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 1712, -2144 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 180, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2240, -2256 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 180, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3264, -3936 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_SHOTGUN, 270, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3232, -3808 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_MEDIKIT, 270, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3216, -3888 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 0, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( -160, -3232 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 0, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 240, -3376 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 135, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 240, -3088 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 225, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 1696, -2688 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 90, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 1920, -2176 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 270, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2976, -3264 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 90, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3008, -3520 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 90, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3008, -3072 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 90, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2816, -2688 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 0, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2496, -3968 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 0, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 2256, -4064 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 0, 4, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3072, -4256 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 90, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( -192, -3296 ), ENTITY_TYPE_ENEMY, ENTITY_SUB_TYPE_ENEMY_SERGEANT, 0, 12, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3616, -3392 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 0, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3712, -3264 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 0, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3472, -2960 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 0, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3376, -3024 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 0, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3568, -2992 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 0, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3616, -3088 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 0, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3664, -3168 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 0, 7, ivec2( 0, 0 ) ),
				DataEntity( ivec2( 3648, -3840 ), ENTITY_TYPE_ITEM, ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS, 0, 7, ivec2( 0, 0 ) )
			)
		);


void MainData( out vec4 fragColor, in ivec2 fragCoord )
{
    if( all(equal(fragCoord, ivec2(0,0) )))
    {
        fragColor = vec4( 88,iResolution.x, iResolution.y, 0 );
        return;
    }

    if ( fragCoord.x < 128 )
    {    
        int sectorIndex = fragCoord.y - 1;
        if ( sectorIndex >= 0 && sectorIndex < map.sectorCount )
        {
            DataSector sector = map.sectors[ sectorIndex ];
            //DATA1(0, 1.000);
            //DATA(1, -80,216,TEX_NUKAGE3,TEX_F_SKY1);
            // Sidedefs
            //DATA(2, 1520,-3168,1672,-3104); 	DATA(3, 164,0.,TEX_X,1); 		DATA3(4, TEX_BROWN144,TEX_X,1);

            if ( fragCoord.x == 0 )
            {
                fragColor = vec4( sector.lightLevel, sector.sideDefCount, 0, 0 );
            }
            else
            if ( fragCoord.x == 1 )
            {
                fragColor = vec4( sector.floorHeight, sector.ceilingHeight, sector.floorTex, sector.ceilingTex );
            }
            else
            {
                uint sideDefIndex = (uint(fragCoord.x) - 2u) / 3u;
                uint sideDefData = (uint(fragCoord.x) - 2u) % 3u;

                if ( sideDefIndex < uint(map.sectors[ sectorIndex ].sideDefCount) )
                {
                    DataSideDef sideDef = UnpackSideDef( map.sideDefs[ uint(sector.firstSideDef) + sideDefIndex ] );
                    uvec4 unpacked = UNPACK_DATA(sideDef.bytePacked);
					uint lowerTex = unpacked.x;
                    uint midTex = unpacked.y;
                    uint upperTex = unpacked.z;
                    uint flags = unpacked.w;
                    
                    if ( sideDefData == 0u )
                    {                    
                        fragColor = vec4( sideDef.A.x, sideDef.A.y, sideDef.B.x, sideDef.B.y );
                    }
                    else
                    if ( sideDefData == 1u )
                    {
                        fragColor = vec4( sideDef.len, sideDef.lightLevel, midTex, sideDef.otherSector );
                    }
                    else
                    {
                        fragColor = vec4( lowerTex, upperTex, flags, 0 );
                    }
                }
                else
                {
                    fragColor = vec4( 0 );
                }
            }
        }
    }
    else
    if( fragCoord.x < 130)
    {
        int entityIndex = fragCoord.y;
        if ( entityIndex < 102 )
        {
            DataEntity entity = map.entities[entityIndex];
            
            if( fragCoord.x == 128)
            {
                fragColor = vec4( entity.type, entity.subType, entity.origin.x, entity.origin.y);
            }
            else
            if( fragCoord.x == 129)
            {
                fragColor = vec4( entity.angle, entity.flags, entity.triggerPos.x, entity.triggerPos.y);
            }
        }
        else
        {
            fragColor = vec4( 0 );
        }
    }
    
    	#define DOOR_START 1    

        #define DOOR_COUNT 6
	    #define DOOR_END ( DOOR_START + DOOR_COUNT )

			//Entities
		
		//BEGIN_BLOCK(vec2(128, 0));
		//BEGIN_ROW(0);	DATA( 0, ENTITY_TYPE_PLAYER, 0, 1056, -3616 );	DATA( 1, 90, 7, 0, 0 );	END_ROW();

        // Doors must be the first entities so we can traverse them below
	    //#define DOOR_START 1    
        
		//BEGIN_ROW(1);
        //DATA( 0, ENTITY_TYPE_DOOR, 0, 1545, -2483 );	DATA( 1, 0, 7, 0, 0 );
		//END_ROW();

		//BEGIN_ROW(2);
        //DATA( 0, ENTITY_TYPE_DOOR, 0, 2926, -3830 );	DATA( 1, 0, 7, 0, 0 );
		//END_ROW();

    
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 vFontCoord = floor( fragCoord - FONT_POS );
    vec2 vFontMax = 10. * FONT_CHAR;
    bool isBigFont = ( vFontCoord.x >= 0.0 && vFontCoord.y >= 0.0 && vFontCoord.x < (vFontMax.x + 16.0) && vFontCoord.y < (vFontMax.y + 16.0) );

    bool isMapData = all( lessThanEqual( fragCoord, vec2( 148, 100 ) ) );
    
    vec2 vMessageUV = vec2(fragCoord.x, (iResolution.y - 1.) - fragCoord.y);
    int iMessageIndex = int(floor(vMessageUV.y / 8.0));
    bool isMessage = (iMessageIndex < MESSAGE_COUNT) && vMessageUV.x < 240.;
    
    vec2 vGridCoord = floor( fragCoord - GRID_POS );

#ifdef WRITE_GRID_DATA
    bool isGridData = ( vGridCoord.x >= 0.0 && vGridCoord.y >= 0.0 && vGridCoord.x < GRID_SIZE.x && vGridCoord.y < GRID_SIZE.y );
#else
    bool isGridData = false;
#endif
    
    if ( !isMapData && !isBigFont && !isMessage && !isGridData) 
    {
    	discard;
    }
            
    fragColor = vec4(0.0,0.0,0.0,0.0);
        
    bool bReuseData = false;
    if ( iFrame > 1 )
    {
	    // Check Resolution
        vec4 info = texelFetch( iChannel0, ivec2(0, 0), 0);
        if ( (info.y == iResolution.x) && (info.z == iResolution.y) )
        {
            bReuseData = true;
        }
    }

	if ( bReuseData )
    {
        fragColor = texelFetch( iChannel0, ivec2(fragCoord), 0 );
        
        //fragColor.r = sin(iTime * 10.0) *0.5 + 0.5; // flash the cache
    }
    else
    {
	    //if( isBigFont )
    	//{
     //       float fFontDigit = floor( vFontCoord.x / 16.0);
     //       vec2 vFontPixel = floor( vFontCoord.xy );
     //       vFontPixel.x = mod( vFontPixel.x, 16.0 );
     //       float fValue = NumFont_BinChar( vFontPixel, fFontDigit );
     //       //float fValue = NumFont_Char( vFontPixel, fFontDigit );            
     //       fragColor =  vec4( fValue );	        
     //       return;
     //   }
        
        if ( isMessage )
        {
            vMessageUV.y = mod(vMessageUV.y, 8.0);
    		PrintHudMessage( vMessageUV, iMessageIndex, fragColor.rgb );	
            fragColor.a = fragColor.r > 0. ? 1.0 : 0.0;
            return;
        }                  

#ifdef WRITE_GRID_DATA
        if ( isGridData )
        {
            vec2 vGridCellWorldMin = GRID_WORLD_MIN + (vGridCoord / GRID_SIZE) * (GRID_WORLD_MAX - GRID_WORLD_MIN);
            vec2 vGridCellWorldMax = GRID_WORLD_MIN + ((vGridCoord + 1.) / GRID_SIZE) * (GRID_WORLD_MAX - GRID_WORLD_MIN);
            
            float fSize = 16.0;
            
            vec4 gridData = GetGridData(vGridCellWorldMin - fSize, vGridCellWorldMax + fSize);
            
            fragColor = gridData;
            return;
        }
#endif // WRITE_GRID_DATA        
        
		MainData( fragColor, ivec2(fragCoord.xy) );
    }
    
    // Flashing sectors
    if ( floor(fragCoord.x) == 0. ) 
    {
        float fTestSectorId = floor(fragCoord.y);
        if ( fTestSectorId == 31. || fTestSectorId == 65.)
        {
        	fragColor.x = (hash(floor(iTime * 10.0)) > 0.2) ? 0.4 : 0.8;
        }
        else
        if ( fTestSectorId == 36. || fTestSectorId == 37. )
        {
            fragColor.x = sin(iTime * 8.0) * 0.5 + 0.5;
            fragColor.x = sqrt(fragColor.x);
        }
    }

    // We need to modify the map data in this pass 
    // but as simply as possible due to constraints in the data shader.
    // Doors are hardcoded to be the first n entities...
    
    // Output to the sector info texel
    if ( floor(fragCoord.x) == 1. )
    {
        int iOutputSectorId = int( floor(fragCoord.y - 1.) );
        
        for (int iDoorIndex = DOOR_START; iDoorIndex < NO_UNROLL( DOOR_END ); iDoorIndex++)
        {
            ivec2 vAddress = ivec2(iDoorIndex, ENTITY_DATA_YPOS);
            vec4 vEntData0 = LoadVec4( iChannel1, vAddress );
            
            //
            vec3 vUnpacked0 = ByteUnpack3( vEntData0.x ) - 1.;
            int iType = int(vUnpacked0.x);
            int iSubType = int(vUnpacked0.y);
            int iSectorId = int(vUnpacked0.z);
            
            // Validate it really is a door (could be blank on frame zero)
            if ( iType == ENTITY_TYPE_DOOR )
            {
                if ( iSectorId == iOutputSectorId )
                {
                    // Update the ceiling height
				    // The entity target data[0].z field stores the height
                    fragColor.y = vEntData0.z;
                }                
            }            
            else if ( iType == ENTITY_TYPE_PLATFORM )
            {
                if ( iSectorId == iOutputSectorId )
                {
                    // Update the floor height
				    // The entity target data[0].z field stores the height
                    fragColor.x = vEntData0.z;
                }                
            }
        }        
                
    }
}
        

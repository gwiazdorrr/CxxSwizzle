// Scene Rendering

//#define DEBUG_IDENTIFY_TEXTURES
//#define DEBUG_IDENTIFY_TEXTURE 21.
//#define DEBUG_IDENTIFY_UNDEFINED_TEXTURES
//#define DEBUG_ENTITY_INFO
//#define DEBUG_VALUES
//#define TEXTURE_PREVIEW 54.0

#define LINUX_WORKAROUND

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


// ---- 8< ---- GLSL Number Printing - @P_Malin ---- 8< ----
// Creative Commons CC0 1.0 Universal (CC-0) 
// https://www.shadertoy.com/view/4sBSWW

float DigitBin(const in int x)
{
    return x==0?480599.0:x==1?139810.0:x==2?476951.0:x==3?476999.0:x==4?350020.0:x==5?464711.0:x==6?464727.0:x==7?476228.0:x==8?481111.0:x==9?481095.0:0.0;
}

float PrintValue(const in vec2 vStringUV, const in float fValue, const in float fMaxDigits, const in float fDecimalPlaces)
{
    if ((vStringUV.y < 0.0) || (vStringUV.y >= 1.0)) return 0.0;
	float fLog10Value = log2(abs(fValue)) / log2(10.0);
	float fBiggestIndex = max(floor(fLog10Value), 0.0);
	float fDigitIndex = fMaxDigits - floor(vStringUV.x);
	float fCharBin = 0.0;
	if(fDigitIndex > (-fDecimalPlaces - 1.01)) {
		if(fDigitIndex > fBiggestIndex) {
			if((fValue < 0.0) && (fDigitIndex < (fBiggestIndex+1.5))) fCharBin = 1792.0;
		} else {		
			if(fDigitIndex == -1.0) {
				if(fDecimalPlaces > 0.0) fCharBin = 2.0;
			} else {
				if(fDigitIndex < 0.0) fDigitIndex += 1.0;
				float fDigitValue = (abs(fValue / (pow(10.0, fDigitIndex))));
                float kFix = 0.0001;
                fCharBin = DigitBin(int(floor(mod(kFix+fDigitValue, 10.0))));
			}		
		}
	}
    return floor(mod((fCharBin / pow(2.0, floor(fract(vStringUV.x) * 4.0) + (floor(vStringUV.y * 5.0) * 4.0))), 2.0));
}

// ---- 8< -------- 8< -------- 8< -------- 8< ----

#define ENABLE_SPRITES

#define DRAW_SKY
#define HEAD_BOB

#define QUANTIZE_FINAL_IMAGE
#define QUANTIZE_TEXTURES
#define PIXELATE_TEXTURES


#define DISCARD_BACKGROUND

const float kDepthFadeScale = (1.0 / 3500.0);
const float kExtraLight = 0.0;

#ifndef CXXSWIZZLE
vec3 SampleTexture( uint iTexture, const in vec2 vUV );
#endif

vec3 GetCameraRayDir( const in vec2 vWindow, const in vec3 vCameraPos, const in vec3 vCameraTarget )
{
	vec3 vForward = normalize(vCameraTarget - vCameraPos);
	vec3 vRight = normalize(cross(vec3(0.0, 1.0, 0.0), vForward));
	vec3 vUp = normalize(cross(vForward, vRight));
	
    const float kFOV = 1.8;
    
	vec3 vDir = normalize(vWindow.x * vRight + vWindow.y * vUp + vForward * kFOV);

	return vDir;
}

///////////////////////////////



#ifdef ENABLE_SPRITES

void Sprite( vec3 vRayOrigin, vec3 vRayDir, vec3 vPos, vec2 vSpriteDir, int iSectorId, vec4 vSpriteInfo, inout TraceResult traceResult )
{
	if ( vSpriteInfo.z <= 0.0 )
        return;
    
    vec2 vSize = vSpriteInfo.zw;
    
	vec2 vA = vPos.xz - vSpriteDir * 0.5 * vSize.x;
	vec2 vB = vPos.xz + vSpriteDir * 0.5 * vSize.x;
    vec2 vD = vB - vA;
    vec2 vOA = vA - vRayOrigin.xz;
    float rcpdenom = 1.0 / Cross2d( vRayDir.xz, vD ); 
    float fHitT = Cross2d( vOA, vD ) * rcpdenom;   
    
    float fDepthBias = 0.0;//-vSize.x * .5;
    
    if ( fHitT > 0.0 && (fHitT + fDepthBias) < traceResult.fDist )
    {
	    float fHitU = Cross2d( vOA, vRayDir.xz ) * rcpdenom;
        float fHitY = vRayDir.y * fHitT + vRayOrigin.y;
        float fSpriteX = fHitU * vSize.x;
        float fSpriteY = fHitY - vPos.y;
        if( (fSpriteX >= 0.0) && (fSpriteX < vSize.x) )
        {
            if( (fSpriteY >= 0.0 ) && (fSpriteY < vSize.y) )
            {             				
                vec2 vSpritePixel = floor( vec2( fSpriteX, fSpriteY ) );
                vec2 vUV = (vSpritePixel + vSpriteInfo.xy + 0.5 ) / iChannelResolution[2].xy;                
                if ( vUV.x > 1.0 ) vUV.x = 2.0 - vUV.x;
                vec4 vSpriteSample = texture( iChannel2, vUV );
                if ( vSpriteSample.a > 0.0 )
                {
					traceResult.fDist = fHitT;                    
                    traceResult.vTexture.rgb = vSpriteSample.rgb;
                    traceResult.iSector = iSectorId;
                    traceResult.fLightLevel = 0.0;
                }                
            }
        }
    }   
}

#endif

void DebugValueSprite( vec3 vRayOrigin, vec3 vRayDir, vec3 vPos, vec2 vSpriteDir, float fValue, vec3 vColor, bool bNoDepthTest, inout TraceResult traceResult )
{    
    vec2 vSize = vec2(4.0, 6.0);
    
	vec2 vA = vPos.xz;
	vec2 vB = vPos.xz + vSpriteDir;
    vec2 vD = vB - vA;
    vec2 vOA = vA - vRayOrigin.xz;
    float rcpdenom = 1.0 / Cross2d( vRayDir.xz, vD ); 
    float fHitT = Cross2d( vOA, vD ) * rcpdenom;    
    
    if ( fHitT > 0.0 && (fHitT < traceResult.fDist || bNoDepthTest ) )
    {
	    float fHitU = Cross2d( vOA, vRayDir.xz ) * rcpdenom;
        float fHitY = vRayDir.y * fHitT + vRayOrigin.y;
        float fSpriteX = fHitU;
        float fSpriteY = fHitY - vPos.y;

        {                        
            vec2 vSpritePixel = vec2( fSpriteX, fSpriteY ) / vSize;

            float fText = PrintValue( vSpritePixel, fValue, 4.0, 2.0 );

            if ( fText > 0.0 )
            {
                traceResult.fDist = fHitT;                    
                traceResult.vTexture.rgb = vColor;
                traceResult.iSector = 0;
                traceResult.fLightLevel = 0.0;
            }                
        }
    }   
}



///////////////////////////
// Sprite Crop Sheet Info
///////////////////////////

#define SPR_RIGHT(V) vec2((V.x + V.z), V.y)
#define SPR_ABOVE(V) vec2(V.x, (V.y + V.w))

const vec4 kSpriteNone				= vec4( 0 );

const vec4 kSpriteBarrel			= vec4(	SPR_RIGHT(kSpriteNone),					23,32);
const vec4 kSpriteBarrelExpl		= vec4(	SPR_RIGHT(kSpriteBarrel),				32,32);

const vec4 kSpriteFireball			= vec4(	SPR_RIGHT(kSpriteBarrelExpl),			20,20);
const vec4 kSpriteFireball2			= vec4(	SPR_RIGHT(kSpriteFireball),				24,24);

const vec4 kSpriteHealthBonus		= vec4(	SPR_RIGHT(kSpriteFireball2),			16,16);
const vec4 kSpriteArmorBonus		= vec4(	SPR_ABOVE(kSpriteHealthBonus),			16,15);
const vec4 kSpriteMedikit			= vec4(	SPR_RIGHT(kSpriteHealthBonus),			28,19);
const vec4 kSpriteStimpack			= vec4(	SPR_RIGHT(kSpriteMedikit),				16,19);
const vec4 kSpriteBlueArmor			= vec4(	SPR_RIGHT(kSpriteStimpack),				32,24);
const vec4 kSpriteGreenArmor		= vec4(	SPR_RIGHT(kSpriteBlueArmor),			32,24);
const vec4 kSpriteShotgun			= vec4(	SPR_RIGHT(kSpriteGreenArmor),			32,16);

const vec4 kSpriteWallImpact1		= vec4(	SPR_RIGHT(kSpriteShotgun),				6,6);
const vec4 kSpriteWallSmokeImpact1	= vec4(	SPR_RIGHT(kSpriteWallImpact1),			8,16);
const vec4 kSpriteFleshImpact1		= vec4(	SPR_RIGHT(kSpriteWallSmokeImpact1),		8,8);

const vec4 kSpriteBloodyMess		= vec4(	SPR_RIGHT(kSpriteFleshImpact1) + vec2(0,16),		42,16);
const vec4 kSpriteDeadTrooper 		= vec4(	SPR_RIGHT(kSpriteBloodyMess),			42,16);
const vec4 kSpriteDeadSergeant 		= vec4(	SPR_RIGHT(kSpriteDeadTrooper),			42,16);
const vec4 kSpriteDeadImp 			= vec4(	SPR_RIGHT(kSpriteDeadSergeant),			42,16);

const vec4 kSpriteBarrelExpl2		= vec4(	480,0,									38,48);

const vec4 kSpriteFloorLamp			= vec4(	480,48,									23,48);
const vec4 kSpriteTallTechnoPillar	= vec4(	SPR_ABOVE(kSpriteFloorLamp),			38,128);

const vec4 kSpriteEnemyBegin = vec4(0, 32., 32, 52);


#define SPRITE_ENEMY_ACTION_STAND 		0.
#define SPRITE_ENEMY_ACTION_ATTACK		1.
#define SPRITE_ENEMY_ACTION_WALK1 		2.
#define SPRITE_ENEMY_ACTION_WALK2 		3.
#define SPRITE_ENEMY_ACTION_PAIN 		4.
#define SPRITE_ENEMY_ACTION_DIE 		5.

#define SPRITE_ENEMY_ACTION_COUNT 		6

#define SPRITE_ENEMY_ACTION_DEAD		10.

#define SPRITE_ENEMY_TYPE_TROOPER 	0.
#define SPRITE_ENEMY_TYPE_IMP 		1.
#define SPRITE_ENEMY_TYPE_SERGEANT 	2.

#define SPRITE_ENEMY_TYPE_COUNT     3

vec4 GetEnemySprite( int iType, float fAction, float fDirection )
{
    vec2 vGridPos = vec2( float(iType) + fDirection * float(SPRITE_ENEMY_TYPE_COUNT), fAction );
    vec2 vPos = kSpriteEnemyBegin.xy +kSpriteEnemyBegin.zw * vGridPos;
	return vec4( vPos, kSpriteEnemyBegin.zw );
}

#ifdef TEXTURE_PREVIEW
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float fTexture = TEXTURE_PREVIEW;
    
    vec2 vTexCoord = floor( fragCoord.xy ) / 2.5;
    fragColor.rgb = SampleTexture( fTexture, vTexCoord );    
    fragColor.a = 1.0;
}
#else
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 vOrigUV = fragCoord.xy / iResolution.xy;
    
    vec2 vResolution = min( iResolution.xy, vec2( 320.0, 200.0 - 32.0 ) );
    
    vec2 vUV = fragCoord.xy / vResolution;
    if ( any( lessThan( vUV, vec2(0) ) ) ||  any( greaterThanEqual( vUV, vec2(1.) )) )
             discard;
    
	vec3 vCameraPos = vec3(0.0);
	vec3 vCameraTarget = vec3(0.0);
        
    vec2 vMouse = (iMouse.xy / iResolution.xy);
	
    
    Entity playerEnt = Entity_Read( STATE_CHANNEL, 0 );
    vCameraPos = playerEnt.vPos;
    
    float fDeathFall = 0.0;
    
    // Death
    if( playerEnt.fHealth <= 0.0 )
    {
        fDeathFall = 1.0 - (playerEnt.fTimer / 1.5);
    }
    
    vCameraPos.y += 6.0 + 34.0 * (1.0 - fDeathFall);

    Sector playerSector = Map_ReadSector( MAP_CHANNEL, playerEnt.iSectorId );

    
    #ifdef HEAD_BOB
	float fBob = sin(vCameraPos.x* 0.04) * 4.0; // head bob
    vCameraPos.y += fBob;
    #endif
        
    // Door crush
    vCameraPos.y = min( vCameraPos.y, playerSector.fCeilingHeight - 1.0 );
    vCameraPos.y = max( vCameraPos.y, playerSector.fFloorHeight + 0.001 );
    
    vCameraTarget = vCameraPos;

    // Mouselook
    float fSinPitch = sin(playerEnt.fPitch);
    float fCosPitch = cos(playerEnt.fPitch);
    vCameraTarget.y += fSinPitch;
    //playerEnt.fYaw += (vMouse.x - 0.5 )* 3.0;

    vCameraTarget.x += sin( playerEnt.fYaw ) * fCosPitch;
    vCameraTarget.z += cos( playerEnt.fYaw ) * fCosPitch;
    
    
    vec2 vWindowCoord =	(vUV * 2.0 - 1.0) * vec2(vResolution.x / vResolution.y, 1.0);
	
    vec3 vRayOrigin = vCameraPos;
    vec3 vRayDir = GetCameraRayDir( vWindowCoord, vCameraPos, vCameraTarget );
    
    float fNoFog = 0.0;
	MapInfo mapInfo = ReadMapInfo( MAP_CHANNEL );
    
	TraceResult traceResult;	
    traceResult = Map_Trace( MAP_CHANNEL, mapInfo, vRayOrigin, vRayDir, playerEnt.iSectorId, FAR_CLIP );

	vec3 vForwards = normalize(vCameraTarget - vCameraPos); 
    
    // sky    
    #ifdef DRAW_SKY    
    if ( traceResult.iTexture == 1u )
    {
        fNoFog = 1.0;
        float fSkyU = (atan(vForwards.x, vForwards.z) * 768.0 / radians(180.0)) + vUV.x * vResolution.x;
        float fSkyV = (vUV.y + fSinPitch) * 210.0 + 41.0;
        fSkyV = min( fSkyV, 255.0 );
        
    	traceResult.vTexture.st = vec2(fSkyU, fSkyV);
	    traceResult.fLightLevel = 100.0f;
    }
    // add extra light for sky, will be clamped to 1.0 later
	#endif    

    // Resovle level textures here
	traceResult.vTexture.rgb = SampleTexture( traceResult.iTexture, traceResult.vTexture.st );    
    

    #ifdef ENABLE_SPRITES
	vec2 vSpriteDir = -normalize(vec2(-vForwards.z, vForwards.x));
	    
    for( int iEnt=0; iEnt<int(ENTITY_MAX_COUNT); iEnt++)
    {
        float fFlipSprite = 1.0;
        vec4 vSpriteInfo = kSpriteNone;
		Entity entity = Entity_Read( STATE_CHANNEL, iEnt );
        if ( entity.iType == ENTITY_TYPE_BARREL )
        {
            if( entity.iSubType == ENTITY_SUB_TYPE_BARREL_EXPLODING && entity.fTimer < 0.05 )
            {
                vSpriteInfo = kSpriteBarrelExpl2;                                      
				entity.iSectorId = 0; // hack fullbright
    		}
            else
            if( entity.iSubType == ENTITY_SUB_TYPE_BARREL_EXPLODING && entity.fTimer < 0.2 )
            {                   
                vSpriteInfo = kSpriteBarrelExpl;
				entity.iSectorId = 0; // hack fullbright
            }
            else
            {
                vSpriteInfo = kSpriteBarrel;                
            }
        }            
        else
        if ( entity.iType == ENTITY_TYPE_ITEM )
        {
            if ( entity.iSubType == ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS )
            {
	            vSpriteInfo = kSpriteHealthBonus;
            }
            else if ( entity.iSubType == ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS )
            {
	            vSpriteInfo = kSpriteArmorBonus;
            }
            else if ( entity.iSubType == ENTITY_SUB_TYPE_ITEM_MEDIKIT )
            {
	            vSpriteInfo = kSpriteMedikit;
            }
            else if ( entity.iSubType == ENTITY_SUB_TYPE_ITEM_STIMPACK )
            {
	            vSpriteInfo = kSpriteStimpack;
            }
            else if ( entity.iSubType == ENTITY_SUB_TYPE_ITEM_GREENARMOR )
            {
	            vSpriteInfo = kSpriteGreenArmor;
            }
            else if ( entity.iSubType == ENTITY_SUB_TYPE_ITEM_BLUEARMOR )
            {
	            vSpriteInfo = kSpriteBlueArmor;
            }
            else if ( entity.iSubType == ENTITY_SUB_TYPE_ITEM_SHOTGUN )
            {
	            vSpriteInfo = kSpriteShotgun;
            }
            
            else
            {
	            vSpriteInfo = kSpriteFireball;                
            }            
        }
        else
        if ( entity.iType == ENTITY_TYPE_DECORATION )
        {
    	    if ( entity.iSubType == ENTITY_SUB_TYPE_DECORATION_BLOODY_MESS )
	        {
	            vSpriteInfo = kSpriteBloodyMess;
            }
            else
    	    if ( entity.iSubType == ENTITY_SUB_TYPE_DECORATION_DEAD_TROOPER )
	        {
	            vSpriteInfo = kSpriteDeadTrooper;
            }
            else
    	    if ( entity.iSubType == ENTITY_SUB_TYPE_DECORATION_DEAD_SERGEANT )
	        {
	            vSpriteInfo = kSpriteDeadSergeant;
            }
            else
    	    if ( entity.iSubType == ENTITY_SUB_TYPE_DECORATION_DEAD_IMP )
	        {
	            vSpriteInfo = kSpriteDeadImp;
            }      
            else
            if ( entity.iSubType == ENTITY_SUB_TYPE_DECORATION_FLOOR_LAMP )
            {
                vSpriteInfo = kSpriteFloorLamp;
                entity.iSectorId = 0; // hack fullbright
            }
            else
            if ( entity.iSubType == ENTITY_SUB_TYPE_DECORATION_TALL_TECHNO_PILLAR )
            {
                vSpriteInfo = kSpriteTallTechnoPillar;
            }
        }            
        else
        if ( entity.iType == ENTITY_TYPE_ENEMY )
        {
            float fAction = SPRITE_ENEMY_ACTION_STAND;
            
            if ( int(entity.fArmor) == ENEMY_STATE_FIRE )
            {
                fAction = SPRITE_ENEMY_ACTION_ATTACK;
            }
            else
            if ( int(entity.fArmor) == ENEMY_STATE_PAIN )
            {
                fAction = SPRITE_ENEMY_ACTION_PAIN;
			}
            else
            if ( int(entity.fArmor) == ENEMY_STATE_DIE )
            {
                if ( iChannelResolution[2].y < 360.0 )
                {	                    
            		fAction = SPRITE_ENEMY_ACTION_PAIN;
                }
                else
                {
            		fAction = SPRITE_ENEMY_ACTION_DIE;
                }
            }
            else
                if ( int(entity.fArmor) == ENEMY_STATE_WALK_RANDOM || int(entity.fArmor) == ENEMY_STATE_WALK_TO_TARGET )
                {
                    if( fract( entity.fTimer ) < 0.5 )
                    {
                        fAction = SPRITE_ENEMY_ACTION_WALK1;
                    }
                    else
                    {
                        fAction = SPRITE_ENEMY_ACTION_WALK2;
                    }
                }
            else
            {
                fAction = SPRITE_ENEMY_ACTION_STAND;
            }
            
            vec3 vCamToEnt = entity.vPos - playerEnt.vPos;
            float fCameraYaw = atan( vCamToEnt.z, vCamToEnt.x );
            float fRelativeYaw = fCameraYaw + entity.fYaw + radians(90.0 + 22.5);
            float fDirection = (fRelativeYaw / radians(45.0)) + 16.0;
            fDirection = mod( floor(fDirection), 8.0 );
            if ( fDirection > 4.0 ) 
            {
                fDirection = 8.0 - fDirection;
                fFlipSprite = -1.0;
            }
            
            vSpriteInfo = GetEnemySprite( entity.iSubType, fAction, fDirection );
        }
        else if ( entity.iType == ENTITY_TYPE_BULLET )
        {
            if ( entity.iSubType == ENTITY_SUB_TYPE_BULLET_WALL )
            {
                if ( entity.fTimer > (0.3 - 0.1) )
                {                    
            		vSpriteInfo = kSpriteWallImpact1;
                    // entity.fSectorId = 0.; hack fullbright
                    
                }
                else
                {
            		vSpriteInfo = kSpriteWallSmokeImpact1;
                }
            }
            else
            if ( entity.iSubType == ENTITY_SUB_TYPE_BULLET_FLESH )
            {
            	vSpriteInfo = kSpriteFleshImpact1;
            }                
        }    
        else if ( entity.iType == ENTITY_TYPE_FIREBALL )
        {
            entity.vPos.y -= 16.0; // Centre fireball
            if ( entity.iSubType == ENTITY_SUB_TYPE_FIREBALL ) 
            {                
				vSpriteInfo = kSpriteFireball;
                entity.iSectorId = 0; // hack fullbright
            }
            else
            {
                vSpriteInfo = kSpriteFireball2;
                entity.iSectorId = 0; // hack fullbright
            }
        }
        
        Sprite( vRayOrigin, vRayDir, entity.vPos, fFlipSprite * vSpriteDir, entity.iSectorId, vSpriteInfo, traceResult );
        
        // Debug
		#ifdef DEBUG_ENTITY_INFO
		{
            bool bNoDepthTest = false;
            if( entity.iType == ENTITY_TYPE_ENEMY )
            {
            	DebugValueSprite( vRayOrigin, vRayDir, entity.vPos + vec3(0,48,0), vSpriteDir, entity.fArmor, vec3(1), bNoDepthTest, traceResult );
            }

            if( entity.iType != ENTITY_TYPE_NONE )
            {
	            // Show health
            	DebugValueSprite( vRayOrigin, vRayDir, entity.vPos + vec3(0,32,0), vSpriteDir, entity.fHealth, vec3(1,0,0), bNoDepthTest, traceResult );
	            // Show timer
            	DebugValueSprite( vRayOrigin, vRayDir, entity.vPos + vec3(0,24,0), vSpriteDir, entity.fTimer, vec3(0,0,1), bNoDepthTest, traceResult );

                // Show sector id
                //DebugValueSprite( vRayOrigin, vRayDir, entity.vPos + vec3(0,16,0), vSpriteDir, entity.fSectorId, vec3(1), bNoDepthTest, traceResult );
                DebugValueSprite( vRayOrigin, vRayDir, entity.vPos + vec3(0,16,0), vSpriteDir, float(entity.iSubType), vec3(1), bNoDepthTest, traceResult );
            }
        }
        #endif
    }        
    #endif	
    
    // Calculate light levels
    Sector sector = Map_ReadSector( MAP_CHANNEL, traceResult.iSector );    
	traceResult.fLightLevel = clamp( sector.fLightLevel + traceResult.fLightLevel + kExtraLight, 0.0, 1.0);

    float fFlash = 0.0;
    {
        if(  playerEnt.fHealth > 0.0 )
        {    
            float fIntensity;
            float fReloadTime;
            float fTimeScale;
            if ( playerEnt.fUseWeapon == 2.0 )
            {
                fReloadTime = 1.0;
                fTimeScale = 15.0;
                fIntensity = 0.2;
                fFlash = 1.0 -  playerEnt.fTimer;
            }
            else
            {
                fReloadTime = 0.5;
                fIntensity = 0.1;
                fTimeScale = 20.0;
            }	  
            fFlash = (fReloadTime -  playerEnt.fTimer) * fTimeScale;
            fFlash = (1.0 - clamp(fFlash, 0.0, 1.0)) * fIntensity;
        }        
    }
    
    float fLightLevel = clamp( traceResult.fLightLevel, 0.0, 1.0 );
    fLightLevel = pow( fLightLevel, 1.5);
    float fDepth = dot(vRayDir, vForwards) * traceResult.fDist;
    float fDepthFade = fDepth * kDepthFadeScale;
    float fApplyFog = 1.0 - fNoFog;
    fDepthFade -= fFlash;
    fLightLevel = clamp( fLightLevel - fDepthFade * fApplyFog, 0.0, 1.0 );   
    //fLightLevel = fLightLevel * fLightLevel;
    
    vec3 vResult = traceResult.vTexture.rgb * fLightLevel;
    
    vResult = clamp(vResult * 1.2, 0.0, 1.0);

    
	/*
		// Debug entity trace
        
        float fEntId = -1.0;
		float fEntTraceDist = Entity_Trace( vRayOrigin, vRayDir, 0.0, fEntId );
        
        if ( fEntTraceDist < FAR_CLIP )
        {
            if ( fEntTraceDist < traceResult.fDist )
            {
	            vResult = vec3( 1, 0, 0 );
            }
            else
            {
	            vResult = vec3( 0, 0, 0.5 );                
			}
        }
    }*/
    
    #ifdef DISCARD_BACKGROUND    
    if(traceResult.iSector == SECTOR_NONE)
    {
        vResult.rgb = vec3(1,0,1);
        //discard;
    }
	#endif    
    
    #ifdef QUANTIZE_FINAL_IMAGE
    vResult = Quantize(vResult, 32.0);
    #endif
    
	fragColor = vec4(vResult, 1.0);

	#ifdef DEBUG_VALUES
    vec2 vFontUV = fragCoord / vec2( 8, 16 );
    vFontUV.y -= 7.0;
    float fFontX = vFontUV.x;
    fragColor.rgb = mix( fragColor.rgb, vec3(0,0,1), PrintValue(vFontUV, playerEnt.vPos.x, 4., 0.) ); 
    vFontUV.x -= 8.;
    fragColor.rgb = mix( fragColor.rgb, vec3(0,0,1), PrintValue(vFontUV, playerEnt.vPos.y, 4., 0.) ); 
    vFontUV.x -= 8.;
    fragColor.rgb = mix( fragColor.rgb, vec3(0,0,1), PrintValue(vFontUV, playerEnt.vPos.z, 4., 0.) ); 
	vFontUV.x = fFontX;
    
    vFontUV.y -= 1.2;
    fragColor.rgb = mix( fragColor.rgb, vec3(1,0,1), PrintValue(vFontUV, float(playerEnt.iSectorId), 4., 0.) );
    #endif
}
#endif

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


/*
float hash(float p)
{
	vec2 p2 = fract(vec2(p * 5.3983, p * 5.4427));
    p2 += dot(p2.yx, p2.xy + vec2(21.5351, 14.3137));
	return fract(p2.x * p2.y * 95.4337);
}

vec3 SampleTexture( const in float fTexture, const in vec2 _vUV )
{
    vec2 vTexureSize = vec2(64);    
    float fXCount = 10.0; // TODO : base on resolution?
    
    vec2 vTexturePos = vec2( mod( floor(fTexture), fXCount ), floor( fTexture / fXCount ) ) * vTexureSize;
    
    vec2 vPixel = vTexturePos + fract(_vUV / vTexureSize) * vTexureSize;
    
    vec2 vSampleUV = vPixel / iChannelResolution[1].xy;
    
    vec4 vSample = texture( iChannel1, vSampleUV );
    
    return vSample.xyz;
}
*/

//////////////////////////////////////////////////////////////
// Texture Helpers
//////////////////////////////////////////////////////////////

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

float Indent(vec2 vTexCoord, vec2 vHigh, vec2 vLow, float fHighIntensity, float fLowIntensity)
{
	vec2 vMin = min(vLow, vHigh);
	vec2 vMax = max(vLow, vHigh);
	if((vTexCoord.x < vMin.x) || (vTexCoord.x > vMax.x) || (vTexCoord.y < vMin.y) || (vTexCoord.y > vMax.y))
		return 1.0;

	if((vTexCoord.x == vHigh.x) || (vTexCoord.y == vHigh.y))
	{
		return fHighIntensity;
	}
	
	if((vTexCoord.x == vLow.x) || (vTexCoord.y == vLow.y))
	{
		return fLowIntensity;
	}
	
	return 1.0;
}

vec4 SmoothBump(const in vec2 vTexCoord, const in vec2 vMin, const in vec2 vMax, const in vec2 vLightDir, const in float fSize)
{
	vec2 vNearest = min( max(vTexCoord, vMin), vMax );
	vec2 vDelta = vNearest - vTexCoord;
    float fDeltaLen = length(vDelta);
	float fDist = (fDeltaLen - fSize) / fSize;
	vec2 vDir = vDelta;
    if(fDeltaLen > 0.0) vDir = vDir / fDeltaLen;
	float fShade = dot(vDir, vLightDir);
	//return clamp(1.0 - (fDist / fSize), 0.0, 1.0) * fShade;
	fShade *= clamp(1.0 - abs((fDist)), 0.0, 1.0);
	return vec4( fShade, fDist, (vTexCoord - vMin + fSize) / (vMax - vMin + fSize * 2.0) );
}


float wrap( const in float x , const in float r )
{
	return fract( x * (1.0 / r) ) * r;
}

vec4 Hexagon( vec2 vUV )
{
	vec2 vIndex;
	
	float fRow = floor(vUV.y);
	
	vec2 vLocalUV = vUV;
	float fRowEven = wrap(fRow, 2.0);
	if(fRowEven < 0.5)
	{
		vLocalUV.x += 0.5;
	}
	
	vIndex = floor(vLocalUV);
	
	vec2 vTileUV = fract(vLocalUV);
	{
		float m = 2.0 / 3.0;
		float c = 2.0 / 3.0;
		if((vTileUV.x *m + c) < vTileUV.y)
		{
			if(fRowEven < 0.5)
			{
				vIndex.x -= 1.0;
			}
			fRowEven = 1.0 - fRowEven;				
			vIndex.y += 1.0;
		}
	}
	
	{
		float m = -2.0 / 3.0;
		float c = 4.0 / 3.0;
		if((vTileUV.x *m + c) < vTileUV.y)
		{
			if(fRowEven >= 0.5)
			{
				vIndex.x += 1.0;
			}
			fRowEven = 1.0 - fRowEven;				
			vIndex.y += 1.0;
		}
	}
	
	vec2 vCenter = vIndex - vec2(0.0, -1.0 / 3.0);
	if(fRowEven > 0.5)
	{
		vCenter.x += 0.5;
	}
	
	vec2 vDelta = vUV - vCenter;
	
	//vDelta = abs(vDelta);
	
	float d1 = vDelta.x;
	float d2 = dot(vDelta, normalize(vec2(2.0/3.0, 1.0)));
	float d3 = dot(vDelta, normalize(vec2(-2.0/3.0, 1.0)));
	
	d2 *= 0.9;
	d3 *= 0.9;
	
	float fDist = max( abs(d1), abs(d2) );
	fDist = max( fDist, abs(d3) );
	
	float fTest = max(max(-d1, -d2), d3);
	
	return vec4(vIndex, abs(fDist), fTest);
}

//////////////////////////////////////////////////////////////


//////////////////////////
// Textures
//////////////////////////

vec3 TexNukage3( vec2 vTexCoord, float fRandom)
{
	float fBlend = 0.0;
	fBlend = smoothstep(0.8, 0.0, fRandom);
	fBlend = min(fBlend, smoothstep(1.0, 0.8, fRandom));
	fBlend *= 1.5;
	vec3 col = mix( vec3(11.0, 23.0, 7.0), vec3(46.0, 83, 39.0), fBlend) / 255.0;
	
	return col;
}

void AddMountain( inout float fShade, const in vec2 vUV, const in float fRandom, const in float fHRandom, const in float fXPos, const in float fWidth, const in float fHeight, const in float fFog)
{
	float fYPos = 1.0 - smoothstep( 0.0, 1.0, abs(fract(fXPos - vUV.x + vUV.y * 0.05 + 0.5) - 0.5) * fWidth );
	fYPos += fHRandom * 0.05 + fRandom * 0.05;
	fYPos *= fHeight;
	float fDist = fYPos - vUV.y;
	if(fDist > 0.0)
	{
		fShade = fRandom * ((1.0 - clamp(sqrt(fDist) * 2.0, 0.0, 1.0)) * 0.3 + 0.1);
		fShade = mix(fShade, 0.6 + 0.1 * fRandom, fFog);
	}	
}

vec3 TexFSky1(vec2 vTexCoord, float fRandom, float fHRandom)
{
	float fShade = 0.6 + 0.1 * fRandom;
	
	vec2 vUV = vTexCoord * (1.0 / vec2(256.0, 128.0));
	vUV.y = 1.0 - vUV.y;
	
	AddMountain( fShade, vUV, fRandom, fHRandom, 0.25, 1.0, 0.85, 0.5 );
	AddMountain( fShade, vUV, fRandom, fHRandom, 1.5, 4.0, 0.78, 0.2 );
	AddMountain( fShade, vUV, fRandom, fHRandom, 1.94, 2.51, 0.8, 0.0 );

	
	return vec3(fShade);
}

vec3 TexFloor7_1( vec2 vTexCoord, float fRandom, vec3 vCol1, vec3 vCol2 )
{
	vec3 col = mix( vCol1, vCol2, fRandom * fRandom * 2.5) / 255.0;
	
	return col;
}

vec3 TexFlat5_5( vec2 vTexCoord, float fRandom )
{
	vec3 col = mix( vec3(63.0, 47.0, 23.0), vec3(147.0, 123.0, 99.0), fRandom) / 255.0;
	
	col *= mod(vTexCoord.x, 2.0) * 0.15 + 0.85;
	
	return col;
}

vec3 TexFloor4_8( vec2 vTexCoord, float fRandom, vec3 vColA, vec3 vColB )
{
	vec3 col = mix( vColA, vColB, fRandom * fRandom) / 255.0;

	vec4 vHex = Hexagon( vTexCoord.yx / 32.0 );
    
    float fShadow = (clamp((0.5 - vHex.z) * 15.0, 0.0, 1.0) * 0.5 + 0.5);
    float fHighlight = 1.0 + clamp(1.0 - (abs(0.45 - vHex.w)) * 32.0, 0.0, 1.0) * 0.5;
    
	col = col * (clamp((0.5 - vHex.z) * 2.0, 0.0, 1.0) * 0.25 + 0.75);
    col = col * fHighlight;
	col = col * fShadow;
	
	return col;
}

vec3 TexCeil3_5( vec2 vTexCoord, float fRandom )
{
	vec3 col = vec3(1.0);
	
	vec2 vTileCoord = vTexCoord;
	vTileCoord.x -= 17.0;
	if( (vTileCoord.x >= 0.0) && (vTileCoord.x < 32.0) ) 
		vTileCoord.y -= 58.0;
	else 
		vTileCoord.y -= 11.0;
	vTileCoord.x = mod(vTileCoord.x, 32.0);
	vTileCoord.y = mod(vTileCoord.y, 64.0);
		
	vec2 vBoxClosest = clamp(vTileCoord, vec2(4.0, 4.0), vec2(28.0, 60.0));
	vec2 vDelta = vTileCoord - vBoxClosest;
	float fDist2 = dot(vDelta, vDelta);

	const float fLight1 = 59.0 / 255.0;
	const float fMed1 = 55.0 / 255.0;
	const float fDark1 = 47.0 / 255.0;
	const float fDark2 = 39.0 / 255.0;

	float fShade = fMed1;	
	fShade = mix( fShade, fLight1, smoothstep(0.6, 0.45, fRandom) );
	fShade = mix( fShade, fDark1, smoothstep(0.45, 0.35, fRandom) );
	
	fShade = mix( fShade, fDark1, step(1.5, fDist2) );
	fShade = mix( fShade, fDark2, step(13.5, fDist2) );
		
	col *= fShade;
	
	if((vTileCoord.x < 12.0) || (vTileCoord.x > 20.0) || (vTileCoord.y < 12.0) || (vTileCoord.y > 52.0))
	{
		float fRRow = floor(mod(vTileCoord.y - 3.5, 7.5));
		float fRColumn = mod(vTileCoord.x - 15.0, 10.0);
		if((fRRow == 2.0) && (fRColumn == 0.0))
		{
			col -= 0.05;
		}
		if((fRRow <= 2.0) && (fRColumn <= 2.0))
		{
			vec2 vOffset = vec2(fRRow - 1.0, fRColumn - 1.0);
			float fDist2 = dot(vOffset, vOffset) / 2.0;
			col += clamp(1.0 - fDist2, 0.0, 1.0) * 0.05;
		}
	}
	
	return col;
}

vec3 TexRandom( vec2 vTexCoord, float fRandom, vec3 vCol0, vec3 vCol1 )
{
	return mix( vCol0, vCol1, fRandom * fRandom);
}

float SdBox( vec2 vPos, vec2 vSize )
{
  vec2 d = abs(vPos) - vSize;
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float SdBox( vec2 vPos, vec2 vMin, vec2 vMax )
{
    vec2 vMid = (vMin + vMax) * .5;
    return SdBox( vPos - vMid, (vMax - vMin) * .5 );
}

float BigDoorYellowStripes( vec2 vTexCoord )
{
    float fDist = 0.0;

    float fIndentSize = 5.0;

    vTexCoord.x = 64. - abs(vTexCoord.x - 64.);
    
    vTexCoord.x = 32. - abs(vTexCoord.x - 32.);
    
    float d;
    d =  min( fIndentSize, -SdBox( vTexCoord, vec2(13,116), vec2(48, 124) ) );
    fDist = max( fDist, d );
    
    float fCorner = clamp( vTexCoord.x - vTexCoord.y + 105., 0.0, 5.0 );
    fDist = min( fDist, fCorner );
    
    return fDist;
}

float BigDoorIndent( vec2 vTexCoord )
{
    float fDist = 0.0;

    float d;

    float fIndentSize = 4.0;    
    
    d = min( fIndentSize, -SdBox( vTexCoord, vec2(82,19), vec2(93,75) ) );
    fDist = max( fDist, d );
        
    d = min( fIndentSize, -SdBox( vTexCoord, vec2(54,20), vec2(68,74) ) );
    fDist = max( fDist, d );
    
    d = min( fIndentSize, -SdBox( vTexCoord, vec2(15, 19), vec2(68, 49) ) );
    float fCorner = clamp( vTexCoord.x + vTexCoord.y - 50.0, 0.0, 5.0 );
    d = min( d, fCorner );    
    fDist = max( fDist, d );
    
    d = min( fIndentSize, -SdBox( vTexCoord, vec2(100, 21), vec2(112, 74) ) );    
    fDist = max( fDist, d );

    d = min( fIndentSize, -SdBox( vTexCoord, vec2(50,123), vec2(79,126) ) );
    fDist = max( fDist, d );
    
    fIndentSize = 2.0;
    
    d = min( fIndentSize, -SdBox( vTexCoord, vec2(41,59), vec2(50,110) ) );
    fDist = max( fDist, d );
    
    fIndentSize = 0.5;

    d = min( fIndentSize, SdBox( vTexCoord, vec2(1, 114), vec2(126, 126) ) );
    fDist = max( fDist, d );
    
    // Rivets
    {
        vec2 vCoord = vTexCoord;

        float m = 10.0;
        float yMax = 128.0;
        fIndentSize = 0.5;
        
        vCoord.x = 64. - abs(vCoord.x - 64.);
        
        if ( vCoord.x > 10.0 )
        {
            vCoord.x -= 8.0;
            m = 6.0;
            
            yMax = 80.;
            fIndentSize = 0.25;
        }

        if ( vCoord.y < yMax )
        {
            vCoord.y = mod(vCoord.y, m);

            d = min( 0.0, -fIndentSize + SdBox( vCoord, vec2(2, 3), vec2(4, 5) ) );
            fDist += d;
        }
    }
    
    d = min( 1.0, BigDoorYellowStripes( vTexCoord ) );
    fDist += d;
 
    return fDist / 5.0;
}

vec3 TexBigDoor2( vec2 vTexCoord, float fRandom, float fHRandom, vec3 vCol0 )
{
    float fShade = 1.0;
    
    fShade = 1.0 + (BigDoorIndent( vTexCoord ) - BigDoorIndent( vTexCoord  + vec2(.5, 1.))) * 2.0;
    
    vec3 vCol = vCol0 * (0.8 + fHRandom * 0.2);
    
	float fMirrorX = 64. - abs(vTexCoord.x - 64.);
    if ( fMirrorX < 6.0 )
    {
        vCol *= 0.75;
    }

    
    float f = BigDoorYellowStripes( vTexCoord );
    if ( f > 0.0 )
    {
        if ( mod(vTexCoord.x + vTexCoord.y, 16.0) > 8.0)
        {
    		vCol = vec3(175,123,31) /255.;
		}
        else
        {
    		vCol = vec3(47)/255.;
        }
    }
    
    vCol *= (fHRandom + vTexCoord.y / 128.0) * 0.2 + 0.8;
    vCol *= fRandom * 0.3 + 0.7;
    
    float fStreak = 1.0 - clamp( fract(fHRandom * 8.0 + vTexCoord.y / 128.0) * 3.0, 0.0, 1.0);
    fStreak = fStreak * (fHRandom * fHRandom * 0.7 + 0.3);
    vCol = mix(vCol, vec3(95,67,35)/255., fStreak );
    
	return vCol * fShade; 
}

vec3 TexDoor3(vec2 vTexCoord, float fRandom, float fHRandom)
{
	float fVNoise = fHRandom + fRandom;
	float fStreak = clamp(abs(fract(fVNoise) - 0.5) * 3.0, 0.0, 1.0);
	fStreak = fStreak * fStreak;
	
	float fShade = 1.0;
	
	fShade = 1.0 - abs((vTexCoord.y / 72.0) - 0.5) * 2.0;
	fShade = fShade * fShade;
	fShade = fShade * 0.2 + 0.3;
	
	fShade = fShade * (fHRandom * 0.2 + 0.8);
	
	fShade *= Indent( vTexCoord, vec2(8.0, 8.0), vec2(64.0 - 8.0, 72.0 - 16.0), 0.8, 1.2);
	fShade *= Indent( vTexCoord, vec2(8.0, 72.0 - 15.0), vec2(64.0 - 8.0, 72.0 - 8.0), 0.8, 1.2);
	
	fShade *= Indent( vTexCoord, vec2(64.0 - 11.0, 46.0), vec2(46.0, 32.0), 0.8, 1.2);
	fShade *= Indent( vTexCoord, vec2(64.0 - 11.0, 56.0), vec2(46.0, 52.0), 0.8, 1.2);
	
	fShade += fRandom * 0.1;

	float fStreakTopAmount = smoothstep( 32.0, 0.0, vTexCoord.y );
	float fStreakBottomAmount = smoothstep( 72.0 -32.0, 72.0, vTexCoord.y );
	
	fShade *= 1.0 - fStreak * max(fStreakTopAmount, fStreakBottomAmount) * 0.2;
	
	if( (vTexCoord.x > 8.0) && (vTexCoord.x < 52.0) )
	{
		vec2 vRepeatCoord = mod( vTexCoord, vec2( 8.0, 48.0 ) );
		vRepeatCoord += vec2(4.0, -12.0);
		if( vRepeatCoord.x == 4.0 )
		{
			if(vRepeatCoord.y == 0.0)
			{
				fShade += 0.1;
			}
			if(vRepeatCoord.y > 0.0)
			{
				fShade *= clamp(vRepeatCoord.y / 16.0, 0.0, 1.0) * 0.3 + 0.7;
			}
		}
	}
	
	return vec3(fShade);
}

vec3 TexLite3( vec2 vTexCoord )
{
	vec2 vLocalCoord = vTexCoord;
	vLocalCoord.y = mod(vLocalCoord.y, 8.0 );
	
	vec2 vClosest = min( max( vLocalCoord, vec2(4.0, 3.5) ), vec2(32.0 - 5.0, 3.5) );
	vec2 vDelta = vLocalCoord - vClosest;
	float fDist = max(abs(vDelta.x), abs(vDelta.y)) / 3.9;
	
	return vec3(1.0 - fDist * 0.65);
}

vec3 TexTLite6_4( vec2 vTexCoord, float fRandom, vec3 vFlatCol, vec3 vLightCol )
{
	vec2 vLocalCoord = vTexCoord;
	vLocalCoord = mod(vLocalCoord, 64.0 );
	
    vec2 vAbsLocal = abs( vLocalCoord - 32. );
    
    float fDist = (vAbsLocal.x + vAbsLocal.y) / 16.0;
    fDist = fDist * fDist;
    
    if ( fDist > 1.0 )
    {
        return vFlatCol * (0.5 + fRandom * 0.5);
    }
    
    float fLight = clamp(1.0 - fDist * fDist, 0.0, 1.0);
	return min( vec3(1.0), vLightCol * (fLight * 0.75 + 0.25) + pow( fLight, 5.0 ) * 0.4);    
}

vec3 TexTLite6_5( vec2 vTexCoord, float fRandom, vec3 vFlatCol, vec3 vLightCol )
{
	vec2 vLocalCoord = vTexCoord;
	vLocalCoord = mod(vLocalCoord, 32.0 );
	
    float fDist = length( vLocalCoord - 16. ) / 8.0;
    
    if ( fDist > 1.0 )
    {
        return vFlatCol * (0.5 + fRandom * 0.5);
    }
    
    float fLight = clamp(1.0 - fDist * fDist, 0.0, 1.0);
	return min( vec3(1.0), vLightCol * (fLight * 0.75 + 0.25) + pow( fLight, 5.0 ) * 0.4);    
}
   

vec3 TexStartan3( vec2 vTexCoord, float fRandom, vec3 col0, vec3 col1 )
{
	vec3 col = col0;
	
	float fVNoise = noise1D(vTexCoord.x * 0.5) - ((vTexCoord.y) / 128.0) + fRandom;
	float fStreak = clamp(abs(fract(fVNoise) - 0.5) * 3.0, 0.0, 1.0);
	fStreak = fStreak * fStreak;
		
	float fBlend2 = smoothstep( 0.0, 32.0, abs(vTexCoord.x - 64.0) );
	fBlend2 *= fBlend2;
	fBlend2 *= fStreak * 0.5 + 0.5;
	col = mix( col, col1, fBlend2 * 0.5);
	
	float fBlend = smoothstep( 24.0, 56.0, abs(vTexCoord.x - 64.0) );
	fBlend *= fBlend;
	fBlend *= fStreak * 0.7 + 0.3;
	col = mix( col, col1 * 1.1, fBlend);
	
	col *= fRandom * fRandom * 0.3 + 0.7;

	vec2 vCoord = vTexCoord;
	vCoord.x = mod(vCoord.x, 32.0);
	
	float fStreakHL = fStreak * 0.075 + 0.075;
	
	float fDistMin = 1.0;
	float fShade = 0.0;
	
	vec4 vBump = SmoothBump( vCoord, vec2(6.0, 8.0), vec2(32.0 - 5.0, 9.0), normalize(vec2(0.0, 1.0)), 3.0 );
	fShade += vBump.x * 0.1;
	fDistMin = min(fDistMin, vBump.y);
	fShade += ((vBump.w >= 0.0) && (vBump.w <= 1.0)) ? (1.0 - vBump.w) * fStreakHL : 0.0;
	vBump = SmoothBump( vCoord, vec2(6.0, 20.0), vec2(32.0 - 6.0, 40.0), normalize(vec2(0.0, 1.0)), 3.0 ); 
	fShade += vBump.x * 0.1;
	fShade += ((vBump.w >= 0.0) && (vBump.w <= 1.0)) ? (1.0 - vBump.w) * fStreakHL : 0.0;
	fDistMin = min(fDistMin, vBump.y);

	vBump = SmoothBump( vCoord, vec2(6.0, 64.0), vec2(32.0 - 6.0, 65.0), normalize(vec2(0.0, 1.0)), 3.0 );
	fShade += vBump.x * 0.1;
	fShade += ((vBump.w >= 0.0) && (vBump.w <= 1.0)) ? (1.0 - vBump.w) * fStreakHL : 0.0;
	fDistMin = min(fDistMin, vBump.y);
	vBump = SmoothBump( vCoord, vec2(6.0, 76.0), vec2(32.0 - 6.0, 110.0), normalize(vec2(0.0, 1.0)), 3.0 ) ;
	fShade += vBump.x * 0.1;
	fShade += ((vBump.w >= 0.0) && (vBump.w <= 1.0)) ? (1.0 - vBump.w) * fStreakHL : 0.0;
	fDistMin = min(fDistMin, vBump.y);

	vBump = SmoothBump( vTexCoord, vec2(-16.0, 50.0), vec2(256.0, 52.0), normalize(vec2(0.0, 1.0)), 3.0 );
	fShade += vBump.x * 0.1;
	fShade += ((vBump.w >= 0.0) && (vBump.w <= 1.0)) ? (1.0 - vBump.w) * fStreakHL : 0.0;
	fDistMin = min(fDistMin, vBump.y);
	vBump = SmoothBump( vTexCoord, vec2(-16.0, 122.0), vec2(256.0, 200.0), normalize(vec2(0.0, 1.0)), 3.0 );
	fShade += vBump.x * 0.05;
	fDistMin = min(fDistMin, vBump.y);

	col *= 1.0 + fShade * 3.0;

	col *= clamp((1.0 - fDistMin) * 1.0, 0.0, 1.0) * 0.3 + 0.7;

	return col;
}

vec3 TexBrown1( vec2 vTexCoord, float fRandom, float fHRandom, vec3 vColA, vec3 vColB, vec3 vDirtCol )
{
	vec3 col = mix( vColA, vColB, fRandom * fRandom) / 255.0;

	if(vTexCoord.x >= 64.0)
	{
		col = col * vec3(1.0, 0.848, 0.646);
		
		col = mix( col, vec3( 0.111, 0.414, 0.3), clamp((fRandom -0.5) * 2.0, 0.0, 1.0)); // green bits
	}
	
	float fVNoise = fHRandom + fRandom;
	
	float fStreak = clamp(abs(fract(fVNoise) - 0.5) * 3.0, 0.0, 1.0);
	fStreak = fStreak * fStreak;

	vec2 vRepeatCoord = vTexCoord;
	vRepeatCoord.x = mod(vRepeatCoord.x, 13.0);
	
	vec4 vBump = SmoothBump( vRepeatCoord, vec2( 5.0, 6.0 ), vec2( 5.0, 12.0), vec2(0.0, 1.0), 1.5);
	float fMask = clamp(1.0 - vBump.y, 0.0, 1.0);
	
	float fStreakAmount = 1.0;
	fStreakAmount *= smoothstep( 0.0, 8.0, vRepeatCoord.y );
	float fStreakWidth = smoothstep( 64.0, 12.0, vRepeatCoord.y );
	float fBase1Dist = smoothstep( 24.0, 75.0, vRepeatCoord.y ) * step(vRepeatCoord.y, 75.0);
	float fBase2Dist = smoothstep( 96.0, 127.0, vRepeatCoord.y );// * step(75.0, vRepeatCoord.y);
	float fBaseDist = max(fBase1Dist, fBase2Dist);
	fStreakWidth = max( fStreakWidth, fBaseDist);
	float fTop2Dist = smoothstep( 127.0, 75.0, vRepeatCoord.y ) * step(75.0, vRepeatCoord.y);
	fStreakWidth = max(fStreakWidth, fTop2Dist);
	float fStreakX = abs(vRepeatCoord.x - 5.0) / 8.0;
	fStreakAmount *= fStreakWidth;
	fStreakAmount *= smoothstep( fStreakWidth, 0.0, fStreakX);
	fStreakAmount = max(fStreakAmount, (fBaseDist - 0.75) * 4.0);
	fStreakAmount *= 1.0 - fMask; 
	col = mix(col, vDirtCol, fStreakAmount * (fStreak * 0.5 + 0.5) );
	
	col += ((vBump.w >= 0.0) && (vBump.w <= 1.0)) ? (vBump.w) * (1.0-vBump.y) * 0.05 : 0.0;
	
	if((vTexCoord.y == 17.0) || (vTexCoord.y == 73.0)) col *= 0.9;
	if((vTexCoord.y == 19.0) || (vTexCoord.y == 75.0)) col *= 1.2;

	col *= 1.0 + clamp(vBump.x, -1.0, 0.0) * 0.6;
		
	return col;
}

vec3 TexDoorstop( vec2 vTexCoord, float fRandom )
{
	float fShade = 1.0 - abs(vTexCoord.x - 3.4) / 4.0;
	
	fShade = fShade * 0.2 + 0.2;
	
	float fSin = sin((vTexCoord.y - 16.0) * 3.14150 * 4.0 / 128.0) * 0.5 + 0.5;
	fShade *= 0.8 + fRandom * 1.2 * fSin;
	
	return vec3(fShade);
}

void DrawScreen(inout vec3 col, const in vec2 vTexCoord, const in vec2 vPos, const in vec2 vSize, const in vec3 vCol)
{
	vec2 vScreenCoord = vTexCoord - vPos;
	col *= Indent( vScreenCoord, vSize, vec2(-1.0), 1.2, 0.5);

	if((vScreenCoord.x >= 0.0) && (vScreenCoord.y >= 0.0) && (vScreenCoord.x < vSize.x) && (vScreenCoord.y < vSize.y))
	{
		col = vCol;
	}
}

bool Screen( inout vec3 col, const in vec2 vTexCoord, const in vec2 vPos, const in vec2 vSize, out vec2 vUV )
{
	vec2 vScreenCoord = vTexCoord - vPos;
	col *= Indent( vScreenCoord, vSize, vec2(-1.0), 1.2, 0.5);

    vUV = vScreenCoord / vSize;
    
	if((vScreenCoord.x >= 0.0) && (vScreenCoord.y >= 0.0) && (vScreenCoord.x < vSize.x) && (vScreenCoord.y < vSize.y))
	{
        return true;
	}
    return false;
}


vec3 TexCompute2( vec2 vTexCoord, float fRandom )
{
    fRandom = 1.0 - fRandom * fRandom;
	vec3 col = vec3(35.0 / 255.0);
	
	col *= Indent( vTexCoord, vec2( -8.0, 0.0), vec2(300.0, 10.0), 1.3, 0.5);
	col *= Indent( vTexCoord, vec2( -8.0, 11.0), vec2(300.0, 27.0), 1.3, 0.5);
	col *= Indent( vTexCoord, vec2( -8.0, 28.0), vec2(300.0, 43.0), 1.3, 0.5);
	{
		vec2 vLocalCoord = vTexCoord;
		vLocalCoord.x = mod(vLocalCoord.x, 21.0);
		col *= Indent( vLocalCoord, vec2( 0.0, 44.0), vec2(20.0, 55.0), 1.3, 0.5);
	}

	if(vTexCoord.y < 40.0)
	{
		vec2 vTileSize = vec2(48.0, 14.0);
		vec2 vIndex = floor(vTexCoord / vTileSize);

		float fIndex = vIndex.x + vIndex.y * 13.0;
		vec2 vMin = vIndex * vTileSize + vec2(Hash(fIndex) * 32.0, 4.0);
		vec2 vSize = vec2(8.0 + Hash(fIndex + 1.0) * 32.0, 4.0);

		vec3 vCol = vec3(0.0);
		float iIndex = floor(mod(fIndex, 5.0));
		if( iIndex < 0.5 ) 
		{
			vCol = mix(vec3(0.0, 0.5, 0.0), vec3(0.0, 0.25, 0.0), fRandom);
		}
		else if(iIndex < 1.5)
		{
			vCol = mix(vec3(1.0, 0.6, 0.02), vec3(0.1), fRandom);
		}
		else if(iIndex < 2.5)
		{
			vCol = vec3(fRandom * 0.5);
		}
		else if(iIndex < 3.5)
		{
			vCol = vec3(fRandom * 0.25);
		}
		else if(iIndex < 4.5)
		{
			vCol = mix(vec3(0.0, 0.0, 0.5), vec3(0.1), fRandom);
		}
		DrawScreen(col, vTexCoord, vMin, vSize, vCol);
	}

	return col;
}

vec3 TexStep6( vec2 vTexCoord, float fRandom, float fHRandom )
{
	vec3 col = mix( vec3(87.0, 67.0, 51.0), vec3(119.0, 95.0, 75.0), fRandom) / 255.0;

	col *= Indent( vTexCoord, vec2(-1.0, 3.0), vec2(32.0, 1.0), 1.3, 0.7);
	col *= Indent( vTexCoord, vec2(-1.0, 8.0), vec2(32.0, 0.0), 1.3, 0.9);

	float fStreak = clamp((vTexCoord.y / 16.0) * 1.5 - fHRandom, 0.0, 1.0);

	col *= fStreak * 0.3 + 0.7;
	
	return col;
}

vec3 TexSupport2( vec2 vTexCoord, float fRandom )
{
	vec3 col;
	float fShade = 0.5;
	
	float f1 = abs(fract((vTexCoord.y + 32.0) / 70.0) - 0.5) * 2.0;
	float f2 = abs(fract((vTexCoord.x + 16.0) / 16.0) - 0.5) * 2.0;
	fShade += f1 * 0.75 + f2 * 0.25;
	fShade = fShade * fShade;

	fShade = fShade * 0.2 + 0.05;
	fShade *= 1.0 + fRandom * 0.4;

	vec2 vLocalCoord = vTexCoord;
	if((vLocalCoord.y < 64.0) || (vLocalCoord.y > 75.0))
	{
		if(vLocalCoord.y > 64.0) vLocalCoord.y -= 8.0;
		vLocalCoord = mod( vLocalCoord, vec2(20.0, 16.0));
		float fIndent = Indent( vLocalCoord, vec2(8.0, 8.0), vec2(16.0, 15.0), 0.9, 1.1);
		fShade += fIndent - 1.0;
	}
	
	col = vec3(fShade);
	
	return col;
}

vec3 TexDoorTrak( vec2 vTexCoord, float fRandom )
{
	float fShade = fRandom * 0.5;
	fShade *= mod(vTexCoord.x, 2.0) * 0.6 + 0.4;
	return vec3(fShade);
}

vec3 TexBrown144( vec2 vTexCoord, float fRandom, float fHRandom )
{
	vec3 col = mix( vec3(39.0, 39.0, 39.0), vec3(51.0, 43.0, 19.0), fRandom) / 255.0;
	
	float fBlend = fHRandom - 0.1;
	fBlend = clamp(fBlend, 0.0, 1.0);
	col = mix( col, col * 2.0 * vec3(0.893, 0.725, 0.161), fBlend);
	return col;
}

vec3 TexNuke24( vec2 vTexCoord, float fRandom, float fHRandom )
{
	vec3 col = mix( vec3(51.0, 43.0, 19.0), vec3(79.0, 59, 35.0), fRandom) / 255.0;
	vec3 colNuke = mix( vec3(11.0, 23.0, 7.0), vec3(46.0, 83, 39.0), fRandom) * 2.5 / 255.0;
	float fBlend = -0.25 -fHRandom + vTexCoord.y / 12.0;// * 3.0;
	fBlend = clamp(fBlend, 0.0, 1.0);
    fBlend= fBlend * fBlend;
	col = mix( col, colNuke, fBlend);    
	return col;
}

vec3 TexPlanet1( vec2 vTexCoord, float fRandom, float fHRandom )
{
    float fShade;
    float fMirrorY = 64.0 - abs(vTexCoord.y - 64.0);
    
    fShade = cos( 3.14 * (fMirrorY - 36.0) / 36.0 ) * 0.5 + 0.5;
    fShade *= fRandom * 0.5 + 0.5;

    float fStreak = clamp(0.0, 1.0, fHRandom + fMirrorY / 32.0);
    fShade *= fStreak;
    
	vec3 col = mix( vec3(59), vec3(147), fShade) / 255.0;

    vec2 vMin = vec2(4,7);
    vec2 vMax = vec2(58,60);
    
    float fIndex = 0.0;
    
    if ( vTexCoord.x > 128.0 )
    {
        vMin.x += 128.;
        vMax.x += 128.;
        
        fIndex += 2.0;
    }
    else
    if ( vTexCoord.x > 64.0 )
    {
        vMin.x += 64.;
        vMax.x += 64.;
        
        fIndex += 1.0;
    }
    
    if ( vTexCoord.y > 64.0 )
    {
        vMin.y += 62.;
        vMax.y += 62.;
        
        fIndex += 3.0;
    }
    
    if ( all( greaterThanEqual( vTexCoord, vMin ) ) &&
         all( lessThanEqual( vTexCoord, vMax ) ))
    {
        col = vec3(0.0);
        
        vec2 vUV = (vTexCoord - (vMin + 1.0)) / (vMax - vMin - 2.);
	    if ( all( greaterThanEqual( vUV, vec2(0.0) ) ) &&
    	     all( lessThan( vUV, vec2(1.0) ) ))
    	{
            col = vec3(0.0);

            float fSize = mod((1.0 + fIndex) * 1.45, 1.0);
            float fOffset = mod((1.0 + fIndex) * 1.84, 1.0);
            
            vec2 vXY = vUV * 2.0 - 1.0;
            vXY += fOffset;
            float l = length(vXY);

            vec3 vPlanetCol = vec3(0.5,0.2,.4) + vec3(8.3,2.24,5.89) * fIndex;
            vPlanetCol = mod( vPlanetCol, vec3(1));
            vPlanetCol *= vec3(1., .2, .01);
            vPlanetCol = normalize(vPlanetCol);
            
            if ( fIndex == 4.0 )
            {
            	col = vPlanetCol * max( 0.0, fRandom - l * 0.5 );
            }
            else
            {
            	// planet                       
                if ( l < fSize )
                {                
                    col = vPlanetCol;

                    vec3 vLight = vec3(0.1,0.5,.4) + vec3(5.435,32.345,32.923) * fIndex;
                    vLight = mod( vLight, 1.0);
                    vLight.xy = vLight.xy * 2.0 - 1.0;
                    vLight = normalize(vLight);


                    vec2 vD = vXY;
                    vec3 vNorm = vec3(vD, sqrt( fSize * fSize - l * l ));
                    float fPlanetShade = max(0.0, dot( vNorm, vLight));
                    col *= fPlanetShade; 
                }
                else
                {
                    float f = min(1.0, fRandom / .75);
                    col = vec3(1.) * pow( f, 100.0) * 0.25;
                    /*
                    if ( fRandom > 0.75 )
                    {
                        col = vec3(0.5);
                    }*/
                }
            }
            
            
            col += 0.1;
            // sheen
            col += max(0.0, 1.0 - (vUV.x + vUV.y) * 2.0) * 0.3;
        }
        
    }
	//if( Screen( col, const in vec2 vTexCoord, const in vec2 vPos, const in vec2 vSize, out vec2 vUV )
    
    
	return col;    
}

float Line( vec2 vCoord, vec2 vA, vec2 vB )
{
    vec2 vDelta = vB - vA;
    vec2 vDir = normalize(vDelta);
    vec2 vOffset = vCoord - vA;
    
    float fProj = dot( vOffset, vDir );
    
     fProj = clamp( fProj, 0.0, length(vDelta) );
    vec2 vClosest = vA + vDir * fProj;
    
    float d = max(0.0, 1.0 - length( vClosest - vCoord ) * 0.25);
    return d * d * d;
    //return 0.0;
}

vec3 TexExitSign( vec2 vTexCoord, float fRandom, float fHRandom )
{
    vec3 vCol = vec3(59) / 255.;
    
    vec2 vBevelMin = vec2(1,1);
    vec2 vBevelMax = vec2(30,14);
   
    if ( vTexCoord.y == (vBevelMin.y-1.) )
    {
        vCol *= 1.5;
    }
    if ( vTexCoord.y == (vBevelMax.y+1.) )
    {
        vCol *= 0.5;
    }
    if ( vTexCoord.x == (vBevelMin.x-1.) )
    {
        vCol *= 1.1;
    }
    if ( vTexCoord.x == (vBevelMax.x+1.) )
    {
        vCol *= 0.75;
    }
    
    if ( all( greaterThanEqual(vTexCoord, vec2(2,2) ) ) &&
        all( lessThanEqual(vTexCoord, vec2(29,13) ) )         
    )
    {
        vCol = vec3(0.0);
        
        vec3 vLetterCol = vec3(227,0,0) / 255.;
        vLetterCol *= 1.0 - (vTexCoord.y / 30.0);
        
        // Verticals
        vec2 vA = vec2(4,4);
        vec2 vB = vec2(4,11);
        
        if ( vTexCoord.x > 23. ) 
        {
            vA.x = vB.x = 25.0;
    	}
        else
        if ( vTexCoord.x > 15. ) 
        {
            vA.x = vB.x = 20.0;
            
        }
                
        vCol = mix( vCol, vLetterCol, Line( vTexCoord, vA, vB ) );
        
        // Horizontals
        vA = vec2(4,4);
        vB = vec2(9,4);
        
        if ( vTexCoord.x > 20. ) 
        {
            vA.x = 22.;
            vB.x = 28.;
        }
        else
        if ( vTexCoord.y > 10. ) 
        {
            vA.y = 11.;
            vB.y = 11.;
        }
        else
        if ( vTexCoord.y > 4. ) 
        {
            vA.y = 7.;
            vB.y = 7.;
            vB.x = 7.;
        }
        
        vCol = mix( vCol, vLetterCol, Line( vTexCoord, vA, vB ) );
                
        vCol = mix( vCol, vLetterCol, Line( vTexCoord, vec2(11,4), vec2(18,11) ) );
        vCol = mix( vCol, vLetterCol, Line( vTexCoord, vec2(11,11), vec2(18,4) ) );
    }
    
    return vCol;
}



float IsTile( vec2 vTexCoord )
{
    float y = 63.5 - abs( vTexCoord.y - 63.5);
    
    float f = y - 5.0;
    float fm = f;
    
    float fRepeat = 29. - 13.;
    
    float x = 64. - abs(vTexCoord.x - 64.);
    
    if ( f > 0.0 && f < fRepeat * 3.5 )
    {
    	fm = mod( fm, fRepeat ); 
    }
    else
    {
        fm = 0.0;
    }
        
    if ( fm > 7.0)
    {
	    if ( x > 23. )
            return 1.0;
    }
    else
    {
	    if ( x > 31. )
            return 1.0;
    }
    return 0.0;
}

vec3 TexCompTile( vec2 vTexCoord, float fRandom )
{   
    // Blue
    vec3 vCol0 = vec3(0.0, 0.0, 35.0 / 255.0);
    vec3 vCol1 = vec3(0.0, 0.0, 200.0 / 255.0);
    vec3 blueCol = mix( vCol0, vCol1, fRandom * fRandom * 2.5);
    
    if ( IsTile(vTexCoord) > 0.0 )
    {
        return blueCol;
    }
    
    vec3 col = vec3(39./255.);
    if ( IsTile( vTexCoord - 1. ) > 0.0 )
    {
        col += 0.1;
    }
    else
    if ( IsTile( vTexCoord + 1. ) > 0.0 )
    {
        col -= 0.1;
    }
    
    if ( vTexCoord.x == 0.0 ) col += 0.1;
    if ( vTexCoord.x == 127.0 ) col -= 0.1;
    if ( vTexCoord.y == 0.0 ) col += 0.1;
    if ( vTexCoord.y == 127.0 ) col -= 0.1;
    if ( vTexCoord.y == 64.0 ) col += 0.1;
    if ( vTexCoord.y == 63.0 ) col -= 0.1;
    
    
    return col;
}

vec3 TexTekWall1( vec2 vTexCoord, float fRandom )
{
	vec3 col = mix( vec3(39.0, 39.0, 39.0), vec3(51.0, 51.0, 51.0), fRandom * fRandom) / 255.0;
	
	return col;
}

void TexAddSwitch( inout vec3 col, vec2 vTexCoord, float fRandom )
{    
    vec2 vMin = vec2(16., 72.);
    vec2 vMax = vec2(47., 103.);

    if ( all( greaterThanEqual( vTexCoord, vMin ) ) && 
        all( lessThanEqual( vTexCoord, vMax ) ))
    {
        col = vec3( 0.5 );
        
        col *= Indent(vTexCoord, vMax, vMin, 0.5, 1.5);
        
        DrawScreen( col, vTexCoord, vec2(22,91), vec2(9,8), vec3(1,0,0));
        DrawScreen( col, vTexCoord, vec2(22,77), vec2(9,8), vec3(0,.3,0));
        
        col *= Indent(vTexCoord, vec2(38,79), vec2(39,96), 0.5, 1.1);

        vec2 vSwMin = vec2(37,81);
        vec2 vSwMax = vSwMin + vec2(3,2);
        if ( all( greaterThanEqual( vTexCoord, vSwMin ) ) && 
        	all( lessThanEqual( vTexCoord, vSwMax ) ))
        {
            col = vec3(0.5, 0.4, 0.3);
            col *= Indent(vTexCoord, vSwMax, vSwMin, 0.5, 1.5);
        }
        
        col *= fRandom * 0.5 + 0.5;
    }    
}

vec3 SampleTexture( uint iTexture, const in vec2 _vUV )
{
    vec3 vCol0 = vec3(0.6);
    vec3 vCol1 = vec3(119.0, 79.0, 43.0) / 255.0;
    vec3 vCol2 = vec3(0);
    
    uint iOrigTexture = iTexture;
    
        
    vec3 col = vec3(1.0, 1.0, 1.0);
#ifdef DEBUG_IDENTIFY_UNDEFINED_TEXTURES    
    col = vec3(1.0, 0.0, 1.0);
#endif  
    
#ifdef DEBUG_IDENTIFY_TEXTURE
    if ( fTexture == DEBUG_IDENTIFY_TEXTURE && (fract(iTime) < 0.5))
    {
        return vec3(1.0, 0.0, 1.0);
    }
#endif     
    
    vec2 vUV = _vUV;
    
    vec2 vSize = vec2(64.0);
    float fPersistence = 0.8;
	float fNoise2Freq = 0.5;
    vec2 vRandomStreak = vec2(0.0);
    
    // Direct Substitutes
    if ( iTexture == TEX_FLOOR1_1 )
    {
        iTexture = TEX_FLAT14;
    }
    else 
    if ( iTexture == TEX_BROWN96 )
    {
        iTexture = TEX_BROWN1;
    }
    else
    if ( iTexture == TEX_COMPTALL)
    {
        iTexture = TEX_STARGR1;
    }
    else
    if ( iTexture == TEX_DOORSTOP )
    {
        iTexture == TEX_DOORTRAK;
    }
    else
    if ( iTexture == TEX_FLAT20 )
    {
        iTexture = TEX_DOORSTOP;
    }
	else
    if (iTexture == TEX_TEKWALL4
       || iTexture == TEX_STEP2)
    {
        // start area pillars
        iTexture = TEX_TEKWALL1;
    }
    else
    if (iTexture == TEX_STEP1)
    {
        // Comp room steps (lights added later)
        iTexture = TEX_STEP6;
    }
    else
    if ( iTexture == TEX_SLADWALL)
    {
        iTexture = TEX_BROWNGRN;
    }
    else
    if ( iTexture == TEX_EXITDOOR )
    {
        iTexture = TEX_DOOR3;
    }
    else
	if ( iTexture == TEX_FLAT23 )
    {
        iTexture = TEX_FLAT18;
    }         
        
        
    
    if ( iTexture == TEX_FLOOR4_8)
    {
        // start area
	    vCol0 = vec3(30.0, 30.0, 30.0);
        vCol1 = vec3(150.0, 150.0, 150.0);
    }
	else        
    if ( iTexture == TEX_FLOOR5_1)
    {
        // Corridor to outside
        iTexture = TEX_FLOOR4_8;
        vCol0 = vec3(51.0, 43.0, 19.0);
        vCol1 = vec3(150.0, 150.0, 150.0);
    }
	else        
    if ( iTexture == TEX_FLOOR5_2 )
    {
        // imp/nukage room
        iTexture = TEX_FLOOR4_8;
        vCol0 = vec3(51.0, 43.0, 19.0);
		vCol1 = vec3(79.0, 59, 35.0);
    }
    
    if ( iTexture == TEX_TLITE6_5)
    {
		vCol0 = vec3(.2);        
		vCol1 = vec3(1,0,0);
    }
    if ( iTexture == TEX_TLITE6_6 )
    {
        iTexture = TEX_TLITE6_5;
        vCol0 = vec3(.25, .2, .1);
        vCol1 = vec3(.8,.6,.4);
    }

    if ( iTexture == TEX_TLITE6_4 )
    {
        vCol0 = vec3(.25, .2, .1);
        vCol1 = vec3(.8,.6,.4);
    }
    else
    if ( iTexture == TEX_TLITE6_1 )
    {
        iTexture = TEX_TLITE6_4;
		vCol0 = vec3(.2);        
		vCol1 = vec3(1);
    }

    if ( iTexture == TEX_BIGDOOR2 )
    {
    	vCol0 = vec3(119) /255.;
    }
    else
    if ( iTexture == TEX_BIGDOOR4 )
    {
        iTexture = TEX_BIGDOOR2;
    	vCol0 = vec3(103,83,63) /255.;
    }
    
    if ( iTexture == TEX_FLOOR7_1 )
    {
	    vCol0 = vec3(51.0, 43.0, 19.0);
        vCol1 = vec3(79.0, 59, 35.0);
	}
    
    if ( iTexture == TEX_CEIL5_2 )
    {
        iTexture = TEX_FLOOR7_1;
	    vCol0 = vec3(51.0, 43.0, 19.0) * .75;
        vCol1 = vec3(79.0, 59, 35.0) * .75;
	}
    
    if ( iTexture == TEX_BROWN1 )
    {
        vCol0 = vec3(119.0, 95.0, 63.0);
        vCol1 = vec3(147.0, 123.0, 99.0);
        vCol2 = vec3(0.3, 0.2, 0.1);
    }
    else
    if ( iTexture == TEX_BROWNGRN )
    {
        iTexture = TEX_BROWN1;
        vCol1 = vec3(43,35,15);
        vCol0 = vec3(47.0, 47.0, 47.0);
        //vCol1 = vec3(147.0, 123.0, 99.0);
        //vCol1 = vec3(123.0, 127.0, 99.0);
        vCol2 = vec3(19,35,11) / 255.;
    }
    
    if ( iTexture == TEX_FLAT14 )
    {
        // Blue noise
 		vCol0 = vec3(0.0, 0.0, 35.0 / 255.0);
        vCol1 = vec3(0.0, 0.0, 200.0 / 255.0);
		fPersistence = 2.0;
    }
    else
    if ( iTexture == TEX_CEIL5_1 )
    {
        // Comp room side ceil
        iTexture = TEX_FLAT14;        
 		vCol0 = vec3(30.0 / 255.0);
        vCol1 = vec3(15.0 / 255.0);
		fPersistence = 2.0;
    }
    else
    if ( iTexture == TEX_FLAT18 )
    {
        // Comp room side floor
        iTexture = TEX_FLAT14;        
 		vCol0 = vec3(70.0 / 255.0);
        vCol1 = vec3(40.0 / 255.0);
		fPersistence = 2.0;
    }   
    else
    if ( iTexture == TEX_COMPSPAN )
    {
        // Comp room wall lower
        iTexture = TEX_FLAT14;        
 		vCol0 = vec3(70.0 / 255.0);
        vCol1 = vec3(30.0 / 255.0);
		fPersistence = 1.0;
    }
    else
    if ( iTexture == TEX_FLOOR6_2 )
    {
        // secret shotgun area ceil
        iTexture = TEX_FLAT14;        
 		vCol0 = vec3(120.0 / 255.0);
        vCol1 = vec3(0.0 / 255.0);
		fPersistence = 1.25;
    }

    if ( iTexture == TEX_FLOOR7_2 )
    {
        // Green armor ceil
        iTexture = TEX_FLAT14;
 		vCol0 = vec3(85,89,60)/255.;
        vCol1 = vec3(0.0, 0.0, 0);
		fPersistence = 0.5;
    }
    
    if ( iTexture == TEX_COMPUTE3 )
    {
        iTexture = TEX_COMPUTE2;
    }       

	if(iTexture == TEX_NUKAGE3)
	{
        float fTest = fract(floor(iTime * 6.0) * (1.0 / 3.0));
        if( fTest < 0.3 )
        {
	        vUV += 0.3 * vSize;
        }
        else if(fTest < 0.6)
        {
            vUV = vUV.yx - 0.3; 
        }
        else
        {
            vUV = vUV + 0.45;
        }
	}
    
    if ( iTexture == TEX_STARTAN1 )
    {
        iTexture = TEX_STARTAN3;
        vCol0 = vec3(131.0, 101.0, 75.0) / 255.0;
        vCol1 = vec3(131.0, 101.0, 75.0) / 255.0;
    }
    else
    if ( iTexture == TEX_STARG3 )
    {
        iTexture = TEX_STARTAN3;
		vCol0 = vec3(0.6);
		vCol1 = vec3(123,127,99) / 255.0;
    }
    else
    if ( iTexture == TEX_STARGR1 )
    {
        iTexture = TEX_STARTAN3;
		vCol0 = vec3(0.6);
		vCol1 = vec3(0.6);  
    }
    else
    if ( iTexture == TEX_SW1STRTN )
    {
        iTexture = TEX_STARTAN3;
        vCol0 = vec3(131.0, 101.0, 75.0) / 255.0;
        vCol1 = vec3(131.0, 101.0, 75.0) / 255.0;
    }   
    
    // Should be a sidedef flag
    if ( iOrigTexture == TEX_TEKWALL1 )
    {
        vUV.x = mod(vUV.x + floor(iTime * 50.0), 64.);
    }
    
	
	if(iTexture == TEX_NUKAGE3) { fPersistence = 1.0; }
	if(iTexture == TEX_F_SKY1) { vSize = vec2(256.0, 128.0); fNoise2Freq = 0.3; }
    if(iTexture == TEX_FLOOR7_1 ||
      iTexture == TEX_CEIL5_2 ) { vSize = vec2(64.0, 32.0); fPersistence = 1.0; }	
    if(iTexture == TEX_FLAT5_5) { fPersistence = 3.0; }
    if(iTexture == TEX_FLOOR4_8) { fPersistence = 0.3; }
    if(iTexture == TEX_CEIL3_5) { fPersistence = 0.9; }	
    if(iTexture == TEX_DOOR3) { vSize = vec2(64.0, 72.0); }	
    if(iTexture == TEX_LITE3) { vSize = vec2(32.0, 128.0); }	
    if(iTexture == TEX_STARTAN3) { vSize = vec2(128.0); fPersistence = 1.0; }	
    if(iTexture == TEX_STARGR1) { vSize = vec2(128.0); fPersistence = 1.0; }	    
    if(iTexture == TEX_BIGDOOR2) { vSize = vec2(128.0, 128.0); fPersistence = 0.5; vRandomStreak = vec2(0,128.); }	    
	if(iTexture == TEX_BROWN1) { vSize = vec2(128.0); fPersistence = 0.7; }	
	if(iTexture == TEX_BROWNGRN) { vSize = vec2(128.0); fPersistence = 0.7; }	    
    if(iTexture == TEX_DOORSTOP) { vSize = vec2(8.0, 128.0); fPersistence = 0.7; }
    if(iTexture == TEX_COMPUTE2) { vSize = vec2(256.0, 56.0); fPersistence = 1.5; }
    if(iTexture == TEX_STEP6) { vSize = vec2(32.0, 16.0); fPersistence = 0.9; }
    if(iTexture == TEX_SUPPORT2) { vSize = vec2(64.0, 128.0); }
    if(iTexture == TEX_DOORTRAK) { vSize = vec2(8.0, 128.0); }
    if(iTexture == TEX_TEKWALL1) {  fPersistence = 1.0;vSize = vec2(64.0, 64.0); }
    if(iTexture == TEX_TLITE6_5) { fPersistence = 1.0; vSize = vec2(64.0, 64.0); }
    if(iTexture == TEX_TLITE6_4) { fPersistence = 1.0; vSize = vec2(64.0, 64.0); }
    if(iTexture == TEX_NUKE24) { vSize = vec2(64.0,24.0); }
    if(iTexture == TEX_COMPTILE) { vSize = vec2(128.); vRandomStreak = vec2(16.0, 0); }
    if(iTexture == TEX_PLANET1) { vSize = vec2(256.0, 128.0); vRandomStreak = vec2(0.0, 255.); }
    if(iTexture == TEX_EXITSIGN) { vSize = vec2(64,16); }
	
#ifdef PREVIEW
	     if(fTexture == TEX_DOOR3) {	vSize = vec2(128.0, 128.0); }	
	else if(fTexture == TEX_COMPUTE2) { vSize = vec2(256.0, 64.0); }
#endif
	
	
#ifdef PREVIEW
    vec2 vTexCoord = floor(fract(vUV) * vSize);
#else
    vec2 vTexCoord = fract(vUV / vSize) * vSize;
    #ifdef PIXELATE_TEXTURES
    vTexCoord = floor(vTexCoord);
    #endif
    vTexCoord.y = vSize.y - vTexCoord.y - 1.0;
#endif
    
	float fHRandom = noise1D(vTexCoord.x * fNoise2Freq);
    float fHOffset =  - ((vTexCoord.y) / vSize.y);

    vec2 vRandomCoord = vTexCoord + float(iTexture);
    vRandomCoord += fHRandom * vRandomStreak;
	float fRandom = fbm( vRandomCoord, fPersistence );

	if(iTexture == TEX_NUKAGE3) 	col = TexNukage3( vTexCoord, fRandom );
	if(iTexture == TEX_F_SKY1) 	col = TexFSky1( vTexCoord, fRandom, fHRandom );
    if(iTexture == TEX_FLOOR7_1) 	col = TexFloor7_1( vTexCoord, fRandom, vCol0, vCol1 );
    if(iTexture == TEX_FLAT5_5) 	col = TexFlat5_5( vTexCoord, fRandom );
    if(iTexture == TEX_FLOOR4_8) 	col = TexFloor4_8( vTexCoord, fRandom, vCol0, vCol1 );
    if(iTexture == TEX_CEIL3_5) 	col = TexCeil3_5( vTexCoord, fRandom );
	if(iTexture == TEX_FLAT14) 	col = TexRandom( vTexCoord, fRandom, vCol0, vCol1 );
	if(iTexture == TEX_DOOR3) 		col = TexDoor3( vTexCoord, fRandom, fHRandom + fHOffset);
	if(iTexture == TEX_LITE3) 		col = TexLite3( vTexCoord );
    if(iTexture == TEX_STARTAN3) 	col = TexStartan3( vTexCoord, fRandom, vCol0, vCol1 );
	if(iTexture == TEX_BIGDOOR2) 	col = TexBigDoor2( vTexCoord, fRandom, fHRandom, vCol0 );
    if(iTexture == TEX_BROWN1) 	col = TexBrown1( vTexCoord, fRandom, fHRandom + fHOffset, vCol0, vCol1, vCol2 );
    if(iTexture == TEX_DOORSTOP) 	col = TexDoorstop( vTexCoord, fRandom );
    if(iTexture == TEX_COMPUTE2) 	col = TexCompute2( vTexCoord, fRandom );
    if(iTexture == TEX_STEP6) 		col = TexStep6( vTexCoord, fRandom, fHRandom + fHOffset );
    if(iTexture == TEX_SUPPORT2) 	col = TexSupport2( vTexCoord, fRandom );
	if(iTexture == TEX_DOORTRAK) 	col = TexDoorTrak( vTexCoord, fRandom );
	if(iTexture == TEX_BROWN144) 	col = TexBrown144( vTexCoord, fRandom, fHRandom  + fHOffset );
    if(iTexture == TEX_TEKWALL1)	col = TexTekWall1( vTexCoord, fRandom );
    if(iTexture == TEX_TLITE6_5)	col = TexTLite6_5( vTexCoord, fRandom, vCol0, vCol1 );
    if(iTexture == TEX_TLITE6_4)	col = TexTLite6_4( vTexCoord, fRandom, vCol0, vCol1 );
    if(iTexture == TEX_NUKE24) 	col = TexNuke24( vTexCoord, fRandom, fHRandom );
	if(iTexture == TEX_PLANET1)	col = TexPlanet1( vTexCoord, fRandom, fHRandom );
#ifndef LINUX_WORKAROUND
	if(iTexture == TEX_EXITSIGN)	col = TexExitSign( vTexCoord, fRandom, fHRandom );
#endif
	
    if (iTexture == TEX_COMPTILE)	col = TexCompTile( vTexCoord, fRandom );
        
    if ( iOrigTexture == TEX_SW1STRTN )
    {
        TexAddSwitch( col, vTexCoord, fRandom );
    }
    else
    if (iOrigTexture == TEX_STEP1)
    {
        /// Add lights
        vec2 d = vTexCoord - vec2(16,8);
        col *= max(0.3, 1.0 - 0.005 * dot(d,d) );
    }    
        
    #ifdef QUANTIZE_TEXTURES
    col = Quantize(col, 32.0);
    #endif
   
#ifdef DEBUG_IDENTIFY_TEXTURES
    vec2 vFontUV = fract(_vUV * vec2(.08) * vec2(1., 2.) ) * 3.0 / vec2(1., 2.);
    if ( PrintValue(vFontUV, fOrigTexture, 2., 0.) > 0.0 )
    {
        col = vec3(1.,0,0);
    }
#endif    

    return col;
}


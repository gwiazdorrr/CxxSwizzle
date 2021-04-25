
#define FAR_CLIP 1000000.0

///////////////////////////
// Util
///////////////////////////

vec3 Quantize( const in vec3 col, float fLevels )
{
	return floor( col * fLevels + 0.5 ) * (1.0 / fLevels);
}

float Cross2d( const in vec2 vA, const in vec2 vB )
{   
    return vA.x * vB.y - vA.y * vB.x;
}

void SetFlag( inout int iBits, int iFlag )
{
    iBits |= iFlag;
}


bool FlagSet( int iBits, int iFlag )
{
    return (iBits & iFlag) != 0;
}


float Hash(float p)
{
	vec2 p2 = fract(vec2(p * 5.3983, p * 5.4427));
    p2 += dot(p2.yx, p2.xy + vec2(21.5351, 14.3137));
	return fract(p2.x * p2.y * 95.4337);
}

///////////////////////////
// Keyboard
///////////////////////////

const int KEY_SPACE = 32;
const int KEY_LEFT  = 37;
const int KEY_UP    = 38;
const int KEY_RIGHT = 39;
const int KEY_DOWN  = 40;
const int KEY_A     = 65;
const int KEY_B     = 66;
const int KEY_C     = 67;
const int KEY_D     = 68;
const int KEY_E     = 69;
const int KEY_F     = 70;
const int KEY_G     = 71;
const int KEY_H     = 72;
const int KEY_I     = 73;
const int KEY_J     = 74;
const int KEY_K     = 75;
const int KEY_L     = 76;
const int KEY_M     = 77;
const int KEY_N     = 78;
const int KEY_O     = 79;
const int KEY_P     = 80;
const int KEY_Q     = 81;
const int KEY_R     = 82;
const int KEY_S     = 83;
const int KEY_T     = 84;
const int KEY_U     = 85;
const int KEY_V     = 86;
const int KEY_W     = 87;
const int KEY_X     = 88;
const int KEY_Y     = 89;
const int KEY_Z     = 90;
const int KEY_COMMA = 188;
const int KEY_PER   = 190;

const int KEY_1 = 	49;
const int KEY_2 = 	50;
const int KEY_3 = 	51;
const int KEY_ENTER = 13;
const int KEY_SHIFT = 16;
const int KEY_CTRL  = 17;
const int KEY_ALT   = 18;
const int KEY_TAB	= 9;

bool Key_IsPressed( sampler2D samp, int key)
{
    return texelFetch( samp, ivec2(key, 0), 0 ).x > 0.0;    
}

bool Key_IsToggled(sampler2D samp, int key)
{
    return texelFetch( samp, ivec2(key, 2), 0 ).x > 0.0;    
}

///////////////////////////
// Packing
///////////////////////////

float BytePack2( vec2 val )
{ 
    return dot( floor(val), vec2(1, 256) );
}

float BytePack2( float x, float y )
{
    return BytePack2( vec2( x, y ) );
}

vec2 ByteUnpack2( float x )
{
    return mod( floor(vec2(x) / vec2(1, 256)), 256. );
}

float BytePack3( vec3 val )
{ 
    return dot( floor(val), vec3(1, 256, 65536) );
}

float BytePack3( float x, float y, float z )
{
    return BytePack3( vec3( x, y, z ) );
}

vec3 ByteUnpack3( float x )
{
    return mod( floor(vec3(x) / vec3(1, 256, 65536)), 256. );
}

///////////////////////////
// Data Storage
///////////////////////////

vec4 LoadVec4( sampler2D sampler, in ivec2 vAddr )
{
    return texelFetch( sampler, vAddr, 0 );
}

vec3 LoadVec3( sampler2D sampler, in ivec2 vAddr )
{
    return LoadVec4( sampler, vAddr ).xyz;
}

bool AtAddress( vec2 p, ivec2 c ) { return all( equal( floor(p), vec2(c) ) ); }

void StoreVec4( in ivec2 vAddr, in vec4 vValue, inout vec4 fragColor, in vec2 fragCoord )
{
    fragColor = AtAddress( fragCoord, vAddr ) ? vValue : fragColor;
}

void StoreVec3( in ivec2 vAddr, in vec3 vValue, inout vec4 fragColor, in vec2 fragCoord )
{
    StoreVec4( vAddr, vec4( vValue, 0.0 ), fragColor, fragCoord);
}


///////////////////////////
// Game State
///////////////////////////
    
vec4 ReadStateData( sampler2D stateSampler, ivec2 address )
{
    return LoadVec4( stateSampler, address );
}

#define MAIN_GAME_STATE_BOOT			0
#define MAIN_GAME_STATE_SKILL_SELECT	1
#define MAIN_GAME_STATE_INIT_LEVEL		2
#define MAIN_GAME_STATE_GAME_RUNNING	3
#define MAIN_GAME_STATE_WIN				4

struct GameState
{
    int iMainState;
    float fStateTimer;
    float fSkill;
    float fGameTime;
    
    vec4 vPrevMouse;
    
    float fMap;
    float fHudFx;
    
    int iMessage;
    float fMessageTimer;
};

void GameState_Reset( out GameState gameState, vec4 vMouse )
{
    gameState.iMainState = MAIN_GAME_STATE_BOOT;
	gameState.fSkill = 0.;
    gameState.fGameTime = 0.;
    gameState.fStateTimer = 0.;
    
    gameState.vPrevMouse = vMouse;
    
    gameState.fMap = 0.0;
    gameState.fHudFx = 0.0;
    
    gameState.iMessage = -1;
    gameState.fMessageTimer = 0.0;
}

GameState GameState_Read( sampler2D stateSampler )
{
    GameState gameState;
    
    ivec2 vAddress = ivec2( 0 );
    
    vec4 vData0 = ReadStateData( stateSampler, vAddress );
    vAddress.x++;

    vec4 vData1 = ReadStateData( stateSampler, vAddress );
    vAddress.x++;

    vec4 vData2 = ReadStateData( stateSampler, vAddress );
    vAddress.x++;

    gameState.iMainState = int(vData0.x);
    gameState.fSkill = vData0.y;
    gameState.fGameTime = vData0.z;
    gameState.fStateTimer = vData0.w;    

    gameState.vPrevMouse = vData1;
    
    gameState.fMap = vData2.x;
    gameState.fHudFx = vData2.y;

    gameState.iMessage = int(vData2.z);
    gameState.fMessageTimer = vData2.w;
    
    return gameState;
}

void GameState_Store( GameState gameState, inout vec4 fragColor, in vec2 fragCoord )
{    
    vec4 vData0 = vec4( gameState.iMainState, gameState.fSkill, gameState.fGameTime, gameState.fStateTimer );    

    vec4 vData1 = vec4( gameState.vPrevMouse );    

    vec4 vData2 = vec4( gameState.fMap, gameState.fHudFx, gameState.iMessage, gameState.fMessageTimer );

    ivec2 vAddress = ivec2( 0 );
    StoreVec4( vAddress, vData0, fragColor, fragCoord );
    vAddress.x++;

    StoreVec4( vAddress, vData1, fragColor, fragCoord );
    vAddress.x++;

    StoreVec4( vAddress, vData2, fragColor, fragCoord );
    vAddress.x++;
}


///////////////////////////
// Map Data
///////////////////////////

vec4 ReadMapData( sampler2D mapSampler, ivec2 address )
{
    return LoadVec4( mapSampler, address );
}
    
#define MAX_SECTOR_COUNT 96
#define MAX_SIDEDEF_COUNT 32

#define SECTOR_NONE -1

struct MapInfo
{
    int iSectorCount;
};
    
MapInfo ReadMapInfo( sampler2D mapSampler )
{
    MapInfo mapInfo;
    
    vec4 vData = ReadMapData( mapSampler, ivec2(0.0, 0.0) );
    
    mapInfo.iSectorCount = int(vData.x);
    
    return mapInfo;
}

struct Sector
{
    int iSectorId;   
    int iSideDefCount;
    float fLightLevel;

    float fFloorHeight;
    float fCeilingHeight;

    uint iFloorTexture;
    uint iCeilingTexture;
};
    
ivec2 GetSectorAddress( int iSectorId )
{
    return ivec2(0, 1 + iSectorId );
}

ivec2 GetSideDefAddress( int iSectorId, int iSideDefIndex )
{
    return ivec2(2 + iSideDefIndex * 3, 1 + iSectorId );
}
    
Sector Map_ReadSector( sampler2D mapSampler, int iSectorId )
{
    Sector sector;
    
    sector.iSectorId = iSectorId;
    
    ivec2 vAddress = GetSectorAddress( iSectorId );
    
    vec4 vData0 = ReadMapData(mapSampler, vAddress);
    vAddress.x += 1;


    vec4 vData1 = ReadMapData(mapSampler, vAddress);
    vAddress.x += 1;

    sector.fLightLevel = vData0.x;
    sector.iSideDefCount = int(vData0.y);

    sector.fFloorHeight = vData1.x;
    sector.fCeilingHeight = vData1.y;
    sector.iFloorTexture = uint(vData1.z);
    sector.iCeilingTexture = uint(vData1.w);
    
    return sector;
}

struct SideDef
{
    vec2 vA;
    vec2 vB;
    float fLength;
    
    int iNextSector;
    
    float fLightLevel;
    
    uint iUpperTexture;
    uint iMiddleTexture;
    uint iLowerTexture;    
    
    float fFlags;
};

SideDef Map_ReadSideDefInfo( sampler2D mapSampler, int iSectorId, int iSideDefIndex )
{
    SideDef sideDef;
    
    ivec2 vAddress = GetSideDefAddress( iSectorId, iSideDefIndex );
    
    vec4 vData0 = ReadMapData(mapSampler, vAddress);
    vAddress.x += 1;

    sideDef.vA = vData0.xy;
    sideDef.vB = vData0.zw;
    
    vec4 vData1 = ReadMapData(mapSampler, vAddress);
    vAddress.x += 1;

    sideDef.fLength = vData1.x;
    sideDef.fLightLevel = vData1.y;    
    sideDef.iMiddleTexture = uint(vData1.z);
    sideDef.iNextSector = int(vData1.w);
    
    vec4 vData2 = ReadMapData(mapSampler, vAddress);
    sideDef.iLowerTexture = uint(vData2.x);
    sideDef.iUpperTexture = uint(vData2.y);
    sideDef.fFlags = vData2.z;
    
    return sideDef;
}

// Return:
//   ray t value
//   sidedef u value
//   edge crossing direction
vec3 Map_SideDefIntersectLine( SideDef sideDef, vec2 vRayPos, vec2 vRayDir )
{
    vec2 vEdgeDir = sideDef.vB - sideDef.vA;
    vec2 vOA = sideDef.vA - vRayPos;
    float fDenom = Cross2d( vRayDir, vEdgeDir );
    float fRcpDenom = 1.0 / fDenom;
    
    vec3 vHitTUD;
    vHitTUD.x = Cross2d( vOA, vEdgeDir ) * fRcpDenom;
    vHitTUD.y = Cross2d( vOA, vRayDir ) * fRcpDenom;
    vHitTUD.z = fDenom;
    
    return vHitTUD;    
}

bool Map_ValidSectorId( MapInfo mapInfo, int iSectorId )
{
    return ( iSectorId >= 0 && iSectorId < mapInfo.iSectorCount );
}

///////////////////////////
// Trace
///////////////////////////

struct TraceResult
{
    float fDist;
    uint iTexture;
    vec3 vTexture; // UV during trace. Resolved to RGB for sprite rendering
  	float fLightLevel;
    int iSector;
};

struct TraceSectorState
{
    float fEnterDist;
    float fExitY;
    int iNextSector;
    bool bNoUpper;
};

void TraceSideDef( sampler2D mapSampler, vec3 vRayOrigin, vec3 vRayDir, inout TraceResult result, inout TraceSectorState sectorState, Sector sector, SideDef sideDef )
{    
	vec3 vHitTUD = Map_SideDefIntersectLine( sideDef, vRayOrigin.xz, vRayDir.xz );
    float fHitT = vHitTUD.x;
    float fHitU = vHitTUD.y;
    float fDenom = vHitTUD.z;

    if( 
        // We hit the edge between the endpoints
        (fHitU >= 0.0) && (fHitU < 1.0) &&
        // Hit point is further than sector entrance distance
        (fHitT > sectorState.fEnterDist) && 
        // Track closest hit
        (fHitT < result.fDist) && 
        // Ignore faces entering sector
        (fDenom < 0.0)
      )
    {
        float fHitY = vRayDir.y * fHitT + vRayOrigin.y;
        sectorState.fExitY = fHitY;
        result.fDist = fHitT;
        
        if( (sectorState.fExitY > sector.fFloorHeight) )                
        {
            sectorState.iNextSector = SECTOR_NONE;
            if( sideDef.iNextSector == SECTOR_NONE && sideDef.iMiddleTexture != 0u && (sectorState.fExitY < sector.fCeilingHeight ) )
            {
                result.vTexture.st = vec2(fHitU * sideDef.fLength, fHitY);
                result.iTexture = sideDef.iMiddleTexture;
                result.fLightLevel = sideDef.fLightLevel;
                sectorState.bNoUpper = false;
            }
            else
            {
                Sector nextSector = Map_ReadSector( mapSampler, sideDef.iNextSector );
                                
		        bool bNoUpper = mod(sideDef.fFlags, 2.0) != 0.0;

	            if (sectorState.fExitY < sector.fCeilingHeight || bNoUpper )
                {                                    
                    if( nextSector.fFloorHeight > sector.fFloorHeight && fHitY > sector.fFloorHeight && fHitY < nextSector.fFloorHeight)
                    {
                        result.vTexture.st = vec2(fHitU * sideDef.fLength, fHitY - nextSector.fFloorHeight);
                        result.iTexture = sideDef.iLowerTexture;
                        result.fLightLevel = sideDef.fLightLevel;
                        sectorState.bNoUpper = false;
                    }            
                    else
                    if( sideDef.iUpperTexture != 0u && nextSector.fCeilingHeight < sector.fCeilingHeight && fHitY < sector.fCeilingHeight && fHitY > nextSector.fCeilingHeight)
                    {
                        result.vTexture.st = vec2(fHitU * sideDef.fLength, fHitY - nextSector.fCeilingHeight);
                        result.iTexture = sideDef.iUpperTexture;
                        result.fLightLevel = sideDef.fLightLevel;
                        sectorState.bNoUpper = false;
                    }
                    else
                    {
                        sectorState.bNoUpper = bNoUpper;
                        sectorState.iNextSector = sideDef.iNextSector;                          
                    }
                }
            }
        }
    }
}


///////////////////////////
// Entity Management
///////////////////////////

#define ENTITY_TYPE_NONE -1

struct Entity
{
    int iId;
    
    int iType;
    int iSubType;
    int iSectorId;    

    vec3 vPos;
    float fYaw;
    float fPitch;
    
    vec3 vVel;
    float fYawVel;
    
    float fHealth;
    float fArmor;    
    
    float fUseWeapon;    
    float fHaveShotgun;
    
    float fTookDamage; // For HUD damage indicator
    int iEvent; // Message event for UI "Picked up an armor bonus" etc.
    
    float fTimer; // Weapon reload for player, think timer for AI
    float fTarget; // AI nemesis and also used for player aim up / down 
    int iFrameFlags;
};
    
#define ENTITY_DATA_YPOS 8
#define ENTITY_DATA_SIZE 4

#define ENTITY_MAX_COUNT 128
    
#define ENTITY_NONE -1
    
int Entity_GetOutputId( vec2 fragCoord )
{
    if (
           int(fragCoord.y) < ENTITY_DATA_YPOS
        || int(fragCoord.y) >= (ENTITY_DATA_YPOS + ENTITY_DATA_SIZE)
        || int(fragCoord.x) >= ENTITY_MAX_COUNT
       )
    {
        return ENTITY_NONE;        
    }

    return int(floor(fragCoord.x));
}

bool Entity_IdValid( int iEntityId )
{
    return ( iEntityId >= 0 && iEntityId < ENTITY_MAX_COUNT );
}

void Entity_Clear( inout Entity entity )
{
    entity.iType = ENTITY_TYPE_NONE;
    entity.vPos = vec3(0);
    entity.iSectorId = SECTOR_NONE;
    entity.fYaw = 0.0;
    entity.vVel = vec3(0);
    entity.fYawVel = 0.0;
    entity.fPitch = 0.0;
    entity.fHealth = 0.0;
    entity.fArmor = 0.0;
    entity.fTimer = 0.0;
    entity.fTarget = float(ENTITY_NONE);
    entity.iFrameFlags = 0;    
    entity.fUseWeapon = 1.;
    entity.fHaveShotgun = 0.;
    entity.fTookDamage = 0.;
    entity.iEvent = 0;
}

Entity Entity_Read( sampler2D stateSampler, int iEntityId )
{
    Entity entity;
    
    entity.iId = iEntityId;
    
    ivec2 vAddress = ivec2( iEntityId, ENTITY_DATA_YPOS );
    
    vec4 vData0 = ReadStateData( stateSampler, vAddress );
    vec3 vUnpacked0x = ByteUnpack3( vData0.x ) - 1.;
    vAddress.y++;
    
    
    entity.iType = int(vUnpacked0x.x);
    entity.iSubType = int(vUnpacked0x.y);
    entity.iSectorId = int(vUnpacked0x.z);
    
    entity.iFrameFlags = int(vData0.y);
    
    entity.fTarget = vData0.z;
    entity.fTimer = vData0.w;
    
    vec4 vData1 = ReadStateData( stateSampler, vAddress );
    vAddress.y++;

    entity.vPos = vData1.xyz;
    entity.fYaw = vData1.w;

    vec4 vData2 = ReadStateData( stateSampler, vAddress );
    vAddress.y++;

    entity.vVel = vData2.xyz;
    entity.fYawVel = vData2.w;

    vec4 vData3 = ReadStateData( stateSampler, vAddress );
    vec2 vUnpacked3x = ByteUnpack2(vData3.x);
    vec2 vUnpacked3y = ByteUnpack2(vData3.y);
    vec2 vUnpacked3z = ByteUnpack2(vData3.z) -1.;
    vAddress.y++;
    
    entity.fHealth = vUnpacked3x.x;
    entity.fArmor = vUnpacked3x.y;
    float fPackedWeaponInfo = vData3.y;
    entity.fUseWeapon = vUnpacked3y.x;
    entity.fHaveShotgun = vUnpacked3y.y;
    entity.fTookDamage = vUnpacked3z.x;
    entity.iEvent = int(vUnpacked3z.y);
    entity.fPitch = vData3.w;
    
    return entity;
}

void Entity_Store( Entity entity, inout vec4 fragColor, in vec2 fragCoord )
{
    if ( !Entity_IdValid( entity.iId ) ) 
        return;

    float fPacked0 = BytePack3( vec3(entity.iType, entity.iSubType, entity.iSectorId ) + 1. );

    entity.fHealth = floor( clamp( entity.fHealth, 0.0, 255.0 ) );
    entity.fArmor = floor( clamp( entity.fArmor, 0.0, 255.0 ) );
    float fPackedHealthAndArmor = BytePack2( vec2( entity.fHealth, entity.fArmor ) ); 
	float fPackedWeaponInfo = BytePack2( vec2( entity.fUseWeapon, entity.fHaveShotgun ) );
	float fPackedEvents = BytePack2( vec2( entity.fTookDamage, entity.iEvent ) + 1. );
    
    ivec2 vAddress = ivec2( entity.iId, ENTITY_DATA_YPOS );
    
    vec4 vData0 = vec4( fPacked0, entity.iFrameFlags, entity.fTarget, entity.fTimer );    
    vec4 vData1 = vec4( entity.vPos, entity.fYaw );
    vec4 vData2 = vec4( entity.vVel, entity.fYawVel );
    vec4 vData3 = vec4( fPackedHealthAndArmor, fPackedWeaponInfo, fPackedEvents, entity.fPitch );

    StoreVec4( vAddress, vData0, fragColor, fragCoord );
    vAddress.y++;

    StoreVec4( vAddress, vData1, fragColor, fragCoord );
    vAddress.y++;

    StoreVec4( vAddress, vData2, fragColor, fragCoord );
    vAddress.y++;

    StoreVec4( vAddress, vData3, fragColor, fragCoord );
    vAddress.y++;
}

///////////////////////////
// Weapon Defs
///////////////////////////

#define WEAPON_TYPE_PISTOL		0.
#define WEAPON_TYPE_SHOTGUN		1.
#define WEAPON_TYPE_FIREBALL	2.

struct WeaponDef
{
    float fWeaponType;
    float fReloadTime;
    int iProjectileCount;
    float fBaseDamagePerProjectile;
    float fRandomDamagePerProjectile;
    float fSpread;
};

WeaponDef Weapon_GetDef( float fWeaponType, bool bEnemy )
{
    WeaponDef weaponDef;

    if ( fWeaponType == WEAPON_TYPE_SHOTGUN )
    {
        weaponDef.fWeaponType = WEAPON_TYPE_SHOTGUN;
        weaponDef.fReloadTime = 1.0;
        weaponDef.iProjectileCount = bEnemy ? 3 : 7;
        weaponDef.fBaseDamagePerProjectile = bEnemy ? 3.0 : 5.0;
        weaponDef.fRandomDamagePerProjectile = bEnemy ? 12.0 : 10.0;
        weaponDef.fSpread = radians(4.0);
    }
    else
    if ( fWeaponType == WEAPON_TYPE_FIREBALL )
    {
        weaponDef.fWeaponType = WEAPON_TYPE_FIREBALL;
        weaponDef.fReloadTime = 1.0;
        weaponDef.iProjectileCount = 1;
        weaponDef.fBaseDamagePerProjectile = 5.0;
        weaponDef.fRandomDamagePerProjectile = 0.0;
        weaponDef.fSpread = radians(0.0);
    }
    else
    {
        // Pistol
        weaponDef.fWeaponType = WEAPON_TYPE_PISTOL;
        weaponDef.fReloadTime = 0.5;
        weaponDef.iProjectileCount = 1;
        weaponDef.fBaseDamagePerProjectile = bEnemy ? 3.0 : 5.0;
        weaponDef.fRandomDamagePerProjectile = bEnemy ? 12.0 : 10.0;
        weaponDef.fSpread = bEnemy ? radians(4.0) : 0.0;
    }
    
    return weaponDef;
}
    
#define ENTITY_TYPE_PLAYER 	0

#define ENTITY_TYPE_BARREL 	1
#define ENTITY_SUB_TYPE_BARREL_INACTIVE			0
#define ENTITY_SUB_TYPE_BARREL_EXPLODING		1
#define ENTITY_SUB_TYPE_BARREL_APPLY_DAMAGE		2

#define ENTITY_TYPE_ITEM 	2
#define ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS 	0
#define ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS 	1
#define ENTITY_SUB_TYPE_ITEM_SHOTGUN 		2
#define ENTITY_SUB_TYPE_ITEM_STIMPACK		3
#define ENTITY_SUB_TYPE_ITEM_GREENARMOR		4
#define ENTITY_SUB_TYPE_ITEM_BLUEARMOR		5
#define ENTITY_SUB_TYPE_ITEM_MEDIKIT		6

#define ENTITY_TYPE_DECORATION 3
#define ENTITY_SUB_TYPE_DECORATION_BLOODY_MESS			0
#define ENTITY_SUB_TYPE_DECORATION_DEAD_TROOPER			1
#define ENTITY_SUB_TYPE_DECORATION_DEAD_SERGEANT		2
#define ENTITY_SUB_TYPE_DECORATION_DEAD_IMP				3
#define ENTITY_SUB_TYPE_DECORATION_TALL_TECHNO_PILLAR	4
#define ENTITY_SUB_TYPE_DECORATION_FLOOR_LAMP			5

#define ENTITY_TYPE_ENEMY	4
#define ENTITY_SUB_TYPE_ENEMY_TROOPER 		0
#define ENTITY_SUB_TYPE_ENEMY_IMP 			1
#define ENTITY_SUB_TYPE_ENEMY_SERGEANT 		2

#define ENTITY_TYPE_BULLET 5
#define	ENTITY_SUB_TYPE_BULLET_WALL			0
#define	ENTITY_SUB_TYPE_BULLET_FLESH		1

#define ENTITY_TYPE_DOOR 6

#define ENTITY_TYPE_PLATFORM 7

#define ENTITY_TYPE_FIREBALL 8
#define ENTITY_SUB_TYPE_FIREBALL 0
#define ENTITY_SUB_TYPE_FIREBALL_IMPACT 1


#define ENTITY_FRAME_FLAG_FIRE_WEAPON		1
#define ENTITY_FRAME_FLAG_DROP_ITEM			2


#define ENEMY_STATE_IDLE				0
#define ENEMY_STATE_PAIN				1
#define ENEMY_STATE_WALK_RANDOM			2
#define ENEMY_STATE_WALK_TO_TARGET		3
#define ENEMY_STATE_STAND				4
#define ENEMY_STATE_FIRE				5
#define ENEMY_STATE_DIE					6


#define EVENT_HEALTH_BONUS 	1
#define EVENT_ARMOR_BONUS 	2
#define EVENT_SHOTGUN 		3
#define EVENT_STIMPACK		4
#define EVENT_GREENARMOR	5
#define EVENT_BLUEARMOR		6
#define EVENT_MEDIKIT		7
#define EVENT_DIED			8

#define MESSAGE_NONE			-1
#define MESSAGE_HEALTH_BONUS 	0
#define MESSAGE_ARMOR_BONUS 	1
#define MESSAGE_SHOTGUN 		2
#define MESSAGE_STIMPACK		3
#define MESSAGE_GREENARMOR		4
#define MESSAGE_BLUEARMOR		5
#define MESSAGE_MEDIKIT			6
#define MESSAGE_HUD_TEXT		7
#define MESSAGE_CHOOSE_SKILL	8
#define MESSAGE_SKILL_1			9
#define MESSAGE_SKILL_2			10
#define MESSAGE_SKILL_3			11
#define MESSAGE_SKILL_4			12
#define MESSAGE_SKILL_5			13
#define MESSAGE_HANGAR			14
#define MESSAGE_FINISHED		15
#define MESSAGE_KILLS			16
#define MESSAGE_ITEMS			17
#define MESSAGE_SECRET			18
#define MESSAGE_TIME			19
#define MESSAGE_SELECT			20
#define MESSAGE_COUNT			21


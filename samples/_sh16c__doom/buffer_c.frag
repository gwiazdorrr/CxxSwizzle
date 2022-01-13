// Update / Game Logic

//#define DEV

#ifdef DEV

#define CHEAT_GOD
#define CHEAT_NOCLIP
#define CHEAT_NOAI

#endif 


// TODO:
//
// * Bug fixes
// ** Only pickup when needed
// ** Fix slidebox bugs *!*

// * Performance
// ** Profile
// ** Entity rendering precalc
// ** Sector update logic in BufC (makes bufA static)

// *Implement more textures
// ** Exit door
// ** Exit area door
// ** Walls with alpha
//
// * Add more decorations 
// ** Dead players
// ** Candelabra

// * Sprites
// ** improve fireball + explode

// * Presentation / UI
// ** Skill select?
// ** Level summary

// * Enemy sound / path finding

// * Viewmodel
// ** Viewmodel hand
// ** Better pistol viewmodel
// ** Better viewmodel anim

// * Map color (brown vs yellow)

// * Gameplay
// ** More authentic movement
// ** Mouse accel
// ** Balance damage / health amounts

// * Ammo / ammo pickups
// ** Clip
// ** Shells
// ** Box of shells

// * Unpinned top / bottom textures

// * Generic door / platform logic

// * Improve enemy poses


#define ALLOW_MAP

#define MAP_CHANNEL iChannel0
#define STATE_CHANNEL iChannel1
#define iChannelKeyboard iChannel3

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


MapInfo g_mapInfo;
GameState g_gameState;

struct Controls
{
    bool menuConfirm;
    bool menuUp;
    bool menuDown;
    
    bool moveForwards;
    bool moveBackwards;
    bool moveLeft;
    bool moveRight;
    bool fire;

    bool turnLeft;
    bool turnRight;

    bool run;
    bool strafe;
    bool open;    
    
    bool weap_2;
    bool weap_3;
    
    bool toggleMouseLook;
    bool toggleInvertY;
};
    
Controls g_controls;

Controls ReadControls()
{
    Controls controls;
    
    bool bArrowUp = Key_IsPressed( iChannelKeyboard, KEY_UP );
    bool bArrowDown = Key_IsPressed( iChannelKeyboard, KEY_DOWN );
    bool bSpace = Key_IsPressed( iChannelKeyboard, KEY_SPACE );        
    bool bEnter = Key_IsPressed( iChannelKeyboard, KEY_ENTER );        
    
    controls.moveForwards = 		Key_IsPressed( iChannelKeyboard, KEY_W ) || bArrowUp;
    controls.moveBackwards = 		Key_IsPressed( iChannelKeyboard, KEY_S ) || bArrowDown;
    controls.moveLeft = 			Key_IsPressed( iChannelKeyboard, KEY_A );
    controls.moveRight = 			Key_IsPressed( iChannelKeyboard, KEY_D );
    controls.fire = 				bSpace;

    controls.turnLeft = 			Key_IsPressed( iChannelKeyboard, KEY_LEFT );
    controls.turnRight = 			Key_IsPressed( iChannelKeyboard, KEY_RIGHT );

    controls.run = 					Key_IsPressed( iChannelKeyboard, KEY_SHIFT );
    controls.strafe = 				Key_IsPressed( iChannelKeyboard, KEY_ALT );        
    controls.open = 				bEnter || Key_IsPressed( iChannelKeyboard, KEY_F );
    
    controls.weap_2 = 				Key_IsPressed( iChannelKeyboard, KEY_2 );
    controls.weap_3 = 				Key_IsPressed( iChannelKeyboard, KEY_3 );        
    
    controls.toggleMouseLook = 		Key_IsToggled( iChannelKeyboard, KEY_L );
    controls.toggleInvertY = 		Key_IsToggled( iChannelKeyboard, KEY_I );
    
    controls.menuConfirm = 			bSpace || bEnter;
    controls.menuUp = 				bArrowUp;
    controls.menuDown = 			bArrowDown;
    
    return controls;
}
    

void Entity_MapSpawn( inout Entity entity, int iSpawnIndex )
{   
    Entity_Clear( entity );
    
    vec4 vData0 = ReadMapData( MAP_CHANNEL, ivec2(128, iSpawnIndex) );
    vec4 vData1 = ReadMapData( MAP_CHANNEL, ivec2(129, iSpawnIndex) );
    
    if ( dot(vData0.zw, vData0.zw) > 0.0 )
    {
        entity.iType = int(vData0.x);
        entity.iSubType = int(vData0.y);
        entity.vPos.xz = vData0.zw;
        entity.vPos.y = 0.0;
        entity.fYaw = radians( 90. - vData1.x );

        entity.iSectorId = Map_SeekSector( MAP_CHANNEL, g_mapInfo, entity.vPos.xz );    

        Sector sector = Map_ReadSector( MAP_CHANNEL, entity.iSectorId );

        entity.vPos.y = sector.fFloorHeight;
        
        if ( entity.iType == ENTITY_TYPE_PLAYER )
        {
            entity.fHealth = 100.0;
        }
        if ( entity.iType == ENTITY_TYPE_ENEMY )
        {
            if ( entity.iSubType == ENTITY_SUB_TYPE_ENEMY_TROOPER )
            {
	            entity.fHealth = 20.0;
            }
            else
            if ( entity.iSubType == ENTITY_SUB_TYPE_ENEMY_SERGEANT )
            {
	            entity.fHealth = 30.0;
            }
            else
            if ( entity.iSubType == ENTITY_SUB_TYPE_ENEMY_IMP )
            {
	            entity.fHealth = 60.0;
            }
        }
        if ( entity.iType == ENTITY_TYPE_BARREL )
        {
            entity.fHealth = 20.0;
        }        
        if ( entity.iType == ENTITY_TYPE_DOOR )
        {        
	        entity.fTarget = sector.fFloorHeight;
        }
        if ( entity.iType == ENTITY_TYPE_PLATFORM )
        {        
            entity.vPos.y = sector.fFloorHeight;
            entity.vPos.xz = vData1.zw; // trigger location
	        entity.fTarget = sector.fFloorHeight;
        }
    }
    else
    {
        entity.iType = ENTITY_TYPE_NONE;
    }
}

void Entity_Fall( inout Entity entity, float fTimestep )
{
    if ( Map_ValidSectorId( g_mapInfo, entity.iSectorId ) )
    {
        Sector sector = Map_ReadSector( MAP_CHANNEL, entity.iSectorId );    	
        float a = -800.;
        entity.vPos.y += entity.vVel.y * fTimestep + .5 * a * fTimestep * fTimestep;
        entity.vVel.y += a * fTimestep;
        if ( entity.vPos.y < sector.fFloorHeight )
        {
            entity.vPos.y = sector.fFloorHeight;
            entity.vVel.y = 0.0;
        }
    }
    else
    {
        entity.vVel.y = 0.0;
    }
}


void SlideVector( inout int iSectorId, inout vec2 vPos, inout vec2 vVel, float fYPos, float fYPos2, float fDropOff )
{       
	bool bReevaluate = false;
    
    int iInitialSector = iSectorId;
    
    // maximum number of sector traversal / wall collisions
    for (int iIter=0; iIter<8; iIter++ )
    {
	    bReevaluate = false;

        float fVelLength = length( vVel );
        if ( length( vVel ) <= 0.0 )
        {
            break;
        }

        vec2 vVelDir = normalize( vVel );

        Sector sector = Map_ReadSector( MAP_CHANNEL, iSectorId );

        for(int iSideDefIndex=0; iSideDefIndex<sector.iSideDefCount; iSideDefIndex++)
        {
            SideDef sideDef = Map_ReadSideDefInfo( MAP_CHANNEL, iSectorId, iSideDefIndex );                        

            vec3 vHitTUD = Map_SideDefIntersectLine( sideDef, vPos, vVelDir );

            float fHitT = vHitTUD.x;
            float fHitU = vHitTUD.y;
            float fDenom = vHitTUD.z;        

			// Only consider cases when we are leaving sector
            if ( fDenom < 0.0 )
            {        
                if ( fHitT > 0.0 && fHitT < fVelLength )
                {
                    if ( fHitU > 0.0 && fHitU < 1.0 )
                    {
                        // We hit a sidedef

                        bool bSolid = true;
                        bReevaluate = true;
                        
                        if ( sideDef.iNextSector != SECTOR_NONE )
                        {
#ifdef CHEAT_NOCLIP                            
                            {   {   {
#else // #ifdef CHEAT_NOCLIP                                
                            // blocking flag
                            if ( mod( floor(sideDef.fFlags / 2.), 2. ) == 0. )
                            {
                                // Check heights of next sector
                                Sector nextSector = Map_ReadSector( MAP_CHANNEL, sideDef.iNextSector );                            
                                if( ( fYPos > nextSector.fFloorHeight && fYPos < nextSector.fCeilingHeight ) && 
                                    ( fYPos2 > nextSector.fFloorHeight && fYPos2 < nextSector.fCeilingHeight ) )
                                {
                                    // Check DropOff (enemies are blocked from walking over ledges higher than this)
                                    if ( nextSector.fFloorHeight >= (sector.fFloorHeight - fDropOff) )
                                    {
#endif // #ifdef CHEAT_NOCLIP                            
                                        // Consider next sector
                                        bSolid = false;
                                        iSectorId = sideDef.iNextSector;                                    
                                    }
                                }
                            }
                        }
                        
                        
                        //if (false)
                        if ( bSolid )
                        {
                            //vVel *= 0.0;
                            //break;

                            // Apply the sliding constraint
                            vec2 vAB = sideDef.vB - sideDef.vA;
                            vec2 vNormal = normalize( vec2( -vAB.y, vAB.x ) );

                            float fDot = dot( vVel, vNormal );
                            vVel -= vNormal * fDot;
                            vVel -= vNormal * 0.2;
                            
                            // Velocity updated, begin testing from initial sector
                            iSectorId = iInitialSector;
                        }    
                        
                        break;                    
                    }
                }
            }                
        }        

        if ( !bReevaluate )
        {
            break;
        }
    }
    
    if (!bReevaluate)
    {
    	vPos += vVel;    
    }
    else
    {
        vVel = vec2(0);
    }
}
            
void Entity_UpdateSlideBox( inout Entity entity, float fTimestep )
{
    entity.fYaw += entity.fYawVel * fTimestep;

    const float fStepHeight = 24.1; // https://www.doomworld.com/vb/doom-general/67054-maximum-height-monsters-can-step-on/
    const float fClearanceHeight = 32.;
    
    float fDropOff = 10000.0;
    if ( entity.iType == ENTITY_TYPE_ENEMY )
    {
        // Enemies 
        fDropOff = 24.0;
    }
    entity.vVel.xz *= fTimestep;
    SlideVector( entity.iSectorId, entity.vPos.xz, entity.vVel.xz, entity.vPos.y + fStepHeight, entity.vPos.y + fClearanceHeight, fDropOff );
    entity.vVel.xz /= fTimestep;
}

float Entity_GetWeaponType( Entity entity )
{
    if ( entity.iType == ENTITY_TYPE_PLAYER )
    {
        //return WEAPON_TYPE_FIREBALL;
        if ( entity.fUseWeapon == 1. )
        {
        	return WEAPON_TYPE_PISTOL;
        }
        else
        {
    		return WEAPON_TYPE_SHOTGUN;
        }
    }
    else
    if ( entity.iType == ENTITY_TYPE_ENEMY )
    {
        if ( entity.iSubType == ENTITY_SUB_TYPE_ENEMY_TROOPER )
        {
            return WEAPON_TYPE_PISTOL;        
        }
        else if ( entity.iSubType == ENTITY_SUB_TYPE_ENEMY_SERGEANT )
        {
            return WEAPON_TYPE_SHOTGUN;        
        }
        else if ( entity.iSubType == ENTITY_SUB_TYPE_ENEMY_IMP )
        {
            return WEAPON_TYPE_FIREBALL;        
        }        
    }

    return WEAPON_TYPE_PISTOL;
}

WeaponDef Entity_GetWeaponDef( Entity entity )
{
    return Weapon_GetDef( Entity_GetWeaponType( entity ), entity.iType == ENTITY_TYPE_ENEMY );
}
    
    
int Enemy_GetState( Entity entity )
{
    return int(entity.fArmor);
}

void Enemy_SetState( inout Entity entity, int iNewState )
{
    if ( Enemy_GetState(entity) == ENEMY_STATE_DIE )
    {
        return;
    }
    
    entity.fArmor = float(iNewState);
    
    bool setRandomTimer = false;
    
    if ( iNewState == ENEMY_STATE_PAIN )
    {
        entity.fTimer = 0.2;    
    }
    else
    if ( iNewState == ENEMY_STATE_DIE )
    {
        SetFlag( entity.iFrameFlags, ENTITY_FRAME_FLAG_DROP_ITEM );
        entity.fTimer = 0.4;    
    }
    else
#ifdef CHEAT_NOAI 
    {
    	entity.fArmor = float(ENEMY_STATE_STAND);        
        entity.fTimer = 0.4;    
    }
#else        
    if ( iNewState == ENEMY_STATE_FIRE )
    {
        SetFlag( entity.iFrameFlags, ENTITY_FRAME_FLAG_FIRE_WEAPON );
        entity.fTimer = 0.3;    
    }
    else
    if ( iNewState == ENEMY_STATE_WALK_RANDOM )
    {
    	float fRandom = Hash( float(entity.iId) + iTime + 3.456 );        
        
        entity.fYaw = fRandom * 3.14 * 2.0;

        float fStepScale = 3.14 / 4.0;
        entity.fYaw = floor( entity.fYaw / fStepScale ) * fStepScale;
        
        setRandomTimer = true;
    }
    else
    if ( iNewState == ENEMY_STATE_WALK_TO_TARGET )
    {
        Entity targetEnt = Entity_Read( STATE_CHANNEL, int(entity.fTarget) );
        vec3 vToTarget = targetEnt.vPos - entity.vPos;
        entity.fYaw = atan(vToTarget.x, vToTarget.z);
        
        float fStepScale = 3.14 / 4.0;
        entity.fYaw = floor( entity.fYaw / fStepScale ) * fStepScale;
        
        setRandomTimer = true;
    }
    else
    {
        setRandomTimer = true;
    }    

    if ( setRandomTimer )
    {
    	float fRandom = Hash( float(entity.iId) + iTime + 0.1 );        
        entity.fTimer = 0.5 + fRandom * fRandom * 1.5;
    }
#endif // #ifndef CHEAT_NOAI        
}

void Enemy_SetRandomHostileState( inout Entity entity, bool notFire )
{
    float fRandom = Hash( float(entity.iId) + iTime );
    
    if ( !notFire && (fRandom < 0.2) )
    {        
        if ( notFire )
        {
        	Enemy_SetState( entity, ENEMY_STATE_STAND );                    
        }
        else
        {
	        Enemy_SetState( entity, ENEMY_STATE_FIRE );        
        }
	}
    else
    if ( fRandom < 0.7 )
    {
        Entity targetEnt = Entity_Read( STATE_CHANNEL, int(entity.fTarget) );
        vec3 vToTarget = targetEnt.vPos - entity.vPos;
        
        if ( length( vToTarget ) < 100.0 )
        {
        	Enemy_SetState( entity, ENEMY_STATE_WALK_RANDOM );
        }
        else
        {
        	Enemy_SetState( entity, ENEMY_STATE_WALK_TO_TARGET );
		}
    }
    else
    {        
        Enemy_SetState( entity, ENEMY_STATE_STAND );        
	}
}

bool Entity_CanHear( Entity entityA, Entity entityB )
{
    // Simple distance check for now...
    // Could do sector coloring somehow?
    if ( length( entityA.vPos - entityB.vPos ) < 400.0 ) 
    {
        return true;
    }
    return false;
}

bool Entity_CanSee( Entity entityA, Entity entityB )
{
    vec3 vRayStart = entityA.vPos + vec3(0,32,0);
    vec3 vRayEnd = entityB.vPos + vec3(0,32,0);            
    vec3 vRayDir = normalize( vRayEnd - vRayStart );
    float fDistToTarget = distance( vRayStart, vRayEnd );
	TraceResult traceResult = Map_Trace( MAP_CHANNEL, g_mapInfo, vRayStart, vRayDir, entityA.iSectorId, fDistToTarget );
    return traceResult.fDist >= fDistToTarget;
}

bool Enemy_CanSee( inout Entity enemy, Entity target )
{
    vec3 vTargetPos = target.vPos;
    vec3 vToTarget = vTargetPos - enemy.vPos;
    float fAng = mod((degrees( atan( vToTarget.x, vToTarget.z ) - enemy.fYaw ) + 180.0), 360.0) - 180.0;

    if ( abs( fAng ) < 45.0 )
    {
        // Line of sight check

        if ( Entity_CanSee( enemy, target ) )
        {
            return true;
        }
    }
    
    return false;
}

void Enemy_UpdateState(  inout Entity entity )
{
    int iState = Enemy_GetState( entity );
    
    if( entity.fHealth <= 0.0 )
    {
        Enemy_SetState( entity, ENEMY_STATE_DIE );
        iState = ENEMY_STATE_DIE;
    }

    if ( iState == ENEMY_STATE_DIE )
    {
        if ( entity.fTimer == 0. )
        {            
            entity.iType = ENTITY_TYPE_DECORATION;
            if ( entity.iSubType == ENTITY_SUB_TYPE_ENEMY_TROOPER )
            {
            	entity.iSubType = ENTITY_SUB_TYPE_DECORATION_DEAD_TROOPER;
    		}
            else
            if ( entity.iSubType == ENTITY_SUB_TYPE_ENEMY_SERGEANT )
            {
            	entity.iSubType = ENTITY_SUB_TYPE_DECORATION_DEAD_SERGEANT;
    		}
            else
            if ( entity.iSubType == ENTITY_SUB_TYPE_ENEMY_IMP )
            {
            	entity.iSubType = ENTITY_SUB_TYPE_DECORATION_DEAD_IMP;
    		}
            else
            {
            	entity.iSubType = ENTITY_SUB_TYPE_DECORATION_BLOODY_MESS;            
            }
        }
        
        return;
    }    
     
    // Check if can see player    
    if ( int(entity.fTarget) == ENTITY_NONE )
    {        
		Entity playerEnt = Entity_Read( STATE_CHANNEL, 0 );
        
        bool wakeUp = false;

        if ( Enemy_CanSee( entity, playerEnt ) )
        {
			wakeUp = true;
        }   

        // Wake if player firing weapon
        if ( !wakeUp )
        {
        	if ( FlagSet( playerEnt.iFrameFlags, ENTITY_FRAME_FLAG_FIRE_WEAPON ) )
            {
                if ( Entity_CanHear( entity, playerEnt ) )
                {
	                wakeUp  = true;
				}
            }            
        }

        if ( wakeUp )
        {
            // target player 
            entity.fTarget = 0.;
        	Enemy_SetState( entity, ENEMY_STATE_STAND );
            iState = ENEMY_STATE_STAND;            
        }
    }
    
    
    if ( iState == ENEMY_STATE_IDLE )
    {
    }
	else
    if ( iState == ENEMY_STATE_PAIN )
    {
        if ( entity.fTimer == 0. )
        {
            Enemy_SetState( entity, ENEMY_STATE_STAND );
        }
    }
	else
    if ( 	iState == ENEMY_STATE_STAND ||
        	iState == ENEMY_STATE_FIRE ||
        	iState == ENEMY_STATE_WALK_TO_TARGET ||
        	iState == ENEMY_STATE_WALK_RANDOM
       )
    {
        if ( int(entity.fTarget) != ENTITY_NONE )
        {
            Entity targetEnt = Entity_Read( STATE_CHANNEL, int(entity.fTarget) );

            if ( targetEnt.fHealth <= 0.0 )
            {
                entity.fTarget = float( ENTITY_NONE );
                Enemy_SetState( entity, ENEMY_STATE_IDLE );
            }
        }
        
        if ( entity.fTimer == 0. )
        {
            if ( iState == ENEMY_STATE_FIRE )
            {
	            Enemy_SetRandomHostileState( entity, true );
            }
            else
            {
	            Enemy_SetRandomHostileState( entity, false );
            }                
        }
    }        
}

bool Entity_IsAlivePlayer( Entity entity )
{
    if ( entity.iType == ENTITY_TYPE_PLAYER )
    {
        if ( entity.fHealth > 0.0 )
        {
            return true;
        }
    }
    
    return false;
}

bool Entity_CanReceiveDamage( Entity entity )
{
    if ( entity.iType == ENTITY_TYPE_BARREL )
        return true;
    
    if ( Entity_IsAlivePlayer( entity ) )
    {
#ifdef CHEAT_GOD        
        return false;
#else
        return true;
#endif    
    }
    
    if ( entity.iType == ENTITY_TYPE_ENEMY )
    {
        if ( Enemy_GetState(entity) == ENEMY_STATE_DIE )
        {
            return false;
        }
        
        return true;
    }
    
    return false;
}

void Entity_ApplyDamage( inout Entity entity, float fAmount, int iSourceId )
{
    if ( !Entity_CanReceiveDamage( entity ) )
    {
        return;
    }
    
    if ( Entity_IsAlivePlayer( entity ) )
    {
        entity.fTookDamage += fAmount;
        if ( entity.fHealth - fAmount <= 0.0 )
        {
            entity.fTimer = 1.5;
            entity.iEvent = EVENT_DIED;
        }
    } 
    
    float fArmorAbsorb = 0.;
    
    if ( Entity_IsAlivePlayer( entity ) )
    {
        fArmorAbsorb = min( fAmount, entity.fArmor );
        entity.fArmor -= fArmorAbsorb;
        fArmorAbsorb *= 0.25; // always absorb 25% (ignore special rules about blue armor)
    }
    
    entity.fHealth -= (fAmount - fArmorAbsorb);
    
    Entity source = Entity_Read( STATE_CHANNEL, iSourceId );

    if ( iSourceId != ENTITY_NONE )
    {
	    entity.vVel.xz += normalize( entity.vPos.xz - source.vPos.xz ) * fAmount;
    }

    if ( entity.iType == ENTITY_TYPE_ENEMY )
    {
        if ( iSourceId != ENTITY_NONE )
        {
            if ( (source.iType == ENTITY_TYPE_ENEMY && entity.iSubType != source.iSubType)
                || source.iType == ENTITY_TYPE_PLAYER )
            {
                // We are now riled with the attacker
                entity.fTarget = float( iSourceId );
            }
        }
        Enemy_SetState( entity, ENEMY_STATE_PAIN );
    }                           
}

bool Entity_IsPlayerTarget( Entity entity )
{
    if ( 	entity.iType == ENTITY_TYPE_PLAYER ||
       	 	entity.iType == ENTITY_TYPE_BARREL || 
       		entity.iType == ENTITY_TYPE_ENEMY )
    {
        return true;
    }
    
    return false;
}

void Player_SelectTarget( inout Entity playerEnt )
{
    // Select target entity (used to aim shots up / down)
    float fBiggestDot = cos( radians( 4.0 ) );
    
    float fClosest = FAR_CLIP;
    
    playerEnt.fTarget = float(ENTITY_NONE);
    
    vec2 vPlayerForwards = vec2( sin( playerEnt.fYaw ), cos( playerEnt.fYaw ) );
    
    for ( int iOtherEntityIndex=0; iOtherEntityIndex<int(ENTITY_MAX_COUNT); iOtherEntityIndex++ )
    {
        Entity otherEntity = Entity_Read( STATE_CHANNEL, iOtherEntityIndex );

        if ( Entity_IsPlayerTarget( otherEntity ) )
        {
            vec3 vToTarget = otherEntity.vPos - playerEnt.vPos;
            
            float fDist = length( vToTarget.xz );
            
            if ( fDist < fClosest ) 
            {
                vec2 vDirToTarget = normalize( vToTarget.xz );
                float fDot = dot( vDirToTarget, vPlayerForwards );
                
                if ( fDot > fBiggestDot )
                {
                    fClosest = fDist;
                    playerEnt.fTarget = float(iOtherEntityIndex);
                }
            }
        }        
    }    
}

void Entity_Think( inout Entity entity, float fTimestep )
{
    entity.fTimer = max( 0.0, entity.fTimer - fTimestep );    

    float fMaxAccel = 1000.0;    
    vec2 vIdealVel = vec2(0);
    
    bool bUseVelSteer = false;
    
    if ( entity.iType == ENTITY_TYPE_PLAYER )
    {        
        bUseVelSteer = true;
                
        if( entity.fHealth > 0.0 )
        {
            vec2 vDir = vec2(0.0);



            if ( g_controls.moveForwards )
            {
                vDir.y += 1.0;
            }
            if ( g_controls.moveBackwards )
            {
                vDir.y -= 1.0;
            }
            if (g_controls.moveLeft || (g_controls.turnLeft && g_controls.strafe))
            {
                vDir.x -= 1.0;
            }
            if (g_controls.moveRight || (g_controls.turnRight && g_controls.strafe))
            {
                vDir.x += 1.0;
            }

            float fspeed = g_controls.run ? 250.0 : 150.0;
            fMaxAccel = g_controls.run ? 2000.0 : 1000.0; 
            float fMaxYawVelAccel = g_controls.run ? 50.0 : 30.0;
            float fTurnspeed = g_controls.run ? 3.5 : 1.5;

            vDir *= fspeed;

            vIdealVel.x = vDir.x * cos( entity.fYaw ) + vDir.y * sin( entity.fYaw );
            vIdealVel.y = vDir.x * -sin( entity.fYaw ) + vDir.y * cos( entity.fYaw );

            float fMouseSpeed = 1.0 / 180.0;
            
            vec2 vMouseDelta = (iMouse.xy - g_gameState.vPrevMouse.xy) * fMouseSpeed;
            vec2 vMouseSign = sign(vMouseDelta);
            vec2 vMouseAbs = abs(vMouseDelta);
            vMouseAbs = pow( vMouseAbs, vec2(1.5));
            vMouseDelta = vMouseAbs * vMouseSign;
            if ( iMouse.z > 0.0 && g_gameState.vPrevMouse.z > 0.0 )
            {
            	entity.fYaw += (vMouseDelta.x);
                
                if ( g_controls.toggleMouseLook )
                {
                    float fPitchInvert = g_controls.toggleInvertY ? -1.0 : 1.0;
                    
                	entity.fPitch += (vMouseDelta.y) * fPitchInvert;
                }
                else
                {
                	entity.fPitch = 0.0;
                }
                float fMaxPitch = radians(89.0);
                entity.fPitch = clamp( entity.fPitch, -fMaxPitch, fMaxPitch);
            }
            
            float fIdealYawVel = 0.0;

            if ( g_controls.turnLeft && !g_controls.strafe )
            {
                fIdealYawVel -= 1.0;
            }
            if ( g_controls.turnRight && !g_controls.strafe )
            {
                fIdealYawVel += 1.0;
            }

            fIdealYawVel *= fTurnspeed;

            float fToIdealYawVel = fIdealYawVel - entity.fYawVel;
            entity.fYawVel += clamp( fToIdealYawVel, -fMaxYawVelAccel * fTimestep, fMaxYawVelAccel * fTimestep );        
            
            // Change weapon
            {
				if ( g_controls.weap_2 )
                {
                    entity.fUseWeapon = 1.;
                }
                
				if ( g_controls.weap_3 )
                {
                    if ( entity.fHaveShotgun > 0. )
                    {
                    	entity.fUseWeapon = 2.;
                    }
                }
            }
            
            
            // Fire weapon
            if ( g_controls.fire )
            {
                if ( entity.fTimer == 0.0 )
                {
                    SetFlag( entity.iFrameFlags, ENTITY_FRAME_FLAG_FIRE_WEAPON );

                    WeaponDef weaponDef = Entity_GetWeaponDef( entity );

                    // reload time
                    entity.fTimer = weaponDef.fReloadTime;
                    
                    Player_SelectTarget( entity );
                }
            }
            
            // Apply Nukage damage
	        #define TEX_NUKAGE 2u
            Sector sector = Map_ReadSector( MAP_CHANNEL, entity.iSectorId );
            if ( sector.iFloorTexture == TEX_NUKAGE )
            {
                float fCurrTime = g_gameState.fGameTime;
                float fNextTime = fCurrTime + fTimestep;
                // apply damage every second
                if ( floor( fCurrTime ) != floor( fNextTime ) )
                {
                    // 5 damage per second
                	Entity_ApplyDamage( entity, 5.0, ENTITY_NONE );
                }
            }                           
        }
        else
        {
            entity.fYawVel *= pow( 0.3, fTimestep );
        }
    }    
    else if ( entity.iType == ENTITY_TYPE_ENEMY )
    {
        bUseVelSteer = true;
        fMaxAccel = 50.;
        
        Enemy_UpdateState( entity );
        
        int iState = Enemy_GetState( entity );
        
        if ( iState == ENEMY_STATE_FIRE )
        {
            Entity targetEnt = Entity_Read( STATE_CHANNEL, int(entity.fTarget) );
            vec3 vToTarget = targetEnt.vPos - entity.vPos;
            entity.fYaw = atan(vToTarget.x, vToTarget.z);
        }
        
        if ( iState == ENEMY_STATE_WALK_TO_TARGET || iState == ENEMY_STATE_WALK_RANDOM )
        {
            float fWalkSpeed = 50.;
            vIdealVel.x = sin( entity.fYaw ) * fWalkSpeed;
            vIdealVel.y = cos( entity.fYaw ) * fWalkSpeed;
        }     
    }    
    else if ( entity.iType == ENTITY_TYPE_BARREL )
    {
        entity.vVel *= pow( 0.3, fTimestep );
        
        if( entity.fHealth <= 0.0 )
        {
            if ( entity.iSubType == ENTITY_SUB_TYPE_BARREL_INACTIVE )
            {
                entity.iSubType = ENTITY_SUB_TYPE_BARREL_EXPLODING;
                entity.fTimer = 1.0;
            }
            else
            if ( entity.iSubType == ENTITY_SUB_TYPE_BARREL_EXPLODING )
            {
                if ( entity.fTimer <= 0.0 )
                {
	                entity.iSubType = ENTITY_SUB_TYPE_BARREL_APPLY_DAMAGE;
                }
            }
            else
            {
				Entity_Clear( entity );                                    
            }
        }        
    }    
    else if ( entity.iType == ENTITY_TYPE_BULLET )
    {
        entity.fTarget = -1.; // Clear target (only deal damage for first frame)
        if ( entity.fTimer <= 0.0 )
        {
            Entity_Clear( entity );
        }
    }
    else if ( entity.iType == ENTITY_TYPE_FIREBALL )
    {
        if ( entity.fTimer <= 0.0 )
        {
            Entity_Clear( entity );
        }
    }
    else if ( entity.iType == ENTITY_TYPE_DOOR )
    {        
        Entity playerEnt = Entity_Read( STATE_CHANNEL, 0 );
        
        bool bPlayerNearby = (distance(entity.vPos.xz, playerEnt.vPos.xz) < 100. );
        bool bTrigger = g_controls.open && bPlayerNearby;
        
        float fOpenDist = entity.fTarget - entity.vPos.y;
        
        // Door state machine
        if ( entity.iSubType == 0 )
        {
	        // Closed        
            if ( bTrigger )
            {
                entity.iSubType = 1;
            }
        }
        else
        if ( entity.iSubType == 1 )
        {
	        // Opening
            fOpenDist += 40.0 * fTimestep;
            
            if ( fOpenDist >= 80.0 )
            {
                fOpenDist = 80.0;
                entity.iSubType = 2;
                
                entity.fTimer = 5.;
            }
        }
        else
        if ( entity.iSubType == 2 )
        {
	        // Open
            if ( bTrigger || entity.fTimer == 0.0 )
            {
                entity.iSubType = 3;
            }
        }
        else
        if ( entity.iSubType == 3 )
        {
	        // Closing
            fOpenDist -= 40.0 * fTimestep;
            
            if ( fOpenDist <= 0.0 )
            {
                fOpenDist = 0.0;
                entity.iSubType = 0;
            }
            
            if ( bTrigger )
            {
                entity.iSubType = 1;
            }            
        }
        
        entity.fTarget = fOpenDist + entity.vPos.y;        
    }
    else
    if ( entity.iType == ENTITY_TYPE_PLATFORM )
    {
        Entity playerEnt = Entity_Read( STATE_CHANNEL, 0 );
        
        bool bPlayerNearby = (distance(entity.vPos.xz, playerEnt.vPos.xz) < 150. );
        
        float fOpenDist = entity.fTarget;
        // Hack - params from spawn data yaw
        float fSpawnYaw = 90. - degrees(entity.fYaw);
        float fMoveSpeed = abs(fSpawnYaw);
        bool bStaysLowered = fSpawnYaw < 0.0;
        
        // Hack - both platforms on this level go to this height
        float fFinalFloorHeight = -48.;
        
        if ( entity.iSubType == 0 )
        {
            // Platform is Up
            
            if ( bPlayerNearby )
            {
                entity.iSubType = 1;
            }
        }
        else if ( entity.iSubType == 1 )
        {
            // Platform is Lowering
            fOpenDist -= fMoveSpeed * fTimestep;
            
            if ( fOpenDist < fFinalFloorHeight )
            {
                fOpenDist = fFinalFloorHeight;
                entity.iSubType = 2;
                
                entity.fTimer = 5.;
            }            
        }
        else if ( entity.iSubType == 2 )
        {
            // Platform is Down
            if ( !bStaysLowered )
            {
                if ( entity.fTimer == 0.0 )
                {
                    entity.iSubType = 3;
                }            
            }
        }
        else if ( entity.iSubType == 3 )
        {
            // Platform is Raising
            fOpenDist += fMoveSpeed * fTimestep;
            
            if ( fOpenDist > entity.vPos.y )
            {
                fOpenDist = entity.vPos.y;
                entity.iSubType = 0;
            }            
        }
        
        entity.fTarget = fOpenDist;
        
        //entity.fTarget = sin( iTime ) * 100.0;
    }
    
    if ( bUseVelSteer )
    {
        vec2 vToIdealVel = vIdealVel - entity.vVel.xz;
        float len = length( vToIdealVel );

        if ( len > 0.0 )
        {
            entity.vVel.xz += normalize(vToIdealVel) * clamp( len, 0.0, fMaxAccel * fTimestep );
        }        
    }
}

bool CanBePushed( Entity entity, Entity otherEntity )
{
    if ( Entity_IsAlivePlayer( entity ) )
    {
        if ( (otherEntity.iType == ENTITY_TYPE_ENEMY && Enemy_GetState( otherEntity ) != ENEMY_STATE_DIE ) ||
             (otherEntity.iType == ENTITY_TYPE_BARREL) ||
             (otherEntity.iType == ENTITY_TYPE_DECORATION && otherEntity.iSubType == ENTITY_SUB_TYPE_DECORATION_FLOOR_LAMP)
           )
        {
            return true;
        }
    }
    else if ( entity.iType == ENTITY_TYPE_ENEMY )
    {
        if (    (otherEntity.iType == ENTITY_TYPE_ENEMY)
             || (otherEntity.iType == ENTITY_TYPE_BARREL)
             //|| (otherEntity.fType == ENTITY_TYPE_PLAYER)
           )
        {
            return true;
        }
    }
    
    return false;
}

void Entity_Collide( inout Entity entity, Entity otherEntity, float fTimestep )
{
    // True if we can be pushed
    if ( CanBePushed( entity, otherEntity ) )
    {
        vec2 vDeltaPos = entity.vPos.xz - otherEntity.vPos.xz;
        vec2 vDeltaVel = entity.vVel.xz - otherEntity.vVel.xz;

        float fLen = length( vDeltaPos );
        float fVelLen = length(vDeltaVel);
        float fCombinedRadius = 20.0;
        if ( fLen > 0.0 && fLen < fCombinedRadius )
        {
            vec2 vNormal = normalize(vDeltaPos);
            
            if ( fVelLen > 0.0 )
            {
                float fProj = dot( vNormal, vDeltaVel );

                if ( fProj < 0.0 )
                {
                    // cancel vel in normal dir
                    vec2 vImpulse = -fProj * vNormal;
                    
                    // Push away
                    float fPenetration = fCombinedRadius - fLen;
                    vImpulse += vNormal * fPenetration * 5.0 * fTimestep;
                    
                    
				    if ( CanBePushed( otherEntity, entity ) )
                    {
                    	entity.vVel.xz += vImpulse * 0.5;
                    }
                    else
                    {
                    	entity.vVel.xz += vImpulse;
                    }
                }
            }            
        }        
    }    
}

void Player_GiveHealth( inout Entity entity, float fAmount, bool mega )
{    
    if ( mega )
    {
	    entity.fHealth = min( entity.fHealth + fAmount, 200.0 );
    }
    else
    {
        if ( entity.fHealth < 100.0 )
        {
		    entity.fHealth = min( entity.fHealth + fAmount, 100.0 );
        }
	}
}

void Player_GiveArmor( inout Entity entity, float fAmount, bool mega )
{
    if ( mega )
    {
	    entity.fArmor = min( entity.fArmor + fAmount, 200.0 );
    }
    else
    {
        if ( entity.fArmor < 100.0 )
        {
		    entity.fArmor = min( entity.fArmor + fAmount, 100.0 );
        }
	}

}

#define PICKUP_RADIUS 25.

bool Fireball_Hit( Entity fireball, Entity other )
{
    if ( fireball.iSubType == ENTITY_SUB_TYPE_FIREBALL  )
    {
        if ( other.iId != int(fireball.fArmor) )
        {                    	
            if ( Entity_CanReceiveDamage( other ) )
            {
                if ( length( other.vPos.xz - fireball.vPos.xz ) < (12. + 16.0) )
                {
                	return true;
                }
            }
        }
    }
    
    return false;
}
            
void Entity_Interact( inout Entity entity, float fTimestep )
{    
    // Apply actions to this entity based on every other entity
    
    float fSpawnQueueSlot = 0.0;
    
    for ( int iOtherEntityIndex=0; iOtherEntityIndex<int(ENTITY_MAX_COUNT); iOtherEntityIndex++ )
    {
        Entity otherEntity = Entity_Read( STATE_CHANNEL, iOtherEntityIndex );

        if ( entity.iType == ENTITY_TYPE_NONE )
        {
            if ( iOtherEntityIndex == entity.iId )
            {
	            entity.fTarget = fSpawnQueueSlot;                
    	        break;
            }

	        // Set this entity's position in spawn queue
            if ( otherEntity.iType == ENTITY_TYPE_NONE )
            {
                fSpawnQueueSlot++;
            }                    
        }
        else
        {        
            // Ignore self
            if ( iOtherEntityIndex == entity.iId )
            {
                continue;
            }

            Entity_Collide( entity, otherEntity, fTimestep );

            if ( Entity_IsAlivePlayer( entity ) )
            {
                if ( otherEntity.iType == ENTITY_TYPE_ITEM )
                {
                    if ( distance(entity.vPos.xz, otherEntity.vPos.xz) < PICKUP_RADIUS)
                    {
                        if( otherEntity.iSubType == ENTITY_SUB_TYPE_ITEM_HEALTH_BONUS )
                        {
                            Player_GiveHealth( entity, 1.0, true );
				            entity.iEvent = EVENT_HEALTH_BONUS;                            
                        }
                        else
                        if( otherEntity.iSubType == ENTITY_SUB_TYPE_ITEM_ARMOR_BONUS )
                        {
                            Player_GiveArmor( entity, 1.0, false );
				            entity.iEvent = EVENT_ARMOR_BONUS;                            
                        }
                        else
                        if ( otherEntity.iSubType == ENTITY_SUB_TYPE_ITEM_MEDIKIT )
                        {
                            Player_GiveHealth( entity, 25.0, false );
				            entity.iEvent = EVENT_MEDIKIT;                            
                        }
                        else
                        if ( otherEntity.iSubType == ENTITY_SUB_TYPE_ITEM_STIMPACK )
                        {
                            Player_GiveHealth( entity, 10.0, false );
				            entity.iEvent = EVENT_STIMPACK;
                        }
                        else
                        if ( otherEntity.iSubType == ENTITY_SUB_TYPE_ITEM_GREENARMOR )
                        {
                            Player_GiveArmor( entity, 100.0, false );
				            entity.iEvent = EVENT_GREENARMOR;
                        }
                        else
                        if ( otherEntity.iSubType == ENTITY_SUB_TYPE_ITEM_BLUEARMOR )
                        {
                            Player_GiveArmor( entity, 200.0, true );
				            entity.iEvent = EVENT_BLUEARMOR;
                        }
                        else
                        if ( otherEntity.iSubType == ENTITY_SUB_TYPE_ITEM_SHOTGUN )
                        {
                            if ( entity.fHaveShotgun == 0. )
                            {
	                    		entity.fUseWeapon = 2.;                        
					            entity.iEvent = EVENT_SHOTGUN;
                            }
                            	
                            entity.fHaveShotgun = 1.;
                        }
                    }
                }
            }
            else
            if ( entity.iType == ENTITY_TYPE_ITEM )
            {
                if ( Entity_IsAlivePlayer( otherEntity ) )
                {
                    if ( distance(entity.vPos.xz, otherEntity.vPos.xz) < PICKUP_RADIUS)
                    {
                        entity.iType = ENTITY_TYPE_NONE;
                    }
                }
            }
            if ( entity.iType == ENTITY_TYPE_FIREBALL )
            {
                if ( Fireball_Hit( entity, otherEntity ) )
                {
                    entity.iSubType = ENTITY_SUB_TYPE_FIREBALL_IMPACT;
                    entity.fTimer = 0.3;
                }
            }
            
            // Process damage from other
            if ( otherEntity.iType == ENTITY_TYPE_BULLET )
            {
                // Bullet is applying damage to this entity
                if ( otherEntity.fTarget == float(entity.iId) )
                {
                    Entity_ApplyDamage( entity, otherEntity.fHealth, int( otherEntity.fArmor ) );
                }
            }
            else
            if ( otherEntity.iType == ENTITY_TYPE_FIREBALL )
            {
                if ( Fireball_Hit( otherEntity, entity ) )
                {
                    Entity_ApplyDamage( entity, otherEntity.fHealth, int( otherEntity.fArmor ) );
                }
            }
            else
            if ( otherEntity.iType == ENTITY_TYPE_BARREL && otherEntity.iSubType == ENTITY_SUB_TYPE_BARREL_APPLY_DAMAGE )
            {
                // Todo : trace?
                float fBlastRadius = 250.0;
                float fMaxDamage = 50.0;
                
                vec3 vSeparation = entity.vPos - otherEntity.vPos;
                float fDamage = fMaxDamage * clamp( 1.0 - length( vSeparation.xz ) / fBlastRadius, 0.0, 1.0 );
                
                if ( Entity_CanSee( otherEntity, entity ) )
                {
				    Entity_ApplyDamage( entity, fDamage, otherEntity.iId );
                }
            }
        }        
    }
}

void Entity_Move( inout Entity entity, float fTimestep )
{    
    if( ( entity.iType == ENTITY_TYPE_PLAYER ) 	||
        ( entity.iType == ENTITY_TYPE_ENEMY ) 	||
        ( entity.iType == ENTITY_TYPE_BARREL )
      )
    {
        //vec2 vPrevPos = entity.vPos.xz;

        Entity_UpdateSlideBox( entity, fTimestep );

        //Map_UpdateSector( g_mapInfo, vPrevPos, entity.vPos.xz, entity.fSectorId );
    }
    
    if( ( entity.iType == ENTITY_TYPE_PLAYER )	||
        ( entity.iType == ENTITY_TYPE_ENEMY )  	||
        ( entity.iType == ENTITY_TYPE_BARREL ) 	|| 
        ( entity.iType == ENTITY_TYPE_ITEM ) 	||
        ( entity.iType == ENTITY_TYPE_DECORATION )
      )
    {
    	Entity_Fall( entity, fTimestep );    
    }
    
    if ( entity.iType == ENTITY_TYPE_BULLET )
    {
        entity.vPos.y += 10.0 * fTimestep;
    }
    
    if ( entity.iType == ENTITY_TYPE_FIREBALL )
    {
        if ( entity.iSubType == ENTITY_SUB_TYPE_FIREBALL )
        {
	        vec2 vPrevPos = entity.vPos.xz;

            float fMaxTraceDist = length( entity.vVel ) * fTimestep;
            vec3 vDir = normalize( entity.vVel );

            TraceResult traceResult = Map_Trace( MAP_CHANNEL, g_mapInfo, entity.vPos, vDir, entity.iSectorId, fMaxTraceDist );

            if ( traceResult.fDist > 0.0 )
            {
            	entity.vPos += vDir * traceResult.fDist;
            }

        	Map_UpdateSector( MAP_CHANNEL, g_mapInfo, vPrevPos, entity.vPos.xz, entity.iSectorId );
            
            if ( traceResult.fDist > 0.0 && traceResult.fDist < fMaxTraceDist  )
            {
                entity.iSubType = ENTITY_SUB_TYPE_FIREBALL_IMPACT;
                entity.fTimer = 0.3;
            }
		}
    }
}

float RayCircleIntersect( vec2 vRayOrigin, vec2 vRayDir, vec2 vCircleOrigin, float fRadius2 )
{
    vec2 vRayOriginToCircleOrigin = vCircleOrigin - vRayOrigin;
    float fProjection = dot( vRayOriginToCircleOrigin, vRayDir );        
    vec2 vClosestPos = vRayOrigin + vRayDir * fProjection;
    vec2 vClosestToCircle = vCircleOrigin - vClosestPos;
    float fClosestDist2 = dot( vClosestToCircle, vClosestToCircle );
    if ( fClosestDist2 < fRadius2 )
    {
        float fHalfChordLength = sqrt( fRadius2 - fClosestDist2 );
        float fHitT = fProjection - fHalfChordLength;

        return fHitT;
	}

    return -1.0;
}

float Entity_Trace( vec3 vRayOrigin, vec3 vRayDir, int iIgnoreEnt, out Entity hitEntity )
{
    hitEntity.iId = -1;
    float fHitEntityDist = FAR_CLIP;
    
    float fOrigDirLen = length( vRayDir.xz );
    
    vec2 vFlatRayDir = vRayDir.xz / fOrigDirLen;

    for ( int iTraceEntityIndex=0; iTraceEntityIndex<int(ENTITY_MAX_COUNT); iTraceEntityIndex++ )
    {
        if ( iTraceEntityIndex == iIgnoreEnt )
            continue;
        
        Entity entity = Entity_Read( STATE_CHANNEL, iTraceEntityIndex );

        
        // TODO: Entity_GetSize function?
        float fRadius = 0.0;
        float fHeight = 0.0;
        if ( entity.iType == ENTITY_TYPE_BARREL )
        {
            fRadius = 12.0;
            fHeight = 32.0;            
        }
        
        if ( entity.iType == ENTITY_TYPE_ENEMY || entity.iType == ENTITY_TYPE_PLAYER )
        {
            fRadius = 15.0;
            fHeight = 54.0;            
        }
 
        if ( fRadius > 0.0 )
        {
            float fHitT = RayCircleIntersect( vRayOrigin.xz, vFlatRayDir, entity.vPos.xz, fRadius * fRadius );
            fHitT /= fOrigDirLen;
			if ( fHitT > 0.0 && fHitT < fHitEntityDist )
            {
                float fEntHitY = vRayOrigin.y + vRayDir.y * fHitT - entity.vPos.y;
                
                if ( fEntHitY > 0. && fEntHitY < fHeight )
                {
                    fHitEntityDist = fHitT;
                    hitEntity = entity;
                }
            }
 	   }
	}
    
    return fHitEntityDist;
}

int Entity_GetSpawnQueueSlotType( int iSpawnQueueSlot, inout Entity spawningEntity, out int iSpawnSubType )
{
    iSpawnSubType = 0;
    
    int iOtherSpawnQueueSlot = 0;
    for ( int iOtherEntityIndex=0; iOtherEntityIndex<int(ENTITY_MAX_COUNT); iOtherEntityIndex++ )
    {
        spawningEntity = Entity_Read( STATE_CHANNEL, iOtherEntityIndex );

        // spawn fired bullets
        if ( FlagSet( spawningEntity.iFrameFlags, ENTITY_FRAME_FLAG_FIRE_WEAPON ) )
        {
            WeaponDef weaponDef = Entity_GetWeaponDef( spawningEntity );

            int iNewSlot = iOtherSpawnQueueSlot + weaponDef.iProjectileCount;
            if ( iNewSlot >= iSpawnQueueSlot ) 
            {
                if ( weaponDef.fWeaponType == WEAPON_TYPE_FIREBALL )
                {
	                return ENTITY_TYPE_FIREBALL;
                }
                else
                {
	                return ENTITY_TYPE_BULLET;
                }                    
            }
            iOtherSpawnQueueSlot = iNewSlot;
        }
        
        // spawn dropped items
        // spawn fired bullets
        if ( FlagSet( spawningEntity.iFrameFlags, ENTITY_FRAME_FLAG_DROP_ITEM ) )
        {
            bool doSpawn = false;
            
            if ( spawningEntity.iSubType == ENTITY_SUB_TYPE_ENEMY_SERGEANT )
            {
                iSpawnSubType = ENTITY_SUB_TYPE_ITEM_SHOTGUN;
                doSpawn = true;
            }
            
            if ( doSpawn )
            {
                int iNewSlot = iOtherSpawnQueueSlot + 1;
                if ( iNewSlot >= iSpawnQueueSlot ) 
                {                
                    return ENTITY_TYPE_ITEM;                    
                }
	            iOtherSpawnQueueSlot = iNewSlot;
            }
        }
        
    }
    
    return ENTITY_TYPE_NONE;
}

bool Entity_SpawnOther( inout Entity entity )
{
    if ( entity.iType == ENTITY_TYPE_NONE )
    {
        Entity spawningEntity;
        
        int iSpawnQueueSlot = int(entity.fTarget);
        int iSpawnSubType;
        int iSpawnType = Entity_GetSpawnQueueSlotType( iSpawnQueueSlot, spawningEntity, iSpawnSubType );
        
        if ( iSpawnType == ENTITY_TYPE_BULLET )
        {
            Entity_Clear( entity );
            entity.iType = ENTITY_TYPE_BULLET;
            entity.iSubType = ENTITY_SUB_TYPE_BULLET_WALL;
            
            WeaponDef weaponDef = Entity_GetWeaponDef( spawningEntity );
            
            vec3 vRayOrigin = spawningEntity.vPos + vec3(0,40.,0);
            // adjust Y to aim at target
            float dy = tan(spawningEntity.fPitch);
            if ( int(spawningEntity.fTarget) != ENTITY_NONE )
            {
		        Entity targetEntity;
                
                targetEntity = Entity_Read( STATE_CHANNEL, int(spawningEntity.fTarget) );
                
                vec3 vTargetPos = targetEntity.vPos;
                
                if( targetEntity.iType == ENTITY_TYPE_BARREL )
                {
                    vTargetPos.y += 30.0;
                }
                else
                {
                    vTargetPos.y += 40.0;                    
				}
                
                vec3 vToTarget = vTargetPos - vRayOrigin;
                dy = vToTarget.y / length(vToTarget.xz);
            }
            float fSRand = Hash( float(iSpawnQueueSlot) + iTime) * 2.0 - 1.0;
            float fSpread = weaponDef.fSpread;
            float fYaw = spawningEntity.fYaw + fSRand * fSpread;            
            vec3 vRayDir = normalize( vec3(sin(fYaw), dy, cos(fYaw)) );

            Entity hitEntity;
            float fHitDist = Entity_Trace( vRayOrigin, vRayDir, spawningEntity.iId, hitEntity );
            
            TraceResult traceResult = Map_Trace( MAP_CHANNEL, g_mapInfo, vRayOrigin, vRayDir, spawningEntity.iSectorId, fHitDist );
			
            if ( hitEntity.iType == ENTITY_TYPE_ENEMY || hitEntity.iType == ENTITY_TYPE_PLAYER )
            {
	            entity.iSubType = ENTITY_SUB_TYPE_BULLET_FLESH;                
            }
            
            // Not sure why this > 0 test is required. Oh well...
            if ( traceResult.fDist > 0.0 &&  traceResult.fDist < fHitDist  )
            {
                fHitDist = traceResult.fDist;
                hitEntity.iId = -1;
	            entity.iSubType = ENTITY_SUB_TYPE_BULLET_WALL;
            }
            
            entity.vPos = vRayOrigin + vRayDir * fHitDist + vec3(0,-8,0);
            float fRand = Hash(float(iSpawnQueueSlot) + iTime + 1.234);
            entity.fHealth = weaponDef.fBaseDamagePerProjectile + weaponDef.fRandomDamagePerProjectile * fRand; // Damage to deal
            entity.fTimer = 0.3; // Impact effect timer
            entity.fTarget = float( hitEntity.iId ); // Which entity did we hit (cleared on first frame)
            entity.fArmor = float( spawningEntity.iId ); // Who fired the bullet
            entity.iSectorId = traceResult.iSector;

            return true;
        }
        else
        if ( iSpawnType == ENTITY_TYPE_FIREBALL )
        {
            Entity_Clear( entity );
            entity.iType = ENTITY_TYPE_FIREBALL;
            entity.iSubType = ENTITY_SUB_TYPE_FIREBALL;
            
            WeaponDef weaponDef = Entity_GetWeaponDef( spawningEntity );
            
            entity.vPos = spawningEntity.vPos + vec3(0,40.,0);
            // adjust Y to aim at target
            float dy = 0.0;
            if ( int(spawningEntity.fTarget) != ENTITY_NONE )
            {
		        Entity targetEntity;
                
                targetEntity = Entity_Read( STATE_CHANNEL, int( spawningEntity.fTarget ) );
                
                vec3 vTargetPos = targetEntity.vPos;
                
                if( targetEntity.iType == ENTITY_TYPE_BARREL )
                {
                    vTargetPos.y += 30.0;
                }
                else
                {
                    vTargetPos.y += 40.0;                    
				}
                
                vec3 vToTarget = vTargetPos - entity.vPos;
                dy = vToTarget.y / length(vToTarget.xz);
            }
            float fSRand = Hash( float(iSpawnQueueSlot) + iTime) * 2.0 - 1.0;
            float fSpread = weaponDef.fSpread;
            float fYaw = spawningEntity.fYaw + fSRand * fSpread;            
            entity.vVel = normalize( vec3(sin(fYaw), dy, cos(fYaw)) );
            float fMoveSpeed = 200.0;
            entity.vVel *= fMoveSpeed;

            entity.fHealth = weaponDef.fBaseDamagePerProjectile; // Damage to deal
            entity.fTimer = 5.0; // Travel timeout
            entity.fArmor = float( spawningEntity.iId ); // Who fired the bullet
            entity.iSectorId = spawningEntity.iSectorId;

            return true;
        }
        else
        if ( iSpawnType == ENTITY_TYPE_ITEM )
        {
            Entity_Clear( entity );
            entity.iType = ENTITY_TYPE_ITEM;
            entity.iSubType = iSpawnSubType;
            entity.vPos = spawningEntity.vPos + vec3(0, 24, 0);
            entity.vVel = vec3(0, 2, 0);
            entity.iSectorId = spawningEntity.iSectorId;
            
            return true;
        }            
    }
    
    return false;
}

void Entity_Update( inout Entity entity, float fTimestep )
{    
    entity.iFrameFlags = 0;
    entity.iEvent = 0;
    entity.fTookDamage = 0.;

    // This is first to ensure consistent state
    if( Entity_SpawnOther( entity ) )
    {
        return;
    }
        
    Entity_Think( entity, fTimestep );

    Entity_Interact( entity, fTimestep );
    
    Entity_Move( entity, fTimestep );    
}

vec4 DrawMap( vec2 vTexCoord )
{
    if ( vTexCoord.y < 0.0 )
    {
        return vec4(0,0,0,1);
    }    
        
    float fScale = 10.0;
        
    vec2 vPixelWorldPos = vTexCoord;    
    vPixelWorldPos -= vec2(200, 150); 
    vPixelWorldPos *= fScale;
    vPixelWorldPos += vec2(1056, -3616);
    
    fScale *= 1.1;
    
    //for(int iSectorIndex=0; iSectorIndex<MAX_SECTOR_COUNT; iSectorIndex++)
    for(int iSectorIndex=0; iSectorIndex < g_mapInfo.iSectorCount; iSectorIndex++)
    {
        //if ( iSectorIndex > g_mapInfo.iSectorCount )
          //  break;

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
                    return vec4(1,1,0,1);
                }
                else
                {
                    return vec4(1,0,0,1);
                }
            }
        }			
    }
        
    return vec4(0.0);
    
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{    
    fragColor = vec4(0.0);

    g_controls = ReadControls();
    
    g_mapInfo = ReadMapInfo( MAP_CHANNEL );
    g_gameState = GameState_Read( STATE_CHANNEL );
    
    float fTimestep = iTimeDelta;//1.0 / 60.0;

    bool bShouldOutputGameState = all( lessThanEqual( floor(fragCoord.xy), vec2( 2, 0 ) ) );
    int iOutputEntityId = Entity_GetOutputId( fragCoord );
    bool bShouldOutputEntity = Entity_IdValid( iOutputEntityId );
    
    if ( !bShouldOutputEntity && !bShouldOutputGameState )
    {
#ifdef ALLOW_MAP        
        if ( iFrame != 1 )
#endif            
        {
        	fragColor = texelFetch( iChannel1, ivec2(floor(fragCoord)), 0 );
	        return;
        }
        
#ifdef ALLOW_MAP        
        fragColor = DrawMap( fragCoord - vec2(0, 32));
        return;
#endif        
        
        //fragColor.rgb = vec3( 1,0,0);
        //discard;
    }
    
    if ( bShouldOutputGameState )
    {
        if ( iFrame == 0 )
        {
            GameState_Reset( g_gameState, iMouse );
        }        
    }
    int iMainGameState = g_gameState.iMainState;
        
    if ( bShouldOutputEntity )
    {
        Entity outputEntity;
        outputEntity = Entity_Read( STATE_CHANNEL, iOutputEntityId );
        
        if ( iMainGameState == MAIN_GAME_STATE_BOOT )
        {
            Entity_Clear( outputEntity );
        }

        if ( iMainGameState == MAIN_GAME_STATE_INIT_LEVEL )
        {
            Entity_MapSpawn( outputEntity, iOutputEntityId );
        }               
        else if ( iMainGameState == MAIN_GAME_STATE_GAME_RUNNING )
        {        
        	Entity_Update( outputEntity, fTimestep );
        }
        
        Entity_Store( outputEntity, fragColor, fragCoord );        
    }
    
    if ( bShouldOutputGameState )
    {
        // Update GameState  

        Entity playerEnt;
        playerEnt = Entity_Read( STATE_CHANNEL, 0 );
                
		g_gameState.fStateTimer += fTimestep;
        
        if ( iMainGameState == MAIN_GAME_STATE_BOOT )
        {
            g_gameState.iMainState = MAIN_GAME_STATE_SKILL_SELECT;			
        }
        
        if ( iMainGameState == MAIN_GAME_STATE_SKILL_SELECT )
        {
            if ( g_controls.menuConfirm || g_gameState.fStateTimer > 2.0 )
            {
                g_gameState.iMainState = MAIN_GAME_STATE_INIT_LEVEL;
	            g_gameState.fStateTimer = 0.0;
            }            
        }
        
        if ( iMainGameState == MAIN_GAME_STATE_GAME_RUNNING )
        {
        	g_gameState.fGameTime += fTimestep;        

            
            bool bPlayerNearExit = (distance( playerEnt.vPos.xz, vec2( 2930, -4768 ) ) < 100.0);        
            if ( bPlayerNearExit && g_controls.open )
            {
                g_gameState.iMainState = MAIN_GAME_STATE_WIN;
	            g_gameState.fStateTimer = 0.0;
            }   

            if ( playerEnt.fHealth <= 0.0 && playerEnt.fTimer <= 0.0 )
            {
                if ( g_controls.menuConfirm )
                {
                    g_gameState.iMainState = MAIN_GAME_STATE_INIT_LEVEL;
		            g_gameState.fStateTimer = 0.0;
                }
            }
            
            if ( playerEnt.fHealth > 0.0 )
            {
                // This does debounce logic for us
                if ( Key_IsToggled( iChannelKeyboard, KEY_TAB ) )
                {
                    g_gameState.fMap = 1.0;
                }
                else
                {
                    g_gameState.fMap = 0.0;                
                }
            }
            
            // Apply / update HUD effects
            {
                if ( playerEnt.iEvent == EVENT_HEALTH_BONUS )
                {
                    g_gameState.fHudFx += 0.25;

                    g_gameState.iMessage = MESSAGE_HEALTH_BONUS;
                    g_gameState.fMessageTimer = 3.0;
                }
                else if ( playerEnt.iEvent == EVENT_ARMOR_BONUS )
                {
                    g_gameState.fHudFx += 0.25;

                    g_gameState.iMessage = MESSAGE_ARMOR_BONUS;
                    g_gameState.fMessageTimer = 3.0;
                }
                else if ( playerEnt.iEvent == EVENT_STIMPACK )
                {
                    g_gameState.fHudFx += 0.5;

                    g_gameState.iMessage = MESSAGE_STIMPACK;
                    g_gameState.fMessageTimer = 3.0;
                }
                else if ( playerEnt.iEvent == EVENT_MEDIKIT )
                {
                    g_gameState.fHudFx += 0.5;

                    g_gameState.iMessage = MESSAGE_MEDIKIT;
                    g_gameState.fMessageTimer = 3.0;
                }
                else if ( playerEnt.iEvent == EVENT_GREENARMOR )
                {
                    g_gameState.fHudFx += 1.0;
                    
                    g_gameState.iMessage = MESSAGE_GREENARMOR;
                    g_gameState.fMessageTimer = 3.0;
                }
                else if ( playerEnt.iEvent == EVENT_BLUEARMOR )
                {
                    g_gameState.fHudFx += 1.0;
                    
                    g_gameState.iMessage = MESSAGE_BLUEARMOR;
                    g_gameState.fMessageTimer = 3.0;
                }
                else if ( playerEnt.iEvent == EVENT_DIED )
                {
                    g_gameState.fHudFx -= 1.0;
                } 
                else if ( playerEnt.iEvent == EVENT_SHOTGUN)
                {
                    g_gameState.iMessage = MESSAGE_SHOTGUN;
                    g_gameState.fMessageTimer = 3.0;
                }

                if ( playerEnt.fTookDamage > 0.0 )
                {
                    g_gameState.fHudFx -= playerEnt.fTookDamage / 8.0;
                }
                
                if ( g_gameState.fHudFx > 0.0 )
                {
                    g_gameState.fHudFx = max( 0.0, g_gameState.fHudFx - fTimestep );
                }        
                else
                if ( g_gameState.fHudFx < 0.0 )
                {
                    g_gameState.fHudFx = min( 0.0, g_gameState.fHudFx + fTimestep );            
                }     
                g_gameState.fHudFx = clamp( g_gameState.fHudFx, -1.0, 1.0 );                
                
                // Update message timer;
                g_gameState.fMessageTimer = max( 0.0, g_gameState.fMessageTimer - fTimestep );
            }
            
        }

        if ( iMainGameState == MAIN_GAME_STATE_INIT_LEVEL )
        {
			g_gameState.fGameTime = 0.0;
			g_gameState.iMainState = MAIN_GAME_STATE_GAME_RUNNING;
            g_gameState.fStateTimer = 0.0;
        }
        
        g_gameState.vPrevMouse = iMouse;
                
        GameState_Store( g_gameState, fragColor, fragCoord );
    }        
}
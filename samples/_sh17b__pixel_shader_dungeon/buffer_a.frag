// gameplay loop

//#define EASY_MODE
//#define KEY_AUTOREPEAT

const float TICK_NUM			= 10.;
const float TILE_FLOOR			= 1.;
const float TILE_DOOR_OPEN		= 2.;
const float TILE_STAIRS_DOWN	= 3.;
const float TILE_TRAP			= 4.;
const float TILE_TRAP_OFF		= 5.;
const float TILE_WALL			= 6.;
const float TILE_WALL_HOLE		= 7.;
const float TILE_DOOR_LOCKED	= 8.;
const float TILE_STAIRS_UP		= 9.;
const float ITEM_KEY			= 1.;
const float ITEM_FOOD			= 2.;
const float ITEM_POTION			= 3.;
const float ITEM_SPAWNER		= 4.;
const float ITEM_SPAWNER_2		= 5.;
const float STATE_START			= 0.;
const float STATE_GAME			= 1.;
const float STATE_NEXT_LEVEL	= 2.;
const float STATE_GAME_OVER		= 3.;

const float KEY_LEFT  			= 37.5f / 256.0f;
const float KEY_UP    			= 38.5f / 256.0f;
const float KEY_RIGHT 			= 39.5f / 256.0f;
const float KEY_DOWN  			= 40.5f / 256.0f;

#define ENEMY_NUM 3
#define LOG_NUM 4

struct GameState
{
    // 0   
    float	tick;
    float 	hp;
    float 	level;
    float 	xp;
    float 	keyNum;
    
	// 1
    vec2 	playerPos;
    float   playerFrame;
    float   playerDir;
    vec2	bodyPos;
    float   bodyId;
    
    // 2
    float 	state;
    float   keyLock;
    float 	stateTime;
    vec2	despawnPos;
    float   despawnId;

    // 3
    vec2	enemyPos[ ENEMY_NUM ];
    float 	enemyFrame[ ENEMY_NUM ];
    float 	enemyDir[ ENEMY_NUM ];
    float 	enemyHP[ ENEMY_NUM ];
    float 	enemyId[ ENEMY_NUM ];
    vec2    enemySpawnPos[ ENEMY_NUM ];
    
    // 4
    vec2	logPos[ LOG_NUM ];
    float   logLife[ LOG_NUM ];
    float   logId[ LOG_NUM ];
    float   logVal[ LOG_NUM ];
};

vec4 LoadValue( int x, int y )
{
    return texelFetch( iChannel0, ivec2( x, y ), 0 );
}    

float PackXY( float a, float b )
{
    return floor( a ) + floor( b ) / 256.;
}

float PackXY( vec2 v )
{
    return PackXY( v.x, v.y );
}

float UnpackX( float a )
{
    return floor( a );
}

float UnpackY( float a )
{
    return fract( a ) * 256.;
}

vec2 UnpackXY( float a )
{
    return vec2( UnpackX( a ), UnpackY( a ) );
}

void LoadState( out GameState s )
{
    vec4 data;

    data = LoadValue( 0, 0 );
    s.tick 		= data.x;
    s.hp    	= UnpackX( data.y );
    s.level    	= UnpackY( data.y );
    s.xp        = data.z;
    s.keyNum    = data.w;
    
    data = LoadValue( 1, 0 );
    s.playerPos   = UnpackXY( data.x );
    s.playerFrame = UnpackX( data.y );
    s.playerDir   = UnpackY( data.y );
    s.bodyPos	  = UnpackXY( data.z );
    s.bodyId      = data.w;
    
    data = LoadValue( 2, 0 );
    s.state      = UnpackX( data.x );
    s.keyLock    = UnpackY( data.x );
    s.stateTime  = data.y;
    s.despawnPos = UnpackXY( data.z );
    s.despawnId  = data.w;

    for ( int i = 0; i < ENEMY_NUM; ++i )
    {
        data = LoadValue( 3, i );
        s.enemyPos[ i ]      = UnpackXY( data.x );
        s.enemyFrame[ i ]    = UnpackX( data.y );
        s.enemyDir[ i ]      = UnpackY( data.y );
        s.enemyHP[ i ]       = UnpackX( data.z );
        s.enemyId[ i ]       = UnpackY( data.z );
        s.enemySpawnPos[ i ] = UnpackXY( data.w );
    }
    
    for ( int i = 0; i < LOG_NUM; ++i )
    {
		data = LoadValue( 4, i );
    	s.logPos[ i ]  = data.xy;
        s.logLife[ i ] = data.z;
        s.logId[ i ]   = UnpackX( data.w );
        s.logVal[ i ]  = UnpackY( data.w );
    }    
}

void StoreValue( vec2 re, vec4 va, inout vec4 fragColor, vec2 fragCoord )
{
    fragCoord = floor( fragCoord );
    fragColor = ( fragCoord.x == re.x && fragCoord.y == re.y ) ? va : fragColor;
}

vec4 SaveState( in GameState s, in vec2 fragCoord, bool reset )
{
    vec4 ret = vec4( 0. );
    StoreValue( vec2( 0., 0. ), vec4( s.tick, PackXY( s.hp, s.level ), s.xp, s.keyNum ), ret, fragCoord );
    StoreValue( vec2( 1., 0. ), vec4( PackXY( s.playerPos ), PackXY( s.playerFrame, s.playerDir ), PackXY( s.bodyPos ), s.bodyId ), ret, fragCoord );
    StoreValue( vec2( 2., 0. ), vec4( PackXY( s.state, s.keyLock ), s.stateTime, PackXY( s.despawnPos ), s.despawnId ), ret, fragCoord );

    for ( int i = 0; i < ENEMY_NUM; ++i )
    {
        StoreValue( vec2( 3., float( i ) ), 
                   vec4( PackXY( s.enemyPos[ i ] ), 
                         PackXY( s.enemyFrame[ i ], s.enemyDir[ i ] ), 
                         PackXY( s.enemyHP[ i ], s.enemyId[ i ] ),
                         PackXY( s.enemySpawnPos[ i ] ) ), ret, fragCoord );
    }
    
    for ( int i = 0; i < LOG_NUM; ++i )
    {
        StoreValue( vec2( 4., float( i ) ), vec4( s.logPos[ i ], s.logLife[ i ], PackXY( s.logId[ i ], s.logVal[ i ] ) ), ret, fragCoord );
    }

	if ( reset )    
    {
        ret = vec4( 0. );
		StoreValue( vec2( 0., 0. ), vec4( 0., 21., 0., 0. ), ret, fragCoord );        
        StoreValue( vec2( 1., 0. ), vec4( PackXY( 3., 2. ), 0., 0., 0. ), ret, fragCoord );
        StoreValue( vec2( 2., 0. ), vec4( s.state, 0., 0., 0. ), ret, fragCoord );
    }
    
    return ret;
}

void LogDmg( inout GameState s, vec2 pos, float val )
{
    float maxLife = -1.;
    if ( s.logPos[ 2 ].x > 0. && s.logPos[ 3 ].x > 0. )
    {
        maxLife = max( s.logLife[ 2 ], s.logLife[ 3 ] );
    }
    
    for ( int i = 2; i < LOG_NUM; ++i )
    {
        if ( s.logPos[ i ].x <= 0. || maxLife == s.logLife[ i ] )
        {
            s.logPos[ i ]  = pos;
            s.logLife[ i ] = 0.;
            s.logId[ i ]   = 0.;
            s.logVal[ i ]  = val;            
            break;
        }
    }   
}

void LogXP( inout GameState s, vec2 pos, float val )
{
    s.logPos[ 0 ]  = pos;
    s.logLife[ 0 ] = 0.;
    s.logId[ 0 ]   = 0.;
	s.logVal[ 0 ]  = val;
}

void LogHeal( inout GameState s, vec2 pos, float val )
{
    s.logPos[ 1 ]  = pos;
    s.logLife[ 1 ] = 0.;
    s.logId[ 1 ]   = 0.;
	s.logVal[ 1 ]  = val;
}

void LogLevelUp( inout GameState s, vec2 pos )
{
    s.logPos[ 0 ]  = pos;
    s.logLife[ 0 ] = 0.;
    s.logId[ 0 ]   = 1.;
}

float Rand( vec2 n )
{
	return fract( sin( dot( n.xy, vec2( 12.9898, 78.233 ) ) ) * 43758.5453 );
}

float MaxXP( float level )
{
	return 10. + level * 5.;	   
}

float MaxHP( float level )
{
	return 21. + level * 3.;	   
}

float EnemyDmg( float id )
{    
    float dmg = 1. + 2. * id + floor( ( 4. + 2. * id ) * Rand( vec2( iTime, iTime ) ) );
#ifdef EASY_MODE
    dmg = floor( dmg * .5 );
#endif
    return dmg;
}

float EnemyHP( float id )
{
    return 8. + id * 15.;
}

float EnemyXP( float id )
{
    return 3. + id * 5.;
}

float TrapDmg( bool player )
{
    float dmg = 10. + floor( 8. * Rand( vec2( iTime, iTime ) ) );
#ifdef EASY_MODE
    if ( player )
    	dmg = floor( dmg * .5 );
#endif    
    return dmg;
}

float PlayerDmg( inout GameState s )
{
    return s.level + 1. + floor( 4. * Rand( vec2( iTime + 11.1, iTime + 11.1 ) ) );
}

void UpdateEnemies( inout GameState s )
{
    for ( int i = 0; i < ENEMY_NUM; ++i )
    {
    	if ( s.tick == 0. )
    	{
            s.enemyFrame[ i ] = 0.;
        }
        
       	if ( s.enemyPos[ i ].x > 0. && s.enemyHP[ i ] <= 0. )
        {
            float xp = EnemyXP( s.enemyId[ i ] );
            s.xp += xp;
            s.bodyPos = s.enemyPos[ i ];
            s.bodyId  = s.enemyId[ i ] + 1.;
            s.enemyPos[ i ] = vec2( 0., 0. );
            LogXP( s, s.playerPos, xp );
        }        

        vec4 map = texelFetch( iChannel1, ivec2( s.enemyPos[ i ] ), 0 );
        if ( map.x == TILE_TRAP && s.tick == 1. )
        {
            float dmg = TrapDmg( false );
            s.enemyHP[ i ] -= dmg;
            LogDmg( s, s.enemyPos[ i ], dmg );
        }        
        
        vec2 move = vec2( 0., 0. );
    	vec2 toPlayer = s.playerPos - s.enemyPos[ i ];
        vec2 moveX = vec2( sign( toPlayer.x ), 0. );
        vec2 moveY = vec2( 0., sign( toPlayer.y ) );
        vec4 mapX  = texelFetch( iChannel1, ivec2( s.enemyPos[ i ] + moveX ), 0 );
        vec4 mapY  = texelFetch( iChannel1, ivec2( s.enemyPos[ i ] + moveY ), 0 );
        
        if ( mapX.x >= TILE_WALL )
        {
            moveX = vec2( 0. );
        }

        if ( mapY.x >= TILE_WALL )
        {
            moveY = vec2( 0. );
        }             
        
        for ( int j = 0; j < ENEMY_NUM; ++j )
        {
            if ( j != i && s.enemyPos[ j ] == s.enemyPos[ i ] + moveX )
            {
                moveX = vec2( 0. );
            }
            if ( j != i && s.enemyPos[ j ] == s.enemyPos[ i ] + moveY )
            {
                moveY = vec2( 0. );
            }
        }

        move = moveX.x != 0. ? moveX : moveY;
        if ( moveX.x != 0. && moveY.y != 0. )
        {
            move = abs( toPlayer.x ) > abs( toPlayer.y ) ? moveX : moveY;
        }
        
        if ( s.tick == TICK_NUM )
        {
        	if ( s.enemyPos[ i ] + move == s.playerPos )
            {
                float dmg = EnemyDmg( s.enemyId[ i ] );
                s.hp -= dmg;
                s.enemyFrame[ i ] = 5.;
                LogDmg( s, s.playerPos, dmg );
            }
			else
            {
				s.enemyPos[ i ] += move;
                s.enemyFrame[ i ] = move.x == 1. ? 1. : ( move.x == -1. ? 2. : ( move.y == 1. ? 3. : ( move.y == -1. ? 4. : 0. ) ) );
                s.enemyDir[ i ] = move.x > 0. ? 0. : ( move.x < 0. ? 1. : s.enemyDir[ i ] );
            }
        }
    }    
}

void CheckSpawnPos( inout vec2 spawnPos, inout float spawnId, vec2 tile )
{
	vec4 map = texelFetch( iChannel1, ivec2( tile ), 0 );
    if ( map.y == ITEM_SPAWNER || map.y == ITEM_SPAWNER_2 )
    {
        spawnPos = tile;
        spawnId  = map.y == ITEM_SPAWNER ? 0. : 1.;
    } 
}

void SpawnEnemies( inout GameState s, vec2 playerMove )
{    
    // despawn out of range
    s.despawnPos = vec2( 0. );
    for ( int i = 0; i < ENEMY_NUM; ++i )
    {
        if ( s.tick == 1. && s.enemyPos[ i ].x > 0. && length( s.playerPos - s.enemyPos[ i ] ) > 5. )
        {
            s.despawnPos    = s.enemySpawnPos[ i ];
            s.despawnId     = s.enemyId[ i ];
            s.enemyPos[ i ] = vec2( 0. );
            break;
        }
    }    
    
    vec2 spawnPos = vec2( 0., 0. );
    float spawnId = 0.;
    CheckSpawnPos( spawnPos, spawnId, s.playerPos + playerMove * 4. );
    CheckSpawnPos( spawnPos, spawnId, s.playerPos + playerMove * 3. - playerMove.yx * 1. );
    CheckSpawnPos( spawnPos, spawnId, s.playerPos + playerMove * 3. + playerMove.yx * 1. );
    CheckSpawnPos( spawnPos, spawnId, s.playerPos + playerMove * 2. - playerMove.yx * 2. );
    CheckSpawnPos( spawnPos, spawnId, s.playerPos + playerMove * 2. + playerMove.yx * 2. );
    CheckSpawnPos( spawnPos, spawnId, s.playerPos + playerMove * 1. - playerMove.yx * 3. );
    CheckSpawnPos( spawnPos, spawnId, s.playerPos + playerMove * 1. + playerMove.yx * 3. );
    CheckSpawnPos( spawnPos, spawnId, s.playerPos + playerMove * 0. - playerMove.yx * 4. );
    CheckSpawnPos( spawnPos, spawnId, s.playerPos + playerMove * 0. + playerMove.yx * 4. );    
    
    if ( spawnPos.x > 0. )
    {
        for ( int i = 0; i < ENEMY_NUM; ++i )
        {        
            if ( s.enemyPos[ i ].x <= 0. )
            {
                s.enemyId[ i ]       = spawnId;
                s.enemyPos[ i ]      = spawnPos;
                s.enemyHP[ i ]       = EnemyHP( spawnId );
                s.enemySpawnPos[ i ] = spawnPos;
                break;
            }
        }    
    }
}

void UpdateLog( inout GameState s )
{
    for ( int i = 0; i < LOG_NUM; ++i )
    {
        s.logLife[ i ] += iTimeDelta;
        if ( s.logLife[ i ] > 1. )
        {
            s.logPos[ i ] = vec2( 0. );
        }
    }
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // don't compute gameplay outside of the data area
    if ( fragCoord.x >= 8. || fragCoord.y >= 8. ) 
    {
		discard;    
    }

    // keys
    bool keyLeft  	= texture( iChannel2, vec2( KEY_LEFT, .25 ) ).x > .5;
    bool keyRight 	= texture( iChannel2, vec2( KEY_RIGHT, .25 ) ).x > .5;
    bool keyUp  	= texture( iChannel2, vec2( KEY_UP, .25 ) ).x > .5;
    bool keyDown 	= texture( iChannel2, vec2( KEY_DOWN, .25 ) ).x > .5;    
    
    GameState s;
    LoadState( s );
    
    s.tick = max( floor( s.tick - 1. ), 0. );    
    
    bool reset = false;
    if ( iFrame < 1 || s.state == STATE_START )
    {
        reset   = true;
        s.state = STATE_GAME;
    }
    else if ( s.state == STATE_GAME )
    {
        if ( s.hp <= 0. )
        {
            s.state 	= STATE_GAME_OVER;
			s.stateTime = 0.;
        }
    }
    else if ( s.state == STATE_NEXT_LEVEL )
    {
        s.state = STATE_GAME;
        s.playerPos = vec2( 3., 2. );
    	for ( int i = 0; i < ENEMY_NUM; ++i )
    	{           
            s.enemyPos[ i ] = vec2( 0. );
    	}
    }
    else if ( s.state == STATE_GAME_OVER )
    {
        s.stateTime += .33 * iTimeDelta;
        if ( s.stateTime >= 1. )
        {
            s.state = STATE_START;
        }
    }

    
    // level up
    if ( s.xp >= MaxXP( s.level ) )
    {
        s.xp -= MaxXP( s.level );
        s.level = min( s.level + 1., 9. );
        s.hp	= MaxHP( s.level );
        LogLevelUp( s, s.playerPos );
    }
    
    
#ifdef KEY_AUTOREPEAT    
    s.keyLock = 0.;
#else
    s.keyLock = keyLeft || keyRight || keyDown || keyUp ? s.keyLock : 0.;    
#endif

    vec2 move = vec2( 0., 0. );
    if ( s.state == STATE_GAME && s.tick == 0. && s.keyLock == 0. )
    {
        s.playerFrame = 0.;
        if ( keyLeft )
        {
            move.x = -1.;
            s.playerDir = 1.;
        }   
        else if ( keyRight )
        {
            move.x = 1.;
            s.playerDir = 0.;
        }   
        else if ( keyDown )
        {
            move.y = -1.;
        }
        else if ( keyUp )
        {
            move.y = 1.;
        }             
    }
  
    vec4 map = texelFetch( iChannel1, ivec2( s.playerPos + move ), 0 );
    if ( abs( move.x + move.y ) > 0. && map.x < TILE_WALL )
    {
        bool enemy = false;
        for ( int i = 0; i < ENEMY_NUM; ++i )
        {
            if ( s.enemyPos[ i ] == s.playerPos + move )        
            {
                float dmg = PlayerDmg( s );
                s.playerFrame = 6.;
                s.enemyHP[ i ] -= dmg;
                LogDmg( s, s.enemyPos[ i ], dmg );
            }
        }
        
        if ( s.playerFrame != 6. )
        {
    		s.playerPos += move;
            s.playerFrame = keyLeft ? 1. : ( keyRight ? 2. : ( keyDown ? 3. : 4. ) );
        
        	if ( map.y == ITEM_KEY )
        	{
            	s.keyNum += 1.;
        	}        
            
        	if ( map.y == ITEM_FOOD || map.y == ITEM_POTION )
        	{
                float heal = map.y == ITEM_FOOD ? 5. : 50.;
                heal = min( heal, MaxHP( s.level ) - s.hp );
            	s.hp += heal;
                LogHeal( s, s.playerPos, heal );
        	}      
            
            if ( map.x == TILE_TRAP )
            {
                float dmg = TrapDmg( true );
                s.hp -= dmg;
                LogDmg( s, s.playerPos, dmg );
            }  
        }
        
        s.tick    = TICK_NUM;
        s.keyLock = 1.;
    }
    else if ( map.x == TILE_DOOR_LOCKED && s.keyNum > 0. )
    {
        s.playerFrame = keyLeft ? 1. : ( keyRight ? 2. : ( keyDown ? 3. : 4. ) );
        s.keyNum -= 1.;
        s.playerPos += move;
        s.tick    = TICK_NUM;
        s.keyLock = 1.;
    }
    else if ( map.x >= TILE_WALL )
    {
     	// wait
        s.playerFrame = 5.;
        s.tick        = TICK_NUM;
        s.keyLock     = 1.;
    }    

    UpdateEnemies( s );
    SpawnEnemies( s, move ); 
	UpdateLog( s );
    
	// next level
    if ( map.x == TILE_STAIRS_DOWN )
    {     
        s.state = STATE_NEXT_LEVEL;
    }
    
    fragColor = SaveState( s, fragCoord, reset );
}
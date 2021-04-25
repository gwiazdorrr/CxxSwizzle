// items and enemies

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
const float ITEM_POTION			= 2.;
const float ITEM_SPAWNER		= 3.;
const float LOG_ID_DMG			= 1.;
const float LOG_ID_XP			= 2.;
const float LOG_ID_LEVEL_UP		= 3.;
const float STATE_START			= 0.;
const float STATE_GAME			= 1.;
const float STATE_NEXT_LEVEL	= 2.;
const float STATE_GAME_OVER		= 3.;

const vec2  REF_RES	            = vec2( 200. );

CXX_CONST int   ENEMY_NUM			= 3;
CXX_CONST int   LOG_NUM				= 4;

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

float saturate( float x )
{
    return clamp( x, 0., 1. );
}

float Smooth( float x )
{
	return smoothstep( 0., 1., saturate( x ) );   
}

void SpriteRat( inout vec3 color, vec2 p )
{
	p -= vec2( 0., 1. );    
    
    int v = 0;
	v = p.y == 11. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 33554946 : ( p.x < 12. ? 131586 : 0 ) ) ) : v;
	v = p.y == 10. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 67241730 : ( p.x < 12. ? 33817604 : 0 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 67371520 : ( p.x < 12. ? 33948678 : 0 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 67371520 : ( p.x < 12. ? 67372036 : 2 ) ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 67372034 : ( p.x < 12. ? 67372036 : 2 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 67372034 : ( p.x < 12. ? 16843010 : 5 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 33554432 : ( p.x < 8. ? 67372036 : ( p.x < 12. ? 66052 : 0 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 33554432 : ( p.x < 8. ? 67372036 : ( p.x < 12. ? 197635 : 0 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 16777216 : ( p.x < 8. ? 33686532 : ( p.x < 12. ? 33817604 : 0 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 16777216 : ( p.x < 8. ? 67372034 : ( p.x < 12. ? 33817602 : 0 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 16777216 : ( p.x < 8. ? 67371522 : ( p.x < 12. ? 16909313 : 0 ) ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 117901063 : ( p.x < 8. ? 50397441 : ( p.x < 12. ? 83951877 : 0 ) ) ) : v;
    float i = float( ( v >> int( 8.0 * p.x ) ) & 255 );
    color = i == 1. ? vec3( 0.16, 0.11, 0.071 ) : color;
    color = i == 2. ? vec3( 0.33, 0.26, 0.18 ) : color;
    color = i == 3. ? vec3( 0.58, 0.25, 0.24 ) : color;
    color = i == 4. ? vec3( 0.51, 0.44, 0.35 ) : color;
    color = i == 5. ? vec3( 0.81, 0.31, 0.33 ) : color;
    color = i == 6. ? vec3( 1, 0.4, 0.3 ) : color;
    color = i == 7. ? vec3( 0.86, 0.53, 0.55 ) : color;
}

void SpriteRatAttack( inout vec3 color, vec2 p )
{
	p -= vec2( 0., 1. );    
    
    int v = 0;
	v = p.y == 10. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 33685504 : ( p.x < 12. ? 33686016 : 2 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 117571584 : ( p.x < 12. ? 67372034 : 516 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 67240448 : ( p.x < 12. ? 67503108 : 518 ) ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 67372034 : ( p.x < 12. ? 67372036 : 132100 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 67372034 : ( p.x < 12. ? 67372036 : 132100 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 33554432 : ( p.x < 8. ? 67372036 : ( p.x < 12. ? 83952644 : 393477 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 33554432 : ( p.x < 8. ? 67372036 : ( p.x < 12. ? 50397953 : 0 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 16777216 : ( p.x < 8. ? 33686532 : ( p.x < 12. ? 33817604 : 0 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 16777216 : ( p.x < 8. ? 67372034 : ( p.x < 12. ? 33817602 : 0 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 16777216 : ( p.x < 8. ? 67371522 : ( p.x < 12. ? 16909313 : 0 ) ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 117901063 : ( p.x < 8. ? 50397441 : ( p.x < 12. ? 100729094 : 0 ) ) ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = i == 1. ? vec3( 0.16, 0.11, 0.071 ) : color;
    color = i == 2. ? vec3( 0.33, 0.26, 0.18 ) : color;
    color = i == 3. ? vec3( 0.58, 0.25, 0.24 ) : color;
    color = i == 4. ? vec3( 0.51, 0.44, 0.35 ) : color;
    color = i == 5. ? vec3( 1, 0.09, 0.09 ) : color;
    color = i == 6. ? vec3( 0.9, 0.36, 0.31 ) : color;
    color = i == 7. ? vec3( 0.86, 0.53, 0.55 ) : color;
}

void SpriteRatWalk( inout vec3 color, vec2 p )
{
    p -= vec2( 0., 1. );
    
    int v = 0;
	v = p.y == 12. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 33554946 : ( p.x < 12. ? 131586 : 0 ) ) ) : v;
	v = p.y == 11. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 67241730 : ( p.x < 12. ? 33817604 : 0 ) ) ) : v;
	v = p.y == 10. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 67371520 : ( p.x < 12. ? 33948678 : 0 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 67371520 : ( p.x < 12. ? 67372036 : 2 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 67372034 : ( p.x < 12. ? 67372036 : 2 ) ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 67372034 : ( p.x < 12. ? 16843010 : 5 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 33554432 : ( p.x < 8. ? 67372036 : ( p.x < 12. ? 66052 : 0 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 33554432 : ( p.x < 8. ? 67372036 : ( p.x < 12. ? 197635 : 0 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 16777216 : ( p.x < 8. ? 33686532 : ( p.x < 12. ? 33817604 : 0 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 16777216 : ( p.x < 8. ? 67372034 : ( p.x < 12. ? 33817602 : 0 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 16777216 : ( p.x < 8. ? 67371522 : ( p.x < 12. ? 16909313 : 0 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 117440512 : ( p.x < 8. ? 16974081 : ( p.x < 12. ? 65793 : 0 ) ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 460551 : ( p.x < 8. ? 83886080 : ( p.x < 12. ? 327680 : 0 ) ) ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = i == 1. ? vec3( 0.16, 0.11, 0.071 ) : color;
    color = i == 2. ? vec3( 0.33, 0.26, 0.18 ) : color;
    color = i == 3. ? vec3( 0.58, 0.25, 0.24 ) : color;
    color = i == 4. ? vec3( 0.51, 0.44, 0.35 ) : color;
    color = i == 5. ? vec3( 0.81, 0.31, 0.33 ) : color;
    color = i == 6. ? vec3( 1, 0.4, 0.3 ) : color;
    color = i == 7. ? vec3( 0.86, 0.53, 0.55 ) : color;
}

void SpriteSkeleton( inout vec3 color, vec2 p )
{
    p -= vec2( 4., 1. );
    p = p.x < 0. ? vec2( 8. ) : p;        
    
    int v = 0;
	v = p.y == 13. ? ( p.x < 4. ? 101056512 : ( p.x < 8. ? 101058054 : 0 ) ) : v;
	v = p.y == 12. ? ( p.x < 4. ? 117900800 : ( p.x < 8. ? 117901063 : ( p.x < 12. ? 6 : 0 ) ) ) : v;
	v = p.y == 11. ? ( p.x < 4. ? 67568384 : ( p.x < 8. ? 67372036 : ( p.x < 12. ? 2 : 0 ) ) ) : v;
	v = p.y == 10. ? ( p.x < 4. ? 117900032 : ( p.x < 8. ? 17237761 : ( p.x < 12. ? 6 : 0 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 117571584 : ( p.x < 8. ? 117704455 : ( p.x < 12. ? 6 : 0 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 33554432 : ( p.x < 8. ? 50726403 : 0 ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 33554434 : 0 ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 117835264 : ( p.x < 8. ? 101123847 : 0 ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 67108870 : ( p.x < 8. ? 328965 : ( p.x < 12. ? 2 : 0 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 1539 : ( p.x < 8. ? 33554944 : ( p.x < 12. ? 2 : 0 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 101057282 : ( p.x < 8. ? 101058054 : ( p.x < 12. ? 2 : 0 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 196608 : ( p.x < 8. ? 393216 : 0 ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 1536 : ( p.x < 8. ? 393216 : 0 ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 771 : ( p.x < 8. ? 50528256 : 0 ) ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = i == 1. ? vec3( 0 ) : color;
    color = i == 2. ? vec3( 0.49 ) : color;
    color = i == 3. ? vec3( 0.64 ) : color;
    color = i == 4. ? vec3( 0.7 ) : color;
    color = i == 5. ? vec3( 0.8 ) : color;
    color = i == 6. ? vec3( 0.81 ) : color;
    color = i == 7. ? vec3( 0.9 ) : color;
}

void SpriteSkeletonWalk( inout vec3 color, vec2 p )
{
    p -= vec2( 4., 1. );
    p = p.x < 0. ? vec2( 8. ) : p;     
    
    int v = 0;
	v = p.y == 13. ? ( p.x < 4. ? 101056512 : ( p.x < 8. ? 101058054 : 0 ) ) : v;
	v = p.y == 12. ? ( p.x < 4. ? 117900800 : ( p.x < 8. ? 117901063 : ( p.x < 12. ? 6 : 0 ) ) ) : v;
	v = p.y == 11. ? ( p.x < 4. ? 67568384 : ( p.x < 8. ? 67372036 : ( p.x < 12. ? 2 : 0 ) ) ) : v;
	v = p.y == 10. ? ( p.x < 4. ? 117900032 : ( p.x < 8. ? 17237761 : ( p.x < 12. ? 6 : 0 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 117571584 : ( p.x < 8. ? 117704455 : ( p.x < 12. ? 6 : 0 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 33554432 : ( p.x < 8. ? 50726403 : 0 ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 33554434 : 0 ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 117835264 : ( p.x < 8. ? 101123847 : 0 ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 67108870 : ( p.x < 8. ? 328965 : ( p.x < 12. ? 2 : 0 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 393984 : ( p.x < 8. ? 33554944 : ( p.x < 12. ? 2 : 0 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 100860416 : ( p.x < 8. ? 101058054 : ( p.x < 12. ? 2 : 0 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 196608 : ( p.x < 8. ? 100990976 : 0 ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 1536 : ( p.x < 8. ? 393984 : 0 ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 197376 : 0 ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = i == 1. ? vec3( 0 ) : color;
    color = i == 2. ? vec3( 0.49 ) : color;
    color = i == 3. ? vec3( 0.64 ) : color;
    color = i == 4. ? vec3( 0.7 ) : color;
    color = i == 5. ? vec3( 0.8 ) : color;
    color = i == 6. ? vec3( 0.81 ) : color;
    color = i == 7. ? vec3( 0.9 ) : color;
}

void SpriteSkeletonAttack( inout vec3 color, vec2 p )
{
    p -= vec2( 4., 1. );
    p = p.x < 0. ? vec2( 10. ) : p;    
    
    int v = 0;
	v = p.y == 13. ? ( p.x < 4. ? 100663296 : ( p.x < 8. ? 101058054 : ( p.x < 12. ? 6 : 0 ) ) ) : v;
	v = p.y == 12. ? ( p.x < 4. ? 117833728 : ( p.x < 8. ? 117901063 : ( p.x < 12. ? 1543 : 0 ) ) ) : v;
	v = p.y == 11. ? ( p.x < 4. ? 117637120 : ( p.x < 8. ? 67372036 : ( p.x < 12. ? 516 : 0 ) ) ) : v;
	v = p.y == 10. ? ( p.x < 4. ? 117637120 : ( p.x < 8. ? 117899527 : ( p.x < 12. ? 1537 : 0 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 33554432 : ( p.x < 8. ? 67569415 : ( p.x < 12. ? 1543 : 0 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 101057794 : ( p.x < 12. ? 3 : 0 ) ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 512 : ( p.x < 12. ? 2 : 0 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 101058048 : ( p.x < 8. ? 100861446 : 0 ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 84213760 : ( p.x < 8. ? 50464005 : ( p.x < 12. ? 2 : 0 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 33554944 : ( p.x < 12. ? 2 : 0 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 101056512 : ( p.x < 8. ? 101058054 : 0 ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 196608 : ( p.x < 8. ? 393216 : 0 ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 1536 : ( p.x < 8. ? 393216 : 0 ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 771 : ( p.x < 8. ? 50528256 : 0 ) ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = i == 1. ? vec3( 0 ) : color;
    color = i == 2. ? vec3( 0.49 ) : color;
    color = i == 3. ? vec3( 0.64 ) : color;
    color = i == 4. ? vec3( 0.7 ) : color;
    color = i == 5. ? vec3( 0.8 ) : color;
    color = i == 6. ? vec3( 0.81 ) : color;
    color = i == 7. ? vec3( 0.9 ) : color;
}

void SpriteWarriorHead( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 7. ? ( p.x < 4. ? 0 : ( p.x < 8. ? 33686016 : ( p.x < 12. ? 2 : 0 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 33685504 : ( p.x < 8. ? 67372034 : ( p.x < 12. ? 516 : 0 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 67371520 : ( p.x < 8. ? 67372036 : ( p.x < 12. ? 132100 : 0 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 67239936 : ( p.x < 8. ? 117901062 : ( p.x < 12. ? 132103 : 0 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 84148736 : ( p.x < 8. ? 84214021 : ( p.x < 12. ? 769 : 0 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 84148992 : ( p.x < 8. ? 101057798 : ( p.x < 12. ? 1029 : 0 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 67239936 : ( p.x < 8. ? 67372038 : ( p.x < 12. ? 1028 : 0 ) ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 67239936 : ( p.x < 8. ? 33686020 : ( p.x < 12. ? 514 : 0 ) ) ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = i == 1. ? vec3( 0 ) : color;
    color = i == 2. ? vec3( 0.64, 0.2, 0.047 ) : color;
    color = i == 3. ? vec3( 0.52, 0.35, 0.22 ) : color;
    color = i == 4. ? vec3( 0.77, 0.47, 0.29 ) : color;
    color = i == 5. ? vec3( 0.72, 0.59, 0.47 ) : color;
    color = i == 6. ? vec3( 0.86, 0.71, 0.59 ) : color;
    color = i == 7. ? vec3( 1, 0.85, 0.75 ) : color;
}

void SpriteWarriorStand( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 6. ? ( p.x < 4. ? 101057536 : ( p.x < 8. ? 33620485 : ( p.x < 12. ? 770 : 0 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 16908802 : ( p.x < 8. ? 117901061 : ( p.x < 12. ? 261 : 0 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 17040897 : ( p.x < 8. ? 33686018 : ( p.x < 12. ? 258 : 0 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 33620736 : ( p.x < 8. ? 117901061 : ( p.x < 12. ? 770 : 0 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 33619968 : ( p.x < 8. ? 33685761 : ( p.x < 12. ? 1 : 0 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 16908544 : ( p.x < 8. ? 33619968 : ( p.x < 12. ? 1 : 0 ) ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 16843008 : ( p.x < 8. ? 16842752 : ( p.x < 12. ? 1 : 0 ) ) ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = i == 1. ? vec3( 0.09, 0.11, 0.18 ) : color;
    color = i == 2. ? vec3( 0.28, 0.35, 0.45 ) : color;
    color = i == 3. ? vec3( 1, 0.36, 0.043 ) : color;
    color = i == 4. ? vec3( 1, 0.65, 0.25 ) : color;
    color = i == 5. ? vec3( 0.54, 0.67, 0.73 ) : color;
    color = i == 6. ? vec3( 1, 0.85, 0.56 ) : color;
    color = i == 7. ? vec3( 0.91, 1, 1 ) : color;
}

void SpriteWarriorWalk( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 6. ? ( p.x < 4. ? 101057536 : ( p.x < 8. ? 33620485 : ( p.x < 12. ? 770 : 0 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 33620481 : ( p.x < 8. ? 117901061 : ( p.x < 12. ? 261 : 0 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 67371521 : ( p.x < 8. ? 33686018 : ( p.x < 12. ? 257 : 0 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 33751296 : ( p.x < 8. ? 33687301 : ( p.x < 12. ? 1797 : 0 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 33619968 : ( p.x < 8. ? 33620225 : ( p.x < 12. ? 1 : 0 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 16842752 : ( p.x < 8. ? 16842752 : 0 ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 16842752 : 0 ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = i == 1. ? vec3( 0.09, 0.11, 0.18 ) : color;
    color = i == 2. ? vec3( 0.29, 0.34, 0.45 ) : color;
    color = i == 3. ? vec3( 1, 0.36, 0.043 ) : color;
    color = i == 4. ? vec3( 1, 0.62, 0.23 ) : color;
    color = i == 5. ? vec3( 0.54, 0.67, 0.73 ) : color;
    color = i == 6. ? vec3( 1, 0.91, 0.62 ) : color;
    color = i == 7. ? vec3( 0.87, 1, 1 ) : color;
}

void SpriteWarriorAttack( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 6. ? ( p.x < 4. ? 101057536 : ( p.x < 8. ? 16910085 : ( p.x < 12. ? 1026 : 0 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 33687298 : ( p.x < 8. ? 117901061 : ( p.x < 12. ? 67503367 : 0 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 16844292 : ( p.x < 8. ? 33686018 : ( p.x < 12. ? 50528770 : 0 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 16777987 : ( p.x < 8. ? 117901058 : ( p.x < 12. ? 261 : 0 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 16777216 : ( p.x < 8. ? 33620226 : ( p.x < 12. ? 263 : 0 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 33619968 : ( p.x < 8. ? 16777217 : ( p.x < 12. ? 258 : 0 ) ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 16842752 : ( p.x < 8. ? 16777217 : ( p.x < 12. ? 257 : 0 ) ) ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = i == 1. ? vec3( 0.09, 0.11, 0.18 ) : color;
    color = i == 2. ? vec3( 0.28, 0.35, 0.45 ) : color;
    color = i == 3. ? vec3( 1, 0.36, 0.043 ) : color;
    color = i == 4. ? vec3( 1, 0.65, 0.25 ) : color;
    color = i == 5. ? vec3( 0.54, 0.67, 0.73 ) : color;
    color = i == 6. ? vec3( 1, 0.85, 0.56 ) : color;
    color = i == 7. ? vec3( 0.91, 1, 1 ) : color;
}

vec2 FrameOffset( float frame, float tick )
{
    vec2 ret = vec2( 0. );
    ret.x = frame == 1. ? 1. : ( frame == 2. ? -1. : 0. );
    ret.y = frame == 3. ? 1. : ( frame == 4. ? -1. : 0. );
    return floor( 16. * ret * ( tick / TICK_NUM ) );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{    
    GameState s;
    LoadState( s );    
    
    vec2 playerPos = s.playerPos * 16. + FrameOffset( s.playerFrame, s.tick ); 
    
    vec2 resMult	= floor( iResolution.xy / REF_RES );
    float resRcp    = 1. / max( min( resMult.x, resMult.y ), 1. );
    vec2 screenSize = floor( iResolution.xy * resRcp );
    vec2 pixel      = floor( fragCoord.xy * resRcp );
	vec2 camera     = clamp( playerPos - floor( screenSize / 2. ), vec2( -32. ), vec2( 32. * 16. + 32. ) - screenSize );
    vec2 world      = pixel + camera;
    vec2 tile		= floor( world / 16. );
    vec2 worldMod16 = floor( mod( world, 16. ) );
    vec4 map		= texelFetch( iChannel1, ivec2( tile ), 0 );
    float tick2     = s.tick > TICK_NUM / 2. ? 1. : 0.;
    
    vec3 color = texelFetch( iChannel2, ivec2( fragCoord ), 0 ).xyz;
    
    vec2 warrior = world - playerPos;
    warrior.x = s.playerDir > 0. ? 13. - warrior.x : warrior.x - 2.;
    warrior.y -= 1.;
    if ( warrior.x >= 0. && warrior.y >= 0. && warrior.x < 16. && warrior.y < 16. 
         && s.hp > 0. )
    {
        float walk = s.playerFrame > 0. && tick2 == 1. ? 1. : 0.;
        if ( warrior.x >= 1. - walk )
        {
        	SpriteWarriorHead( color, warrior - vec2( 1. - walk, 7. ) );   
        }

        if ( s.playerFrame == 6. && tick2 == 1. )
        {
            SpriteWarriorAttack( color, warrior );
        }        
        else if ( walk == 1. )
        {
			SpriteWarriorWalk( color, warrior );
        }
        else if ( warrior.x >= 1. )
        {
			SpriteWarriorStand( color, warrior - vec2( 1., 0. ) );
        }
    }
    
    for ( int i = 0; i < ENEMY_NUM; ++i )
    {
    	vec3 enemyColor = color;
        
        vec2 enemy = world - s.enemyPos[ i ] * 16. + FrameOffset( s.enemyFrame[ i ], s.tick );
		enemy.x = s.enemyDir[ i ] > 0. ? 15. - enemy.x : enemy.x;
        if ( enemy.x >= 0. && enemy.y >= 0. && enemy.x < 16. && enemy.y < 16. && s.enemyPos[ i ].x > 0. )
    	{
            if ( s.enemyFrame[ i ] == 5. && tick2 == 1. )
            {
                if ( s.enemyId[ i ] == 0. )
                	SpriteRatAttack( enemyColor, enemy );
                else
                    SpriteSkeletonAttack( enemyColor, enemy );
            }
            else if ( s.enemyFrame[ i ] > 0. && tick2 == 1. )
            {
                if ( s.enemyId[ i ] == 0. )
                	SpriteRatWalk( enemyColor, enemy );
                else
                	SpriteSkeletonWalk( enemyColor, enemy );
            }
            else
            {
                if ( s.enemyId[ i ] == 0. )
        			SpriteRat( enemyColor, enemy );
                else
                    SpriteSkeleton( enemyColor, enemy );
			}
        }
        
        // fog of war
        float dist = length( playerPos - s.enemyPos[ i ] * 16. );
		color = mix( color, enemyColor, saturate( ( 64. + 16. - dist ) / 32. ) );
    }
    
	fragColor = vec4( color, 1. );
}
// background

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
const float ITEM_SPAWNER		= 3.;
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

void SpriteEarth( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 15. ? ( p.x < 8. ? 17961233 : ( p.x < 16. ? 536941073 : 0 ) ) : v;
	v = p.y == 14. ? ( p.x < 8. ? 18944274 : ( p.x < 16. ? 269549841 : 0 ) ) : v;
	v = p.y == 13. ? ( p.x < 8. ? 269553937 : ( p.x < 16. ? 554766353 : 0 ) ) : v;
	v = p.y == 12. ? ( p.x < 8. ? 1118481 : ( p.x < 16. ? 572662032 : 0 ) ) : v;
	v = p.y == 11. ? ( p.x < 8. ? 554766609 : ( p.x < 16. ? 555815424 : 0 ) ) : v;
	v = p.y == 10. ? ( p.x < 8. ? 572662049 : ( p.x < 16. ? 286261777 : 0 ) ) : v;
	v = p.y == 9. ? ( p.x < 8. ? 16781602 : ( p.x < 16. ? 268440097 : 0 ) ) : v;
	v = p.y == 8. ? ( p.x < 8. ? 4385 : ( p.x < 16. ? 536875298 : 0 ) ) : v;
	v = p.y == 7. ? ( p.x < 8. ? 303108641 : ( p.x < 16. ? 286265617 : 0 ) ) : v;
	v = p.y == 6. ? ( p.x < 8. ? 35790865 : ( p.x < 16. ? 304152593 : 0 ) ) : v;
	v = p.y == 5. ? ( p.x < 8. ? 16843025 : ( p.x < 16. ? 286327056 : 0 ) ) : v;
	v = p.y == 4. ? ( p.x < 8. ? 16781841 : ( p.x < 16. ? 286261521 : 0 ) ) : v;
	v = p.y == 3. ? ( p.x < 8. ? 285352193 : ( p.x < 16. ? 287310081 : 0 ) ) : v;
	v = p.y == 2. ? ( p.x < 8. ? 70162 : ( p.x < 16. ? 269484049 : 0 ) ) : v;
	v = p.y == 1. ? ( p.x < 8. ? 4625 : ( p.x < 16. ? 269488417 : 0 ) ) : v;
	v = p.y == 0. ? ( p.x < 8. ? 529 : ( p.x < 16. ? 268505361 : 0 ) ) : v;
    float i = float( ( v >> int( 4. * p.x ) ) & 15 );
    color = vec3( 0.16, 0.14, 0.14 );
    color = i == 1. ? vec3( 0.17, 0.16, 0.15 ) : color;
    color = i == 2. ? vec3( 0.19, 0.17, 0.16 ) : color;
}

void SpriteEarth2( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 13. ? 0 : v;
	v = p.y == 12. ? ( p.x < 8. ? 0 : ( p.x < 16. ? 8960 : 0 ) ) : v;
	v = p.y == 11. ? ( p.x < 8. ? 0 : ( p.x < 16. ? 69920 : 0 ) ) : v;
	v = p.y == 10. ? ( p.x < 8. ? 16 : 0 ) : v;
	v = p.y == 9. ? 0 : v;
	v = p.y == 8. ? ( p.x < 8. ? 3342336 : 0 ) : v;
	v = p.y == 7. ? ( p.x < 8. ? 35794944 : ( p.x < 16. ? 65536 : 0 ) ) : v;
	v = p.y == 6. ? ( p.x < 8. ? 17899520 : 0 ) : v;
	v = p.y == 5. ? 0 : v;
	v = p.y == 4. ? 0 : v;
	v = p.y == 3. ? ( p.x < 8. ? 0 : ( p.x < 16. ? 1 : 0 ) ) : v;
	v = p.y == 2. ? 0 : v;
	v = p.y == 1. ? ( p.x < 8. ? 8192 : ( p.x < 16. ? 12288 : 0 ) ) : v;
	v = p.y == 0. ? ( p.x < 8. ? 0 : ( p.x < 16. ? 70144 : 0 ) ) : v;
    float i = float( ( v >> int( 4. * p.x ) ) & 15 );
    color = i == 1. ? vec3( 0.42, 0.4, 0.38 ) : color;
    color = i == 2. ? vec3( 0.55, 0.53, 0.51 ) : color;
    color = i == 3. ? vec3( 0.72, 0.69, 0.65 ) : color;
}

void SpriteWater( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 15. ? ( p.x < 8. ? 286404882 : ( p.x < 16. ? 842142226 : 0 ) ) : v;
	v = p.y == 14. ? ( p.x < 8. ? 286261795 : ( p.x < 16. ? 286405169 : 0 ) ) : v;
	v = p.y == 13. ? ( p.x < 8. ? 269553970 : ( p.x < 16. ? 286339873 : 0 ) ) : v;
	v = p.y == 12. ? ( p.x < 8. ? 17965330 : ( p.x < 16. ? 554840865 : 0 ) ) : v;
	v = p.y == 11. ? ( p.x < 8. ? 1179665 : ( p.x < 16. ? 322122513 : 0 ) ) : v;
	v = p.y == 10. ? ( p.x < 8. ? 19988481 : ( p.x < 16. ? 858923265 : 0 ) ) : v;
	v = p.y == 9. ? ( p.x < 8. ? 303108385 : ( p.x < 16. ? 303116817 : 0 ) ) : v;
	v = p.y == 8. ? ( p.x < 8. ? 273 : ( p.x < 16. ? 287449616 : 0 ) ) : v;
	v = p.y == 7. ? ( p.x < 8. ? 17826065 : ( p.x < 16. ? 303112464 : 0 ) ) : v;
	v = p.y == 6. ? ( p.x < 8. ? 285212945 : ( p.x < 16. ? 285212945 : 0 ) ) : v;
	v = p.y == 5. ? ( p.x < 8. ? 16777233 : ( p.x < 16. ? 553648400 : 0 ) ) : v;
	v = p.y == 4. ? ( p.x < 8. ? 17825793 : ( p.x < 16. ? 287375360 : 0 ) ) : v;
	v = p.y == 3. ? ( p.x < 8. ? 268435457 : ( p.x < 16. ? 571613184 : 0 ) ) : v;
	v = p.y == 2. ? ( p.x < 8. ? 268439826 : ( p.x < 16. ? 822153489 : 0 ) ) : v;
	v = p.y == 1. ? ( p.x < 8. ? 268439827 : ( p.x < 16. ? 805380369 : 0 ) ) : v;
	v = p.y == 0. ? ( p.x < 8. ? 286339345 : ( p.x < 16. ? 841158944 : 0 ) ) : v;
    float i = float( ( v >> int( 4. * p.x ) ) & 15 );
    color = vec3( 0.21, 0.38, 0.29 );
    color = i == 1. ? vec3( 0.24, 0.41, 0.33 ) : color;
    color = i == 2. ? vec3( 0.27, 0.44, 0.36 ) : color;
    color = i == 3. ? vec3( 0.3, 0.47, 0.37 ) : color;
}

void SpriteFloor( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 15. ? ( p.x < 4. ? 33555717 : ( p.x < 8. ? 84149251 : ( p.x < 12. ? 67371267 : 67437569 ) ) ) : v;
	v = p.y == 14. ? ( p.x < 4. ? 16844037 : ( p.x < 8. ? 16777216 : ( p.x < 12. ? 65793 : 50594817 ) ) ) : v;
	v = p.y == 13. ? ( p.x < 4. ? 33620997 : ( p.x < 8. ? 197893 : ( p.x < 12. ? 33752323 : 50660608 ) ) ) : v;
	v = p.y == 12. ? ( p.x < 4. ? 67109893 : ( p.x < 8. ? 328709 : ( p.x < 12. ? 67306756 : 84149504 ) ) ) : v;
	v = p.y == 11. ? ( p.x < 4. ? 83886851 : ( p.x < 8. ? 263428 : ( p.x < 12. ? 50660099 : 84214272 ) ) ) : v;
	v = p.y == 10. ? ( p.x < 4. ? 83886337 : ( p.x < 8. ? 17040644 : ( p.x < 12. ? 65537 : 16843009 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 50397957 : ( p.x < 8. ? 328965 : ( p.x < 12. ? 84215042 : 67371266 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 83952645 : ( p.x < 8. ? 17106181 : ( p.x < 12. ? 84149508 : 67436547 ) ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 33620485 : ( p.x < 8. ? 16909317 : ( p.x < 12. ? 84214788 : 67240197 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 16842752 : ( p.x < 8. ? 257 : ( p.x < 12. ? 84149508 : 65540 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 50594305 : ( p.x < 8. ? 132101 : ( p.x < 12. ? 67372293 : 67371269 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 67437825 : ( p.x < 8. ? 17040645 : ( p.x < 12. ? 50595075 : 67371010 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 67371777 : ( p.x < 8. ? 16974852 : ( p.x < 12. ? 65792 : 84213760 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 16777216 : ( p.x < 8. ? 16843009 : ( p.x < 12. ? 50528256 : 65537 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 33620485 : ( p.x < 8. ? 67437828 : ( p.x < 12. ? 84148226 : 67437312 ) ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 83952645 : ( p.x < 8. ? 84214532 : ( p.x < 12. ? 84148484 : 84215040 ) ) ) : v;
    float i = float( ( v >> int( 8.0 * p.x ) ) & 255 );
    color = vec3( 0.21, 0.2, 0.19 );
    color = i == 1. ? vec3( 0.23, 0.23, 0.21 ) : color;
    color = i == 2. ? vec3( 0.27, 0.27, 0.25 ) : color;
    color = i == 3. ? vec3( 0.3, 0.29, 0.28 ) : color;
    color = i == 4. ? vec3( 0.31, 0.31, 0.29 ) : color;
    color = i == 5. ? vec3( 0.33, 0.32, 0.31 ) : color;
}

void SpriteWood( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 15. ? ( p.x < 8. ? 17830178 : ( p.x < 16. ? 286401058 : 0 ) ) : v;
	v = p.y == 14. ? 0 : v;
	v = p.y == 13. ? ( p.x < 8. ? 303182643 : ( p.x < 16. ? 806424850 : 0 ) ) : v;
	v = p.y == 12. ? 0 : v;
	v = p.y == 11. ? ( p.x < 8. ? 807481906 : ( p.x < 16. ? 858993459 : 0 ) ) : v;
	v = p.y == 10. ? ( p.x < 8. ? 537989394 : ( p.x < 16. ? 304222738 : 0 ) ) : v;
	v = p.y == 9. ? 0 : v;
	v = p.y == 8. ? ( p.x < 8. ? 572732211 : ( p.x < 16. ? 34677025 : 0 ) ) : v;
	v = p.y == 7. ? ( p.x < 8. ? 554832402 : ( p.x < 16. ? 19013905 : 0 ) ) : v;
	v = p.y == 6. ? 0 : v;
	v = p.y == 5. ? ( p.x < 8. ? 288568115 : ( p.x < 16. ? 858993410 : 0 ) ) : v;
	v = p.y == 4. ? ( p.x < 8. ? 286331426 : ( p.x < 16. ? 286331137 : 0 ) ) : v;
	v = p.y == 3. ? 0 : v;
	v = p.y == 2. ? ( p.x < 8. ? 572732208 : ( p.x < 16. ? 303174161 : 0 ) ) : v;
	v = p.y == 1. ? 0 : v;
	v = p.y == 0. ? ( p.x < 8. ? 34677011 : ( p.x < 16. ? 858993459 : 0 ) ) : v;
    float i = float( ( v >> int( 4. * p.x ) ) & 15 );
    color = vec3( 0.25, 0.18, 0.098 );
    color = i == 1. ? vec3( 0.31, 0.23, 0.11 ) : color;
    color = i == 2. ? vec3( 0.35, 0.26, 0.12 ) : color;
    color = i == 3. ? vec3( 0.4, 0.29, 0.12 ) : color;
}

void SpriteWall( inout vec3 color, vec2 p )
{    
    int v = 0;
	v = p.y == 15. ? ( p.x < 4. ? 100927239 : ( p.x < 8. ? 17106437 : ( p.x < 12. ? 67372806 : 328452 ) ) ) : v;
	v = p.y == 14. ? ( p.x < 4. ? 117901063 : ( p.x < 8. ? 17171974 : ( p.x < 12. ? 101123847 : 393991 ) ) ) : v;
	v = p.y == 13. ? ( p.x < 4. ? 84280838 : ( p.x < 8. ? 329222 : ( p.x < 12. ? 84281094 : 329221 ) ) ) : v;
	v = p.y == 12. ? ( p.x < 4. ? 1 : ( p.x < 8. ? 65537 : ( p.x < 12. ? 256 : 16777472 ) ) ) : v;
	v = p.y == 11. ? ( p.x < 4. ? 17105926 : ( p.x < 8. ? 117703683 : ( p.x < 12. ? 17171975 : 117901062 ) ) ) : v;
	v = p.y == 10. ? ( p.x < 4. ? 17172231 : ( p.x < 8. ? 117835526 : ( p.x < 12. ? 460295 : 117901063 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 329222 : ( p.x < 8. ? 84346629 : ( p.x < 12. ? 17106437 : 84281093 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 16842753 : ( p.x < 8. ? 16843009 : ( p.x < 12. ? 16777473 : 65536 ) ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 67372036 : ( p.x < 8. ? 101058052 : ( p.x < 12. ? 67372545 : 100664836 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 117900038 : ( p.x < 8. ? 101058311 : ( p.x < 12. ? 67503872 : 100730374 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 84280839 : ( p.x < 8. ? 84215046 : ( p.x < 12. ? 84280577 : 83887366 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 65793 : ( p.x < 8. ? 16777473 : ( p.x < 12. ? 257 : 0 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 84281095 : ( p.x < 8. ? 117703424 : ( p.x < 12. ? 33818119 : 117900032 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 117835270 : ( p.x < 8. ? 117901057 : ( p.x < 12. ? 117900806 : 101122817 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 84280583 : ( p.x < 8. ? 101123328 : ( p.x < 12. ? 84215046 : 101123584 ) ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 16842753 : ( p.x < 8. ? 16842752 : ( p.x < 12. ? 16777472 : 16843008 ) ) ) : v;
    float i = float( ( v >> int( 8.0 * p.x ) ) & 255 );
    color = vec3( 0.33 );
    color = i == 1. ? vec3( 0.38 ) : color;
    color = i == 2. ? vec3( 0.5, 0.6, 0.56 ) : color;
    color = i == 3. ? vec3( 0.55, 0.65, 0.6 ) : color;
    color = i == 4. ? vec3( 0.61, 0.71, 0.66 ) : color;
    color = i == 5. ? vec3( 0.67 ) : color;
    color = i == 6. ? vec3( 0.75 ) : color;
    color = i == 7. ? vec3( 0.85 ) : color;
}

void SpriteWallHole( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 15. ? ( p.x < 4. ? 100992775 : ( p.x < 8. ? 50660869 : ( p.x < 12. ? 84215558 : 33883141 ) ) ) : v;
	v = p.y == 14. ? ( p.x < 4. ? 117901063 : ( p.x < 8. ? 33817606 : ( p.x < 12. ? 100992261 : 33948679 ) ) ) : v;
	v = p.y == 13. ? ( p.x < 4. ? 84280838 : ( p.x < 8. ? 117901060 : ( p.x < 12. ? 67569415 : 33883653 ) ) ) : v;
	v = p.y == 12. ? ( p.x < 4. ? 33686019 : ( p.x < 8. ? 101058054 : ( p.x < 12. ? 101058054 : 50463490 ) ) ) : v;
	v = p.y == 11. ? ( p.x < 4. ? 100926726 : ( p.x < 8. ? 101058054 : ( p.x < 12. ? 101058054 : 117900550 ) ) ) : v;
	v = p.y == 10. ? ( p.x < 4. ? 100927239 : ( p.x < 8. ? 67078 : ( p.x < 12. ? 101056768 : 117900550 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 100926982 : ( p.x < 8. ? 518 : ( p.x < 12. ? 100794368 : 84280582 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 100794883 : ( p.x < 8. ? 16777734 : ( p.x < 12. ? 100794369 : 33751558 ) ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 100926725 : ( p.x < 8. ? 16843014 : ( p.x < 12. ? 100729089 : 100795398 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 100992006 : ( p.x < 8. ? 33620998 : ( p.x < 12. ? 100925698 : 100860934 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 100926983 : ( p.x < 8. ? 67372550 : ( p.x < 12. ? 101057540 : 84018182 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 16909059 : ( p.x < 8. ? 67503622 : ( p.x < 12. ? 101057542 : 33686017 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 67372807 : ( p.x < 8. ? 67503617 : ( p.x < 12. ? 33949190 : 117900034 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 84280838 : ( p.x < 8. ? 50529026 : ( p.x < 12. ? 84083202 : 101123074 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 84280583 : ( p.x < 8. ? 50594818 : ( p.x < 12. ? 67371779 : 101123586 ) ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 50528771 : ( p.x < 8. ? 33686018 : ( p.x < 12. ? 50463234 : 50529026 ) ) ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = vec3( 0.059, 0.059, 0.035 );
    color = i == 1. ? vec3( 0.18, 0.19, 0.16 ) : color;
    color = i == 2. ? vec3( 0.28, 0.29, 0.29 ) : color;
    color = i == 3. ? vec3( 0.4, 0.44, 0.42 ) : color;
    color = i == 4. ? vec3( 0.54, 0.58, 0.56 ) : color;
    color = i == 5. ? vec3( 0.64, 0.69, 0.67 ) : color;
    color = i == 6. ? vec3( 0.75 ) : color;
    color = i == 7. ? vec3( 0.88 ) : color;
}

void SpriteDoorClosed( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 15. ? ( p.x < 4. ? 117769991 : ( p.x < 8. ? 50661125 : ( p.x < 12. ? 84215559 : 50660613 ) ) ) : v;
	v = p.y == 14. ? ( p.x < 4. ? 16844551 : ( p.x < 8. ? 16843009 : ( p.x < 12. ? 16843009 : 50790657 ) ) ) : v;
	v = p.y == 13. ? ( p.x < 4. ? 67176199 : ( p.x < 8. ? 33686018 : ( p.x < 12. ? 33686018 : 50659586 ) ) ) : v;
	v = p.y == 12. ? ( p.x < 4. ? 67175171 : ( p.x < 8. ? 16843010 : ( p.x < 12. ? 33620225 : 50528514 ) ) ) : v;
	v = p.y == 11. ? ( p.x < 4. ? 16844039 : ( p.x < 8. ? 258 : ( p.x < 12. ? 33619968 : 117899522 ) ) ) : v;
	v = p.y == 10. ? ( p.x < 4. ? 16844295 : ( p.x < 8. ? 258 : ( p.x < 12. ? 33619968 : 117899522 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 16844295 : ( p.x < 8. ? 16843010 : ( p.x < 12. ? 33620225 : 84345090 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 33620739 : ( p.x < 8. ? 33686018 : ( p.x < 12. ? 33817090 : 50528514 ) ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 33621253 : ( p.x < 8. ? 33686018 : ( p.x < 12. ? 16908802 : 100925697 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 33621255 : ( p.x < 8. ? 33817090 : ( p.x < 12. ? 16909314 : 117702913 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 16844295 : ( p.x < 8. ? 33686018 : ( p.x < 12. ? 33686018 : 84148482 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 16843779 : ( p.x < 8. ? 33686018 : ( p.x < 12. ? 33686018 : 50528514 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 16844295 : ( p.x < 8. ? 33686018 : ( p.x < 12. ? 33686018 : 117899522 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 67176199 : ( p.x < 8. ? 33686018 : ( p.x < 12. ? 33686018 : 117899522 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 67175687 : ( p.x < 8. ? 33686018 : ( p.x < 12. ? 33686018 : 117899522 ) ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 16843523 : ( p.x < 8. ? 16843009 : ( p.x < 12. ? 16843009 : 50528513 ) ) ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = vec3( 0 );
    color = i == 1. ? vec3( 0.19, 0.14, 0.14 ) : color;
    color = i == 2. ? vec3( 0.4, 0.26, 0.098 ) : color;
    color = i == 3. ? vec3( 0.35 ) : color;
    color = i == 4. ? vec3( 0.53, 0.38, 0.19 ) : color;
    color = i == 5. ? vec3( 0.59, 0.67, 0.64 ) : color;
    color = i == 6. ? vec3( 0.8, 0.64, 0.49 ) : color;
    color = i == 7. ? vec3( 0.8 ) : color;
}

void SpriteDoorOpen( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 15. ? ( p.x < 4. ? 100927239 : ( p.x < 8. ? 33818116 : ( p.x < 12. ? 67372806 : 33817604 ) ) ) : v;
	v = p.y == 14. ? ( p.x < 4. ? 16844551 : ( p.x < 8. ? 16843009 : ( p.x < 12. ? 16843009 : 33947905 ) ) ) : v;
	v = p.y == 13. ? ( p.x < 4. ? 67078 : ( p.x < 8. ? 0 : ( p.x < 12. ? 0 : 33816832 ) ) ) : v;
	v = p.y == 12. ? ( p.x < 4. ? 66050 : ( p.x < 8. ? 0 : ( p.x < 12. ? 0 : 33685760 ) ) ) : v;
	v = p.y == 11. ? ( p.x < 4. ? 66566 : ( p.x < 8. ? 0 : ( p.x < 12. ? 0 : 117899520 ) ) ) : v;
	v = p.y == 10. ? ( p.x < 4. ? 66823 : ( p.x < 8. ? 0 : ( p.x < 12. ? 0 : 117899520 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 66822 : ( p.x < 8. ? 0 : ( p.x < 12. ? 0 : 67502336 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 66050 : ( p.x < 8. ? 0 : ( p.x < 12. ? 0 : 33685760 ) ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 66564 : ( p.x < 8. ? 0 : ( p.x < 12. ? 0 : 84082944 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 66566 : ( p.x < 8. ? 0 : ( p.x < 12. ? 0 : 100860160 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 66823 : ( p.x < 8. ? 0 : ( p.x < 12. ? 0 : 67305728 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 66306 : ( p.x < 8. ? 0 : ( p.x < 12. ? 0 : 33685760 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 66823 : ( p.x < 8. ? 0 : ( p.x < 12. ? 0 : 117899520 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 67078 : ( p.x < 8. ? 0 : ( p.x < 12. ? 0 : 101122304 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 66567 : ( p.x < 8. ? 0 : ( p.x < 12. ? 0 : 101122304 ) ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 66050 : ( p.x < 8. ? 0 : ( p.x < 12. ? 0 : 33685760 ) ) ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = i == 1. ? vec3( 0.15, 0.15, 0.14 ) : color;
    color = i == 2. ? vec3( 0.35 ) : color;
    color = i == 3. ? vec3( 0.53, 0.37, 0.21 ) : color;
    color = i == 4. ? vec3( 0.59, 0.67, 0.64 ) : color;
    color = i == 5. ? vec3( 0.8, 0.64, 0.49 ) : color;
    color = i == 6. ? vec3( 0.75 ) : color;
    color = i == 7. ? vec3( 0.85 ) : color;
}

void SpriteDoorLocked( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 15. ? ( p.x < 4. ? 117769991 : ( p.x < 8. ? 50661125 : ( p.x < 12. ? 84215559 : 50660613 ) ) ) : v;
	v = p.y == 14. ? ( p.x < 4. ? 16844551 : ( p.x < 8. ? 16843009 : ( p.x < 12. ? 16843009 : 50790657 ) ) ) : v;
	v = p.y == 13. ? ( p.x < 4. ? 67176199 : ( p.x < 8. ? 33685762 : ( p.x < 12. ? 33686017 : 50659586 ) ) ) : v;
	v = p.y == 12. ? ( p.x < 4. ? 67175171 : ( p.x < 8. ? 16843010 : ( p.x < 12. ? 33620225 : 50528514 ) ) ) : v;
	v = p.y == 11. ? ( p.x < 4. ? 16844039 : ( p.x < 8. ? 258 : ( p.x < 12. ? 33619968 : 117899522 ) ) ) : v;
	v = p.y == 10. ? ( p.x < 4. ? 16844295 : ( p.x < 8. ? 84214017 : ( p.x < 12. ? 16843525 : 117899522 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 16844295 : ( p.x < 8. ? 50529537 : ( p.x < 12. ? 16974595 : 84345090 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 33620739 : ( p.x < 8. ? 16974594 : ( p.x < 12. ? 16974593 : 50528513 ) ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 33621253 : ( p.x < 8. ? 84215045 : ( p.x < 12. ? 84215045 : 100925697 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 33621255 : ( p.x < 8. ? 50529029 : ( p.x < 12. ? 50529027 : 117702913 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 16844295 : ( p.x < 8. ? 197381 : ( p.x < 12. ? 50529024 : 84148481 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 16843779 : ( p.x < 8. ? 197381 : ( p.x < 12. ? 50529024 : 50528513 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 16844295 : ( p.x < 8. ? 50529029 : ( p.x < 12. ? 50529027 : 117899521 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 33621767 : ( p.x < 8. ? 50529027 : ( p.x < 12. ? 50529027 : 117899521 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 33621255 : ( p.x < 8. ? 16843009 : ( p.x < 12. ? 16843265 : 117899521 ) ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 16843523 : ( p.x < 8. ? 16843009 : ( p.x < 12. ? 16843009 : 50528513 ) ) ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = vec3( 0.024, 0.0078, 0 );
    color = i == 1. ? vec3( 0.19, 0.13, 0.11 ) : color;
    color = i == 2. ? vec3( 0.38, 0.25, 0.094 ) : color;
    color = i == 3. ? vec3( 0.42, 0.39, 0.38 ) : color;
    color = i == 4. ? vec3( 0.53, 0.38, 0.2 ) : color;
    color = i == 5. ? vec3( 0.59, 0.62, 0.56 ) : color;
    color = i == 6. ? vec3( 0.8, 0.64, 0.49 ) : color;
    color = i == 7. ? vec3( 0.8 ) : color;
}

void SpriteStairsDown( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 15. ? ( p.x < 4. ? 50529284 : ( p.x < 8. ? 67306243 : ( p.x < 12. ? 67372036 : 50594820 ) ) ) : v;
	v = p.y == 14. ? ( p.x < 4. ? 117901060 : ( p.x < 8. ? 117835271 : ( p.x < 12. ? 101123847 : 50726407 ) ) ) : v;
	v = p.y == 13. ? ( p.x < 4. ? 33751553 : ( p.x < 8. ? 33751555 : ( p.x < 12. ? 33620483 : 50791170 ) ) ) : v;
	v = p.y == 12. ? ( p.x < 4. ? 50529028 : ( p.x < 8. ? 16908548 : ( p.x < 12. ? 16843265 : 67568129 ) ) ) : v;
	v = p.y == 11. ? ( p.x < 4. ? 117835523 : ( p.x < 8. ? 16843011 : ( p.x < 12. ? 16842753 : 67567873 ) ) ) : v;
	v = p.y == 10. ? ( p.x < 4. ? 117835524 : ( p.x < 8. ? 50463235 : ( p.x < 12. ? 65539 : 67567873 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 101123844 : ( p.x < 8. ? 84215044 : ( p.x < 12. ? 2 : 50790400 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 117901060 : ( p.x < 8. ? 84280580 : ( p.x < 12. ? 33620482 : 50790401 ) ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 117900804 : ( p.x < 8. ? 84215044 : ( p.x < 12. ? 84149250 : 50724866 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 117900803 : ( p.x < 8. ? 84215044 : ( p.x < 12. ? 84149506 : 50790657 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 101123844 : ( p.x < 8. ? 84215044 : ( p.x < 12. ? 67372034 : 50725890 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 101123844 : ( p.x < 8. ? 100992260 : ( p.x < 12. ? 67372035 : 50725377 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 101123844 : ( p.x < 8. ? 84215043 : ( p.x < 12. ? 67437570 : 67568129 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 67306243 : ( p.x < 8. ? 50529028 : ( p.x < 12. ? 16908546 : 50790658 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 101123588 : ( p.x < 8. ? 117901062 : ( p.x < 12. ? 117835270 : 50792199 ) ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 67371780 : ( p.x < 8. ? 67371779 : ( p.x < 12. ? 67306499 : 67372035 ) ) ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = vec3( 0.13 );
    color = i == 1. ? vec3( 0.2 ) : color;
    color = i == 2. ? vec3( 0.26 ) : color;
    color = i == 3. ? vec3( 0.31 ) : color;
    color = i == 4. ? vec3( 0.35 ) : color;
    color = i == 5. ? vec3( 0.44 ) : color;
    color = i == 6. ? vec3( 0.51 ) : color;
    color = i == 7. ? vec3( 0.56 ) : color;
}

void SpriteStairsUp( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 15. ? ( p.x < 4. ? 50529284 : ( p.x < 8. ? 84215045 : ( p.x < 12. ? 84214533 : 84215043 ) ) ) : v;
	v = p.y == 14. ? ( p.x < 4. ? 67569412 : ( p.x < 8. ? 50463234 : ( p.x < 12. ? 33751811 : 84215043 ) ) ) : v;
	v = p.y == 13. ? ( p.x < 4. ? 50791940 : ( p.x < 8. ? 50594820 : ( p.x < 12. ? 67437829 : 84215042 ) ) ) : v;
	v = p.y == 12. ? ( p.x < 4. ? 50726404 : ( p.x < 8. ? 50791943 : ( p.x < 12. ? 84215045 : 84215042 ) ) ) : v;
	v = p.y == 11. ? ( p.x < 4. ? 67503875 : ( p.x < 8. ? 50726662 : ( p.x < 12. ? 67371780 : 84214786 ) ) ) : v;
	v = p.y == 10. ? ( p.x < 4. ? 67503876 : ( p.x < 8. ? 67503878 : ( p.x < 12. ? 50791943 : 50529027 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 50792196 : ( p.x < 8. ? 50792199 : ( p.x < 12. ? 67569414 : 50594819 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 16843009 : ( p.x < 8. ? 67569415 : ( p.x < 12. ? 67503878 : 50791942 ) ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 17105665 : ( p.x < 8. ? 67503623 : ( p.x < 12. ? 67569158 : 50726663 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 17105664 : ( p.x < 8. ? 65793 : ( p.x < 12. ? 67503878 : 50791942 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 50529025 : ( p.x < 8. ? 16974597 : 50726663 ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 50529025 : ( p.x < 8. ? 16974595 : ( p.x < 12. ? 257 : 50726406 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 50529025 : ( p.x < 8. ? 50529027 : ( p.x < 12. ? 197379 : 67569158 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 50397440 : ( p.x < 8. ? 50529027 : ( p.x < 12. ? 196865 : 65536 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 16974080 : ( p.x < 8. ? 16974593 : ( p.x < 12. ? 50397441 : 197377 ) ) ) : v;
	v = p.y == 0. ? 0 : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = vec3( 0.12 );
    color = i == 1. ? vec3( 0.17 ) : color;
    color = i == 2. ? vec3( 0.21, 0.2, 0.19 ) : color;
    color = i == 3. ? vec3( 0.23, 0.23, 0.22 ) : color;
    color = i == 4. ? vec3( 0.26, 0.26, 0.25 ) : color;
    color = i == 5. ? vec3( 0.31, 0.31, 0.29 ) : color;
    color = i == 6. ? vec3( 0.5 ) : color;
    color = i == 7. ? vec3( 0.53 ) : color;
}

void SpriteKey( inout vec3 color, vec2 p )
{
    p -= vec2( 5., 2. );
    p = p.x < 0. ? vec2( 0. ) : p;    
    
    int v = 0;
	v = p.y == 11. ? ( p.x < 8. ? 139824 : 0 ) : v;
	v = p.y == 10. ? ( p.x < 8. ? 2232611 : 0 ) : v;
	v = p.y == 9. ? ( p.x < 8. ? 1179666 : 0 ) : v;
	v = p.y == 8. ? ( p.x < 8. ? 1245202 : 0 ) : v;
	v = p.y == 7. ? ( p.x < 8. ? 1192482 : 0 ) : v;
	v = p.y == 6. ? ( p.x < 8. ? 74256 : 0 ) : v;
	v = p.y == 5. ? ( p.x < 8. ? 4608 : 0 ) : v;
	v = p.y == 4. ? ( p.x < 8. ? 4608 : 0 ) : v;
	v = p.y == 3. ? ( p.x < 8. ? 4608 : 0 ) : v;
	v = p.y == 2. ? ( p.x < 8. ? 2232832 : 0 ) : v;
	v = p.y == 1. ? ( p.x < 8. ? 135680 : 0 ) : v;
	v = p.y == 0. ? ( p.x < 8. ? 2232832 : 0 ) : v;
    float i = float( ( v >> int( 4. * p.x ) ) & 15 );
    color = i == 1. ? vec3( 0.45 ) : color;
    color = i == 2. ? vec3( 0.83 ) : color;
    color = i == 3. ? vec3( 0.95 ) : color;
}

void SpriteFood( inout vec3 color, vec2 p )
{
    p -= vec2( 4., 4. );
    p = p.x < 0. ? vec2( 0. ) : p;
    
    int v = 0;
	v = p.y == 7. ? ( p.x < 8. ? 3355392 : 0 ) : v;
	v = p.y == 6. ? ( p.x < 8. ? 52498736 : 0 ) : v;
	v = p.y == 5. ? ( p.x < 8. ? 839979795 : 0 ) : v;
	v = p.y == 4. ? ( p.x < 8. ? 839979283 : 0 ) : v;
	v = p.y == 3. ? ( p.x < 8. ? 839979283 : 0 ) : v;
	v = p.y == 2. ? ( p.x < 8. ? 841027875 : 0 ) : v;
	v = p.y == 1. ? ( p.x < 8. ? 52568624 : 0 ) : v;
	v = p.y == 0. ? ( p.x < 8. ? 3355392 : 0 ) : v;
    float i = float( ( v >> int( 4. * p.x ) ) & 15 );
    color = i == 1. ? vec3( 0.24 ) : color;
    color = i == 2. ? vec3( 0.29, 0.74, 0.79 ) : color;
    color = i == 3. ? vec3( 0.91 ) : color;
}

void SpritePotion( inout vec3 color, vec2 p )
{
    p -= vec2( 4., 2. );
    p = p.x < 0. ? vec2( 0. ) : p;
    
    int v = 0;
	v = p.y == 11. ? ( p.x < 4. ? 50331648 : ( p.x < 8. ? 4 : 0 ) ) : v;
	v = p.y == 10. ? ( p.x < 4. ? 84280832 : ( p.x < 8. ? 394757 : 0 ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 50724864 : ( p.x < 8. ? 1540 : 0 ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 17170432 : ( p.x < 8. ? 1537 : 0 ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 17170432 : ( p.x < 8. ? 1537 : 0 ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 16844288 : ( p.x < 8. ? 393473 : 0 ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 16843014 : ( p.x < 8. ? 100729089 : 0 ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 33686022 : ( p.x < 8. ? 100796162 : 0 ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 33686022 : ( p.x < 8. ? 100796162 : 0 ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 33686022 : ( p.x < 8. ? 100794882 : 0 ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 33687040 : ( p.x < 8. ? 393730 : 0 ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 101056512 : ( p.x < 8. ? 1542 : 0 ) ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = i == 1. ? vec3( 0.24 ) : color;
    color = i == 2. ? vec3( 0.91, 0, 0.12 ) : color;
    color = i == 3. ? vec3( 0.6, 0.38, 0.2 ) : color;
    color = i == 4. ? vec3( 0.71, 0.49, 0.31 ) : color;
    color = i == 5. ? vec3( 0.83, 0.76, 0.71 ) : color;
    color = i == 6. ? vec3( 0.85 ) : color;
    color = i == 7. ? vec3( 1 ) : color;
}

void SpriteRatDead( inout vec3 color, vec2 p )
{
    p -= vec2( 2., 1. );
    p = p.x < 0. ? vec2( 9. ) : p;  
    
    int v = 0;
	v = p.y == 8. ? ( p.x < 4. ? 33685504 : ( p.x < 8. ? 5 : 0 ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 67371520 : ( p.x < 8. ? 1 : ( p.x < 12. ? 131586 : 0 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 67371522 : ( p.x < 8. ? 50397442 : ( p.x < 12. ? 84149252 : 0 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 67372034 : ( p.x < 8. ? 67240452 : ( p.x < 12. ? 33817604 : 0 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 67371521 : ( p.x < 8. ? 50594308 : ( p.x < 12. ? 16843268 : 0 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 67372033 : ( p.x < 8. ? 67372036 : ( p.x < 12. ? 84149250 : 0 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 67371264 : ( p.x < 8. ? 67371524 : ( p.x < 12. ? 50594818 : 0 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 16844289 : ( p.x < 8. ? 67240450 : ( p.x < 12. ? 16908804 : 0 ) ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 257 : ( p.x < 8. ? 16843009 : ( p.x < 12. ? 100729089 : 460551 ) ) ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = i == 1. ? vec3( 0.16, 0.11, 0.071 ) : color;
    color = i == 2. ? vec3( 0.33, 0.26, 0.18 ) : color;
    color = i == 3. ? vec3( 0.58, 0.25, 0.24 ) : color;
    color = i == 4. ? vec3( 0.51, 0.44, 0.35 ) : color;
    color = i == 5. ? vec3( 0.81, 0.31, 0.33 ) : color;
    color = i == 6. ? vec3( 0.8, 0.49, 0.51 ) : color;
    color = i == 7. ? vec3( 0.93, 0.58, 0.6 ) : color;
}

void SpriteSkeletonDead( inout vec3 color, vec2 p )
{
    p -= vec2( 4., 1. );
    p = p.x < 0. ? vec2( 0. ) : p;    
    
    int v = 0;
	v = p.y == 6. ? ( p.x < 4. ? 84215040 : ( p.x < 8. ? 328965 : 0 ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 101058053 : ( p.x < 8. ? 84280838 : 0 ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 67372547 : ( p.x < 8. ? 33817604 : 0 ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 17171971 : ( p.x < 8. ? 83953158 : 0 ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 101057024 : ( p.x < 8. ? 84280326 : 0 ) ) : v;
	v = p.y == 1. ? ( p.x < 4. ? 50462720 : ( p.x < 8. ? 197893 : 0 ) ) : v;
	v = p.y == 0. ? ( p.x < 4. ? 33554432 : ( p.x < 8. ? 131072 : 0 ) ) : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = i == 1. ? vec3( 0 ) : color;
    color = i == 2. ? vec3( 0.49 ) : color;
    color = i == 3. ? vec3( 0.64 ) : color;
    color = i == 4. ? vec3( 0.7 ) : color;
    color = i == 5. ? vec3( 0.81 ) : color;
    color = i == 6. ? vec3( 0.9 ) : color;
}

void SpriteTrap( inout vec3 color, vec2 p, float pulse )
{
    int v = 0;
	v = p.y == 15. ? 0 : v;
	v = p.y == 14. ? 0 : v;
	v = p.y == 13. ? ( p.x < 4. ? 33554432 : ( p.x < 8. ? 16777218 : ( p.x < 12. ? 33554434 : 1 ) ) ) : v;
	v = p.y == 12. ? ( p.x < 4. ? 117506048 : ( p.x < 8. ? 84017157 : ( p.x < 12. ? 84017157 : 5 ) ) ) : v;
	v = p.y == 11. ? ( p.x < 4. ? 117440512 : ( p.x < 8. ? 83887621 : ( p.x < 12. ? 83887621 : 1543 ) ) ) : v;
	v = p.y == 10. ? ( p.x < 4. ? 100663296 : ( p.x < 8. ? 67108870 : ( p.x < 12. ? 67108868 : 4 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 4. ? 33554432 : ( p.x < 8. ? 16777218 : ( p.x < 12. ? 33554434 : 2 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 4. ? 83951616 : ( p.x < 8. ? 84017157 : ( p.x < 12. ? 117571589 : 517 ) ) ) : v;
	v = p.y == 7. ? ( p.x < 4. ? 83886080 : ( p.x < 8. ? 83887623 : ( p.x < 12. ? 117638661 : 1029 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 4. ? 67108864 : ( p.x < 8. ? 67108868 : ( p.x < 12. ? 100664070 : 6 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 4. ? 33554432 : ( p.x < 8. ? 16777218 : ( p.x < 12. ? 33554434 : 2 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 4. ? 117571584 : ( p.x < 8. ? 117571589 : ( p.x < 12. ? 84017157 : 5 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 4. ? 83886080 : ( p.x < 8. ? 83887621 : ( p.x < 12. ? 84018181 : 1031 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 4. ? 67108864 : ( p.x < 8. ? 67108868 : ( p.x < 12. ? 100663300 : 516 ) ) ) : v;
	v = p.y == 1. ? 0 : v;
	v = p.y == 0. ? 0 : v;
    float i = float( ( v >> int( 8. * p.x ) ) & 255 );
    color = i == 1. ? vec3( 0.13, 0.13, 0.12 ) : color;
    color = i == 2. ? vec3( 0.21, 0.2, 0.19 ) : color;
    color = i == 3. ? vec3( 0.33, 0.32, 0.31 ) : color;
    color = i == 4. ? vec3( 0.37, 0.37, 0.36 ) : color;
    color = i == 5. ? pulse * vec3( 0.93, 0.35, 0 ) : color;
    color = i == 6. ? vec3( 0.44, 0.43, 0.42 ) : color;
    color = i == 7. ? pulse * vec3( 1, 0.47, 0.051 ) : color;
}

void SpriteMoss( inout vec3 color, vec2 p )
{
    int v = 0;
	v = p.y == 15. ? 0 : v;
	v = p.y == 14. ? 0 : v;
	v = p.y == 13. ? 0 : v;
	v = p.y == 12. ? ( p.x < 8. ? 318767104 : ( p.x < 16. ? 3145731 : 0 ) ) : v;
	v = p.y == 11. ? ( p.x < 8. ? 318767360 : ( p.x < 16. ? 52428851 : 0 ) ) : v;
	v = p.y == 10. ? ( p.x < 8. ? 321912832 : ( p.x < 16. ? 1114129 : 0 ) ) : v;
	v = p.y == 9. ? ( p.x < 8. ? 322109440 : ( p.x < 16. ? 51523586 : 0 ) ) : v;
	v = p.y == 8. ? ( p.x < 8. ? 322109440 : ( p.x < 16. ? 51589120 : 0 ) ) : v;
	v = p.y == 7. ? ( p.x < 8. ? 305135616 : ( p.x < 16. ? 34799667 : 0 ) ) : v;
	v = p.y == 6. ? ( p.x < 8. ? 285212672 : ( p.x < 16. ? 819 : 0 ) ) : v;
	v = p.y == 5. ? ( p.x < 8. ? 0 : ( p.x < 16. ? 819 : 0 ) ) : v;
	v = p.y == 4. ? ( p.x < 8. ? 196608 : 0 ) : v;
	v = p.y == 3. ? ( p.x < 8. ? 3354624 : ( p.x < 16. ? 256 : 0 ) ) : v;
	v = p.y == 2. ? ( p.x < 8. ? 1118464 : 0 ) : v;
	v = p.y == 1. ? ( p.x < 8. ? 204800 : 0 ) : v;
	v = p.y == 0. ? 0 : v;
    float i = float( ( v >> int( 4. * p.x ) ) & 15 );
    color = i == 1. ? vec3( 0.37, 0.39, 0.14 ) : color;
    color = i == 2. ? vec3( 0.39, 0.41, 0.16 ) : color;
    color = i == 3. ? vec3( 0.41, 0.42, 0.18 ) : color;
}

vec2 FrameOffset( float frame, float tick )
{
    vec2 ret = vec2( 0. );
    ret.x = frame == 1. ? 1. : ( frame == 2. ? -1. : 0. );
    ret.y = frame == 3. ? 1. : ( frame == 4. ? -1. : 0. );
    return floor( 16. * ret * ( tick / TICK_NUM ) );
}

float Rand( vec2 n )
{
	return fract( sin( dot( n.xy, vec2( 12.9898, 78.233 ) ) ) * 43758.5453 );
}

float WaterSDF( vec2 p )
{
    float ret = length( p - vec2( 16., 16. ) ) - 16.;
    ret = min( ret, length( p - vec2( 20., 60. ) ) - 16. );
    ret = min( ret, length( p - vec2( 18., 38. ) ) - 12. );
    
    ret = min( ret, length( p - vec2( 286., 20. ) ) - 36. );
    
    ret = min( ret, length( p - vec2( 20., 120. ) ) - 16. );
    ret = min( ret, length( p - vec2( 20., 160. ) ) - 16. );
    
    ret = min( ret, length( p - vec2( 16., 400. ) ) - 20. );
    
    ret = min( ret, length( p - vec2( 470., 30. ) ) - 20. );
    ret = min( ret, length( p - vec2( 480., 80. ) ) - 20. );
    ret = min( ret, length( p - vec2( 430., 10. ) ) - 20. );
    
    ret = min( ret, length( p - vec2( 415., 320. ) ) - 30. );
    
    ret = min( ret, length( p - vec2( 130., 320. ) ) - 40. );
    
	ret = min( ret, length( p - vec2( 300., 100. ) ) - 50. );    
    
    ret = min( ret, length( p - vec2( 80., 480. ) ) - 50. ); 
    
    ret = min( ret, length( p - vec2( 80., 224. ) ) - 50. ); 
    
    ret = min( ret, length( p - vec2( 200., 360. ) ) - 50. ); 
    
    ret += sin( p.y * .75 ) * 1.;    
    return floor( ret );
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
    
    vec3 color = vec3( 0. );
    SpriteEarth( color, worldMod16 );
    if ( Rand( tile / 32. ) > .8 )
    {
    	SpriteEarth2( color, worldMod16 );
    }

    float waterSDF = WaterSDF( world );
    if ( map.x != 0. )
    {
		SpriteFloor( color, worldMod16 );
        
 		vec3 water;
        SpriteWater( water, floor( mod( floor( world + vec2( 0., float( iFrame ) * .2 ) ), 16. ) ) );
        
        float alpha = waterSDF >= 0. ? 0. : 1.;
        alpha = waterSDF == -1. ? 0.25 : alpha;
        alpha = waterSDF == -2. ? 0.5 : alpha;
        
    	color = mix( color, water, alpha );
    }
    
    // decoration
    if ( map.x == TILE_FLOOR && waterSDF >= 0. )
    {
		if ( Rand( tile / 32. - 3.15 ) > .8 )
    	{
       		 SpriteMoss( color, worldMod16 );
        } 
        else if ( Rand( tile / 32. - 7.19 ) > .9 )
        {
            SpriteEarth2( color, worldMod16 );
        }
        else if ( Rand( tile / 32. - 13.19 ) > .95 )
        {
            SpriteTrap( color, worldMod16, 0. );
        }   
    }
    
    if ( map.x == TILE_STAIRS_UP )
    {
        SpriteStairsUp( color, worldMod16 );
    }
    if ( map.x == TILE_STAIRS_DOWN )
    {
        SpriteStairsDown( color, worldMod16 );
    }     
    
    if ( map.x == TILE_WALL )
    {
    	SpriteWall( color, worldMod16 );
    }
    if ( map.x == TILE_WALL_HOLE )
    {
    	SpriteWallHole( color, worldMod16 );
    }       
    if ( map.x == TILE_DOOR_LOCKED )
    {
        SpriteDoorLocked( color, worldMod16 );
    }
    if ( map.x == TILE_DOOR_OPEN )
    {
        bool open = s.playerPos == tile || map.z > 0.;
		for ( int i = 0; i < ENEMY_NUM; ++i )        
        {
            if ( tile == s.enemyPos[ i ] )
            {
                open = true;
            }
        }

        if ( open )
        {
        	SpriteDoorOpen( color, worldMod16 );
        }
        else
        {
            SpriteDoorClosed( color, worldMod16 );
        }
    }
    if ( map.x == TILE_TRAP || map.x == TILE_TRAP_OFF )
    {
        SpriteTrap( color, worldMod16, map.x == TILE_TRAP ? sin( iTime * 2. ) * .25 + .75 : 0. );
    }

    if ( map.y == ITEM_KEY )
    {
        SpriteKey( color, worldMod16 );
    }
    if ( map.y == ITEM_FOOD )
    {
        SpriteFood( color, worldMod16 );
    }
    if ( map.y == ITEM_POTION )
    {
        SpritePotion( color, worldMod16 );
    }
    
    if ( map.z == 1. )
    {
        SpriteRatDead( color, worldMod16 );
    }
    
    if ( map.z == 2. )
    {
        SpriteSkeletonDead( color, worldMod16 );
    }
    
	fragColor = vec4( color, 1. );
}
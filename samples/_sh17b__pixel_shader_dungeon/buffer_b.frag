// map

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

vec4 Map( vec2 p )
{
    int v = 0;
	v = p.y == 31. ? ( p.x < 8. ? 842150450 : ( p.x < 16. ? 573710370 : ( p.x < 24. ? 8754 : 589443584 ) ) ) : v;
	v = p.y == 30. ? ( p.x < 8. ? 286331154 : ( p.x < 16. ? 286335521 : ( p.x < 24. ? 589308177 : 554766882 ) ) ) : v;
	v = p.y == 29. ? ( p.x < 8. ? 286331154 : ( p.x < 16. ? 572592401 : ( p.x < 24. ? 287318289 : 554766609 ) ) ) : v;
	v = p.y == 28. ? ( p.x < 8. ? 572662306 : ( p.x < 16. ? 539042338 : ( p.x < 24. ? 555753745 : 554766882 ) ) ) : v;
	v = p.y == 27. ? ( p.x < 8. ? 0 : ( p.x < 16. ? 590422016 : ( p.x < 24. ? 555754002 : 572662272 ) ) ) : v;
	v = p.y == 26. ? ( p.x < 8. ? 589505314 : ( p.x < 16. ? 554770432 : ( p.x < 24. ? 555745810 : 0 ) ) ) : v;
	v = p.y == 25. ? ( p.x < 8. ? 554766610 : ( p.x < 16. ? 554770432 : ( p.x < 24. ? 555889170 : 589443634 ) ) ) : v;
	v = p.y == 24. ? ( p.x < 8. ? 554766610 : ( p.x < 16. ? 286334976 : ( p.x < 24. ? 286331153 : 554766609 ) ) ) : v;
	v = p.y == 23. ? ( p.x < 8. ? 555884834 : ( p.x < 16. ? 554770995 : ( p.x < 24. ? 555885073 : 554836514 ) ) ) : v;
	v = p.y == 22. ? ( p.x < 8. ? 287318304 : ( p.x < 16. ? 554770705 : ( p.x < 24. ? 555889169 : 554827776 ) ) ) : v;
	v = p.y == 21. ? ( p.x < 8. ? 572530976 : ( p.x < 16. ? 286335266 : ( p.x < 24. ? 555946257 : 555885106 ) ) ) : v;
	v = p.y == 20. ? ( p.x < 8. ? 143666 : ( p.x < 16. ? 572727584 : ( p.x < 24. ? 286331425 : 554832145 ) ) ) : v;
	v = p.y == 19. ? ( p.x < 8. ? 135442 : ( p.x < 16. ? 539037984 : ( p.x < 24. ? 571613713 : 554766882 ) ) ) : v;
	v = p.y == 18. ? ( p.x < 8. ? 135442 : ( p.x < 16. ? 539042336 : ( p.x < 24. ? 34734609 : 554832384 ) ) ) : v;
	v = p.y == 17. ? ( p.x < 8. ? 570560786 : ( p.x < 16. ? 539042355 : ( p.x < 24. ? 856896017 : 572662274 ) ) ) : v;
	v = p.y == 16. ? ( p.x < 8. ? 305279266 : ( p.x < 16. ? 539042065 : ( p.x < 24. ? 286331409 : 2 ) ) ) : v;
	v = p.y == 15. ? ( p.x < 8. ? 286331168 : ( p.x < 16. ? 857809169 : ( p.x < 24. ? 286331427 : 143922 ) ) ) : v;
	v = p.y == 14. ? ( p.x < 8. ? 304226592 : ( p.x < 16. ? 286331153 : ( p.x < 24. ? 286331153 : 590483729 ) ) ) : v;
	v = p.y == 13. ? ( p.x < 8. ? 570433824 : ( p.x < 16. ? 572596770 : ( p.x < 24. ? 286331426 : 554832418 ) ) ) : v;
	v = p.y == 12. ? ( p.x < 8. ? 8480 : ( p.x < 16. ? 287383552 : ( p.x < 24. ? 286331394 : 554832386 ) ) ) : v;
	v = p.y == 11. ? ( p.x < 8. ? 840966450 : ( p.x < 16. ? 287383586 : ( p.x < 24. ? 571613698 : 554766850 ) ) ) : v;
	v = p.y == 10. ? ( p.x < 8. ? 287318290 : ( p.x < 16. ? 286335009 : ( p.x < 24. ? 34734082 : 572658176 ) ) ) : v;
	v = p.y == 9. ? ( p.x < 8. ? 287318290 : ( p.x < 16. ? 572596257 : ( p.x < 24. ? 34746882 : 135680 ) ) ) : v;
	v = p.y == 8. ? ( p.x < 8. ? 304095506 : ( p.x < 16. ? 2171682 : ( p.x < 24. ? 571544064 : 36835891 ) ) ) : v;
	v = p.y == 7. ? ( p.x < 8. ? 301998354 : ( p.x < 16. ? 2167057 : ( p.x < 24. ? 286331392 : 34672913 ) ) ) : v;
	v = p.y == 6. ? ( p.x < 8. ? 570434082 : ( p.x < 16. ? 2171426 : ( p.x < 24. ? 304226816 : 34742818 ) ) ) : v;
	v = p.y == 5. ? ( p.x < 8. ? 0 : ( p.x < 16. ? 2170880 : ( p.x < 24. ? 301989888 : 34738450 ) ) ) : v;
	v = p.y == 4. ? ( p.x < 8. ? 36844082 : ( p.x < 16. ? 36778496 : ( p.x < 24. ? 302134048 : 34672914 ) ) ) : v;
	v = p.y == 3. ? ( p.x < 8. ? 571543826 : ( p.x < 16. ? 571544099 : ( p.x < 24. ? 302125346 : 34738449 ) ) ) : v;
	v = p.y == 2. ? ( p.x < 8. ? 286331154 : ( p.x < 16. ? 286331153 : ( p.x < 24. ? 302125329 : 34746930 ) ) ) : v;
	v = p.y == 1. ? ( p.x < 8. ? 571543826 : ( p.x < 16. ? 571544098 : ( p.x < 24. ? 302125346 : 34672913 ) ) ) : v;
	v = p.y == 0. ? ( p.x < 8. ? 35791394 : ( p.x < 16. ? 35791360 : ( p.x < 24. ? 570565152 : 35791394 ) ) ) : v;
    float t = float( ( v >> int( 4. * p.x ) ) & 15 );
    t = t == 2. ? 6. : t;
    t = t == 3. ? 7. : t;
    t = p.x == 15. && p.y == 30. ? 2. : t;
    t = p.x == 9. && p.y == 29. ? 2. : t;
    t = p.x == 11. && p.y == 29. ? 2. : t;
    t = p.x == 12. && p.y == 27. ? 2. : t;
    t = p.x == 17. && p.y == 27. ? 2. : t;
    t = p.x == 17. && p.y == 25. ? 2. : t;
    t = p.x == 15. && p.y == 24. ? 2. : t;
    t = p.x == 18. && p.y == 24. ? 2. : t;
    t = p.x == 21. && p.y == 24. ? 2. : t;
    t = p.x == 23. && p.y == 24. ? 2. : t;
    t = p.x == 28. && p.y == 24. ? 2. : t;
    t = p.x == 2. && p.y == 23. ? 2. : t;
    t = p.x == 6. && p.y == 23. ? 2. : t;
    t = p.x == 15. && p.y == 21. ? 2. : t;
    t = p.x == 18. && p.y == 21. ? 2. : t;
    t = p.x == 30. && p.y == 21. ? 2. : t;
    t = p.x == 16. && p.y == 20. ? 2. : t;
    t = p.x == 23. && p.y == 20. ? 2. : t;
    t = p.x == 11. && p.y == 19. ? 2. : t;
    t = p.x == 21. && p.y == 19. ? 2. : t;
    t = p.x == 28. && p.y == 19. ? 2. : t;
    t = p.x == 2. && p.y == 16. ? 2. : t;
    t = p.x == 6. && p.y == 15. ? 2. : t;
    t = p.x == 12. && p.y == 15. ? 2. : t;
    t = p.x == 11. && p.y == 14. ? 2. : t;
    t = p.x == 24. && p.y == 14. ? 2. : t;
    t = p.x == 2. && p.y == 11. ? 2. : t;
    t = p.x == 21. && p.y == 11. ? 2. : t;
    t = p.x == 28. && p.y == 11. ? 2. : t;
    t = p.x == 13. && p.y == 10. ? 2. : t;
    t = p.x == 21. && p.y == 9. ? 2. : t;
    t = p.x == 7. && p.y == 8. ? 2. : t;
    t = p.x == 27. && p.y == 8. ? 2. : t;
    t = p.x == 22. && p.y == 7. ? 2. : t;
    t = p.x == 28. && p.y == 4. ? 2. : t;
    t = p.x == 24. && p.y == 3. ? 2. : t;
    t = p.x == 10. && p.y == 2. ? 2. : t;
    t = p.x == 14. && p.y == 2. ? 2. : t;
    t = p.x == 17. && p.y == 2. ? 2. : t;
    t = p.x == 26. && p.y == 29. ? 8. : t;
    t = p.x == 22. && p.y == 25. ? 8. : t;
    t = p.x == 21. && p.y == 17. ? 8. : t;
    t = p.x == 13. && p.y == 14. ? 8. : t;
    t = p.x == 18. && p.y == 14. ? 8. : t;
    t = p.x == 12. && p.y == 4. ? 8. : t;
    t = p.x == 6. && p.y == 2. ? 8. : t;
    t = p.x == 17. && p.y == 29. ? 4. : t;
    t = p.x == 4. && p.y == 24. ? 4. : t;
    t = p.x == 16. && p.y == 23. ? 4. : t;
    t = p.x == 29. && p.y == 23. ? 4. : t;
    t = p.x == 2. && p.y == 18. ? 4. : t;
    t = p.x == 17. && p.y == 17. ? 4. : t;
    t = p.x == 8. && p.y == 15. ? 4. : t;
    t = p.x == 9. && p.y == 15. ? 4. : t;
    t = p.x == 21. && p.y == 15. ? 4. : t;
    t = p.x == 20. && p.y == 14. ? 4. : t;
    t = p.x == 22. && p.y == 14. ? 4. : t;
    t = p.x == 21. && p.y == 13. ? 4. : t;
    t = p.x == 29. && p.y == 12. ? 4. : t;
    t = p.x == 14. && p.y == 11. ? 4. : t;
    t = p.x == 26. && p.y == 4. ? 4. : t;
    t = p.x == 12. && p.y == 2. ? 4. : t;
    t = p.x == 2. && p.y == 2. ? 9. : t;
    t = p.x == 29. && p.y == 29. ? 3. : t;
    float i = 0.;
    i = p.x == 1. && p.y == 30. ? 1. : i;
    i = p.x == 1. && p.y == 24. ? 1. : i;
    i = p.x == 17. && p.y == 16. ? 1. : i;
    i = p.x == 1. && p.y == 7. ? 1. : i;
    i = p.x == 25. && p.y == 5. ? 1. : i;
    i = p.x == 1. && p.y == 3. ? 1. : i;
    i = p.x == 19. && p.y == 3. ? 1. : i;
    i = p.x == 8. && p.y == 30. ? 2. : i;
    i = p.x == 18. && p.y == 30. ? 2. : i;
    i = p.x == 27. && p.y == 28. ? 2. : i;
    i = p.x == 13. && p.y == 26. ? 2. : i;
    i = p.x == 14. && p.y == 26. ? 2. : i;
    i = p.x == 6. && p.y == 25. ? 2. : i;
    i = p.x == 13. && p.y == 25. ? 2. : i;
    i = p.x == 14. && p.y == 25. ? 2. : i;
    i = p.x == 10. && p.y == 22. ? 2. : i;
    i = p.x == 19. && p.y == 21. ? 2. : i;
    i = p.x == 3. && p.y == 19. ? 2. : i;
    i = p.x == 17. && p.y == 19. ? 2. : i;
    i = p.x == 30. && p.y == 18. ? 2. : i;
    i = p.x == 10. && p.y == 16. ? 2. : i;
    i = p.x == 23. && p.y == 16. ? 2. : i;
    i = p.x == 27. && p.y == 14. ? 2. : i;
    i = p.x == 14. && p.y == 12. ? 2. : i;
    i = p.x == 19. && p.y == 12. ? 2. : i;
    i = p.x == 23. && p.y == 12. ? 2. : i;
    i = p.x == 1. && p.y == 10. ? 2. : i;
    i = p.x == 11. && p.y == 3. ? 2. : i;
    i = p.x == 13. && p.y == 3. ? 2. : i;
    i = p.x == 27. && p.y == 3. ? 2. : i;
    i = p.x == 23. && p.y == 1. ? 2. : i;
    i = p.x == 1. && p.y == 29. ? 3. : i;
    i = p.x == 8. && p.y == 10. ? 3. : i;
    i = p.x == 19. && p.y == 7. ? 3. : i;
    i = p.x == 3. && p.y == 30. ? 4. : i;
    i = p.x == 12. && p.y == 30. ? 4. : i;
    i = p.x == 2. && p.y == 25. ? 4. : i;
    i = p.x == 20. && p.y == 24. ? 4. : i;
    i = p.x == 22. && p.y == 24. ? 4. : i;
    i = p.x == 6. && p.y == 22. ? 4. : i;
    i = p.x == 12. && p.y == 21. ? 4. : i;
    i = p.x == 19. && p.y == 20. ? 4. : i;
    i = p.x == 27. && p.y == 20. ? 4. : i;
    i = p.x == 12. && p.y == 19. ? 4. : i;
    i = p.x == 1. && p.y == 17. ? 4. : i;
    i = p.x == 19. && p.y == 16. ? 4. : i;
    i = p.x == 17. && p.y == 14. ? 4. : i;
    i = p.x == 15. && p.y == 10. ? 4. : i;
    i = p.x == 12. && p.y == 9. ? 4. : i;
    i = p.x == 7. && p.y == 7. ? 4. : i;
    i = p.x == 20. && p.y == 7. ? 4. : i;
    i = p.x == 9. && p.y == 2. ? 4. : i;
    i = p.x == 19. && p.y == 2. ? 4. : i;
    i = p.x == 10. && p.y == 29. ? 5. : i;
    i = p.x == 22. && p.y == 29. ? 5. : i;
    i = p.x == 25. && p.y == 29. ? 5. : i;
    i = p.x == 13. && p.y == 24. ? 5. : i;
    i = p.x == 30. && p.y == 24. ? 5. : i;
    i = p.x == 16. && p.y == 16. ? 5. : i;
    i = p.x == 30. && p.y == 11. ? 5. : i;
    i = p.x == 6. && p.y == 10. ? 5. : i;
    i = p.x == 1. && p.y == 8. ? 5. : i;
    i = p.x == 21. && p.y == 7. ? 5. : i;
    i = p.x == 26. && p.y == 5. ? 5. : i;
    return p.x < 0. || p.y < 0. || p.x > 31. || p.y > 31. ? vec4( 0. ) : vec4( t, i, 0., 0. );
}

float Rand( vec2 n )
{
	return fract( sin( dot( n.xy, vec2( 12.9898, 78.233 ) ) ) * 43758.5453 );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // don't compute map outside of map area
    if ( fragCoord.x >= 40. || fragCoord.y >= 40. ) 
    {
		discard;    
    }    
    
    vec2 tile = floor( fragCoord );
    
    vec4 map = texelFetch( iChannel1, ivec2( tile ), 0 );
    
    GameState s;
    LoadState( s );    
    
    // create map
	if ( s.state == STATE_START || s.state == STATE_NEXT_LEVEL || iFrame < 1 )
    {
        map = Map( tile );
    }   
    
    if ( tile == s.playerPos )
    {
     	if ( s.tick == 1. && ( map.y == ITEM_KEY || map.y == ITEM_FOOD || map.y == ITEM_POTION ) )
        {
            // pickup item
			map.y = 0.;
        }
     	if ( map.x == TILE_DOOR_LOCKED && s.tick == TICK_NUM )
        {
			map.x = TILE_DOOR_OPEN;
        }    
     	if ( map.x == TILE_TRAP && s.tick == 1. )
        {
			map.x = TILE_TRAP_OFF;
        }           
    }
    
    for ( int i = 0; i < ENEMY_NUM; ++i )
    {
        if ( tile == s.enemyPos[ i ] && map.x == TILE_TRAP && s.tick == 1. )
        {
            map.x = TILE_TRAP_OFF;
        }
    }
    
    if ( tile == s.bodyPos && map.x == TILE_TRAP )
    {
		map.x = TILE_TRAP_OFF;
    }
    
    // fog of war
    if ( s.state == STATE_GAME && length( s.playerPos - tile ) < 5. )
    {    
        map.w = 1.;
    }
    
    if ( s.state == STATE_GAME && tile == s.bodyPos )
    {
        map.z = s.bodyId;
    }

    if ( tile == s.playerPos && s.hp <= 0. )
    {
        map.z = 2.;
    }
    
    for ( int i = 0; i < ENEMY_NUM; ++i )
    {
        if ( s.state == STATE_GAME && s.enemyPos[ i ].x > 0. && tile == s.enemySpawnPos[ i ] )
        {
            map.y = 0.;
        }
    }
    
    if ( s.state == STATE_GAME && s.despawnPos.x > 0. && tile == s.despawnPos )
    {
        map.y = s.despawnId > 0. ? ITEM_SPAWNER_2 : ITEM_SPAWNER;
    }

 	fragColor = map;
}
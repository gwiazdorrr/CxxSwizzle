// Based on "Pixel Dungeon" (http://pixeldungeon.watabou.ru/)

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

vec2 FrameOffset( float frame, float tick )
{
    vec2 ret = vec2( 0. );
    ret.x = frame == 1. ? 1. : ( frame == 2. ? -1. : 0. );
    ret.y = frame == 3. ? 1. : ( frame == 4. ? -1. : 0. );
    return floor( 16. * ret * ( tick / TICK_NUM ) );
}

float EnemyHP( float id )
{
    return 8. + id * 15.;
}

float MaxXP( float level )
{
	return 10. + level * 5.;	   
}

float MaxHP( float level )
{
	return 21. + level * 3.;	   
}

float TextSDF( vec2 p, float glyph )
{
    p = abs( p.x - .5 ) > .5 || abs( p.y - .5 ) > .5 ? vec2( 0. ) : p;
    return texture( iChannel3, p / 16. + fract( vec2( glyph, 15. - floor( glyph / 16. ) ) / 16. ) ).w - 127. / 255.;
}

void PrintChar( inout float sdf, inout vec2 p, float c )
{
    p.x -= 4.;
    sdf = min( sdf, TextSDF( p * .1, c ) );
}

void PrintVal( inout float sdf, inout vec2 p, float val )
{
    if ( val > 9. )
    {
        p.x -= 4.;
        float d = floor( val * 0.1 );
        sdf = min( sdf, TextSDF( p * .1, 48. + d ) );
        val -= d * 10.;
    }
    
    p.x -= 4.;
	sdf = min( sdf, TextSDF( p * .1, 48. + val ) );
}

void RastText( inout vec3 color, float t, float l, vec3 textColor )
{
    float alpha = Smooth( 1. - ( 2. * l - 1. ) );
    color = mix( color, vec3( 0. ), saturate( exp( -t * 20. ) ) * alpha );
    color = mix( color, textColor, Smooth( -t * 100. ) * alpha );    
}

void DrawText( inout vec3 color, vec2 edge, vec2 center, vec2 world, in GameState s )
{
    // xp
    if ( s.logPos[ 0 ].x > 0. )
    {
        float t = 1e4;
        
        vec2 p = world;
        p -= s.logPos[ 0 ] * 16.;
        p.x += 8.;
        p.y -= s.logLife[ 0 ] * 16.;
        PrintChar( t, p, 43. );
        PrintVal( t, p, s.logVal[ 0 ] );
        PrintChar( t, p, 69. );
        PrintChar( t, p, 88. );
        PrintChar( t, p, 80. );
        
		if ( s.logId[ 0 ] > 0. )
        {
            p = world;
            p -= s.logPos[ 0 ] * 16.;
            p.x += 16.;
            p.y -= s.logLife[ 0 ] * 16. - 8.;
           	PrintChar( t, p, 76. );
            PrintChar( t, p, 69. );
            PrintChar( t, p, 86. );
            PrintChar( t, p, 69. );
            PrintChar( t, p, 76. );
            PrintChar( t, p, 32. );
            PrintChar( t, p, 85. );
            PrintChar( t, p, 80. );
            PrintChar( t, p, 33. );
        }
        
        RastText( color, t, s.logLife[ 0 ], vec3( 1., 1., 0. ) );
    }    
    
    // heal
    if ( s.logPos[ 1 ].x > 0. )
    {
        float t = 1e4; 
        vec2 p = world;
        p -= s.logPos[ 1 ] * 16.;
        p.x += 8.;
        p.y -= s.logLife[ 1 ] * 16.;      
        PrintChar( t, p, 43. );
        PrintVal( t, p, s.logVal[ 1 ] );
        PrintChar( t, p, 72. );
        PrintChar( t, p, 80. );
        RastText( color, t, s.logLife[ 1 ], vec3( 0., 1., 0. ) ); 
    }
    
    // dmg
    for ( int i = 2; i < LOG_NUM; ++i )
    {
		float t = 1e4;        
        
        if ( s.logPos[ i ].x > 0. )
        {
            vec2 p = world;
            p -= s.logPos[ i ] * 16.;
            p.y -= s.logLife[ i ] * 16.;        
            PrintVal( t, p, s.logVal[ i ] );
        }
        
        RastText( color, t, s.logLife[ i ], vec3( 1., 0., 0. ) );     
    }
    
    // game over
    if ( s.state == STATE_GAME_OVER )
    {      
        float alpha = Smooth( ( s.stateTime - 0.33 ) * 4. );
        
        color = mix( color, color.yyy * .5, alpha );
        
        float t = 1e4; 
        
        vec2 p = .25 * center;
        p.x += 24.;
        p.y += 6.;
        PrintChar( t, p, 89. );
        PrintChar( t, p, 79. );
        PrintChar( t, p, 85. );
        p.x -= 4.;
        PrintChar( t, p, 68. );
        PrintChar( t, p, 73. );
        PrintChar( t, p, 69. );
        PrintChar( t, p, 68. );
        
        RastText( color, t, 1. - alpha, vec3( 1., 0., 0. ) );     
    }
    
    // level
    vec2 p = edge + vec2( 2.2, 20.8 );
    float t = 1e4;
    PrintChar( t, p, 48. + s.level + 1. );
    color = mix( color, vec3( 1. ), Smooth( -t * 100. ) ); 
}

float Rectangle( vec2 p, vec2 b )
{
    vec2 d = abs( p ) - b;
    return min( max( d.x, d.y ), 0. ) + length( max( d, 0. ) );
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
    vec2 edgeFlt	= fragCoord.xy * resRcp - vec2( 0., screenSize.y );
    vec2 centerFlt	= fragCoord.xy * resRcp - screenSize / 2.;
    vec2 worldFlt	= fragCoord.xy * resRcp - vec2( 0., 16. ) + camera;
    float fog		= texture( iChannel1, ( tile + worldMod16 / 16. ) / iChannelResolution[ 0 ].xy ).w;
    float tick2     = s.tick > TICK_NUM / 2. ? 1. : 0.;
    
    vec3 color = texelFetch( iChannel2, ivec2( fragCoord ), 0 ).xyz;
    
    color *= fog;    
    float light = length( playerPos + 8. - world );
	color *= vec3( Smooth( ( 4. * 16. - light ) * .05 ) * .8 + .2 );
    
    for ( int i = 0; i < ENEMY_NUM; ++i )
    {
        float maxHP = EnemyHP( s.enemyId[ i ] );
		vec2 enemy = world - s.enemyPos[ i ] * 16. + FrameOffset( s.enemyFrame[ i ], s.tick );        
        if ( s.enemyPos[ i ].x > 0.
             && s.enemyHP[ i ] < maxHP
             && enemy.x >= 0. && enemy.x < 16. && enemy.y - 16. >= 0. && enemy.y - 16. < 2. )
        {
            color = enemy.x < 16. * s.enemyHP[ i ] / maxHP ? vec3( 0., 1., 0. ) : vec3( 1., 0., 0. );
        }
    }
    
	if ( pixel.x >= screenSize.x - 8. - 16. * s.keyNum && pixel.x < screenSize.x - 8. )
    {
        SpriteKey( color, vec2( mod( pixel.x - 8., 16. ), pixel.y - screenSize.y + 24. ) );
    }
    
    if ( pixel.y >= screenSize.y - 9. && pixel.x < 4. * MaxHP( s.level ) + 7. + pixel.y - screenSize.y )
    {
       	color = vec3( .33, .35, .31 );
    }
    if ( pixel.y >= screenSize.y - 8. && pixel.x < 4. * MaxHP( s.level ) + 6. + pixel.y - screenSize.y )
    {
       	color = vec3( .64, .65, .58 );
    }
    if ( pixel.y >= screenSize.y - 7. && pixel.x < 4. * MaxHP( s.level ) + 5. + pixel.y - screenSize.y )
    {
       	color = vec3( .24, .25, .22 );
    }    
    if ( pixel.y >= screenSize.y - 3. )
    {
  		color = vec3( .33, .35, .31 );
    }
    if ( pixel.y >= screenSize.y - 2. )
    {
  		color = vec3( .48, .5, .45 );
    }    
    if ( pixel.y >= screenSize.y - 1. )
    {
        color = vec3( .24, .25, .22 );
    }
    
    float rect = floor( Rectangle( pixel - vec2( 6., screenSize.y - 16. ), vec2( 5. ) ) );
    color = rect == 0. ? vec3( .64, .65, .58 ) : color;
    color = rect == 1. ? vec3( .33, .35, .31 ) : color;
    color = rect <  0. ? vec3( .24, .25, .22 ) : color;
    
    float xpBar = s.xp / MaxXP( s.level );
    if ( pixel.y >= screenSize.y - 1. && pixel.x < screenSize.x * xpBar )
    {
        color = mix( vec3( 1., .8, .4 ), vec3( 1. ), pixel.x / screenSize.x );
    } 
    
    if ( pixel.y >= screenSize.y - 7. && pixel.y < screenSize.y - 3. && pixel.x < 4. * s.hp + 5. + pixel.y - screenSize.y )
    {
       color = mix( vec3( .65, .22, .29 ), vec3( .9, .4, .36 ), ( pixel.y - screenSize.y + 5. ) / 3. );
    }
    
    if ( pixel.x == 0. && pixel.y >= screenSize.y - 9. && pixel.y < screenSize.y - 2. )
    {
       	color = vec3( .33, .35, .31 );
    }
    
    DrawText( color, edgeFlt, centerFlt, worldFlt, s );
    
	fragColor = vec4( color, 1. );
}
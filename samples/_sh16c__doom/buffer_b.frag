// Sprite Sheet

// Pickup / Enemy Sprites

//////////////////////////////////////////////////////////////

#define NO_UNROLL(X) (X + min(0,iFrame))
#define NO_UNROLLU(X) (X + uint(min(0,iFrame)))


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
    
CXX_IGNORE(SceneResult Scene_GetDistance( const vec3 vPos );    )

vec3 Scene_GetNormal(const in vec3 vPos)
{
    const float fDelta = 0.01;
    
#if 1    
    // dont unroll normal calculation ( from https://www.shadertoy.com/view/ldd3DX )
    
    vec3 n = vec3(0.0);
    for( int i=0; i<NO_UNROLL(4); i++ )
    {
        vec3 e = 0.5773*(2.0*vec3((((i+3)>>1)&1),((i>>1)&1),(i&1))-1.0);
        n += e*Scene_GetDistance(vPos+e*fDelta).fDist;
    }
    
    return normalize( n );
#else
    
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
#endif
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


vec4 Raymarch_GetColor( vec3 vRayOrigin, vec3 vRayDir )
{
    vec4 vColor = vec4(0);
    
    return vColor;
}

struct PoseLeg
{
    vec3 vHip;
    vec3 vKnee;
    vec3 vAnkle;
    vec3 vToe;
};

struct PoseArm
{
    vec3 vShoulder;
    vec3 vElbow;
    vec3 vWrist;
    vec3 vHand;
};

struct Pose
{
    PoseLeg leftLeg;
    PoseLeg rightLeg;
    PoseArm leftArm;
    PoseArm rightArm;
    
    vec3 vHeadUp;
    vec3 vHeadFd;
};
    
struct CharDef
{
    float fToe;
    float fAnkle;
    float fKnee;
    float fHip;
    
    float fShoulder;
    float fElbow;
    float fWrist;
    float fHand;
    
    float fHead1;
    float fHead2;
        
    vec3 vCol;

    vec3 vUniformColor0;
    vec3 vUniformColor1;
    vec3 vBootsColor;
    vec3 vSkinColor;

    vec3 vEyeColor;
    vec3 vHairColor;
};

void CharDef_Default( inout CharDef charDef )
{    
    charDef.fToe = 2.5;
    charDef.fAnkle = 3.0;
    charDef.fKnee = 3.0;
    charDef.fHip = 4.0;
    
    charDef.fShoulder = 3.0;
    charDef.fElbow = 2.5;
    charDef.fWrist = 2.4;
    charDef.fHand = 2.5;
    
    charDef.fHead1 = 4.0;
    charDef.fHead2 = 3.0;    
    
    charDef.vCol = vec3( 0.4, 1.0, 0.2 );
    charDef.vEyeColor = vec3( 1, 0, 0 );
    
    charDef.vUniformColor0 = vec3( 0, 1, 0 );
    charDef.vUniformColor1 = vec3( 0, 0, 1 );
    charDef.vBootsColor = vec3(0.5);
    charDef.vSkinColor = vec3(239, 163, 115) / 255.;    
    charDef.vHairColor = vec3( 0 );
}

void CharDef_Imp( inout CharDef charDef )
{    
    CharDef_Default(charDef);
    
    float fExtra = 1.0;
    
    charDef.fToe += fExtra;
    charDef.fAnkle += fExtra;
    charDef.fKnee += fExtra;
    charDef.fHip += fExtra;
    
    charDef.fShoulder += fExtra;
    charDef.fElbow += fExtra;
    charDef.fWrist += fExtra;
    charDef.fHand += fExtra;
    
    charDef.fHead1 += 0.4;
    charDef.fHead2 += 0.4;  
    
    charDef.vCol = vec3( 1.0, 0.5, 0.2 );
    charDef.vUniformColor0 = charDef.vCol;
	charDef.vUniformColor1 = charDef.vCol * 0.7;
	charDef.vSkinColor = charDef.vCol * 1.2;
    charDef.vBootsColor = charDef.vCol * 0.5;
    charDef.vHairColor = charDef.vSkinColor;

    charDef.vEyeColor = vec3( 1.0, 0.9, .1 ) * 10.;
}

void CharDef_Trooper( inout CharDef charDef )
{    
    CharDef_Default(charDef);

    float fExtra = 0.1;
    
    charDef.fToe += fExtra;
    charDef.fAnkle += fExtra;
    charDef.fKnee += fExtra;
    charDef.fHip += fExtra;
    
    charDef.fShoulder += fExtra;
    charDef.fElbow += fExtra;
    charDef.fWrist += fExtra;
    charDef.fHand += fExtra;
    
    charDef.fHead1 = 3.5;
    charDef.fHead2 = 3.2;      

    charDef.vUniformColor0 = vec3( 87, 67, 44 ) / 255.;
    charDef.vUniformColor1 = 1.5 * vec3( 90, 71, 49 ) / 255.;
    charDef.vBootsColor = 1.5 * vec3(63,71,43) / 255.;

    charDef.vHairColor = vec3( 67, 147, 55) / 255.;

    charDef.fHead1 = 3.5;
    charDef.fHead2 = 3.2;      
}

void CharDef_Sergeant( inout CharDef charDef )
{    
    CharDef_Default(charDef);

    float fExtra = 0.2;
    
    charDef.fToe += fExtra;
    charDef.fAnkle += fExtra;
    charDef.fKnee += fExtra;
    charDef.fHip += fExtra;
    
    charDef.fShoulder += fExtra;
    charDef.fElbow += fExtra;
    charDef.fWrist += fExtra;
    charDef.fHand += fExtra;
    
    charDef.fHead1 = 3.5;
    charDef.fHead2 = 3.2;      

    charDef.vCol = vec3( 0.3, 0.3, 0.3 );

    charDef.vUniformColor0 = vec3( 0.3, .3, .3 );
    charDef.vUniformColor1 = vec3( 0.1, 0.1, 0.1 );
    charDef.vBootsColor = vec3(0.5,0,0);

    charDef.vHairColor = vec3( 179, 115, 71) / 255.;
}

struct Scene
{
    float fCameraRotation;
    vec3 vLightDir;
    vec3 vLightColor;
    vec3 vAmbientLight;
    
    bool bArmor;
    bool bCharacter;
    
    CharDef charDef;
    Pose pose;
    
    vec3 vWeaponStart;
    vec3 vWeaponDir;
    
};
    
Scene g_scene;

void Pose_MirrorLeft( inout Pose pose )
{
    vec3 vMirror = vec3( -1.0, 1, 1 );
    pose.rightLeg.vHip = pose.leftLeg.vHip * vMirror;
    pose.rightLeg.vKnee = pose.leftLeg.vKnee * vMirror;
    pose.rightLeg.vAnkle = pose.leftLeg.vAnkle * vMirror;
    pose.rightLeg.vToe = pose.leftLeg.vToe * vMirror;

    pose.rightArm.vShoulder = pose.leftArm.vShoulder * vMirror;
    pose.rightArm.vElbow = pose.leftArm.vElbow * vMirror;
    pose.rightArm.vWrist = pose.leftArm.vWrist * vMirror;
    pose.rightArm.vHand = pose.leftArm.vHand * vMirror;
}


void Pose_Flip( inout Pose pose )
{
    vec3 vTemp;
    vec3 vMirror = vec3( -1.0, 1, 1 );
        
	#define SWAP_FLIP(X,Y) vTemp=X;X=Y*vMirror; Y=vTemp*vMirror;
    
    SWAP_FLIP(pose.rightLeg.vHip, pose.leftLeg.vHip);
    SWAP_FLIP(pose.rightLeg.vKnee, pose.leftLeg.vKnee);
    SWAP_FLIP(pose.rightLeg.vAnkle, pose.leftLeg.vAnkle);
    SWAP_FLIP(pose.rightLeg.vToe, pose.leftLeg.vToe);

    SWAP_FLIP(pose.rightArm.vShoulder, pose.leftArm.vShoulder);
    SWAP_FLIP(pose.rightArm.vElbow, pose.leftArm.vElbow);
    SWAP_FLIP(pose.rightArm.vWrist, pose.leftArm.vWrist);
    SWAP_FLIP(pose.rightArm.vHand, pose.leftArm.vHand);
    
    pose.vHeadUp *= vMirror;
    pose.vHeadFd *= vMirror;
}

void Pose_Clear( inout Pose pose )
{
    pose.leftLeg.vHip = vec3( 2.0, 24.0, 1.0 );
    pose.leftLeg.vKnee = vec3( 5.0, 14.0, -2.0 );
    pose.leftLeg.vAnkle = vec3( 5.0, 3.0, -0.0 );
    pose.leftLeg.vToe = vec3( 5.0, 3.0, -4.0 );

    pose.leftArm.vShoulder = vec3( 3.5, 40.0, 0.0 );
    pose.leftArm.vElbow = vec3( 8.0, 30.0, 1.0 );
    pose.leftArm.vWrist = vec3( 9.0, 28.0, -4.0 );
    pose.leftArm.vHand = vec3( 10.0, 28.0, -5.0 );
    
    Pose_MirrorLeft( pose );
    
    pose.vHeadUp = vec3(0, 1, 0);
    pose.vHeadFd = vec3(0, 0, -1);
}

void Pose_Attack( inout Pose pose )
{
    pose.leftLeg.vHip = vec3( 2.0, 24.0, 1.0 );
    pose.leftLeg.vKnee = vec3( 5.0, 14.0, -2.0 );
    pose.leftLeg.vAnkle = vec3( 5.0, 3.0, -0.0 );
    pose.leftLeg.vToe = vec3( 5.0, 3.0, -2.0 );

    pose.leftArm.vShoulder = vec3( 3.5, 40.0, -1.0 );
    pose.leftArm.vElbow = vec3( 8.0, 34.0, -5.0 );
    pose.leftArm.vWrist = vec3( 1.2, 37.0, -10.0 );
    pose.leftArm.vHand = vec3( 0.0, 37.0, -10.0 );
    
    Pose_MirrorLeft( pose );
        
    pose.leftLeg.vHip.z = 3.0;
    pose.leftLeg.vKnee.z = 5.0;
    pose.leftLeg.vAnkle.z = 8.0;
    pose.leftLeg.vAnkle.y = 4.0;
    pose.leftLeg.vToe.z = 5.0;

    pose.rightLeg.vHip.z = 1.0;
    pose.rightLeg.vKnee.z = -5.0;
    pose.rightLeg.vAnkle.z = -8.0;
    pose.rightLeg.vToe.z = -7.0;

    pose.leftArm.vShoulder.z += 1.;
    pose.rightArm.vShoulder.z -= 1.;

    pose.leftArm.vHand.y += 1.8;
    pose.leftArm.vHand.z -= 3.;
    pose.leftArm.vWrist.z -= 3.;
    pose.rightArm.vHand.z += 1.;
    pose.rightArm.vWrist.z += 1.;
    
    pose.vHeadUp = vec3(0, 1, -.5);
    pose.vHeadFd = vec3(0, 0, -1);    
}

void Pose_Walk1( inout Pose pose )
{
    pose.leftLeg.vHip = vec3( 2.0, 24.0, 1.0 );
    pose.leftLeg.vKnee = vec3( 5.0, 14.0, -4.0 );
    pose.leftLeg.vAnkle = vec3( 5.0, 8.0, 7.0 );
    pose.leftLeg.vToe = vec3( 5.0, 4.0, 6.0 );

    pose.rightLeg.vHip = vec3( -2.0, 24.0, 1.0 );
    pose.rightLeg.vKnee = vec3( -5.0, 14.0, -2.0 );
    pose.rightLeg.vAnkle = vec3( -5.0, 3.0, 2.0 );
    pose.rightLeg.vToe = vec3( -5.0, 2.0, -2.0 );

    pose.leftArm.vShoulder.z += 1.;
    pose.rightArm.vShoulder.z -= 1.;
    
    pose.vHeadUp = vec3(0, 1, -.5);
    pose.vHeadFd = vec3(0.1, 0, -1);    
    
    
    pose.rightArm.vShoulder = vec3( -3.5, 40.0, 1.0 );
    pose.rightArm.vElbow = vec3( -8.0, 30.0, -0.0 );
    pose.rightArm.vWrist = vec3( -9.0, 24.0, -8.0 );
    pose.rightArm.vHand = vec3( -9.0, 23.0, -9.0 );    

    pose.leftArm.vShoulder = vec3( 3.5, 40.0, -1.0 );
    pose.leftArm.vElbow = vec3( 8.0, 30.0, 2.0 );
    pose.leftArm.vWrist = vec3( 9.0, 24.0, -4.0 );
    pose.leftArm.vHand = vec3( 9.0, 23.0, -6.0 );
}

void Pose_Walk2( inout Pose pose )
{
    Pose_Walk1( pose );
    Pose_Flip( pose );
}

void Pose_Pain( inout Pose pose )
{
    pose.leftLeg.vHip = vec3( 2.0, 24.0, 1.0 );
    pose.leftLeg.vKnee = vec3( 4.0, 15.0, -2.0 );
    pose.leftLeg.vAnkle = vec3( 4.0, 4.0, -0.0 );
    pose.leftLeg.vToe = vec3( 4.0, 3.0, -4.0 );

    pose.leftArm.vShoulder = vec3( 3.5, 36.0, 6.0 );
    pose.leftArm.vElbow = vec3(10.0, 27.0, 4.0 );
    pose.leftArm.vWrist = vec3( 12.0, 25.0, -6.0 );
    pose.leftArm.vHand = vec3( 12.5, 26.0, -6.0 );
    
    Pose_MirrorLeft( pose );

    pose.rightLeg.vHip = vec3( -2.0, 24.0, 1.0 );
    pose.rightLeg.vKnee = vec3( -4.0, 15.0, 2.0 );
    pose.rightLeg.vAnkle = vec3( -4.0, 4.0, 3.0 );
    pose.rightLeg.vToe = vec3( -4.0, 3.0, -2.0 );

    
    pose.vHeadUp = vec3(0, 1, .5);
    pose.vHeadFd = vec3(0, .5, -1);
    
}

void Pose_Die( inout Pose pose )
{
    pose.leftLeg.vHip = vec3( 2.0, 20.0, -3.0 );
    pose.leftLeg.vKnee = vec3( 4.0, 12.0, -10.0 );
    pose.leftLeg.vAnkle = vec3( 4.0, 4.0, -6.0 );
    pose.leftLeg.vToe = vec3( 4.0, 3.0, -11.0 );

    pose.leftArm.vShoulder = vec3( 3.5, 28.0, 9.0 );
    pose.leftArm.vElbow = vec3(10.0, 23.0, 4.0 );
    pose.leftArm.vWrist = vec3( 9.0, 28.0, -6.0 );
    pose.leftArm.vHand = vec3( 8.5, 29.0, -6.0 );
    
    Pose_MirrorLeft( pose );

    pose.leftArm.vWrist.y -= 4.0;
    pose.leftArm.vHand.y -= 5.0;
    pose.leftArm.vHand.z -= 2.0;
    
    pose.rightArm.vHand.z -= 2.0;
    
    pose.rightLeg.vKnee.x -= 3.0;
        
    pose.vHeadUp = vec3(0, 1, .6);
    pose.vHeadFd = vec3(0, .5, -1);
    
}

void Pose_Weapon()
{
    
    g_scene.vWeaponStart = g_scene.pose.rightArm.vHand + vec3( 0, 2, 0);
    g_scene.vWeaponDir = normalize( g_scene.pose.leftArm.vHand - g_scene.vWeaponStart );
}

void Weapon_None()
{
	g_scene.vWeaponStart = vec3(-10,-100,0);
    g_scene.vWeaponDir = vec3(1, 0, 0);
}

void Weapon_DefaultPos()
{
	g_scene.vWeaponStart = vec3(-10,6,0);
    g_scene.vWeaponDir = vec3(1, 0, 0);
    
}


void Pose_Dead( inout Pose pose )
{
    float dy = 4.0;
    pose.leftLeg.vHip = vec3( 2.0, dy + 3.0, 1.0 );
    pose.leftLeg.vKnee = vec3( 4.0, dy + 4.0, -2.0 );
    pose.leftLeg.vAnkle = vec3( 15.0, dy + 7.0, -0.0 );
    pose.leftLeg.vToe = vec3( 6.0, dy + 4.0, -2.0 );

    pose.leftArm.vShoulder = vec3( 3.5, dy + 5.0, 0.0 );
    pose.leftArm.vElbow = vec3( 4.0, dy + 4.0, 1.0 );
    pose.leftArm.vWrist = vec3( 3.0, dy + 6.0, -4.0 );
    pose.leftArm.vHand = vec3( 5.0, dy + 4.0, -5.0 );
    
    Pose_MirrorLeft( pose );

    pose.leftArm.vElbow.y += 5.;
    pose.leftLeg.vKnee.y += 4.;
    
    pose.vHeadUp = vec3(1, 0.3, 0);
    pose.vHeadFd = vec3(0, 0, -1);
    
    Weapon_None();
}

void Scene_Clear()
{
    g_scene.vLightDir = normalize( vec3(0, 1, -0.3) );
    g_scene.vLightColor = vec3(1, 1, 1);
    g_scene.vAmbientLight = vec3(0.4);
    
    g_scene.bCharacter = false;
    g_scene.bArmor = false;
        
    g_scene.fCameraRotation = 0.0;
    
	
    Weapon_None();
    
    Pose_Clear( g_scene.pose );
    
    CharDef_Default( g_scene.charDef );
}



CXX_CONST float kMaxTraceDist = 1000.0;

#define MAT_BG 0.0
#define MAT_CHARACTER 1.0
#define MAT_WHITE 2.0
#define MAT_GREY 3.0
#define MAT_SHOTGUN 4.0
#define MAT_WOOD 5.0
#define MAT_HEAD 6.0
#define MAT_EYE 7.0

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

vec3 Segment3( vec3 vPos, vec3 vP0, vec3 vP1, float r0, float r1 )
{
    //return vec3( Segment( vPos, vP0, vP1, r0, r1 ) );
    vec3 vDir = normalize(vP1 - vP0);
    float len = length( vP1 - vP0 );
    float t = dot( vDir, vPos - vP0 ) / len;
    float clamped_t = clamp( t, 0.0, 1.0 );

    vec3 vClosestOnLine = mix( vP0, vP1, clamped_t);
    vec3 vDirToClosest = vPos - vClosestOnLine;
    float fClosestDist = length( vDirToClosest );
    
    float d = fClosestDist - mix(r0, r1, t);

    vec3 fd = vec3(1,0,0);
    
    return vec3( d, t, dot( fd, normalize(vDirToClosest))  );
}


/*
float Limb( vec3 vPos, vec3 vP0, vec3 vP1, vec3 vPerpDir, float l1, float l2, float rStart, float rMid, float rEnd )
{
    vec3 vStartToEnd = vP1 - vP0;
    vec3 vDir = normalize( vStartToEnd );
    
    vec3 vTemp = cross( vDir, vPerpDir);
    vec3 vPerDir = normalize( cross( vDir, vTemp) );
    
    float d = length( vStartToEnd );
    
    float d1 = (l1 * l1 - l2 *  l2 + d * d) / ( 2.0 * d );
    float d2 = d - d1;
    
    float h = sqrt( l1 * l1 - d1 * d1 );

    vec3 vMidPoint = vP0 + vDir * d1 + vPerpDir * h;
    
    return LimbSegments();
    float fSD1 = Segment( vPos, vP0, vMidPoint, rStart, rMid );
    float fSD2 = Segment( vPos, vMidPoint, vP1, rMid, rEnd );

    return min( fSD1, fSD2 );
}
*/
float SmoothMin( float a, float b, float k )
{
	//return min(a,b);
	
	
    //float k = 0.06;
	float h = clamp( 0.5 + 0.5*(b-a)/k, 0.0, 1.0 );
	return mix( b, a, h ) - k*h*(1.0-h);
}

vec3 SmoothMin3( vec3 a, vec3 b, float k )
{
	//return min(a,b);
	
	
    //float k = 0.06;
	float h = clamp( 0.5 + 0.5*(b.x-a.x)/k, 0.0, 1.0 );
	return mix( b, a, h ) - k*h*(1.0-h);
}

vec3 LimbCombine( vec3 fA, vec3 fB )
{
    return SmoothMin3( fA, fB, 0.5);
    //return min( fA, fB );
}

vec3 Chain( vec3 vPos, vec4 vA, vec4 vB, vec4 vC, vec4 vD, vec3 vWeights )
{
    vec3 vD1 = Segment3( vPos, vA.xyz, vB.xyz, vA.w, vB.w );
    vec3 vD2 = Segment3( vPos, vB.xyz, vC.xyz, vB.w, vC.w );
    vec3 vD3 = Segment3( vPos, vC.xyz, vD.xyz, vC.w, vD.w );
    
    vD1.y = vD1.y * vWeights.x;
    vD2.y = vWeights.x + vD2.y * vWeights.y;
    vD3.y = vWeights.x + vWeights.y + vD3.y * vWeights.z;
    
    vec3 vResult = LimbCombine( vD1, vD2 );    
    vResult = LimbCombine( vResult, vD3 );    
    return vResult;
}

vec3 LegDist( vec3 vPos, PoseLeg leg )
{    
    return Chain( 
        vPos,
        vec4(leg.vToe, g_scene.charDef.fToe),
        vec4(leg.vAnkle, g_scene.charDef.fAnkle),
        vec4(leg.vKnee, g_scene.charDef.fKnee), 
        vec4(leg.vHip, g_scene.charDef.fHip), 
        vec3(0.1, 0.4, 0.5 )
    );
}

vec3 ArmDist( vec3 vPos, PoseArm arm )
{
    return Chain( 
        vPos,
        vec4(arm.vShoulder, g_scene.charDef.fShoulder), 
        vec4(arm.vElbow, g_scene.charDef.fElbow), 
        vec4(arm.vWrist, g_scene.charDef.fWrist),
        vec4(arm.vHand, g_scene.charDef.fHand),
        vec3(0.5, 0.4, 0.1 )
    );    
}


vec3 TorsoDistance( vec3 vPos, vec3 vA, vec3 vB, vec3 vC, vec3 vD, float fSizeTop, float fSizeBase )
{    
    vec3 vMidBase = (vA + vB) * 0.5;
    vec3 vMidTop = (vC + vD) * 0.5;
    
    vec3 vAxis = vMidTop - vMidBase;
    float fAxisLen = length( vAxis );
    vec3 vAxisDir = normalize( vAxis );
    
    float fProjection = dot( vAxisDir, vPos - vMidBase );
    
    float fNormProj = fProjection / fAxisLen;
    float fNormProjClamped = clamp( fNormProj, 0., 1. );
    
    //vec3 vProjected = vMidBase + vAxisDir * clamp( fProjection, 0., fAxisLen );
    
    vec3 vSpinePos = mix( vMidBase, vMidTop, fNormProjClamped );

    vec3 vPerpBase = (vB - vA) * 0.5;
    vec3 vPerpTop = (vD - vC) * 0.5;
    
    vec3 vPerp = mix( vPerpBase, vPerpTop, fNormProj );
    vec3 vPerpDir = normalize( vPerp );
    
    float fSideLen = length( vPerp );
    
    float fPerpProj = dot( vPerpDir, vPos - vSpinePos );
    float fPerpProjClamped = clamp( fPerpProj, -fSideLen, fSideLen );
    
	vec3 vBackPlanePos = vSpinePos + vPerpDir * fPerpProjClamped;
    
    float fSize = mix( fSizeBase, fSizeTop, fNormProjClamped );
    
    //float fNormPerpProj = fPerpProj / length(vPerp);
    //float fNormPerpProjClamped = clamp( fNormPerpProj, -1., 1.);
    
    //vec3 vSidePos = vSpinePos + vPerp;
    //vec3 vBackPlanePos = mix( vSpinePos, vSidePos, fNormPerpProjClamped );
    
    
    float fDistance = length(vPos - vBackPlanePos) - fSize;
    
    return vec3( fDistance, fNormProj, fPerpProj / (fSideLen * 2.0) + 0.5);
}

float BodyCombine( float fA, float fB )
{
    return SmoothMin( fA, fB, 0.5);
    //return min( fA, fB );
}
vec3 BodyCombine3( vec3 fA, vec3 fB )
{
    return SmoothMin3( fA, fB, 0.5);
    //return min( fA, fB );
}

SceneResult Character_GetDistance( vec3 vPos )
{
    SceneResult result = SceneResult( kMaxTraceDist, MAT_BG, vec3(0.0) );


    vec3 vLeftLeg = LegDist( vPos, g_scene.pose.leftLeg );
    vec3 vRightLeg = LegDist( vPos, g_scene.pose.rightLeg );
    vec3 vLeftArm = ArmDist( vPos, g_scene.pose.leftArm );
    vec3 vRightArm = ArmDist( vPos, g_scene.pose.rightArm );
    vec3 vTorsoDist = TorsoDistance( 
        vPos,
        g_scene.pose.leftLeg.vHip, 
        g_scene.pose.rightLeg.vHip,
        g_scene.pose.leftArm.vShoulder,
        g_scene.pose.rightArm.vShoulder,
    	g_scene.charDef.fShoulder, g_scene.charDef.fHip);
        
    vTorsoDist.y += 1.0;
    vLeftArm.y += 2.0;
    vRightArm.y += 2.0;            
    vLeftArm.z += 1.0;
    vLeftLeg.z += 1.0;
        
    vec3 vLimbDist = vec3(10000.0);
    vLimbDist = BodyCombine3( vLimbDist, vLeftLeg );
    vLimbDist = BodyCombine3( vLimbDist, vRightLeg );
    vLimbDist = BodyCombine3( vLimbDist, vLeftArm );
    vLimbDist = BodyCombine3( vLimbDist, vRightArm );        
    vec3 vCharacterDist = BodyCombine3( vLimbDist, vTorsoDist );

    //vCharacterDist.x -= fbm( vLimbDist.xy * 10., 0.9 ) * 2.0;
    
    float fNeckSize = 1.0;
    float fNeckLen = 3.0;

    vec3 vNeckBase = (g_scene.pose.leftArm.vShoulder + g_scene.pose.rightArm.vShoulder) * 0.5;
    vec3 vNeckTop = vNeckBase + g_scene.pose.vHeadUp * fNeckLen;
    vec3 vNeckDist = Segment3( vPos, vNeckBase, vNeckTop, fNeckSize, fNeckSize );
    
    float fHead1 = g_scene.charDef.fHead1;
    float fHead2 = g_scene.charDef.fHead2;
    float fHeadTop = 6.0;
    float fHeadChin = 2.0;
    
    vec3 vHeadBase = vNeckBase + g_scene.pose.vHeadUp * ( fHeadChin + fHead2);
    vec3 vHead2 = vHeadBase  + g_scene.pose.vHeadFd * (fHead2 * .5);
    vec3 vHead1 = vHeadBase + g_scene.pose.vHeadUp * (fHeadTop - fHead1);
    
    vec3 vHeadDist = Segment3( vPos, vHead1, vHead2, fHead1, fHead2 );
    vHeadDist = SmoothMin3( vHeadDist, vNeckDist, 0.5 );

    vec3 vNosePos = vHead1 + g_scene.pose.vHeadFd * fHead1 * 1.2 - g_scene.pose.vHeadUp * 1.5;
    float fNoseDist = length( vPos - vNosePos ) - 1.;
    vHeadDist.x = min( vHeadDist.x, fNoseDist ); // keep material


    vec3 vEyePerp = normalize( cross(g_scene.pose.vHeadFd, g_scene.pose.vHeadUp) );

    vec3 vEyePos1 = vHead1 + g_scene.pose.vHeadFd * fHead1 + vEyePerp * 1.5;
    float fEyeDist1 = length( vPos - vEyePos1 ) - 1.;

    vec3 vEyePos2 = vHead1 + g_scene.pose.vHeadFd * fHead1 - vEyePerp * 1.5;
    float fEyeDist2 = length( vPos - vEyePos2 ) - 1.;
    
    float fEyeDist = min( fEyeDist1, fEyeDist2 );
        
    vHeadDist.x = max( vHeadDist.x, -(fEyeDist - 0.2) );
    
    
    //vCharacterDist = BodyCombine3( vCharacterDist, vHeadDist );
    result = Scene_Union( result, SceneResult( vHeadDist.x, MAT_HEAD, vHeadDist.yzz ) );    
    
    
    
    result = Scene_Union( result, SceneResult( fEyeDist, MAT_EYE, g_scene.charDef.vCol ) );    
        
    result = Scene_Union( result, SceneResult( vCharacterDist.x, MAT_CHARACTER, vCharacterDist.yzz ) );    

         
    return result;
}


SceneResult Armor_GetDistance( vec3 vPos )
{
    vPos.x = abs(vPos.x);

    vPos.y += 1.;
    
    float fScale = 1.4;
    vPos /= fScale;
    
    float fDist1 = Segment( vPos, vec3(0,10,2), vec3(6, 9, 0), 4., 4. );
    float fDist2 = Segment( vPos, vec3(0,10,0), vec3(0, 5, 0), 5., 6. );
    float fDist = SmoothMin( fDist1, fDist2, 1.5 );

    float fDistHole1 = Segment( vPos, vec3(0,12,-1), vec3(0, -2, -0.5), 3.5, 4.5 );    
    float fDistHole2 = Segment( vPos, vec3(0,10,2), vec3(8, 8, -1), 3.0, 3.0 );
    float fDistHole = SmoothMin( fDistHole1, fDistHole2, 1.5 );

    SceneResult result = SceneResult( fDist, MAT_WHITE, vec3(0.0) );
        
    result = Scene_Subtract( result, SceneResult( fDistHole, MAT_GREY, vec3(0.0) ));
    
    result.fDist *= fScale;
    

    return result;
}


SceneResult Scene_GetDistance( const vec3 vPos )
{
    SceneResult result = SceneResult( kMaxTraceDist, MAT_BG, vec3(0.0) );
        
	if ( g_scene.bCharacter )
    {
	    result = Scene_Union( result, Character_GetDistance( vPos ) );
    }

    {
/*        float fDist = Segment( vPos, vec3(-10,10,0), vec3(13, 10, 0), 1., 1. );
        result = Scene_Union( result, SceneResult( fDist, MAT_SHOTGUN, g_scene.charDef.vCol ) );    

        float fDist2 = Segment( vPos, vec3(-12,6,0), vec3(-4, 10, 0), 2., 1. );
        result = Scene_Union( result, SceneResult( fDist2, MAT_WOOD, g_scene.charDef.vCol ) );    */
        
        vec3 vStart = g_scene.vWeaponStart;
        vec3 vDir = g_scene.vWeaponDir;
        float fDist = Segment( vPos, vStart, vStart + vDir * 23.0, 1., 1. );
        result = Scene_Union( result, SceneResult( fDist, MAT_SHOTGUN, g_scene.charDef.vCol ) );    

        float fDist2 = Segment( vPos, vStart + vDir * -2. + vec3(0,-4,0), vStart + vDir * 6., 2., 1. );
        result = Scene_Union( result, SceneResult( fDist2, MAT_WOOD, g_scene.charDef.vCol ) );
    }
    
    if ( g_scene.bArmor )
    {        
	    result = Scene_Union( result, Armor_GetDistance( vPos ) );
    }
    
    return result;
}




vec4 Raymarch_Sprite( vec2 fragCoord, vec4 vSpriteInfo )
{
    vec4 vResult = vec4( 0 );
    
    vec2 vSpritePos = fragCoord - vSpriteInfo.xy;
    float fSpriteX = vSpritePos.x - vSpriteInfo.z * 0.5;
    
    float fRotation = g_scene.fCameraRotation;
    
    //fRotation += iTime;
    
    vec3 vCameraPos;
    vCameraPos.x = cos(fRotation) * fSpriteX;
    vCameraPos.y = vSpritePos.y;
    vCameraPos.z = sin(fRotation) * fSpriteX;
    
    vec3 vCameraDir = vec3(-sin(fRotation), 0, cos(fRotation));
    
    vCameraPos -= vCameraDir * 200.0;

	SceneResult sceneResult = Scene_Trace( vCameraPos, vCameraDir, 1000.0 );
    
    if ( sceneResult.fDist > 400.0 )
    {
        return vResult;
    }

    vec3 vHitPos = vCameraPos + vCameraDir * sceneResult.fDist;
    
    vec3 vNormal = Scene_GetNormal( vHitPos );
    
    float fShade = max( 0.0, dot( vNormal, g_scene.vLightDir ) );
    
    float fSpecIntensity = 1.0;
    
    float fFBM1 = fbm( sceneResult.vUVW.xy * 30.0 * vec2(1.0, 0.4), 0.2 );    
    float fFBM2 = fbm( sceneResult.vUVW.xy * 30.0 * vec2(1.0, 0.4) + 5.0, 0.5 );
    vec3 vDiffuseCol = vec3(1.);
    if ( sceneResult.fObjectId == MAT_CHARACTER )
    {
        float fUniformBlend = smoothstep( 0.5, 0.6, fFBM1 );
        
        vDiffuseCol = mix( g_scene.charDef.vUniformColor0, 
                          g_scene.charDef.vUniformColor1, 
                          fUniformBlend );
        
        vDiffuseCol = mix( vDiffuseCol, g_scene.charDef.vSkinColor, step(2.2,sceneResult.vUVW.x) );

        float fBootBlend = step(sceneResult.vUVW.x, .4);
        
        fBootBlend = max( fBootBlend, step( abs(2.5 - sceneResult.vUVW.x), 0.2 ) ); // arm thing
        
        vDiffuseCol = mix( vDiffuseCol, g_scene.charDef.vBootsColor, fBootBlend );
                
        float fGoreBlend = smoothstep( 0.6, 0.7, fFBM2 );
        fGoreBlend = max( fGoreBlend, step(2.9,sceneResult.vUVW.x) ); // bloody hands
        
        vDiffuseCol = mix( vDiffuseCol, 
                          vec3(1,0,0), 
                          fGoreBlend );
        
        //vDiffuseCol = fract(sceneResult.vUVW);//g_scene.charDef.vCol;
        //vDiffuseCol = sceneResult.vUVW.xxx / 5.0;//g_scene.charDef.vCol;
    }
    else if ( sceneResult.fObjectId == MAT_SHOTGUN )
    {
        vDiffuseCol = vec3( 0.2 );
    }
    else if ( sceneResult.fObjectId == MAT_WOOD )
    {
        vDiffuseCol = vec3( 0.4, 0.2, .1 );
    }
    else if ( sceneResult.fObjectId == MAT_HEAD )
    {
        vDiffuseCol = g_scene.charDef.vSkinColor;
        float fHairBlend = step( sceneResult.vUVW.x + fFBM1 * 0.5, 0.1 );
        vDiffuseCol = mix( vDiffuseCol, g_scene.charDef.vHairColor, fHairBlend );
    }
    else if ( sceneResult.fObjectId == MAT_EYE )
    {
        vDiffuseCol = g_scene.charDef.vEyeColor;
    }
    else if ( sceneResult.fObjectId == MAT_GREY )
    {
        vDiffuseCol = vec3( 0.2 );
        fSpecIntensity = 0.1;
    }
    
    vec3 vDiffuseLight = g_scene.vAmbientLight + fShade * g_scene.vLightColor;
    vResult.rgb = vDiffuseCol * vDiffuseLight;
    
    vec3 vRefl = reflect( vec3(0, 0, 1), vNormal );
    float fDot = max(0.0, dot( vRefl, g_scene.vLightDir )) * fShade;
    float fSpec = pow( fDot, 5.0 );
    vResult.rgb += fSpec * fSpecIntensity;
    
    vResult.rgb = 1.0 - exp2( vResult.rgb * -2.0 );
    vResult.rgb = pow( vResult.rgb, vec3(1.0 / 1.5) );
    
    vResult.a = 1.0;
    
    return vResult;
}



//////////////////////////////////////////////////////////////

bool MaskBarrel(vec2 vTexCoord)
{
	vec2 vSize = vec2(23.0, 32.0);
	
	vTexCoord = floor(vTexCoord);
	
	// remove corner pixels
	vec2 vWrapCoord = fract((vTexCoord + vec2(2.0, 1.0) ) / vSize) * vSize;
	
	return ( (vWrapCoord.x >= 4.0) || (vWrapCoord.y >= 2.0) );
}

vec4 TexBar1A( vec2 vTexCoord, float fRandom, float fHRandom )
{
    if ( any( lessThan( vTexCoord, vec2(0) ) ) ) return vec4(0);
    if ( any( greaterThan( vTexCoord, vec2(22,32) ) ) ) return vec4(0);
    vTexCoord.y = 31. - vTexCoord.y;
	vec3 col = vec3(123.0, 127.0, 99.0) / 255.0;
	
	float fBrownStreakBlend = smoothstep( 2.0, 1.0, abs(vTexCoord.x - 3.5));
	col = mix(col, vec3(0.724, 0.736, 0.438), fBrownStreakBlend);
	
	if( (vTexCoord.y == 1.0) && (vTexCoord.x > 3.0) && (vTexCoord.x < 18.0) )
	{
		col = col * clamp(((vTexCoord.x / 18.0)), 0.0, 1.0);		
	}
	else
	{
		col = col * clamp((1.0 - (vTexCoord.x / 18.0)), 0.0, 1.0);		
	}
	
	float fNukageBlend = 0.0;
	if( (vTexCoord.y == 1.0) && (vTexCoord.x > 8.0) && (vTexCoord.x < 14.0) )
	{
		fNukageBlend = 1.0;
	}	
	if( (vTexCoord.y == 2.0) && (vTexCoord.x > 2.0) && (vTexCoord.x < 20.0) )
	{
		fNukageBlend = 1.0;
	}	
	col = mix(col, vec3(0.172, 0.560, 0.144) * fRandom, fNukageBlend);
	
	
	if(vTexCoord.x < 1.0)
	{
		col += 0.1;
	}
	
	float fBlend = clamp(((vTexCoord.x - 20.0) / 3.0), 0.0, 1.0);
	col += fBlend * 0.2;
	
	float fBumpY = 8.0;
	if(vTexCoord.y > 14.0) fBumpY += 9.0;
	if(vTexCoord.y > 23.0) fBumpY += 8.0;

	vec4 vBump = SmoothBump( vTexCoord, vec2(2.0, fBumpY), vec2(23.0 - 2.0, fBumpY), normalize(vec2(-0.2, 1.0)), 1.25 );	
	col += vBump.x * 0.2;

	// rim highlights
	{
		vec2 vOffset = (vTexCoord - vec2(17.0, 0.0)) / vec2(8.0, 2.0);
		col += clamp(1.0 - dot(vOffset, vOffset), 0.0, 1.0) * 0.2;
	}	
	{
		vec2 vOffset = (vTexCoord - vec2(20.0, 1.0)) / vec2(4.0, 1.0);
		col += clamp(1.0 - dot(vOffset, vOffset), 0.0, 1.0) * 0.2;
	}	
	{
		vec2 vOffset = (vTexCoord - vec2(3.0, 2.0)) / vec2(4.0, 2.0);
		col += clamp(1.0 - dot(vOffset, vOffset), 0.0, 1.0) * 0.2;
	}	
	
	col *= 0.5 + fRandom * 0.5;
	
    vec4 vResult;
    vResult.rgb = col;
    vResult.a = MaskBarrel( vTexCoord ) ? 1. : 0.;
    
	return vResult;
}


vec4 CosApprox( vec4 x )
{
	x = abs(fract(x * (0.5))*2.0 - 1.0);
	vec4 x2 = x*x;
	return( ( x2 * 3.0) - ( 2.0 * x2*x) );
}

bool MaskCorpseSprite(vec2 vTexCoord)
{
	//vTexCoord = floor(vTexCoord);
    
    vec2 vUV = vTexCoord.xy / vec2(48.0, 22.0);
    vec2 vOffset = vUV;
    vOffset = vOffset * 2.0 -vec2(1.0, 0.6);
    float fDist = dot(vOffset, vOffset);
    fDist += dot(CosApprox(vTexCoord.xyxy * vec4(0.55, 0.41, 0.25, 0.1)), vec4(0.2 * -vOffset.y));
	return fDist < 0.4;
}

vec4 TexPlayW( vec2 vTexCoord, float fRandom, float fHRandom )
{
    vec3 col = mix(vec3(190.0, 10.0, 10.0), vec3(50, 16.0, 16.0 ), fRandom * vTexCoord.y/18.0) / 255.0;
    
    vec4 vResult = vec4(0);
    vResult.rgb = col;
    vResult.a = MaskCorpseSprite( vTexCoord ) ? 1. : 0.;
    
	return vResult;
}

vec4 TexFireball( vec2 vTexCoord, float fRandom, float fHRandom, float fSize, float fType )
{
    float fLen = length( vTexCoord - vec2(fSize) ) / fSize;
    
    vec4 vResult = vec4(0);
    
    vResult.rgb = mix( vec3( 1., 0.5, 0.4), vec3(0.6, 0.2, 0.05), fLen );//vec3(sin(fLen * 10.0) * 0.5 + 0.5,0,1);
    if ( fType == 0. )
    {
    	vResult.rgb += vec3( 1., 0.3, 0.1) * ( exp( fLen * -3.0)) * 3.;
    }
    else
    {
        fLen += fRandom - 0.25;
    }
    vResult.rgb *= fRandom * 0.5 + 0.5;

    if ( fLen < 1.0 )
    {
        vResult.a = 1.;
    }
    
    return vResult;
}

vec4 TexHealthBonus( vec2 vTexCoord, float fRandom, float fHRandom )
{
    float fLen = length( vTexCoord - vec2(8.0, 6.0) ) / 5.5;
    
    vec4 vResult = vec4(0);
    
    vec3 vCol = vec3(0,0,1);
    if ( all( greaterThan( vTexCoord, vec2( 6, 13) ) ) && all( lessThan( vTexCoord, vec2(10, 16 ) ) ) )
	{
        vCol = vec3(1., .5, .2) * 2.;
	}
    
    float fShade = clamp( vTexCoord.y / 10.0, 0.0, 1.0);
    vResult.rgb = vCol * fShade * fRandom;
    if ( fLen < 1.0 )
    {
        vResult.a = 1.;
    }
    
    if ( all( greaterThan( vTexCoord, vec2( 6, 4) ) ) && all( lessThan( vTexCoord, vec2(10, 16 ) ) ) )
	{
        vResult.a = 1.;
	}
    
    
    return vResult;
}
    
vec4 TexArmorBonus( vec2 vTexCoord, float fRandom, float fHRandom )
{
    vec2 vXY = vTexCoord - vec2(7.5,7.0);

    vec4 vResult = vec4(0);
    
    float fRad = 8.0;
    if ( vXY.y < 0.0 )
    {
        vXY.y += 1.0;
        vXY.x *= 0.9;
    }

    vec3 vCol = mix( vec3(75), vec3(95, 67, 35), step(0.55, fRandom)) / 255.;
    
    
    float fLen = length( vXY );
    
    if ( fLen < fRad )
    {
    	vec3 vNormal = normalize( vec3( vXY, sqrt( fRad * fRad - fLen * fLen) ) );
        vec3 vLight = normalize( vec3( 0.4, 0.3, 3.0 ) );
        
                
        float fShade = max(0.0, dot( vLight, vNormal ) );
        
        float fHighlight = pow(fShade, 50.0) * 0.2;
        
        fShade = fShade * 0.8 + 0.2;
        
        
        vResult = vec4( vCol * fShade + fHighlight, 1 );
    }
    
    if ( (vTexCoord.x == 6. || vTexCoord.x == 9.) && ( vTexCoord.y >= 0. && vTexCoord.y < 5.) )
    {
        vResult.rgb *= 0.5;
    }
    else
    if ( (vTexCoord.x >= 6. && vTexCoord.x <= 9.) && vTexCoord.y == 0. )
    {
        vResult.rgb *= 0.5;
    }
    
    float fMirrorX = 7.5 - abs( vTexCoord.x - 7.5 );
    vec2 vMirror = vec2( fMirrorX, vTexCoord.y );
    
    bool bEye = false;
    if( all( greaterThanEqual( vMirror, vec2(2,4) ) ) &&
        all( lessThanEqual( vMirror, vec2(5,5) ) ))
    {
        bEye = true; 
    }

    if( all( greaterThanEqual( vMirror, vec2(4,2) ) ) &&
        all( lessThanEqual( vMirror, vec2(5,3) ) ))
    {
        bEye = true; 
    }
    
    if( bEye )
	{
        float fEyeShade = clamp( 1.0 - (10. -vMirror.x - vMirror.y) / 8.0, 0.0, 1.0);
		vResult.rgb = fEyeShade * vec3(83,175,71) / 255.;
	}

    if( all( greaterThanEqual( vMirror, vec2(4,0) ) ) &&
        all( lessThanEqual( vMirror, vec2(5,1) ) ))
    {
        vResult = vec4(0.0);
    }
    
    
    /*

    vec3 vCol = vec3(0,1,.5);
    if ( all( greaterThan( vTexCoord, vec2(4, 6) ) ) && all( lessThan( vTexCoord, vec2(8, 9 ) ) ) )
	{
        vCol *= 0.5;
	}
    if ( all( greaterThan( vTexCoord, vec2(9, 6) ) ) && all( lessThan( vTexCoord, vec2(13, 9 ) ) ) )
	{
        vCol *= 0.5;
	}
    
    float fShade = clamp( vTexCoord.y / 10.0, 0.0, 1.0);
    vResult.rgb = vCol * fShade * fRandom;
    if ( fLen < 1.0 )
    {
        vResult.a = 1.;
    }
    
    if ( length( vTexCoord ) < 5.5 )
    {
        vResult.a = 0.;
    }
    if ( length( vTexCoord - vec2(16,0)) < 5.5 )
    {
        vResult.a = 0.;
    }
	*/
    
    return vResult;
}

vec4 TexWallImpact( vec2 vTexCoord, float fRandom, float fHRandom )
{
    vec4 vResult = vec4(0);
    
    float fLen = length( vTexCoord - vec2(3.0) ) / 3.0;
    fLen = sqrt(fLen);
    fLen = max( 0.0, fLen * 1.3 - 0.3 );
    vResult.rgb = mix( vec3(1.,1.,.8), vec3(0.4,0.2,0.0), fLen * fLen);
    vResult.rgb *= 1.5;
    

    vResult.a = step(fLen, 1.0);
    
    return vResult;  
}

vec4 TexBulletSmokeImpact( vec2 vTexCoord, float fRandom, float fHRandom )
{
    vec4 vResult = vec4(0);
    
    vResult.rgb = vec3(1.0,1.0,1.0) * fRandom;
    
    float fLen = length( (vTexCoord - vec2(4.0, 8.0)) / vec2(6.0, 10.0) ) ;

    fRandom = 1.0 - fRandom;
    
    vResult.a = step(fLen + fRandom * fRandom * 2.0, 1.0);
    
    return vResult;    
}

vec4 TexFleshImpact( vec2 vTexCoord, float fRandom, float fHRandom )
{
    vec4 vResult = vec4(0);
    
    vResult.rgb = vec3(1.0,0.0,0.0) * fRandom;
    
    float fLen = length( vTexCoord - vec2(4.0) ) / 4.2;

    vResult.a = step(fLen + fRandom * fRandom * 1.5, 1.0);
    
    return vResult;  
}
    
vec4 TexEnemyPlaceholder( vec2 vTexCoord, float fRandom, float fHRandom )
{
    vec4 vResult = vec4(0);
    
    vResult.rgb = vec3(0,0.2,0);
    if ( (length( vTexCoord - vec2(16.0, 50.) ) < 5.0) )
    {
        vResult.a = 1.;
    }
    if ( (length( vTexCoord - vec2(16.0, 30.) ) < 12.0) )
    {
        vResult.a = 1.;
    }
    
    if ( (length( vTexCoord - vec2(9.0, 5.) ) < 5.0) )
    {
        vResult.a = 1.;
    }

    if ( (length( vTexCoord - vec2(31. -9.0, 5.) ) < 5.0) )
    {
        vResult.a = 1.;
    }

    if ( (length( vTexCoord - vec2(9.0, 15.) ) < 5.0) )
    {
        vResult.a = 1.;
    }

    if ( (length( vTexCoord - vec2(31. -9.0, 15.) ) < 5.0) )
    {
        vResult.a = 1.;
    }
        
    return vResult;
}

vec4 TexShotgunItem( vec2 vTexCoord, float fRandom, float fHRandom )
{
    vec4 vResult = vec4(1);
    
    vResult.rgb = vec3(0.3) * fRandom;
 
    return vResult;    
}

vec4 TexPlaceholder( vec2 vTexCoord, float fRandom, float fHRandom, vec3 vCol )
{
    vec4 vResult = vec4(1);
    
    vResult.rgb = vCol * fRandom;
 
    return vResult;
}


bool RedCross( vec2 vTexCoord )
{
    if ( all( greaterThan( vTexCoord, vec2( -4, -2) ) ) && all( lessThan( vTexCoord, vec2(3, 1 ) ) ) )
	{
        return true;
	}

    if ( all( greaterThan( vTexCoord, vec2( -2, -4) ) ) && all( lessThan( vTexCoord, vec2(1, 3 ) ) ) )
	{
        return true;
	}
    
    return false;
}

float Flap( vec2 vTexCoord )
{
    float f1 = vTexCoord.y - 4.0;
    float f2 = vTexCoord.y + vTexCoord.x - 9.0;

    float fFlap = min(f1,f2);
    
    return fFlap / 3.0;
    
}

vec4 TexMedikit( vec2 vTexCoord, float fRandom, float fHRandom, float fEdge )
{
    vec4 vResult = vec4(1);
    
    float xMirror = 13.5 - abs(vTexCoord.x - 13.5);
    
    if ( xMirror - vTexCoord.y + 15.0 - fEdge < 0.0 )
    {
        return vec4(0.0);
    }
    
    xMirror -= fEdge * .5;
        
    vResult.rgb = vec3(111)/255.;
 
    if ( vTexCoord.y > 15. )
    {
		vResult.rgb = vec3(139)/255.;
    }
    else
    if ( vTexCoord.y > 14. )
    {
		vResult.rgb = vec3(167)/255.;
    }        
    
    float fFlap = Flap( vec2( xMirror, vTexCoord.y ) );
    
    // Strap
    if ( xMirror >= 6. && xMirror <= 8. )
    {
        if ( vTexCoord.y < 6. )
        {
            if ( fFlap >= 1.0 )
            {
	        	vResult.rgb = vec3(151) / 255.;
            }
            else
            {
	        	vResult.rgb = vec3(39) / 255.;
            }
        }
    }
        
    if ( fFlap > 0.0 && fFlap < 1.0 )
    {
        vResult.rgb *= 1.0 - fFlap;
    }

    if ( vTexCoord.y < 1. )
    {
        vResult.rgb *= 0.75;
    }

    
    vResult.rgb *= (fRandom * 0.2 + 0.8);

    vec2 vCrossPos = vTexCoord - vec2(14, 11 );
    if ( RedCross( vCrossPos ) )
	{
        float fDist = 1.0 - length( vCrossPos ) / 4.5;
        vResult.rgb = fDist * vec3(155,0,0)/255.;
        
	}

    /*if ( RedCross( vCrossPos + vec2(1,-1) ) )
    {
        vResult.rgb *= 0.9;        
    }
    
    if ( RedCross( vCrossPos - vec2(1,-1) ) )
    {
			vResult.rgb *= 1.1;        
    }*/
                        
    return vResult;
}

vec4 TexFloorLamp( vec2 vTexCoord, float fRandom, float fHRandom )
{
    vec4 vResult = vec4(0);

    float y = floor( vTexCoord.y );
    float xMirror = floor( abs(vTexCoord.x - 11.) );

    float fWidth = 6.0;;
    float fSpread = 1.0 / 4.0;
    float fLightPos = 10.0;
    
    if ( y == 47.0 )
    {
        fWidth = 3.0;
    }
    else
    if ( y == 46.0 )
    {
        fWidth = 5.0;
    }
    else
    if ( y == 7.0 || y == 10.0 || y == 13.0 )
    {
        fWidth = 7.0;
        fSpread = 1.0 / 8.;
        fLightPos = 8.0;
    }
    else
    if ( y == 2.0 || y == 3.0 )
    {
        fWidth = 11.0;
    }
    else
    if ( y == 1.0 )
    {
        fWidth = 9.0;
    }
    else
    if ( y == 4.0 )
    {
        fWidth = 10.0;
    }
    if ( y == 5.0 )
    {
        fWidth = 8.0;
    }
    
    if ( y <= 5.0 )
    {
        fSpread = 1.0 / 20.;
        fLightPos = y * .5 + 5.;
    }
    
    if ( xMirror < fWidth )
    {
        if ( y >= 41. && y <= 44. )
        {
            vec2 delta = vTexCoord - vec2(11, 42.5);
            float d = dot( delta, delta );
	        
            float fShade1 = 0.8 - (xMirror / 6.0);
            
            float fShade2 = 1.0 - d / 8.0;
            float fShade = max(0.0, max( fShade1, fShade2 ) );
            
            vec3 vCol = vec3(215, 187, 67)/255.;
            vCol += fShade * vec3(1, 0.75, 0.5);
    	    vResult = vec4( vCol,1.0);
        }
        else
        {
	        float fShade= max(0.0, 1.0 - abs(vTexCoord.x - fLightPos ) * fSpread ) * 0.25;
    	    vResult = vec4(vec3(fShade),1.0);
        }
    }
    
    return vResult;
}

vec3 Project( vec3 a, vec3 b )
{
	return a - b * dot(a, b);
}
/*
void TraceSlab( const in vec3 vRayOrigin, const in vec3 vRayDir, const in vec3 vSlabOrigin, const in vec3 vSlabDir, const in float fThickness, out float fNear, out float fFar, inout vec3 vNormal )
{
    
    float t0 = projOffset0 / projDir;
    float t1 = projOffset1 / projDir;
    
    
    
    if ( f0 > 
}
*/
float TraceCylinder( const in vec3 vRayOrigin, const in vec3 vRayDir, const in vec3 vCylinderOrigin, const in vec3 vCylinderDir, const in float fLength, const in float fCylinderRadius, inout vec3 vNormal )
{	
	
	vec3 vOffset = vCylinderOrigin - vRayOrigin;
	
	vec3 vProjOffset = Project(vOffset, vCylinderDir);
	vec3 vProjDir = Project(vRayDir, vCylinderDir);
	float fProjScale = length(vProjDir);
	vProjDir /= fProjScale;
	
	// intersect circle in projected space
	
	float fTClosest = dot(vProjOffset, vProjDir);
	
	vec3 vClosest = vProjDir * fTClosest;
	float fDistClosest = length(vClosest - vProjOffset);
	if(fDistClosest < fCylinderRadius)
	{		
		float fHalfChordLength = sqrt(fCylinderRadius * fCylinderRadius - fDistClosest * fDistClosest);
		float fTIntersectMin = clamp((fTClosest - fHalfChordLength) / fProjScale, 0.0, 100000.0);
		float fTIntersectMax = (fTClosest + fHalfChordLength) / fProjScale;	
		
		if(fTIntersectMax > fTIntersectMin)
		{	
            vec3 vHitPos = vRayOrigin + vRayDir * fTIntersectMin;
            float hitU = dot( vHitPos - vCylinderOrigin, vCylinderDir );
            if ( hitU >= 0.0 && hitU <= fLength )
            {
				vNormal = normalize(vProjDir * (fTClosest - fHalfChordLength) - vProjOffset );
				return fTIntersectMin;
            }
		}		
	}
    
    // end cap

    vec3 vEndPos = vCylinderOrigin;
	vNormal = vCylinderDir;
    
    if ( dot( vRayDir, vCylinderDir) < 0.0 )
    {
        vEndPos += vCylinderDir * fLength;
        vNormal = -vNormal;
    }
    
    vec3 vEndOffset = vEndPos - vRayOrigin;
    float endDot = dot( vEndOffset , vCylinderDir );
    float dirDot = dot( vRayDir, vCylinderDir );
    
    float capT = endDot / dirDot;
    
    if ( capT > 0.0 )
    {
        vec3 vCapPos = vRayOrigin + vRayDir * capT;
        
        if ( length( vEndPos - vCapPos ) < fCylinderRadius )
        {
            return capT;
        }
    }
    
    
    return 100000.0;
}

vec4 TexTallTechnoPillar( vec2 vTexCoord, float fRandom, float fHRandom )
{
    vec3 vRayOrigin = vec3(0.0, 64.0, -300.0);
    vec3 vRayTarget = vec3( vTexCoord.x - 38. * .5, vTexCoord.y, 0.0);
    vec3 vRayDir = normalize( vRayTarget - vRayOrigin );
    
    vec3 vNormal;
    
    float t = TraceCylinder( vRayOrigin, vRayDir, vec3(0, 3, 0), vec3(0,1,0), 127.0 - 6., 12.0, vNormal );

    vec3 vNormal2;
    float t2; 
    t2 = TraceCylinder( vRayOrigin, vRayDir, vec3(0, 3, 0), vec3(0,1,0), 6.0, 16.0, vNormal2 );
    if ( t2 < t )
    {
        t = t2;
        vNormal = vNormal2;
    }

    t2 = TraceCylinder( vRayOrigin, vRayDir, vec3(0, 127. - 3. - 6., 0), vec3(0,1,0), 6.0, 16.0, vNormal2 );
    if ( t2 < t )
    {
        t = t2;
        vNormal = vNormal2;
    }


    t2 = TraceCylinder( vRayOrigin, vRayDir, vec3(0, 32, 0), vec3(0,1,0), 2.0, 16.0, vNormal2 );
    if ( t2 < t )
    {
        t = t2;
        vNormal = vNormal2;
    }
    
    t2 = TraceCylinder( vRayOrigin, vRayDir, vec3(0, 36, 0), vec3(0,1,0), 2.0, 16.0, vNormal2 );
    if ( t2 < t )
    {
        t = t2;
        vNormal = vNormal2;
    }
    t2 = TraceCylinder( vRayOrigin, vRayDir, vec3(0, 40, 0), vec3(0,1,0), 2.0, 16.0, vNormal2 );
    if ( t2 < t )
    {
        t = t2;
        vNormal = vNormal2;
    }
    
    
    //vec3 vNormal1;
    if ( t > 5000. )
    {
        return vec4(0.);
    }
    
    vec3 vLight = normalize( vec3(-1., -0.5, -2 ) );
    
    float fShade = max(0.0, dot( vNormal, vLight ));
    
    vec3 vCol = vec3(0.2) + fRandom * 0.1;
    
    vec3 vPos = vRayOrigin + vRayDir * t;
    
    if ( vPos.y > 43. && vPos.y < 118. )
    {
        float f = fRandom / .75;
        //f *= 0.75 + fHRandom * 0.25;
        vCol = vec3( pow( f, 5.0) );
    }
    
    vCol *= fShade;
    return vec4(vCol,1);
    
    // float fShade = fRandom - fHRandom * 0.5;
    //return vec4(fShade,fShade, fShade, 1);
}

void AddGlow( inout vec3 vResult, vec2 vSpritePixel, vec2 vGlowPos, vec3 vCol, float f )
{
    float fLen = length( vSpritePixel - vGlowPos );
    
    vResult += exp2(-fLen * f) * vCol;
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


vec4 TexBarExpl( vec2 vTexCoord, float fRandom, float fHRandom )
{
    vec4 vResult = TexBar1A( vTexCoord - vec2(4,0), fRandom, fHRandom );
    
    vec4 vExplode = MuzzleFlash( vTexCoord, vec2(16,16), vec2(60, 60));
    
    if ( vExplode.a > 0.0 )
    {
        vResult.rgb += vExplode.rgb * 0.2;
        vResult.a = 1.0;
        return vResult;
    }
    
    //AddGlow( vResult.rgb, vTexCoord, vec2(13., 20.0), vec3(1,.9, .5) * 2., 0.15 );
    //vResult *= TexFireball( vTexCoord, fRandom, fHRandom );
	return vResult;
}

vec4 TexBarExpl2( vec2 vTexCoord, float fRandom, float fHRandom )
{
    vec4 vResult = MuzzleFlash( vTexCoord.yx, vec2(24,19), vec2(90, 82));    
    
	return vResult;
}

/////////

vec2 SpritePixel( vec2 sheetPixel, vec4 vSpriteInfo )
{
    return floor(sheetPixel - vSpriteInfo.xy);
}

bool IsInSprite( vec2 sheetPixel, vec4 vSpriteInfo )
{
    return all( greaterThanEqual(sheetPixel, vSpriteInfo.xy) ) && all( lessThan(sheetPixel, vSpriteInfo.xy + vSpriteInfo.zw) );
}

bool SpriteSelectIterate( vec4 vSpriteInfo, vec2 sheetPixel, inout vec4 vOutSpriteInfo, inout float fOutSpriteIndex, inout float fTestIndex )
{
    bool isInSprite = IsInSprite( sheetPixel, vSpriteInfo );
    
    if ( isInSprite )
    {
        vOutSpriteInfo = vSpriteInfo;
        fOutSpriteIndex = fTestIndex;
    }
    
    fTestIndex++;
    return isInSprite;
}

bool SpriteProcessIterate( inout float fTestIndex, float fSpriteIndex )
{
    bool bResult = false;
    if ( fSpriteIndex == fTestIndex )
    {
        bResult = true;
    }
    
    fTestIndex++;
    
    return bResult;
}

///////////////////////////
// Sprite Crop Sheet Info
///////////////////////////

#define SPR_RIGHT(V) vec2((V.x + V.z), V.y)
#define SPR_ABOVE(V) vec2(V.x, (V.y + V.w))

CXX_CONST vec4 kSpriteNone				= vec4( 0 );

CXX_CONST vec4 kSpriteBarrel			= vec4(	SPR_RIGHT(kSpriteNone),					23,32);
CXX_CONST vec4 kSpriteBarrelExpl		= vec4(	SPR_RIGHT(kSpriteBarrel),				32,32);

CXX_CONST vec4 kSpriteFireball			= vec4(	SPR_RIGHT(kSpriteBarrelExpl),			20,20);
CXX_CONST vec4 kSpriteFireball2			= vec4(	SPR_RIGHT(kSpriteFireball),				24,24);

CXX_CONST vec4 kSpriteHealthBonus		= vec4(	SPR_RIGHT(kSpriteFireball2),			16,16);
CXX_CONST vec4 kSpriteArmorBonus		= vec4(	SPR_ABOVE(kSpriteHealthBonus),			16,15);
CXX_CONST vec4 kSpriteMedikit			= vec4(	SPR_RIGHT(kSpriteHealthBonus),			28,19);
CXX_CONST vec4 kSpriteStimpack			= vec4(	SPR_RIGHT(kSpriteMedikit),				16,19);
CXX_CONST vec4 kSpriteBlueArmor			= vec4(	SPR_RIGHT(kSpriteStimpack),				32,24);
CXX_CONST vec4 kSpriteGreenArmor		= vec4(	SPR_RIGHT(kSpriteBlueArmor),			32,24);
CXX_CONST vec4 kSpriteShotgun			= vec4(	SPR_RIGHT(kSpriteGreenArmor),			32,16);

CXX_CONST vec4 kSpriteWallImpact1		= vec4(	SPR_RIGHT(kSpriteShotgun),				6,6);
CXX_CONST vec4 kSpriteWallSmokeImpact1	= vec4(	SPR_RIGHT(kSpriteWallImpact1),			8,16);
CXX_CONST vec4 kSpriteFleshImpact1		= vec4(	SPR_RIGHT(kSpriteWallSmokeImpact1),		8,8);

CXX_CONST vec4 kSpriteBloodyMess		= vec4(	SPR_RIGHT(kSpriteFleshImpact1) + vec2(0,16),		42,16);
CXX_CONST vec4 kSpriteDeadTrooper 		= vec4(	SPR_RIGHT(kSpriteBloodyMess),			42,16);
CXX_CONST vec4 kSpriteDeadSergeant 		= vec4(	SPR_RIGHT(kSpriteDeadTrooper),			42,16);
CXX_CONST vec4 kSpriteDeadImp 			= vec4(	SPR_RIGHT(kSpriteDeadSergeant),			42,16);

CXX_CONST vec4 kSpriteBarrelExpl2		= vec4(	480,0,									38,48);

CXX_CONST vec4 kSpriteFloorLamp			= vec4(	480,48,									23,48);
CXX_CONST vec4 kSpriteTallTechnoPillar	= vec4(	SPR_ABOVE(kSpriteFloorLamp),			38,128);

CXX_CONST vec4 kSpriteEnemyBegin = vec4(0, 32., 32, 52);


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

vec4 GetEnemySprite( float fType, float fAction, float fDirection )
{
    vec2 vGridPos = vec2( fType + fDirection * float(SPRITE_ENEMY_TYPE_COUNT), fAction );
    vec2 vPos = kSpriteEnemyBegin.xy +kSpriteEnemyBegin.zw * vGridPos;
	return vec4( vPos, kSpriteEnemyBegin.zw );
}



bool GetEnemySpriteId( vec2 fragCoord, out float fEnemySpriteType, out float fEnemySpriteAction, out float fEnemySpriteDirection )
{
    vec2 vGridLocal = fragCoord - kSpriteEnemyBegin.xy;
    vec2 vGridPos = floor( vGridLocal / kSpriteEnemyBegin.zw );
    
    fEnemySpriteType = mod( vGridPos.x, float(SPRITE_ENEMY_TYPE_COUNT) );
    fEnemySpriteAction = vGridPos.y;
    fEnemySpriteDirection = floor( vGridPos.x / float(SPRITE_ENEMY_TYPE_COUNT) );

    return ( vGridPos.x >= 0. && vGridPos.y >= 0. && vGridPos.x < (float(SPRITE_ENEMY_TYPE_COUNT) * 5.0) && vGridPos.y < float(SPRITE_ENEMY_ACTION_COUNT) );        
}




void Pose_Enemy( float fEnemySpriteType, float fEnemySpriteAction, float fEnemySpriteDirection )
{
    bool bWeapon = false;
    if ( fEnemySpriteType == float(SPRITE_ENEMY_TYPE_IMP))
    {
        CharDef_Imp( g_scene.charDef );            
    }            
    else
        if ( fEnemySpriteType == float(SPRITE_ENEMY_TYPE_TROOPER))
        {
            CharDef_Trooper( g_scene.charDef );
        }   
    else
        if ( fEnemySpriteType == float(SPRITE_ENEMY_TYPE_SERGEANT))
        {
            CharDef_Sergeant( g_scene.charDef );
            bWeapon = true;
        }                        

    //////
    
    if ( fEnemySpriteAction == float(SPRITE_ENEMY_ACTION_STAND))
    {
        Pose_Clear( g_scene.pose );
    }            
    else
        if ( fEnemySpriteAction == float(SPRITE_ENEMY_ACTION_ATTACK))
        {
            Pose_Attack( g_scene.pose );
        }            
    else
        if ( fEnemySpriteAction == float(SPRITE_ENEMY_ACTION_WALK1))
        {
            Pose_Walk1( g_scene.pose );
        }   
    else
        if ( fEnemySpriteAction == float(SPRITE_ENEMY_ACTION_WALK2))
        {
            Pose_Walk2( g_scene.pose );
        }                        
    else
        if ( fEnemySpriteAction == float(SPRITE_ENEMY_ACTION_PAIN))
        {
            Pose_Pain( g_scene.pose );
        }                        
    else 
        if ( fEnemySpriteAction == float(SPRITE_ENEMY_ACTION_DIE))
    {
        Pose_Die( g_scene.pose );
        bWeapon = false;        
    }                        
    else 
        if ( fEnemySpriteAction == float(SPRITE_ENEMY_ACTION_DEAD))
    {
        Pose_Dead( g_scene.pose );
        bWeapon = false;
    }                        

                          
    if ( bWeapon )
    {
    	Pose_Weapon();
    }
    else
    {
		Weapon_None();
    }    

    g_scene.fCameraRotation = fEnemySpriteDirection * radians(45.0);            
}


///////////////////////////////////////

#define FONT_POS 	ivec2(280,0)
#define FONT_CHAR 	ivec2(16,0)

vec4 NumFont_Char( ivec2 vTexCoord, int iDigit )
{
 	if ( iDigit < 0 || iDigit > 10 )
    	return vec4(0.0);
    
    //vTexCoord = floor(vTexCoord * vec2(14.0, 16.0)) + 0.5 + vec2(480,96);
    vTexCoord = vTexCoord + FONT_POS;
    vTexCoord += FONT_CHAR * iDigit;
    
    float fSample_TL = texelFetch( iChannel1, (vTexCoord - ivec2(-1, 1) ), 0 ).a;
    float fSample_TC = texelFetch( iChannel1, (vTexCoord - ivec2( 0, 1) ), 0 ).a;
    float fSample_TR = texelFetch( iChannel1, (vTexCoord - ivec2( 1, 1) ), 0 ).a;
    
    float fSample_CL = texelFetch( iChannel1, (vTexCoord - ivec2(-1, 0) ), 0 ).a;
    float fSample_CC = texelFetch( iChannel1, (vTexCoord - ivec2( 0, 0) ), 0 ).a;
    float fSample_CR = texelFetch( iChannel1, (vTexCoord - ivec2( 1, 0) ), 0 ).a;
    float fSample_CS = texelFetch( iChannel1, (vTexCoord - ivec2( 2, 0) ), 0 ).a;

    float fSample_BL = texelFetch( iChannel1, (vTexCoord - ivec2(-1,-1) ), 0 ).a;
    float fSample_BC = texelFetch( iChannel1, (vTexCoord - ivec2( 0,-1) ), 0 ).a;
    float fSample_BR = texelFetch( iChannel1, (vTexCoord - ivec2( 1,-1) ), 0 ).a;
    float fSample_BS = texelFetch( iChannel1, (vTexCoord - ivec2( 2,-1) ), 0 ).a;
    
    
    float fSample_SC = texelFetch( iChannel1, (vTexCoord - ivec2( 0,-2) ), 0 ).a;
    float fSample_SR = texelFetch( iChannel1, (vTexCoord - ivec2( 1,-2) ), 0 ).a;
    float fSample_SS = texelFetch( iChannel1, (vTexCoord - ivec2( 2,-2) ), 0 ).a;
   
    float fOutline = min( 1.0, 
		fSample_TL + fSample_TC + fSample_TR +
		fSample_CL + fSample_CC + fSample_CR +
		fSample_BL + fSample_BC + fSample_BR );
    
    float fShadow = min( 1.0, 
		fSample_CC + fSample_CR + fSample_CS +
		fSample_BC + fSample_BR + fSample_BS + 
		fSample_SC + fSample_SR + fSample_SS);
    	
    float fMain = fSample_CC;
    
    vec4 vResult = vec4(0.0);
    
    float fAlpha = min( 1.0, fOutline + fMain + fShadow );
    
    float fShade = fSample_TL * 1.5 + fSample_BR * -1.5 + fSample_TC * 1.0 + fSample_CL * 1.0 
        + fSample_BC * -1.0 + fSample_CR * -1.0;
    
    fShade = clamp( fShade * 0.25, 0.0, 1.0 );
    
    fShade = fShade * .3 + .7;
    
    vec3 vColor = vec3( .2 ); // drop shadow
    
    if ( fOutline > 0.0 )
        vColor = vec3(.4, 0, 0); // outline

    if ( fMain > 0.0 )
        vColor = vec3(fShade, 0, 0); // main text
            
    vResult = vec4(vColor, fAlpha);
    
    return vResult;
}

///////////////////////////////////////

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    bool bReuseData = false;
    if ( iFrame > 0 )
    {
	    // Check Resolution
        vec4 info = texelFetch( iChannel0, ivec2(0, 0), 0 );
        if ( (info.x == iResolution.x) && (info.y == iResolution.y) )
        {
            bReuseData = true;
        }
    }

	if ( bReuseData )
    {
        fragColor = texelFetch( iChannel0, ivec2(floor(fragCoord)), 0 );
        return;
        //vResult.r = sin(iTime * 10.0) *0.5 + 0.5; // flash the cache
    }    
    
    ivec2 vFontCoord = ivec2(floor(fragCoord)) - FONT_POS;
    
    vec4 vResult = vec4(0.2,0,0,1);
    
    float fPersistence = 0.8;
	float fNoise2Freq = 0.5;    
    
    vec2 vSpritePixel = vec2(0);
    vec4 vSpriteInfo = vec4(0);
    
    float fSpriteIndex = -1.;
    
    bool bRaymarch = false;
    Scene_Clear();
    
    
    #if 0
        bRaymarch = true;
    
    	#if 1
    		g_scene.bArmor = true;
    	#endif
    
    	#if 0
            g_scene.bCharacter = true;

            //float fType = SPRITE_ENEMY_TYPE_TROOPER;
            float fType = SPRITE_ENEMY_TYPE_SERGEANT;
            //float fType = SPRITE_ENEMY_TYPE_IMP;

            //float fAction = SPRITE_ENEMY_ACTION_STAND;
            float fAction = SPRITE_ENEMY_ACTION_WALK1;
            //float fAction = SPRITE_ENEMY_ACTION_WALK2;
            //float fAction = SPRITE_ENEMY_ACTION_ATTACK;
            //float fAction = SPRITE_ENEMY_ACTION_PAIN;
            //float fAction = SPRITE_ENEMY_ACTION_DIE;
            //float fAction = SPRITE_ENEMY_ACTION_DEAD;


            Pose_Enemy( fType, fAction, 0. );

    	#endif

        vec2 vSize = vec2(512, 512);
        float fScale = 6.5;
        vec2 vPos = fragCoord.xy;
        vPos.x = (vPos.x - vSize.x * .5) / fScale + vSize.x * .5;
        vPos.y /= fScale;
    
    	g_scene.fCameraRotation = iTime;
    	fragColor = Raymarch_Sprite( vPos.xy, vec4(0,0,512,512) );
    
    	return;
    #endif
    
    
    float fTestIndex = 0.;
    if ( SpriteSelectIterate( kSpriteFireball, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {
    }
    if ( SpriteSelectIterate( kSpriteFireball2, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {
    }
    if ( SpriteSelectIterate( kSpriteBarrel, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {        
    }
    if ( SpriteSelectIterate( kSpriteBarrelExpl, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {        
    }
    if ( SpriteSelectIterate( kSpriteBarrelExpl2, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {        
    }
    if ( SpriteSelectIterate( kSpriteBloodyMess, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {        
        fPersistence = 1.0;
    }
    if ( SpriteSelectIterate( kSpriteHealthBonus, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {        
    }
    if ( SpriteSelectIterate( kSpriteArmorBonus, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {        
        fPersistence = 0.6;
    }
    if ( SpriteSelectIterate( kSpriteMedikit, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {        
    }
    if ( SpriteSelectIterate( kSpriteStimpack, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {        
    }
    if ( SpriteSelectIterate( kSpriteBlueArmor, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {        
        bRaymarch = true;
        g_scene.bArmor = true;
    }
    if ( SpriteSelectIterate( kSpriteGreenArmor, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {        
        bRaymarch = true;
        g_scene.bArmor = true;
	    g_scene.vLightDir = normalize( vec3(0.2, 1, -0.2) );
        
    }
    if ( SpriteSelectIterate( kSpriteShotgun, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {        
        bRaymarch = true;
        g_scene.bCharacter = false;
        Weapon_DefaultPos();
    }
    if ( SpriteSelectIterate( kSpriteWallImpact1, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {        
    }
    if ( SpriteSelectIterate( kSpriteWallSmokeImpact1, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {        
        fPersistence = 0.95;
    }
    if ( SpriteSelectIterate( kSpriteFleshImpact1    , fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {        
    }
    if ( SpriteSelectIterate( kSpriteDeadTrooper    , fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {        
        bRaymarch = true;
        g_scene.bCharacter = true;
	    CharDef_Trooper( g_scene.charDef );            
        Pose_Dead( g_scene.pose );
    }
    if ( SpriteSelectIterate( kSpriteDeadSergeant    , fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {
        bRaymarch = true;
        g_scene.bCharacter = true;
	    CharDef_Sergeant( g_scene.charDef );                    
        Pose_Dead( g_scene.pose );
    }
    if ( SpriteSelectIterate( kSpriteDeadImp    , fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {
        bRaymarch = true;
        g_scene.bCharacter = true;
	    CharDef_Imp( g_scene.charDef );                    
        Pose_Dead( g_scene.pose );
    }
    if ( SpriteSelectIterate( kSpriteFloorLamp, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {
    }
    if ( SpriteSelectIterate( kSpriteTallTechnoPillar, fragCoord, vSpriteInfo, fSpriteIndex, fTestIndex ) )
    {
        fNoise2Freq = 2.0;
        fPersistence = 1.0;
    }
    
    float fEnemySpriteType;
    float fEnemySpriteAction;
    float fEnemySpriteDirection;
    bool isEnemySprite = GetEnemySpriteId( fragCoord, fEnemySpriteType, fEnemySpriteAction, fEnemySpriteDirection );

    ivec2 vFontMax = 10 * FONT_CHAR;
    if( vFontCoord.x >= 0 && vFontCoord.y >= 0 && vFontCoord.x < (vFontMax.x + 16) && vFontCoord.y < (vFontMax.y + 16) )
    {
        fSpriteIndex = 777.;
    }
    else
    if ( isEnemySprite )
    {
        bRaymarch = true;
        g_scene.bCharacter = true;
		Pose_Enemy( fEnemySpriteType, fEnemySpriteAction, fEnemySpriteDirection );
        
        fSpriteIndex = 666.;
        vSpriteInfo = GetEnemySprite( fEnemySpriteType, fEnemySpriteAction, fEnemySpriteDirection );
        
        //fragColor.rgb= vec3( fEnemySpriteType, fEnemySpriteAction, fEnemySpriteDirection) / 8.0; return;
    }
        
    if ( fSpriteIndex == -1.0 )
    {
        discard;
    }
		
	vec4 vRaymarchResult = vec4( 0 );
    vRaymarchResult = Raymarch_Sprite( fragCoord.xy, vSpriteInfo );
    
    {    
        vSpritePixel = SpritePixel( fragCoord, vSpriteInfo );

        float fRandom = fbm( vSpritePixel, fPersistence );
        float fHRandom = noise1D( vSpritePixel.x * fNoise2Freq );// - (vSpritePixel.y / vSpriteInfo.w);

        fTestIndex = 0.;

	    if( vFontCoord.x >= 0 && vFontCoord.y >= 0 && vFontCoord.x < (vFontMax.x + 16) && vFontCoord.y < (vFontMax.y + 16) )
        {

            int iFontDigit = vFontCoord.x / 16;
            ivec2 vFontPixel = vFontCoord.xy;
            vFontPixel.x = vFontPixel.x % 16;
            vResult = NumFont_Char( vFontPixel, iFontDigit );
            //float fValue = NumFont_BinChar( vFontPixel, fFontDigit );
            //vResult =  vec4( fValue );
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            vResult = TexFireball( vSpritePixel, fRandom, fHRandom, vSpriteInfo.z * .5, 0. );
        }
		if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            vResult = TexFireball( vSpritePixel, fRandom, fHRandom, vSpriteInfo.z * .5, 1. );
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            vResult = TexBar1A( vSpritePixel, fRandom, fHRandom );
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            vResult = TexBarExpl( vSpritePixel, fRandom, fHRandom );
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            vResult = TexBarExpl2( vSpritePixel, fRandom, fHRandom );
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            vResult = TexPlayW( vSpritePixel, fRandom, fHRandom  );
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            vResult = TexHealthBonus( vSpritePixel, fRandom, fHRandom  );
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            vResult = TexArmorBonus( vSpritePixel, fRandom, fHRandom  );
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            // Medikit
            vResult = TexMedikit( vSpritePixel, fRandom, fHRandom, 0.0 );
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            // Stimpack
            vResult = TexMedikit( vSpritePixel + vec2(6,0), fRandom, fHRandom, 5.0 );
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            // blue armor
            //vResult = TexPlaceholder( vSpritePixel, fRandom, fHRandom, vec3(0,0,1) );
            vResult = vRaymarchResult;            
            vResult.rgb *= fRandom * vec3(0,0,1);            
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            // green armor
            vResult = vRaymarchResult;            
            vResult.rgb *= fRandom * vec3(0,1.0,0);
            //vResult = TexPlaceholder( vSpritePixel, fRandom, fHRandom, vec3(0,1.0,0) );
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            // Shotgun
            //vResult = TexShotgunItem( vSpritePixel, fRandom, fHRandom  );
            vResult = vRaymarchResult;       
            vResult.rgb *= fRandom;
            
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            vResult = TexWallImpact( vSpritePixel, fRandom, fHRandom  );
        }        
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            vResult = TexBulletSmokeImpact( vSpritePixel, fRandom, fHRandom  );
        }        
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            vResult = TexFleshImpact( vSpritePixel, fRandom, fHRandom  );
        }        
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            // Dead trooper
            vResult = TexPlayW( vSpritePixel, fRandom, fHRandom );
            float fMask = fRandom * fRandom * ((16.0 - vSpritePixel.y) / 4.0);
            if ( (vRaymarchResult.a - fMask ) > 0. )
            {
            	vResult = vRaymarchResult;            
	            vResult.xyz *= fRandom;
            }
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            // Dead sergeant
            vResult = TexPlayW( vSpritePixel, fRandom, fHRandom );
            float fMask = fRandom * fRandom * ((16.0 - vSpritePixel.y) / 4.0);
            if ( (vRaymarchResult.a - fMask ) > 0. )
            {
            	vResult = vRaymarchResult;            
	            vResult.xyz *= fRandom;
            }
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            // Dead imp
            vResult = TexPlayW( vSpritePixel, fRandom, fHRandom );
            float fMask = fRandom * fRandom * ((16.0 - vSpritePixel.y) / 4.0);
            if ( (vRaymarchResult.a - fMask ) > 0. )
            {
            	vResult = vRaymarchResult;            
	            vResult.xyz *= fRandom;
            }
        }
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            vResult = TexFloorLamp( vSpritePixel, fRandom, fHRandom  );
        }        
        if ( SpriteProcessIterate( fTestIndex, fSpriteIndex) )
        {
            vResult = TexTallTechnoPillar( vSpritePixel, fRandom, fHRandom  );
        }        
        
        
        if ( isEnemySprite )
        {
            vResult = vRaymarchResult;
            vResult.xyz *= fRandom;
            
            if ( fEnemySpriteAction == float(SPRITE_ENEMY_ACTION_ATTACK) )
            {
	            AddGlow( vResult.xyz, vSpritePixel, vec2(16. - 8. * sin( g_scene.fCameraRotation), 36), vec3(1,.9, .5) * 3., 1.0 );
            }
        }

        vResult.rgb = clamp( vResult.rgb, 0.0, 1.0 );
        vResult.rgb = Quantize(vResult.rgb, 32.0);
    }
    
	

 
    // Flash alpha channel
#if 0
    if ( vResult.a == 0. )
    {
        float fFlash = 1.0;
        //float fFlash = sin(iTime* 10.) * 0.5 + 0.5;
        vResult.rgb = mix ( vec3(0,0,0.3), vec3(1,.5, 1), fFlash );
    }
#endif    
    
    if ( floor ( fragCoord ) == vec2(0.0) )
    {
        vResult = vec4( iResolution.x, iResolution.y, 0, 0 );
    }
    
    fragColor = vResult;
}
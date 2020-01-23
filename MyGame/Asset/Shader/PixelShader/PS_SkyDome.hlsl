
//cbuffer SkyBuffer : (b5)
//{
//	float _InnerRadius = 1000;
//	float _OuterRadius = 1250;
//
//	float _Kr = 0.0025;	// レイリー散乱定数
//	float _Km = 0.001;	// ミー散乱定数
//}

static const float _InnerRadius = 4750;
static const float _OuterRadius = 5000;

static const float _Kr = 0.0025;	// レイリー散乱定数
static const float _Km = 0.01;		// ミー散乱定数


static const float Pi = 3.141592653589;

static const float fSamples = 2.0;

static const float3 three_primary_colors = float3(0.68, 0.55, 0.44);
static const float3 v3InvWaveLength = 1.0 / pow(three_primary_colors, 4.0);

static const float fOuterRadius = _OuterRadius;
static const float fInnerRadius = _InnerRadius;

static const float fESun = 40.0;	// 太陽の明るさ
static const float fKrESun = _Kr * fESun;
static const float fKmESun = _Km * fESun;

static const float fKr4PI = _Kr * 4.0 * Pi;
static const float fKm4PI = _Km * 4.0 * Pi;

static const float fScale = 1.0 / (_OuterRadius - _InnerRadius);
static const float fScaleDepth = 0.25;
static const float fScaleOverScaleDepth = fScale / fScaleDepth;

static const float g = -0.999f;
static const float g2 = g * g;


struct CAMERA
{
	float4 eyePos;
	float4 direction;
};

struct DIRECTIONAL_LIGHT
{
	float4		direction;
	float4		diffuse;
	float4		ambient;
};

// カメラ
cbuffer CameraBuffer : register(b2)
{
	CAMERA Camera;
}

// 平行光源
cbuffer DirLightBuffer : register(b4)
{
	DIRECTIONAL_LIGHT DirLight;
}


float Scale(float fcos) {
	float x = 1.0 - fcos;
	return fScaleDepth * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.8 + x * 5.25))));
}

float3 IntersectionPos(float3 dir, float3 a, float radius)
{
	float b = dot(a, dir);
	float c = dot(a, a) - radius * radius;
	float d = max(b * b - c, 0.0);

	return a + dir * (-b + sqrt(d));
}

// 頂点シェーダから送られてくるデータ
struct VS_OUTPUT {
	float4 pos		: SV_POSITION; 	// POSITIONだと表示されない
	float3 worldPos : TEXCOORD1;
	float2 uv		: TEXCOORD0;
};

// エントリポイント
float4 main(VS_OUTPUT input) : SV_Target
{
	float3 worldPos = input.worldPos;
	worldPos = IntersectionPos(normalize(worldPos), float3(0.0, fInnerRadius, 0.0), fOuterRadius);
	float3 v3CameraPos = float3(0.0, 0.0, 0.0);//Camera.eyePos.xyz;
	v3CameraPos.y = fInnerRadius;
	float3 v3LightDir = -DirLight.direction.xyz;

	float3 v3Ray = worldPos - v3CameraPos;
	float fFar = length(v3Ray);
	v3Ray /= fFar;

	float3 v3Start = v3CameraPos;
	float fCameraHeight = length(v3CameraPos);
	float fStartAngle = dot(v3Ray, v3Start) / fCameraHeight;
	float fStartDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
	float fStartOffset = fStartDepth * Scale(fStartAngle);

	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * fScale;
	float3 v3SampleRay = v3Ray * fSampleLength;
	float3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	float3 v3FrontColor = 0.0;
	for (int n = 0; n < int(fSamples); n++) {
		float fHeight = length(v3SamplePoint);
		float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
		float fLightAngle = dot(v3LightDir, v3SamplePoint) / fHeight;
		float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
		float fScatter = (fStartOffset + fDepth * (Scale(fLightAngle) - Scale(fCameraAngle)));
		float3 v3Attenuate = exp(-fScatter * (v3InvWaveLength * fKr4PI + fKm4PI));
		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	float3 c0 = v3FrontColor * (v3InvWaveLength * fKrESun);
	float3 c1 = v3FrontColor * fKmESun;
	float3 v3Direction = v3CameraPos - worldPos;

	float fcos = dot(v3LightDir, v3Direction) / length(v3Direction);
	float fcos2 = fcos * fcos;

	float rayleighPhase = 0.75 * (1.0 + fcos2);
	float miePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fcos2) / pow(1.0 + g2 - 2.0 * g * fcos, 1.5);

	float4 col = 1.0;
	col.rgb = rayleighPhase * c0 + miePhase * c1;
	return col;
}
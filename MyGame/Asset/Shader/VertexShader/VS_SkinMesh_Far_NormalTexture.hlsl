
//------------------------------------------------------------------------------------------------
// 構造体定義
//------------------------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------------------------
// レジスタ定義
//------------------------------------------------------------------------------------------------

// マトリクスバッファ&ファーバッファー
cbuffer WorldBuffer : register(b0)
{
	matrix World;
	uint FarMaxNum;		// 描画回数
	float FarScaleOffset;  // 法線方向に拡大していく間隔

	float2 FarDumy;
}

// ビューバッファ
cbuffer ViewBuffer : register(b1)
{
	matrix View;
}

// プロジェクションバッファ
cbuffer ProjectionBuffer : register(b2)
{
	matrix Projection;
}

// ボーン行列
cbuffer Armature : register(b3)
{
	matrix BoneArray[256];
}

// ライトの影用バッファ
cbuffer LightShadowBuffer : register(b4)
{
	matrix LightView;
	matrix LightProjection;
}

// カメラ
cbuffer CameraBuffer : register(b5)
{
	CAMERA Camera;
}

// 平行光源
cbuffer DirLightBuffer : register(b6)
{
	DIRECTIONAL_LIGHT DirLight;
}

// 頂点レイアウト
struct VS_INPUT {
	float4 pos        : POSITION;
	float2 uv         : TEXCOORD;
	float3 normal     : NORMAL;
	int4   boneIndices: BLENDINDICES;
	float4 boneWeights: BLENDWEIGHT;
	float3 tangent    : TANGENT;
	float3 binormal   : BINORMAL;
	uint instanceID : SV_InstanceID;	// インスタンシングの番号
};

// ピクセルシェーダに送るデータ
struct VS_OUTPUT {
	float4 pos			: SV_POSITION;		// POSITIONだと表示されない
	float4 posw			: POSITION0;		// 計算用のワールド座標
	float2 uv		    : TEXCOORD0;
	float3 normal		: NORMAL;
	float4 lightViewPos : POSITION1;		// ライトビューの深度テクスチャ座標
	float3 eyePos		: POSITION2;		// 接空間上の視線ベクトル
	float3 lightDir		: POSITION3;		// 接空間上のライトベクトル
	float  farAlpha		: PSIZE;		    // 毛の先端部分をだんだん透明にしていく値
};


//=============================================================================
// 頂点シェーダ	------ インスタンシング -------
//=============================================================================
VS_OUTPUT main(VS_INPUT In)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	In.pos.w = 1.0f;

	//ボーンウェイト計算
	matrix worldTransform = mul(BoneArray[In.boneIndices[0]], In.boneWeights[0]);
	worldTransform += mul(BoneArray[In.boneIndices[1]], In.boneWeights[1]);
	worldTransform += mul(BoneArray[In.boneIndices[2]], In.boneWeights[2]);
	worldTransform += mul(BoneArray[In.boneIndices[3]], In.boneWeights[3]);

	// 移動成分をなくしたワールド行列
	float3x3 worldMat = (float3x3)worldTransform;

	output.posw = mul(float4(In.pos.xyz + In.normal * FarScaleOffset * (float)In.instanceID, 1.0f), worldTransform);	// ここで拡大していく
	output.pos = mul(output.posw, View);
	output.pos = mul(output.pos, Projection);
	output.uv = In.uv;
	output.normal = normalize(mul(In.normal, worldMat));
	output.farAlpha = 1.0 - ((float)In.instanceID / (float)FarMaxNum);		// インスタンス番号に上がるにつれて値を小さくする

	// ライトの深度テクスチャのの座標に変換
	output.lightViewPos = output.posw;
	output.lightViewPos = mul(output.lightViewPos, LightView);
	output.lightViewPos = mul(output.lightViewPos, LightProjection); //ライトビューにおける位置(変換後)

	// 接空間、従法線を使う処理
	float3 N = output.normal;
	float3 T = normalize(mul(In.tangent, worldMat));
	float3 B = normalize(mul(In.binormal, worldMat));
	// 転置の順に格納y
	float3x3 tangentToObject = float3x3(T.x, B.x, N.x,
		T.y, B.y, N.y,
		T.z, B.z, N.z);

	float3 eyeDir = normalize(output.posw.xyz - Camera.eyePos.xyz);
	float3 lightDir = DirLight.direction.xyz;

	// 接空間に変換
	//output.eyeDir = normalize(mul(eyeDir, tangentToObject));
	output.eyePos = mul(Camera.eyePos.xyz, tangentToObject);
	output.posw.xyz = mul(output.posw.xyz, tangentToObject);
	output.lightDir = normalize(mul(lightDir, tangentToObject));

	return output;
}
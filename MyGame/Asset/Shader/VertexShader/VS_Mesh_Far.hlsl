
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

// ライトの影用バッファ
cbuffer LightShadowBuffer : register(b4)
{
	matrix LightView;
	matrix LightProjection;
}

// 頂点レイアウト
struct VS_INPUT {
	float4 pos        : POSITION;
	float2 uv         : TEXCOORD;
	float3 normal     : NORMAL;
	uint instanceID : SV_InstanceID;	// インスタンシングの番号
};

// ピクセルシェーダに送るデータ
struct VS_OUTPUT {
	float4 pos			: SV_POSITION;		// POSITIONだと表示されない
	float4 posw			: POSITION0;		// 計算用のワールド座標
	float2 uv		    : TEXCOORD0;
	float3 normal		: NORMAL;
	float4 lightViewPos : POSITION1;		// ライトビューの深度テクスチャ座標
	float  farAlpha : PSIZE;		    // 毛の先端部分をだんだん透明にしていく値
};


//=============================================================================
// 頂点シェーダ	------ インスタンシング -------
//=============================================================================
VS_OUTPUT main(VS_INPUT In)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	In.pos.w = 1.0f;

	output.posw = mul(float4(In.pos.xyz + In.normal * FarScaleOffset * (float)In.instanceID, 1.0f), World);
	output.pos = mul(output.posw, View);
	output.pos = mul(output.pos, Projection);
	output.uv = In.uv;
	output.normal = mul(In.normal, (float3x3)World);
	output.farAlpha = 1.0 - ((float)In.instanceID / (float)FarMaxNum);		// インスタンス番号に上がるにつれて値を小さくする

	// ライトの深度テクスチャのの座標に変換
	output.lightViewPos = output.posw;
	output.lightViewPos = mul(output.lightViewPos, LightView);
	output.lightViewPos = mul(output.lightViewPos, LightProjection); //ライトビューにおける位置(変換後)

	return output;
}
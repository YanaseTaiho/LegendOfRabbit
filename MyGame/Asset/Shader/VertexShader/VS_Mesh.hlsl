

//*****************************************************************************
// 定数バッファ
//*****************************************************************************

// マトリクスバッファ
cbuffer WorldBuffer : register(b0)
{
	matrix World;
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
};

// ピクセルシェーダに送るデータ
struct VS_OUTPUT {
	float4 pos			: SV_POSITION;		// POSITIONだと表示されない
	float4 posw			: POSITION0;		// 計算用のワールド座標
	float2 uv		    : TEXCOORD0;
	float3 normal		: NORMAL;
	float4 lightViewPos : POSITION1;		// ライトビューの深度テクスチャ座標
};


//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_OUTPUT main(VS_INPUT In)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);

	In.pos.w = 1.0f;

	output.posw = mul(In.pos, World);
	output.pos = mul(In.pos, wvp);
	output.uv = In.uv;
	output.normal = mul(In.normal, (float3x3)World);

	// ライトの深度テクスチャのの座標に変換
	output.lightViewPos = output.posw;
	output.lightViewPos = mul(output.lightViewPos, LightView);
	output.lightViewPos = mul(output.lightViewPos, LightProjection); //ライトビューにおける位置(変換後)

	return output;
}


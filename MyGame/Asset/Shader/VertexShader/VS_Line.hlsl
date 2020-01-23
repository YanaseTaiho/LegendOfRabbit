

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

// カラーバッファ
cbuffer ColorBuffer : register(b3)
{
	float4 Color;
}

// 頂点レイアウト
struct VS_INPUT {
	float4 pos        : POSITION;
};

// ピクセルシェーダに送るデータ
struct VS_OUTPUT {
	float4 pos     : SV_POSITION;
	float4 color   : COLOR;
	float3 posw	   : POSITION0;
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

	output.pos = mul(In.pos, wvp);
	output.color = Color;
	output.posw = mul(In.pos, World).xyz;

	return output;
}


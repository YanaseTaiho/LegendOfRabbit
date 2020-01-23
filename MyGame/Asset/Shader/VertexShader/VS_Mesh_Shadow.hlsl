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

// 頂点レイヤー
struct VS_INPUT {
    float4 pos      : POSITION;
};

// ピクセルシェーダに送るデータ (空レンダリング
struct VS_OUTPUT {
    float4 pos   : SV_POSITION;
	float4 depth : POSITION0;
};

//------------------------------------------------------------------------------------------------
// 関数定義
//------------------------------------------------------------------------------------------------
// エントリポイント
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
	input.pos.w = 1.0f;
    output.pos = mul(input.pos, World);
    output.pos = mul(output.pos, LightView);
    output.pos = mul(output.pos, LightProjection);
	output.depth = output.pos;

    return output;
}
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

// 頂点レイアウト
struct VS_INPUT {
	float4 pos        : POSITION;
	float2 tex        : TEXCOORD;
	float3 normal     : NORMAL;
	int4   boneIndices: BLENDINDICES;
	float4 boneWeights: BLENDWEIGHT;
};

// ピクセルシェーダに送るデータ (空レンダリング)
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

	//ボーンウェイト計算
	matrix worldTransform = mul(BoneArray[input.boneIndices[0]], input.boneWeights[0]);
	worldTransform += mul(BoneArray[input.boneIndices[1]], input.boneWeights[1]);
	worldTransform += mul(BoneArray[input.boneIndices[2]], input.boneWeights[2]);
	worldTransform += mul(BoneArray[input.boneIndices[3]], input.boneWeights[3]);

	float4 posl = mul(input.pos, worldTransform);
	posl = mul(posl, LightView);
	posl = mul(posl, LightProjection);

	// 最終的な頂点位置
	output.pos = posl;
	output.depth = output.pos;

    return output;
}
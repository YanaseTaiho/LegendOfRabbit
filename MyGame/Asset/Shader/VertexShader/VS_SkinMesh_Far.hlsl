//------------------------------------------------------------------------------------------------
// 構造体宣言
//------------------------------------------------------------------------------------------------

// マトリクスバッファ
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


// 頂点レイアウト
struct VS_INPUT {
    float4 pos        : POSITION;
    float2 uv         : TEXCOORD;
    float3 normal     : NORMAL;
	int4   boneIndices: BLENDINDICES;
	float4 boneWeights: BLENDWEIGHT;
	uint instanceID : SV_InstanceID;	// インスタンシングの番号
};

// ピクセルシェーダに送るデータ
struct VS_OUTPUT {
    float4 pos     : SV_POSITION;		// POSITIONだと表示されない
	float4 posw    : POSITION0;			// 計算用のワールド座標
    float2 uv      : TEXCOORD0;
	float3 normal  : NORMAL;
	float4 lightViewPos : POSITION1;	// ライトビューの深度テクスチャ座標
	float  farAlpha : PSIZE;		    // 毛の先端部分をだんだん透明にしていく値
};

//------------------------------------------------------------------------------------------------
// 関数定義
//------------------------------------------------------------------------------------------------
// エントリポイント
VS_OUTPUT main(VS_INPUT In) {

    VS_OUTPUT output = (VS_OUTPUT)0;
   
    //ボーンウェイト計算
	matrix worldTransform = mul(BoneArray[In.boneIndices[0]], In.boneWeights[0]);
	worldTransform += mul(BoneArray[In.boneIndices[1]], In.boneWeights[1]);
	worldTransform += mul(BoneArray[In.boneIndices[2]], In.boneWeights[2]);
	worldTransform += mul(BoneArray[In.boneIndices[3]], In.boneWeights[3]);

	In.pos.w = 1.0f;

	float4 posw = mul(float4(In.pos.xyz + In.normal * FarScaleOffset * (float)In.instanceID, 1.0f), worldTransform);	// ここで拡大していく
	float4 pos = mul(posw, View);
	pos = mul(pos, Projection);
	float3 nor = mul(In.normal, (float3x3)worldTransform);

	// 最終的な頂点位置
	output.pos = pos;
	output.posw = posw;
	output.uv = In.uv;
	output.normal = nor;
	output.farAlpha = 1.0 - ((float)In.instanceID / (float)FarMaxNum);		// インスタンス番号に上がるにつれて値を小さくする

	// ライトの深度テクスチャのの座標に変換
	output.lightViewPos = mul(In.pos, worldTransform);
	output.lightViewPos = mul(output.lightViewPos, LightView);
	output.lightViewPos = mul(output.lightViewPos, LightProjection); //ライトビューにおける位置(変換後)
	
    return output;
}
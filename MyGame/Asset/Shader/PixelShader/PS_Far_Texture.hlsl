//------------------------------------------------------------------------------------------------
// リソース
//------------------------------------------------------------------------------------------------
Texture2D<float4> Texture : register(t0);
Texture2D<float4> DepthTexture : register(t1);
Texture2D<float4> FarTexture : register(t4);

//------------------------------------------------------------------------------------------------
// サンプラーステート
//------------------------------------------------------------------------------------------------
SamplerState Sampler : register(s0);
SamplerState SamplerLinear : register(s1);

//------------------------------------------------------------------------------------------------
// 構造体定義
//------------------------------------------------------------------------------------------------

struct MATERIAL
{
	float4 ambient;
	float4 diffuse;
	float4 emissive;
	float4 bump;

	float4 specular;
	float specularFactor;

	float shininess;
	float reflection;
	float height;		// 0 ～ 1
};

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

// ベースカラー
cbuffer BaseColorBuffer : register(b0)
{
	float4 BaseColor;
}
// マテリアル
cbuffer MaterialBuffer : register(b1)
{
	MATERIAL Material;
};
// カメラ
cbuffer CameraBuffer : register(b2)
{
	CAMERA Camera;
}
// ライトを使用するかしないか
cbuffer UseLightBuffer : register(b3)
{
	int UseLight;	// 0 : 使用しない、1 : 使用する
	int UseCastShadow;
	int2 use_dummy;
}
// 平行光源
cbuffer DirLightBuffer : register(b4)
{
	DIRECTIONAL_LIGHT DirLight;
}


// 頂点シェーダから送られてくるデータ
struct VS_OUTPUT {
	float4 pos          : SV_POSITION;		// POSITIONだと表示されない
	float4 posw         : POSITION0;		// 計算用のワールド座標
	float2 uv           : TEXCOORD0;
	float3 normal       : NORMAL;
	float4 lightViewPos : POSITION1;		// ライトビューの深度テクスチャ座標
	float  farAlpha     : PSIZE;		    // 毛の先端部分をだんだん透明にしていく値
};

static const float2 offset = float2(0.0f, 0.0f);
static const float2 telling = float2(1.0f, 1.0f);

// エントリポイント
//[earlydepthstencil]	// 最初にＺテストを行う
float4 main(VS_OUTPUT input) : SV_Target
{
	float4 outColor;

	float2 texCoord = offset + input.uv * telling;
	float4 texColor = Texture.Sample(Sampler, texCoord);
	outColor = texColor;
	outColor *= Material.diffuse;
	outColor += Material.ambient;

	// ライトあり
	if (UseLight == 1)
	{
		// 平行光源
		{
			// スペキュラ計算
			float specularFactor;
			float3 normal = normalize(input.normal);
			float3 lightDir = DirLight.direction.xyz;
			float3 eyeDir = normalize(input.posw.xyz - Camera.eyePos.xyz);
			float dotNL = dot(normal, -lightDir);
			// ディフューズ計算
			float lightDiffuseFactor = saturate(0.5 - 0.5 * -dotNL);
			float3 lightSpecular = normalize(2.0 * normal * -dotNL - lightDir);
			specularFactor = pow(saturate(dot(lightSpecular, eyeDir)), /*硬さ*/Material.specular.w) * /*強度*/Material.reflection;

			// ディフューズ
			outColor *= lightDiffuseFactor * DirLight.diffuse;
			// アンビエント
			outColor += Material.ambient.w * DirLight.ambient;
			// スペキュラ
			outColor += specularFactor * Material.specular;

			// 影になってない時だけ処理
			//if (shadowIntensity <= 0.0f)
			{
				// リムライト
				float dotLE = dot(eyeDir, -lightDir);
				float dotNE = dot(normal, eyeDir);
				float rimFactor = pow(saturate(dotLE), 30.0);
				float rimLight = pow(1.0 - abs(dotNE), 3.0);
				outColor += rimLight * rimFactor * DirLight.diffuse;
			}
		}
	}
	// エミッシブ
	outColor.rgb += Material.emissive.rgb;
	outColor.rgb = outColor.rgb * Material.emissive.w;

	float4 farTex = FarTexture.Sample(Sampler, texCoord);	// ファーテクスチャ参照

	outColor.a = Material.diffuse.a * texColor.a * farTex.r * input.farAlpha;	// 先端に近づくほど透過していく
	outColor *= BaseColor;

	return outColor;
}
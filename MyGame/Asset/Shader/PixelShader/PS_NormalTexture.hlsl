//------------------------------------------------------------------------------------------------
// リソース
//------------------------------------------------------------------------------------------------
Texture2D<float4> Texture : register(t0);
Texture2D<float4> DepthTexture : register(t1);
Texture2D<float4> NormalTexture : register(t2);
Texture2D<float4> HeightTexture : register(t3);

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
	float3 eyePos	    : POSITION2;		// 接空間上のライトベクトルと視線ベクトルの中間ベクトル
	float3 lightDir  	: POSITION3;		// 接空間上のライトベクトル
};

static const float2 offset = float2(0.0f, 0.0f);
static const float2 telling = float2(1.0f, 1.0f);

// エントリポイント
[earlydepthstencil]	// 最初にＺテストを行う
float4 main(VS_OUTPUT input) : SV_Target
{
	float4 outColor;

	float2 texCoord = offset + input.uv * telling;

	float4 texColor = Texture.Sample(Sampler, texCoord);
	outColor = Material.diffuse;
	outColor += Material.ambient;

	//影の処理
	float shadowIntensity = 0.0f;	// 最終的な影の影響度
	if (UseCastShadow == 1)
	{
		float2 projectTexCoord;
		float3 lightSpacePos = input.lightViewPos.xyz / input.lightViewPos.w;
		projectTexCoord.x = 0.5f + (0.5f * lightSpacePos.x);
		projectTexCoord.y = 0.5f - (0.5f * lightSpacePos.y);
		// 深度テクスチャの範囲内のみ影を描画
		if ((saturate(projectTexCoord.x) == projectTexCoord.x) &&
			(saturate(projectTexCoord.y) == projectTexCoord.y))
		{
			float4 depthColor = DepthTexture.Sample(SamplerLinear, projectTexCoord);
			float depthValue = depthColor.r;
			float lightLength = lightSpacePos.z - 0.008f;

			if (depthColor.a > 0.0f && depthValue < lightLength)
			{
				const float2 poissonDisk[4] = {
					float2(-0.94201624, -0.39906216),
					float2(0.94558609, -0.76890725),
					float2(-0.094184101, -0.92938870),
					float2(0.34495938, 0.29387760)
				};

				for (int i = 0; i < 4; i++) {
					if (DepthTexture.Sample(SamplerLinear, projectTexCoord + poissonDisk[i] * 0.001).r < lightLength) {
						shadowIntensity += 0.1f;
					}
				}
			}
		}
	}

	// ライトあり
	if (UseLight == 1)
	{
		// 平行光源
		{
			float3 lightDir = input.lightDir;
			float3 eyeDir = normalize(input.posw.xyz - input.eyePos);
			// ハイトテクスチャからデータを取得
			float heightMap = (HeightTexture.Sample(Sampler, texCoord).r - 0.5); // 高さを取得
			float hScale = heightMap * (Material.height * 0.2);
			texCoord = texCoord - hScale * eyeDir.xy;	// 高さ情報をもとに少しずらす
			// ノーマルテクスチャから法線を決定
			float3 normal = normalize(NormalTexture.Sample(Sampler, texCoord).xyz * 2.0 - 1.0);
			normal.y = -normal.y;	// 右手座標から左手座標に変換
			float dotNL = dot(normal, lightDir);
			// ディフューズ計算
			float lightDiffuseFactor = saturate(0.5 - 0.5 * dotNL);
			// スペキュラ計算
			float3 lightSpecular = normalize(2.0 * normal * dotNL - lightDir);
			float specularFactor = pow(saturate(dot(lightSpecular, eyeDir)), /*硬さ*/Material.specular.w) * /*強度*/Material.reflection;

			// ディフューズ
			outColor *= lightDiffuseFactor * DirLight.diffuse * Material.bump;
			// アンビエント
			outColor += Material.ambient.w * DirLight.ambient;
			// スペキュラ
			outColor += specularFactor * Material.specular;

			// 影になってない時だけ処理
			if (shadowIntensity <= 0.0)
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

	outColor *= texColor;

	// 影の影響度を反映
	outColor -= outColor * shadowIntensity;
	// エミッシブ
	outColor += Material.emissive;
	outColor = outColor * Material.emissive.w;

	outColor.a = Material.diffuse.a * texColor.a;
	outColor *= BaseColor;

	return outColor;
}
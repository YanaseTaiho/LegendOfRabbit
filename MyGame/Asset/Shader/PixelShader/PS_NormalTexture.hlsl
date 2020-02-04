//------------------------------------------------------------------------------------------------
// ���\�[�X
//------------------------------------------------------------------------------------------------
Texture2D<float4> Texture : register(t0);
Texture2D<float4> DepthTexture : register(t1);
Texture2D<float4> NormalTexture : register(t2);
Texture2D<float4> HeightTexture : register(t3);

//------------------------------------------------------------------------------------------------
// �T���v���[�X�e�[�g
//------------------------------------------------------------------------------------------------
SamplerState Sampler : register(s0);
SamplerState SamplerLinear : register(s1);

//------------------------------------------------------------------------------------------------
// �\���̒�`
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
	float height;		// 0 �` 1
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
// ���W�X�^��`
//------------------------------------------------------------------------------------------------

// �x�[�X�J���[
cbuffer BaseColorBuffer : register(b0)
{
	float4 BaseColor;
}
// �}�e���A��
cbuffer MaterialBuffer : register(b1)
{
	MATERIAL Material;
};
// �J����
cbuffer CameraBuffer : register(b2)
{
	CAMERA Camera;
}
// ���C�g���g�p���邩���Ȃ���
cbuffer UseLightBuffer : register(b3)
{
	int UseLight;	// 0 : �g�p���Ȃ��A1 : �g�p����
	int UseCastShadow;
	int2 use_dummy;
}
// ���s����
cbuffer DirLightBuffer : register(b4)
{
	DIRECTIONAL_LIGHT DirLight;
}


// ���_�V�F�[�_���瑗���Ă���f�[�^
struct VS_OUTPUT {
	float4 pos          : SV_POSITION;		// POSITION���ƕ\������Ȃ�
	float4 posw         : POSITION0;		// �v�Z�p�̃��[���h���W
	float2 uv           : TEXCOORD0;
	float3 normal       : NORMAL;
	float4 lightViewPos : POSITION1;		// ���C�g�r���[�̐[�x�e�N�X�`�����W
	float3 eyePos	    : POSITION2;		// �ڋ�ԏ�̃��C�g�x�N�g���Ǝ����x�N�g���̒��ԃx�N�g��
	float3 lightDir  	: POSITION3;		// �ڋ�ԏ�̃��C�g�x�N�g��
};

static const float2 offset = float2(0.0f, 0.0f);
static const float2 telling = float2(1.0f, 1.0f);

// �G���g���|�C���g
[earlydepthstencil]	// �ŏ��ɂy�e�X�g���s��
float4 main(VS_OUTPUT input) : SV_Target
{
	float4 outColor;

	float2 texCoord = offset + input.uv * telling;

	float4 texColor = Texture.Sample(Sampler, texCoord);
	outColor = Material.diffuse;
	outColor += Material.ambient;

	//�e�̏���
	float shadowIntensity = 0.0f;	// �ŏI�I�ȉe�̉e���x
	if (UseCastShadow == 1)
	{
		float2 projectTexCoord;
		float3 lightSpacePos = input.lightViewPos.xyz / input.lightViewPos.w;
		projectTexCoord.x = 0.5f + (0.5f * lightSpacePos.x);
		projectTexCoord.y = 0.5f - (0.5f * lightSpacePos.y);
		// �[�x�e�N�X�`���͈͓̔��̂݉e��`��
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

	// ���C�g����
	if (UseLight == 1)
	{
		// ���s����
		{
			float3 lightDir = input.lightDir;
			float3 eyeDir = normalize(input.posw.xyz - input.eyePos);
			// �n�C�g�e�N�X�`������f�[�^���擾
			float heightMap = (HeightTexture.Sample(Sampler, texCoord).r - 0.5); // �������擾
			float hScale = heightMap * (Material.height * 0.2);
			texCoord = texCoord - hScale * eyeDir.xy;	// �����������Ƃɏ������炷
			// �m�[�}���e�N�X�`������@��������
			float3 normal = normalize(NormalTexture.Sample(Sampler, texCoord).xyz * 2.0 - 1.0);
			normal.y = -normal.y;	// �E����W���獶����W�ɕϊ�
			float dotNL = dot(normal, lightDir);
			// �f�B�t���[�Y�v�Z
			float lightDiffuseFactor = saturate(0.5 - 0.5 * dotNL);
			// �X�y�L�����v�Z
			float3 lightSpecular = normalize(2.0 * normal * dotNL - lightDir);
			float specularFactor = pow(saturate(dot(lightSpecular, eyeDir)), /*�d��*/Material.specular.w) * /*���x*/Material.reflection;

			// �f�B�t���[�Y
			outColor *= lightDiffuseFactor * DirLight.diffuse * Material.bump;
			// �A���r�G���g
			outColor += Material.ambient.w * DirLight.ambient;
			// �X�y�L����
			outColor += specularFactor * Material.specular;

			// �e�ɂȂ��ĂȂ�����������
			if (shadowIntensity <= 0.0)
			{
				// �������C�g
				float dotLE = dot(eyeDir, -lightDir);
				float dotNE = dot(normal, eyeDir);
				float rimFactor = pow(saturate(dotLE), 30.0);
				float rimLight = pow(1.0 - abs(dotNE), 3.0);
				outColor += rimLight * rimFactor * DirLight.diffuse;
			}
		}
	}

	outColor *= texColor;

	// �e�̉e���x�𔽉f
	outColor -= outColor * shadowIntensity;
	// �G�~�b�V�u
	outColor += Material.emissive;
	outColor = outColor * Material.emissive.w;

	outColor.a = Material.diffuse.a * texColor.a;
	outColor *= BaseColor;

	return outColor;
}
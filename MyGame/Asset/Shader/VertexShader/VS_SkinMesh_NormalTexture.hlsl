
//------------------------------------------------------------------------------------------------
// �\���̒�`
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
// ���W�X�^��`
//------------------------------------------------------------------------------------------------

// �}�g���N�X�o�b�t�@
cbuffer WorldBuffer : register(b0)
{
	matrix World;
}

// �r���[�o�b�t�@
cbuffer ViewBuffer : register(b1)
{
	matrix View;
}

// �v���W�F�N�V�����o�b�t�@
cbuffer ProjectionBuffer : register(b2)
{
	matrix Projection;
}

// �{�[���s��
cbuffer Armature : register(b3)
{
	matrix BoneArray[256];
}

// ���C�g�̉e�p�o�b�t�@
cbuffer LightShadowBuffer : register(b4)
{
	matrix LightView;
	matrix LightProjection;
}

// �J����
cbuffer CameraBuffer : register(b5)
{
	CAMERA Camera;
}

// ���s����
cbuffer DirLightBuffer : register(b6)
{
	DIRECTIONAL_LIGHT DirLight;
}


// ���_���C�A�E�g
struct VS_INPUT {
	float4 pos        : POSITION;
	float2 uv         : TEXCOORD;
	float3 normal     : NORMAL;
	int4   boneIndices: BLENDINDICES;
	float4 boneWeights: BLENDWEIGHT;
	float3 tangent    : TANGENT;
	float3 binormal   : BINORMAL;
};

// �s�N�Z���V�F�[�_�ɑ���f�[�^
struct VS_OUTPUT {
	float4 pos			: SV_POSITION;		// POSITION���ƕ\������Ȃ�
	float4 posw			: POSITION0;		// �v�Z�p�̃��[���h���W
	float2 uv		    : TEXCOORD0;
	float3 normal		: NORMAL;
	float4 lightViewPos : POSITION1;		// ���C�g�r���[�̐[�x�e�N�X�`�����W
	float3 eyePos		: POSITION2;		// �ڋ�ԏ�̎����x�N�g��
	float3 lightDir		: POSITION3;		// �ڋ�ԏ�̃��C�g�x�N�g��
};


//=============================================================================
// ���_�V�F�[�_
//=============================================================================
VS_OUTPUT main(VS_INPUT In)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	In.pos.w = 1.0f;

	//�{�[���E�F�C�g�v�Z
	matrix worldTransform = mul(BoneArray[In.boneIndices[0]], In.boneWeights[0]);
	worldTransform += mul(BoneArray[In.boneIndices[1]], In.boneWeights[1]);
	worldTransform += mul(BoneArray[In.boneIndices[2]], In.boneWeights[2]);
	worldTransform += mul(BoneArray[In.boneIndices[3]], In.boneWeights[3]);

	// �ړ��������Ȃ��������[���h�s��
	float3x3 worldMat = (float3x3)worldTransform;

	output.posw = mul(In.pos, worldTransform);
	output.pos = mul(output.posw, View);
	output.pos = mul(output.pos, Projection);
	output.uv = In.uv;
	output.normal = normalize(mul(In.normal, worldMat));

	// ���C�g�̐[�x�e�N�X�`���̂̍��W�ɕϊ�
	output.lightViewPos = output.posw;
	output.lightViewPos = mul(output.lightViewPos, LightView);
	output.lightViewPos = mul(output.lightViewPos, LightProjection); //���C�g�r���[�ɂ�����ʒu(�ϊ���)

	// �ڋ�ԁA�]�@�����g������
	float3 N = output.normal;
	float3 T = normalize(mul(In.tangent, worldMat));
	float3 B = normalize(mul(In.binormal, worldMat));
	// �]�u�̏��Ɋi�[y
	float3x3 tangentToObject = float3x3(T.x, B.x, N.x,
										T.y, B.y, N.y,
										T.z, B.z, N.z);

	float3 eyeDir = normalize(output.posw.xyz - Camera.eyePos.xyz);
	float3 lightDir = DirLight.direction.xyz;

	// �ڋ�Ԃɕϊ�
	//output.eyeDir = normalize(mul(eyeDir, tangentToObject));
	output.eyePos = mul(Camera.eyePos.xyz, tangentToObject);
	output.posw.xyz = mul(output.posw.xyz, tangentToObject);
	output.lightDir = normalize(mul(lightDir, tangentToObject));

	return output;
}
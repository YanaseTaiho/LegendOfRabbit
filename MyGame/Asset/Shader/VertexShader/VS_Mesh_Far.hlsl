
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

// �}�g���N�X�o�b�t�@&�t�@�[�o�b�t�@�[
cbuffer WorldBuffer : register(b0)
{
	matrix World;
	uint FarMaxNum;		// �`���
	float FarScaleOffset;  // �@�������Ɋg�債�Ă����Ԋu

	float2 FarDumy;
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

// ���C�g�̉e�p�o�b�t�@
cbuffer LightShadowBuffer : register(b4)
{
	matrix LightView;
	matrix LightProjection;
}

// ���_���C�A�E�g
struct VS_INPUT {
	float4 pos        : POSITION;
	float2 uv         : TEXCOORD;
	float3 normal     : NORMAL;
	uint instanceID : SV_InstanceID;	// �C���X�^���V���O�̔ԍ�
};

// �s�N�Z���V�F�[�_�ɑ���f�[�^
struct VS_OUTPUT {
	float4 pos			: SV_POSITION;		// POSITION���ƕ\������Ȃ�
	float4 posw			: POSITION0;		// �v�Z�p�̃��[���h���W
	float2 uv		    : TEXCOORD0;
	float3 normal		: NORMAL;
	float4 lightViewPos : POSITION1;		// ���C�g�r���[�̐[�x�e�N�X�`�����W
	float  farAlpha : PSIZE;		    // �т̐�[���������񂾂񓧖��ɂ��Ă����l
};


//=============================================================================
// ���_�V�F�[�_	------ �C���X�^���V���O -------
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
	output.farAlpha = 1.0 - ((float)In.instanceID / (float)FarMaxNum);		// �C���X�^���X�ԍ��ɏオ��ɂ�Ēl������������

	// ���C�g�̐[�x�e�N�X�`���̂̍��W�ɕϊ�
	output.lightViewPos = output.posw;
	output.lightViewPos = mul(output.lightViewPos, LightView);
	output.lightViewPos = mul(output.lightViewPos, LightProjection); //���C�g�r���[�ɂ�����ʒu(�ϊ���)

	return output;
}
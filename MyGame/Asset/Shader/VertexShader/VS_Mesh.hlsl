

//*****************************************************************************
// �萔�o�b�t�@
//*****************************************************************************

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
};

// �s�N�Z���V�F�[�_�ɑ���f�[�^
struct VS_OUTPUT {
	float4 pos			: SV_POSITION;		// POSITION���ƕ\������Ȃ�
	float4 posw			: POSITION0;		// �v�Z�p�̃��[���h���W
	float2 uv		    : TEXCOORD0;
	float3 normal		: NORMAL;
	float4 lightViewPos : POSITION1;		// ���C�g�r���[�̐[�x�e�N�X�`�����W
};


//=============================================================================
// ���_�V�F�[�_
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

	// ���C�g�̐[�x�e�N�X�`���̂̍��W�ɕϊ�
	output.lightViewPos = output.posw;
	output.lightViewPos = mul(output.lightViewPos, LightView);
	output.lightViewPos = mul(output.lightViewPos, LightProjection); //���C�g�r���[�ɂ�����ʒu(�ϊ���)

	return output;
}


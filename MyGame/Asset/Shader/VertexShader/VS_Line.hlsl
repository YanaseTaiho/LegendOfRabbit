

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

// �J���[�o�b�t�@
cbuffer ColorBuffer : register(b3)
{
	float4 Color;
}

// ���_���C�A�E�g
struct VS_INPUT {
	float4 pos        : POSITION;
};

// �s�N�Z���V�F�[�_�ɑ���f�[�^
struct VS_OUTPUT {
	float4 pos     : SV_POSITION;
	float4 color   : COLOR;
	float3 posw	   : POSITION0;
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

	output.pos = mul(In.pos, wvp);
	output.color = Color;
	output.posw = mul(In.pos, World).xyz;

	return output;
}


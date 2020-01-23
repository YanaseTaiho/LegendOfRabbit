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

// ���_���C���[
struct VS_INPUT {
    float4 pos      : POSITION;
};

// �s�N�Z���V�F�[�_�ɑ���f�[�^ (�󃌃��_�����O
struct VS_OUTPUT {
    float4 pos   : SV_POSITION;
	float4 depth : POSITION0;
};

//------------------------------------------------------------------------------------------------
// �֐���`
//------------------------------------------------------------------------------------------------
// �G���g���|�C���g
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
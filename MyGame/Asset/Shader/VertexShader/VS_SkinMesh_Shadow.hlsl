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

// ���_���C�A�E�g
struct VS_INPUT {
	float4 pos        : POSITION;
	float2 tex        : TEXCOORD;
	float3 normal     : NORMAL;
	int4   boneIndices: BLENDINDICES;
	float4 boneWeights: BLENDWEIGHT;
};

// �s�N�Z���V�F�[�_�ɑ���f�[�^ (�󃌃��_�����O)
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

	//�{�[���E�F�C�g�v�Z
	matrix worldTransform = mul(BoneArray[input.boneIndices[0]], input.boneWeights[0]);
	worldTransform += mul(BoneArray[input.boneIndices[1]], input.boneWeights[1]);
	worldTransform += mul(BoneArray[input.boneIndices[2]], input.boneWeights[2]);
	worldTransform += mul(BoneArray[input.boneIndices[3]], input.boneWeights[3]);

	float4 posl = mul(input.pos, worldTransform);
	posl = mul(posl, LightView);
	posl = mul(posl, LightProjection);

	// �ŏI�I�Ȓ��_�ʒu
	output.pos = posl;
	output.depth = output.pos;

    return output;
}
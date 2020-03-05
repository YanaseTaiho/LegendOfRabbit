//------------------------------------------------------------------------------------------------
// �\���̐錾
//------------------------------------------------------------------------------------------------

// �}�g���N�X�o�b�t�@
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
    float2 uv         : TEXCOORD;
    float3 normal     : NORMAL;
	int4   boneIndices: BLENDINDICES;
	float4 boneWeights: BLENDWEIGHT;
	uint instanceID : SV_InstanceID;	// �C���X�^���V���O�̔ԍ�
};

// �s�N�Z���V�F�[�_�ɑ���f�[�^
struct VS_OUTPUT {
    float4 pos     : SV_POSITION;		// POSITION���ƕ\������Ȃ�
	float4 posw    : POSITION0;			// �v�Z�p�̃��[���h���W
    float2 uv      : TEXCOORD0;
	float3 normal  : NORMAL;
	float4 lightViewPos : POSITION1;	// ���C�g�r���[�̐[�x�e�N�X�`�����W
	float  farAlpha : PSIZE;		    // �т̐�[���������񂾂񓧖��ɂ��Ă����l
};

//------------------------------------------------------------------------------------------------
// �֐���`
//------------------------------------------------------------------------------------------------
// �G���g���|�C���g
VS_OUTPUT main(VS_INPUT In) {

    VS_OUTPUT output = (VS_OUTPUT)0;
   
    //�{�[���E�F�C�g�v�Z
	matrix worldTransform = mul(BoneArray[In.boneIndices[0]], In.boneWeights[0]);
	worldTransform += mul(BoneArray[In.boneIndices[1]], In.boneWeights[1]);
	worldTransform += mul(BoneArray[In.boneIndices[2]], In.boneWeights[2]);
	worldTransform += mul(BoneArray[In.boneIndices[3]], In.boneWeights[3]);

	In.pos.w = 1.0f;

	float4 posw = mul(float4(In.pos.xyz + In.normal * FarScaleOffset * (float)In.instanceID, 1.0f), worldTransform);	// �����Ŋg�債�Ă���
	float4 pos = mul(posw, View);
	pos = mul(pos, Projection);
	float3 nor = mul(In.normal, (float3x3)worldTransform);

	// �ŏI�I�Ȓ��_�ʒu
	output.pos = pos;
	output.posw = posw;
	output.uv = In.uv;
	output.normal = nor;
	output.farAlpha = 1.0 - ((float)In.instanceID / (float)FarMaxNum);		// �C���X�^���X�ԍ��ɏオ��ɂ�Ēl������������

	// ���C�g�̐[�x�e�N�X�`���̂̍��W�ɕϊ�
	output.lightViewPos = mul(In.pos, worldTransform);
	output.lightViewPos = mul(output.lightViewPos, LightView);
	output.lightViewPos = mul(output.lightViewPos, LightProjection); //���C�g�r���[�ɂ�����ʒu(�ϊ���)
	
    return output;
}

// ���_�V�F�[�_���瑗���Ă���f�[�^
struct VS_OUTPUT {
	float4 pos     : SV_POSITION;	// POSITION���ƕ\������Ȃ�
	float4 color   : COLOR;
};

// �G���g���|�C���g
float4 main(VS_OUTPUT input) : SV_Target
{
	return input.color;
}
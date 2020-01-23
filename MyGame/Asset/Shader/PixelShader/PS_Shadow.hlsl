struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 depth : POSITION0;
};

[earlydepthstencil]	// �ŏ��ɂy�e�X�g���s��
float4 main(PS_INPUT input) : SV_Target
{
	float depth = input.depth.z / input.depth.w;
	return float4(depth, depth, depth, 1.0f);
}
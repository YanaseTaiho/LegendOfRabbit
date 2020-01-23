
// 頂点シェーダから送られてくるデータ
struct VS_OUTPUT {
	float4 pos     : SV_POSITION;	// POSITIONだと表示されない
	float4 color   : COLOR;
};

// エントリポイント
float4 main(VS_OUTPUT input) : SV_Target
{
	return input.color;
}
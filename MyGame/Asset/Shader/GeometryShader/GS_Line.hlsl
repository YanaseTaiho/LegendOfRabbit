
cbuffer LineBuffer : register(b0)
{
	float Thickness;	// ラインの太さ
	float3 CameraPos;
}

// ビューバッファ
cbuffer ViewBuffer : register(b1)
{
	matrix View;
}

// プロジェクションバッファ
cbuffer ProjectionBuffer : register(b2)
{
	matrix Projection;
}


struct VSOutput
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
	float3 posw : POSITION0;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

[maxvertexcount(6)]
void main(
	line VSOutput input[2],
	inout TriangleStream< GSOutput > output
)
{
	float width = Thickness / 2.0f;
	float3 dir = input[0].posw - input[1].posw;
	float3 cameraDir = (input[0].posw + dir * 0.5) - CameraPos;
	float3 dirCross = normalize(cross(dir, cameraDir));
	float4 offset = float4(dirCross * width, 0.0);
	float4 interpolation = float4(normalize(dir) * 0.015, 0.0);
	//float4 offset = float4(width, 0.0, 0.0, 0.0);

	matrix WP = mul(View, Projection);

	{
		GSOutput element;
		element.pos = float4(input[0].posw, 1.0) - offset + interpolation;
		element.pos = mul(element.pos, WP);
		element.col = input[0].col;
		output.Append(element);
	}
	{
		GSOutput element;
		element.pos = float4(input[0].posw, 1.0) + offset + interpolation;
		element.pos = mul(element.pos, WP);
		element.col = input[0].col;
		output.Append(element);
	}
	{
		GSOutput element;
		element.pos = float4(input[1].posw, 1.0) + offset - interpolation;
		element.pos = mul(element.pos, WP);
		element.col = input[1].col;
		output.Append(element);
	}

	output.RestartStrip();

	{
		GSOutput element;
		element.pos = float4(input[1].posw, 1.0) + offset - interpolation;
		element.pos = mul(element.pos, WP);
		element.col = input[1].col;
		output.Append(element);
	}
	{
		GSOutput element;
		element.pos = float4(input[1].posw, 1.0) - offset - interpolation;
		element.pos = mul(element.pos, WP);
		element.col = input[1].col;
		output.Append(element);
	}
	{
		GSOutput element;
		element.pos = float4(input[0].posw, 1.0) - offset + interpolation;
		element.pos = mul(element.pos, WP);
		element.col = input[0].col;
		output.Append(element);
	}

	output.RestartStrip();
}
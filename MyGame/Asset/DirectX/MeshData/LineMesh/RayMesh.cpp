#include "RayMesh.h"
#include "../../Shader/ConstantBuffer.h"
#include "../../Shader/Shader.h"

using namespace MyDirectX;

RayMesh::RayMesh()
{
	std::vector<VTX_LINE> vertex;
	vertex.resize(2);
	meshData.CreateVertexBuffer(&vertex);
}

void RayMesh::Draw(const Vector3& start, const Vector3& end, const Color& color, const float width)
{
	meshData.vertex[0].pos = start.XMFLOAT();
	meshData.vertex[1].pos = end.XMFLOAT();

	// ƒpƒ‰ƒ[ƒ^‚ÌŽó‚¯“n‚µ
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_WORLD, Matrix4());
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_COLOR, color);

	ConstantBuffer::SetVSRegister(0, CB_TYPE::CB_WORLD);
	ConstantBuffer::SetVSRegister(3, CB_TYPE::CB_COLOR);

	auto manager = ShaderFormatManager::Instance();

	if (width > 0.0f)
	{
		CB_LINE_WIDTH cb;
		cb.Thickness = width;
		cb.CameraPos = RendererSystem::GetCameraMatrix().position();
		ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_LINE_WIDTH, cb);
		ConstantBuffer::SetGSRegister(0, CB_TYPE::CB_LINE_WIDTH);
		Shader::SetShader(manager->GetVertexFormat(VERTEX_SHADER_TYPE::LINE), manager->GetPixelFormat(PIXEL_SHADER_TYPE::LINE), manager->GetGeometryFormat(GEOMETRY_SHADER_TYPE::LINE));
	}
	else
	{
		Shader::SetShader(manager->GetVertexFormat(VERTEX_SHADER_TYPE::LINE), manager->GetPixelFormat(PIXEL_SHADER_TYPE::LINE));
	}

	meshData.IASetBuffer();
	meshData.SetVertexBuffer();

	RendererSystem::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	meshData.Draw();
}

void RayMesh::Draw(const Vector3& start, const Vector3& dir, const float& length, const Color& color, const float width)
{
	Draw(start, start + dir * length, color, width);
}


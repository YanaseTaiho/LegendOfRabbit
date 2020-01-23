#include "LineMesh.h"
#include "../../Shader/ConstantBuffer.h"
#include "../../Shader/Shader.h"

using namespace MyDirectX;

LineMesh::LineMesh()
{

}

void LineMesh::Draw(const Matrix4 & matrix, const Color & color, const float width)
{
	// ƒpƒ‰ƒ[ƒ^‚ÌŽó‚¯“n‚µ
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_WORLD, matrix);
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

	RendererSystem::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	meshData.Draw();
}

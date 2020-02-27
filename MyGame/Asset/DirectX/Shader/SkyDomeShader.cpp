#include "SkyDomeShader.h"
#include "../Material/Material.h"

using namespace MyDirectX;

void SkyDomeShader::Draw(const Material * material, const MeshData<VTX_MESH>* mesh, unsigned short startIndex, unsigned short indexNum)
{
	SetVertexShaderFormat(VERTEX_SHADER_TYPE::MESH);
	SetPixelShaderFormat(PIXEL_SHADER_TYPE::SKYDOME);
	ResetGeometryShaderFormat();

	SetShader();

	mesh->DrawIndexed(indexNum, startIndex);
}

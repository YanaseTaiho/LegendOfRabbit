#include "SkyDomeShader.h"
#include "../Material/Material.h"

using namespace MyDirectX;

void SkyDomeShader::SetOption(const Material * material)
{	
	SetVertexShaderFormat(VERTEX_SHADER_TYPE::MESH);
	SetPixelShaderFormat(PIXEL_SHADER_TYPE::SKYDOME);
	ResetGeometryShaderFormat();
}

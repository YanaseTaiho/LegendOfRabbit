#include "GeometryShaderInfo.h"

using namespace MyDirectX;

GeometryShaderInfo::GeometryShaderInfo()
{
}

GeometryShaderInfo::~GeometryShaderInfo()
{
	if (m_pGeomtryShader) m_pGeomtryShader->Release();
}

void GeometryShaderInfo::CreateShader(const std::wstring gsPath)
{
	ID3DBlob *pCompileGS = NULL;

	D3DCompileFromFile(gsPath.c_str(), NULL, NULL, "main", "gs_5_0", NULL, 0, &pCompileGS, NULL);
	RendererSystem::GetDevice()->CreateGeometryShader(pCompileGS->GetBufferPointer(), pCompileGS->GetBufferSize(), NULL, &m_pGeomtryShader);

	pCompileGS->Release();
}

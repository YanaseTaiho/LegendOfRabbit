#include "PixelShaderInfo.h"

using namespace MyDirectX;

PixelShaderInfo::~PixelShaderInfo()
{
	if (m_pPixelShader)     m_pPixelShader->Release();
	if (m_pRasterizerState) m_pRasterizerState->Release();
	if (m_pBlendState)      m_pBlendState->Release();
}

void PixelShaderInfo::Initialize()
{
	CreateBlendState();
}

void PixelShaderInfo::CreateShader(const std::wstring pixelPath)
{
	ID3DBlob *pCompilePS = NULL;

	D3DCompileFromFile(pixelPath.c_str(), NULL, NULL, "main", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	RendererSystem::GetDevice()->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &m_pPixelShader);

	pCompilePS->Release();
}

void PixelShaderInfo::CreateRasterizerState(D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode)
{
	D3D11_RASTERIZER_DESC rdc = {};

	rdc.FillMode = fillMode;
	rdc.CullMode = cullMode;
	rdc.FrontCounterClockwise = FALSE;

	RendererSystem::GetDevice()->CreateRasterizerState(&rdc, &m_pRasterizerState);
}

void PixelShaderInfo::CreateBlendState()
{
	D3D11_BLEND_DESC alphaDesc;
	alphaDesc.AlphaToCoverageEnable = FALSE;
	alphaDesc.IndependentBlendEnable = FALSE;
	alphaDesc.RenderTarget[0].BlendEnable = TRUE;
	alphaDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alphaDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	alphaDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	alphaDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	alphaDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	alphaDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	RendererSystem::GetDevice()->CreateBlendState(&alphaDesc, &m_pBlendState);
}

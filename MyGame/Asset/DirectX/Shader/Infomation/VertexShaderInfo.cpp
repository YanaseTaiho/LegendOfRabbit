#include "VertexShaderInfo.h"

using namespace MyDirectX;

VertexShaderInfo::VertexShaderInfo()
{

}

VertexShaderInfo::~VertexShaderInfo()
{
	if (m_pVtxShader)      m_pVtxShader->Release();
	if (m_pVtxLayout)      m_pVtxLayout->Release();
}

void VertexShaderInfo::CreateShader(const std::wstring vtxPath, const LAYOUT *pLayoutTypes, int layoutCnt)
{
	ID3DBlob *pCompileVS = NULL;

	D3DCompileFromFile(vtxPath.c_str(), NULL, NULL, "main", "vs_5_0", NULL, 0, &pCompileVS, NULL);
	RendererSystem::GetDevice()->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &m_pVtxShader);

	D3D11_INPUT_ELEMENT_DESC *layout = new D3D11_INPUT_ELEMENT_DESC[layoutCnt];
	for (int i = 0; i < layoutCnt; i++)
	{
		switch (pLayoutTypes[i])
		{
		case LAYOUT::POSITION:
			layout[i].SemanticName = "POSITION";
			layout[i].SemanticIndex = 0;
			layout[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			layout[i].InputSlot = 0;
			layout[i].AlignedByteOffset = 0;
			layout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			layout[i].InstanceDataStepRate = 0;
			break;

		case LAYOUT::TEXCOORD:
			layout[i].SemanticName = "TEXCOORD";
			layout[i].SemanticIndex = 0;
			layout[i].Format = DXGI_FORMAT_R32G32_FLOAT;
			layout[i].InputSlot = 0;
			layout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			layout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			layout[i].InstanceDataStepRate = 0;
			break;

		case LAYOUT::NORMAL:
			layout[i].SemanticName = "NORMAL";
			layout[i].SemanticIndex = 0;
			layout[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			layout[i].InputSlot = 0;
			layout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			layout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			layout[i].InstanceDataStepRate = 0;
			break;

		case LAYOUT::BLENDWEIGHT:
			layout[i].SemanticName = "BLENDWEIGHT";
			layout[i].SemanticIndex = 0;
			layout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			layout[i].InputSlot = 0;
			layout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			layout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			layout[i].InstanceDataStepRate = 0;
			break;

		case LAYOUT::BLENDINDICES:
			layout[i].SemanticName = "BLENDINDICES";
			layout[i].SemanticIndex = 0;
			layout[i].Format = DXGI_FORMAT_R32G32B32A32_SINT;
			layout[i].InputSlot = 0;
			layout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			layout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			layout[i].InstanceDataStepRate = 0;
			break;

		case LAYOUT::TANGENT:
			layout[i].SemanticName = "TANGENT";
			layout[i].SemanticIndex = 0;
			layout[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			layout[i].InputSlot = 0;
			layout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			layout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			layout[i].InstanceDataStepRate = 0;
			break;

		case LAYOUT::BINORMAL:
			layout[i].SemanticName = "BINORMAL";
			layout[i].SemanticIndex = 0;
			layout[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			layout[i].InputSlot = 0;
			layout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			layout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			layout[i].InstanceDataStepRate = 0;
			break;

		default:
			MessageBox(NULL, "CreateVtxLayout", "", MB_OK);
		}
	}

	RendererSystem::GetDevice()->CreateInputLayout(layout, layoutCnt, pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &m_pVtxLayout);

	pCompileVS->Release();

	delete[] layout;
}

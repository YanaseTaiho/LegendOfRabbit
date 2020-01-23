#ifndef _VERTEXSHDAERDATA_H_
#define _VERTEXSHDAERDATA_H_

#include "../../Renderer/System/RendererSystem.h"
#include <string>

namespace MyDirectX
{
	enum class LAYOUT
	{
		POSITION,
		TEXCOORD,
		NORMAL,
		BLENDWEIGHT,	// float4
		BLENDINDICES,	// int4
		TANGENT,
		BINORMAL
	};

	class VertexShaderInfo
	{
	public:
		VertexShaderInfo();
		~VertexShaderInfo();

		void CreateShader(const std::wstring vtxPath, const LAYOUT *pLayoutTypes, int layoutCnt);

		ID3D11VertexShader *m_pVtxShader;
		ID3D11InputLayout *m_pVtxLayout;

	private:

		
	};
}

#endif // !_VERTEXSHDAERDATA_H_
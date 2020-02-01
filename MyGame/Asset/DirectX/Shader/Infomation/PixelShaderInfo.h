#ifndef _PIXELSHADERDATA_H_
#define _PIXELSHADERDATA_H_

#include "../../Renderer/System/RendererSystem.h"
#include <string>

namespace MyDirectX
{
	class PixelShaderInfo
	{
	public:
		PixelShaderInfo() {}
		~PixelShaderInfo();

		void Initialize();
		void CreateShader(const std::wstring pixelPath);
		//void CreateRasterizerState(D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode);

		ID3D11PixelShader *m_pPixelShader;
		ID3D11BlendState *m_pBlendState;
	private:

		void CreateBlendState();
	};
}

#endif // !_PIXELSHADERDATA_H_


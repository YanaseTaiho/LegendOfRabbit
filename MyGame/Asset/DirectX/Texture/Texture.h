#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "../Renderer/System/RendererSystem.h"
#include <string>

namespace MyDirectX
{
	class TextureManager;

	class Texture
	{
		friend TextureManager;
	public:
		Texture() { m_ShaderResourceView = NULL; }
		~Texture() { Unload(); }

		ID3D11ShaderResourceView * const * GetShaderResourceView() { return &m_ShaderResourceView; }

		std::string GetName() { return name; }
		UINT GetWidth() { return width; }
		UINT GetHeight() { return height; }

	private:

		bool Load(std::string filename);
		void Unload();

		ID3D11ShaderResourceView*	m_ShaderResourceView;
		std::string name;
		UINT width;
		UINT height;
	};
}

#endif
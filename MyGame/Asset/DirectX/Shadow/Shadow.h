#ifndef _SHADOW_H_
#define _SHADOW_H_

#include "../DirectX.h"

namespace MyDirectX
{

#define DEPTH_TEXTURE_WIDTH (2048)
#define DEPTH_TEXTURE_HEIGHT (2048)

	class Shadow
	{
	public:
		void Initialize();
		void Crear();
		void SetViewPort();
		void SetRenderTarget();
		void SetResources();
		void Release();

		ID3D11Texture2D* depthTexture;	// �����_�[�^�[�Q�b�g�p
		ID3D11Texture2D* depthTextureSTV;	// �X�e���V���r���[�p
	    ID3D11RenderTargetView*	renderTargetView;
		ID3D11ShaderResourceView* shaderResourceView;
		ID3D11DepthStencilView* depthStencilView;
		ID3D11SamplerState * samplerState;
		// �[�x�e�N�X�`���p�r���[�|�[�g
		D3D11_VIEWPORT* viewPort;
	};
}

#endif // !_SHADOW_H_


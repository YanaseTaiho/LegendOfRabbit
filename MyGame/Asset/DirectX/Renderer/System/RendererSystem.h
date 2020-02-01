#ifndef ___CRENDERER_H_
#define ___CRENDERER_H_ 

#include "../../DirectX.h"
#include "Main/Common.h"
#include <memory>

#include "FrameWork/Matrix.h"

using namespace FrameWork;

namespace MyDirectX
{
#define SCREEN_WIDTH	(1920)			// �E�C���h�E�̕�
#define SCREEN_HEIGHT	(1080)			// �E�C���h�E�̍���

	class Texture;
	class Shadow;

	enum class Rasterizer : int
	{
		FILL_SOLID_AND_CULL_BACK,
		FILL_SOLID_AND_CULL_NONE,
		MAX_NUM
	};

	static std::string enum_to_string(Rasterizer rasterizer)
	{
		switch (rasterizer)
		{
		case Rasterizer::FILL_SOLID_AND_CULL_BACK: return "FILL_SOLID_AND_CULL_BACK";
		case Rasterizer::FILL_SOLID_AND_CULL_NONE:  return "FILL_SOLID_AND_CULL_NONE";
		}
		return "";
	}

	class RendererSystem
	{
	private:
		static D3D_FEATURE_LEVEL       m_FeatureLevel;

		static ID3D11Device*           m_D3DDevice;
		static ID3D11DeviceContext*    m_ImmediateContext;
		static IDXGISwapChain*         m_SwapChain;
		static ID3D11RenderTargetView* m_RenderTargetView;
		static ID3D11DepthStencilView* m_DepthStencilView;
		static ID3D11Texture2D* m_DepthTexture;

		static D3D11_VIEWPORT* m_viewPort;

		static Matrix4 cameraMatrix;
		static Matrix4 viewMatrix;
		static Matrix4 projectionMatrix;

		static ID3D11DepthStencilState* m_DepthStateEnable;
		static ID3D11DepthStencilState* m_DepthStateDisable;

		static ID3D11RasterizerState *m_RasterizerStates[(int)Rasterizer::MAX_NUM];

		static ID3D11SamplerState* m_SamplerState;

		static Shadow * m_Shadow;

	public:
		static void Init();
		static void Uninit();
		static void Begin();
		static void End();

		static HWND GetWindow() { return hWnd; }
		static void ResetRenderTarget(LPARAM lParam);

		// ���C�g���g�p���邩���Ȃ���
		static void SetUseLightOption(bool useLight, bool useCastShadow);
		static void SetDepthEnable(bool Enable);
		static void SetRasterizerState(D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode);

		static void SetWorldViewProjection2D();
		static void SetCameraMatrix(const Matrix4 & CameraMatrix);
		static void SetViewMatrix(const Matrix4 & ViewMatrix);
		static void SetProjectionMatrix(const Matrix4 & ProjectionMatrix);
		static void SetTexture(UINT startSlot, Texture* texture);
		static void SetTexture(UINT startSlot, const std::weak_ptr<Texture>& texture);

		static void SetDefaultOption();	// �ʏ�̃����_�����O
		static void SetShadowOption();	// �e�Ɏg���[�x�e�N�X�`���̃����_�����O

		static void SetRasterizerState(Rasterizer type);

		static void SetShadowTexture();
		static ID3D11ShaderResourceView* GetShadowTexture();

		static ID3D11Device* GetDevice(void) { return m_D3DDevice; }
		static ID3D11DeviceContext* GetDeviceContext(void) { return m_ImmediateContext; }
		static ID3D11RenderTargetView * GetRenderTargetView() { return m_RenderTargetView; }
		static IDXGISwapChain * GetSwapChain() { return m_SwapChain; }
		static ID3D11DepthStencilView** GetDepthStencilView() { return &m_DepthStencilView; }
		static D3D11_VIEWPORT** GetViewPort() { return &m_viewPort; }

		static Matrix4& GetCameraMatrix() { return cameraMatrix; }
		static Matrix4& GetViewMatrix() { return viewMatrix; }
		static Matrix4& GetProjectionMatrix() { return projectionMatrix; }

		static HWND hWnd;
	};
}

#endif //!___RENDERER_H_
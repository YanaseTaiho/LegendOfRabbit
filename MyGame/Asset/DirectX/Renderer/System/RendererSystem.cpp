#include "RendererSystem.h"
#include "../../Texture/Texture.h"
#include "../../Shadow/Shadow.h"

#include "../../Shader/ConstantBuffer.h"
#include "../../MeshData/LineMesh/DebugLine.h"
#include "../../MeshData/PlaneMesh/PlaneMesh.h"

using namespace MyDirectX;

D3D_FEATURE_LEVEL       RendererSystem::m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

ID3D11Device*           RendererSystem::m_D3DDevice = NULL;
ID3D11DeviceContext*    RendererSystem::m_ImmediateContext = NULL;
IDXGISwapChain*         RendererSystem::m_SwapChain = NULL;
ID3D11RenderTargetView* RendererSystem::m_RenderTargetView = NULL;
ID3D11DepthStencilView* RendererSystem::m_DepthStencilView = NULL;
ID3D11Texture2D* RendererSystem::m_DepthTexture = NULL;

D3D11_VIEWPORT* RendererSystem::m_viewPort = NULL;

ID3D11DepthStencilState* RendererSystem::m_DepthStateEnable;
ID3D11DepthStencilState* RendererSystem::m_DepthStateDisable;

ID3D11RasterizerState* RendererSystem::m_RasterizerStates[(int)Rasterizer::MAX_NUM];

ID3D11SamplerState * RendererSystem::m_SamplerState;

Shadow * RendererSystem::m_Shadow;

Matrix4 RendererSystem::cameraMatrix;
Matrix4 RendererSystem::viewMatrix;
Matrix4 RendererSystem::projectionMatrix;

HWND RendererSystem::hWnd;

void RendererSystem::Init()
{
	HRESULT hr = S_OK;

	// デバイス、スワップチェーン、コンテキスト生成
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = SCREEN_WIDTH;
	sd.BufferDesc.Height = SCREEN_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = GetWindow();
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	hr = D3D11CreateDeviceAndSwapChain( NULL,
										D3D_DRIVER_TYPE_HARDWARE,
										NULL,
										0,
										NULL,
										0,
										D3D11_SDK_VERSION,
										&sd,
										&m_SwapChain,
										&m_D3DDevice,
										&m_FeatureLevel,
										&m_ImmediateContext );


	// レンダーターゲットビュー生成、設定
	ID3D11Texture2D* pBackBuffer = NULL;
	m_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	m_D3DDevice->CreateRenderTargetView( pBackBuffer, NULL, &m_RenderTargetView );
	pBackBuffer->Release();

	//ステンシル用テクスチャー作成
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory( &td, sizeof(td) );
	RECT rect;
	::GetClientRect(hWnd, &rect);
	td.Width			= sd.BufferDesc.Width;
	td.Height			= sd.BufferDesc.Height;
//	td.Width			= rect.right;
//	td.Height			= rect.bottom;
	td.MipLevels		= 1;
	td.ArraySize		= 1;
	td.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc		= sd.SampleDesc;
	td.Usage			= D3D11_USAGE_DEFAULT;
	td.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags	= 0;
	td.MiscFlags		= 0;
	m_D3DDevice->CreateTexture2D( &td, NULL, &m_DepthTexture);
	
	//ステンシルターゲット作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory( &dsvd, sizeof(dsvd) );
	dsvd.Format			= td.Format;
	dsvd.ViewDimension	= D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags			= 0;
	m_D3DDevice->CreateDepthStencilView(m_DepthTexture, &dsvd, &m_DepthStencilView );


	m_ImmediateContext->OMSetRenderTargets( 1, &m_RenderTargetView, m_DepthStencilView );

	m_viewPort = new D3D11_VIEWPORT();
	// ビューポート設定
	m_viewPort->Width = (FLOAT)SCREEN_WIDTH;
	m_viewPort->Height = (FLOAT)SCREEN_HEIGHT;
	m_viewPort->MinDepth = 0.0f;
	m_viewPort->MaxDepth = 1.0f;
	m_viewPort->TopLeftX = 0;
	m_viewPort->TopLeftY = 0;
	m_ImmediateContext->RSSetViewports( 1, m_viewPort);

	
	{
		// ラスタライザステート設定
		D3D11_RASTERIZER_DESC rd;
		ZeroMemory(&rd, sizeof(rd));
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_NONE;
		rd.DepthClipEnable = TRUE;
		rd.MultisampleEnable = FALSE;

		ID3D11RasterizerState *rs;
		m_D3DDevice->CreateRasterizerState(&rd, &rs);

		m_ImmediateContext->RSSetState(rs);

		// ブレンドステート設定
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		ID3D11BlendState* blendState = NULL;
		m_D3DDevice->CreateBlendState(&blendDesc, &blendState);
		m_ImmediateContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);
	}


	// 深度ステンシルステート設定
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory( &depthStencilDesc, sizeof( depthStencilDesc ) );
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;

	m_D3DDevice->CreateDepthStencilState( &depthStencilDesc, &m_DepthStateEnable );//深度有効ステート

	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ZERO;
	m_D3DDevice->CreateDepthStencilState( &depthStencilDesc, &m_DepthStateDisable );//深度無効ステート

	m_ImmediateContext->OMSetDepthStencilState( m_DepthStateEnable, NULL );

	// ラスタライザ生成
	{
		D3D11_RASTERIZER_DESC rdc = {};

		rdc.FillMode = D3D11_FILL_SOLID;
		rdc.CullMode = D3D11_CULL_BACK;
		rdc.FrontCounterClockwise = FALSE;

		RendererSystem::GetDevice()->CreateRasterizerState(&rdc, &m_RasterizerStates[(int)Rasterizer::FILL_SOLID_AND_CULL_BACK]);
	}
	{
		D3D11_RASTERIZER_DESC rdc = {};

		rdc.FillMode = D3D11_FILL_SOLID;
		rdc.CullMode = D3D11_CULL_NONE;
		rdc.FrontCounterClockwise = FALSE;

		RendererSystem::GetDevice()->CreateRasterizerState(&rdc, &m_RasterizerStates[(int)Rasterizer::FILL_SOLID_AND_CULL_NONE]);
	}

	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	m_D3DDevice->CreateSamplerState( &samplerDesc, &m_SamplerState);

	m_ImmediateContext->PSSetSamplers( 0, 1, &m_SamplerState);

	// デバッグ用
//#if defined(_DEBUG) || defined(DEBUG)
	DebugLine::CreateDebugLines();
//#endif
	// シェーダーのバッファを一括生成
	ConstantBuffer::Create();

	// 影の描画に必要な初期化
	m_Shadow = new Shadow();
	m_Shadow->Initialize();

	// 板ポリゴン初期化
	PlaneMesh::CreateMesh();
}



void RendererSystem::Uninit()
{
	PlaneMesh::ReleaseMesh();
//#if defined(_DEBUG) || defined(DEBUG)
	DebugLine::Release();
//#endif

	// バッファ解放
	ConstantBuffer::Release();
	ShaderFormatManager::Destroy();

	// オブジェクト解放
	if( m_ImmediateContext )	m_ImmediateContext->ClearState();
	if( m_RenderTargetView )	m_RenderTargetView->Release();
	if( m_SwapChain )			m_SwapChain->Release();
	if( m_ImmediateContext )	m_ImmediateContext->Release();
	if( m_D3DDevice )			m_D3DDevice->Release();
	if( m_SamplerState )		m_SamplerState->Release();
	if (m_Shadow)
	{
		m_Shadow->Release();
		delete m_Shadow;
	}
}


void RendererSystem::Begin()
{
	// バックバッファクリア
	float ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	m_ImmediateContext->ClearRenderTargetView( m_RenderTargetView, ClearColor );
	m_ImmediateContext->ClearDepthStencilView( m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}


void RendererSystem::End()
{
	m_SwapChain->Present( 1, 0 );
}

// ウィンドウサイズ変更時に呼び出す
void RendererSystem::ResetRenderTarget(LPARAM lParam)
{
	if (m_RenderTargetView) { m_RenderTargetView->Release(); m_RenderTargetView = NULL; }

	m_SwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);

	ID3D11Texture2D* pBackBuffer = NULL;
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	m_D3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_RenderTargetView);
	pBackBuffer->Release();

	m_viewPort->Width = LOWORD(lParam);
	m_viewPort->Height = HIWORD(lParam);
	m_ImmediateContext->RSSetViewports(1, m_viewPort);

	if (m_DepthTexture) { m_DepthTexture->Release(); m_DepthTexture = NULL; }
	if (m_DepthStencilView) { m_DepthStencilView->Release(); m_DepthStencilView = NULL; }

	//ステンシル用テクスチャー作成
	D3D11_TEXTURE2D_DESC td;
	DXGI_SWAP_CHAIN_DESC sd;
	m_SwapChain->GetDesc(&sd);

	ZeroMemory(&td, sizeof(td));

	td.Width =  sd.BufferDesc.Width;
	td.Height = sd.BufferDesc.Height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc = sd.SampleDesc;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	m_D3DDevice->CreateTexture2D(&td, NULL, &m_DepthTexture);
	//m_DepthStencilView->
	//ステンシルターゲット作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = td.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags = 0;
	m_D3DDevice->CreateDepthStencilView(m_DepthTexture, &dsvd, &m_DepthStencilView);
}

void RendererSystem::SetUseLightOption(bool useLight, bool useCastShadow)
{
	CB_USE_LIGHT cb;
	cb.useLight = (useLight) ? 1 : 0;
	cb.useCastShadow = (useCastShadow) ? 1 : 0;
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_USE_LIGHT, cb);
	ConstantBuffer::SetPSRegister(3, CB_TYPE::CB_USE_LIGHT);
}

void RendererSystem::SetDepthEnable( bool Enable )
{
	if( Enable )
		m_ImmediateContext->OMSetDepthStencilState( m_DepthStateEnable, NULL );
	else
		m_ImmediateContext->OMSetDepthStencilState( m_DepthStateDisable, NULL );
}

void RendererSystem::SetRasterizerState(D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode)
{

}

void RendererSystem::SetWorldViewProjection2D()
{
	Matrix4 world;
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_WORLD, world);
	ConstantBuffer::SetVSRegister(0, CB_TYPE::CB_WORLD);

	Matrix4 view;
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_VIEW, view);
	ConstantBuffer::SetVSRegister(1, CB_TYPE::CB_VIEW);

	Matrix4 projection;
	projection = XMMatrixOrthographicOffCenterLH( 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f );
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_PROJECTION, projection);
	ConstantBuffer::SetVSRegister(2, CB_TYPE::CB_PROJECTION);

}

void RendererSystem::SetCameraMatrix(const Matrix4 & CameraMatrix)
{
	cameraMatrix = CameraMatrix;
}

void RendererSystem::SetViewMatrix(const Matrix4 & ViewMatrix)
{
	viewMatrix = ViewMatrix;

	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_VIEW, viewMatrix);
	ConstantBuffer::SetVSRegister(1, CB_TYPE::CB_VIEW);
	ConstantBuffer::SetGSRegister(1, CB_TYPE::CB_VIEW);
}

void RendererSystem::SetProjectionMatrix(const Matrix4 & ProjectionMatrix)
{
	projectionMatrix = ProjectionMatrix;

	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_PROJECTION, projectionMatrix);
	ConstantBuffer::SetVSRegister(2, CB_TYPE::CB_PROJECTION);
	ConstantBuffer::SetGSRegister(2, CB_TYPE::CB_PROJECTION);
}

void RendererSystem::SetTexture(UINT startSlot, Texture * texture)
{
	if (texture != NULL)
	{
		m_ImmediateContext->PSSetShaderResources(startSlot, 1, texture->GetShaderResourceView());
	}
	else
	{
		ID3D11ShaderResourceView* nullShaderResourceViews = NULL;
		m_ImmediateContext->PSSetShaderResources(startSlot, 1, &nullShaderResourceViews);
	}
}

void RendererSystem::SetTexture(UINT startSlot,const std::weak_ptr<Texture>& texture)
{
	if (!texture.expired())
	{
		m_ImmediateContext->PSSetShaderResources(startSlot, 1, texture.lock()->GetShaderResourceView());
	}
	else
	{
		ID3D11ShaderResourceView* nullShaderResourceViews = NULL;
		m_ImmediateContext->PSSetShaderResources(startSlot, 1, &nullShaderResourceViews);
	}
}

void RendererSystem::SetDefaultOption()
{
	m_ImmediateContext->RSSetViewports(1, m_viewPort);
	m_ImmediateContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
	m_ImmediateContext->PSSetSamplers(0, 1, &m_SamplerState);

	m_ImmediateContext->PSSetShaderResources(1, 1, &m_Shadow->shaderResourceView);
}

void RendererSystem::SetShadowOption()
{
	m_Shadow->Crear();	// 一旦深度テクスチャをリセット
	m_Shadow->SetViewPort();
	m_Shadow->SetRenderTarget();
	m_Shadow->SetResources();
}

void RendererSystem::SetRasterizerState(Rasterizer type)
{
	// ラスタライザセット
	m_ImmediateContext->RSSetState(m_RasterizerStates[(int)type]);
}

void MyDirectX::RendererSystem::SetShadowTexture()
{
	m_ImmediateContext->PSSetShaderResources(0, 1, &m_Shadow->shaderResourceView);
}

ID3D11ShaderResourceView* MyDirectX::RendererSystem::GetShadowTexture()
{
	return m_Shadow->shaderResourceView;
}

#include "Shadow.h"
#include "../Renderer/System/RendererSystem.h"
#include "../Shader/ConstantBuffer.h"

using namespace MyDirectX;

void Shadow::Initialize()
{
	D3D11_TEXTURE2D_DESC renderTextureDesc;
	ZeroMemory(&renderTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	renderTextureDesc.Width = DEPTH_TEXTURE_WIDTH;
	renderTextureDesc.Height = DEPTH_TEXTURE_HEIGHT;
	renderTextureDesc.MipLevels = 1;
	renderTextureDesc.ArraySize = 1;
	renderTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTextureDesc.SampleDesc.Count = 1;
	renderTextureDesc.SampleDesc.Quality = 0;
	renderTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	renderTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderTextureDesc.CPUAccessFlags = 0;
	renderTextureDesc.MiscFlags = 0;

	// レンダーターゲット深度テクスチャ生成
	if (FAILED(RendererSystem::GetDevice()->CreateTexture2D(&renderTextureDesc, NULL, &depthTexture)))
	{
		MessageBox(NULL, "テクスチャ生成失敗", "シャドウ初期化", MB_OK);
		return;
	}

	//深度マップテクスチャー用　レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC descRT;
	ZeroMemory(&descRT, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	descRT.Format = renderTextureDesc.Format;
	descRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	descRT.Texture2D.MipSlice = 0;

	// レンダーターゲット生成
	if (FAILED(RendererSystem::GetDevice()->CreateRenderTargetView(depthTexture, &descRT, &renderTargetView)))
	{
		MessageBox(NULL, "レンダーターゲット生成失敗", "シャドウ初期化", MB_OK);
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = renderTextureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	// 影を書き込む時に使う
	if (FAILED(RendererSystem::GetDevice()->CreateShaderResourceView(depthTexture, &srvDesc, &shaderResourceView)))
	{
		MessageBox(NULL, "シェーダーリソースビュー生成失敗", "シャドウ初期化", MB_OK);
		return;
	}

	//深度マップテクスチャをレンダーターゲットにする際のデプスステンシルビュー用のテクスチャーを作成
	D3D11_TEXTURE2D_DESC stencilTextureDesc;
	ZeroMemory(&stencilTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	stencilTextureDesc.Width = DEPTH_TEXTURE_WIDTH;
	stencilTextureDesc.Height = DEPTH_TEXTURE_HEIGHT;
	stencilTextureDesc.MipLevels = 1;
	stencilTextureDesc.ArraySize = 1;
	stencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	stencilTextureDesc.SampleDesc.Count = 1;
	stencilTextureDesc.SampleDesc.Quality = 0;
	stencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	stencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	stencilTextureDesc.CPUAccessFlags = 0;
	stencilTextureDesc.MiscFlags = 0;

	RendererSystem::GetDevice()->CreateTexture2D(&stencilTextureDesc, NULL, &depthTextureSTV);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = stencilTextureDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//そのテクスチャーに対しデプスステンシルビュー(DSV)を作成
	if (FAILED(RendererSystem::GetDevice()->CreateDepthStencilView(depthTextureSTV, &depthStencilViewDesc, &depthStencilView)))
	{
		MessageBox(NULL, "深度ステンシルビューの生成失敗", "シャドウ初期化", MB_OK);
		return;
	}

	// サンプラーステート作成
	CD3D11_SAMPLER_DESC	desc;
	ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

	RendererSystem::GetDevice()->CreateSamplerState(&desc, &samplerState);
	RendererSystem::GetDeviceContext()->PSSetSamplers(1, 1, &samplerState);

	// ビューポート設定
	viewPort = new D3D11_VIEWPORT();
	viewPort->Width = (FLOAT)DEPTH_TEXTURE_WIDTH;
	viewPort->Height = (FLOAT)DEPTH_TEXTURE_HEIGHT;
	viewPort->MinDepth = 0.0f;
	viewPort->MaxDepth = 1.0f;
	viewPort->TopLeftX = 0;
	viewPort->TopLeftY = 0;


	// 深度テクスチャの範囲を (-1.0, 1.0) -> (0.0, 1.0) に変換用
	/*Vector3 pos = Vector3(0.5f, 0.5f, 0.5f);
	Vector3 scale = Vector3(0.5f, 0.5f, 0.5f);
	Quaternion rot = Quaternion::Identity();
	Matrix4 textureBias(pos, scale, rot);
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_TEXTURE_BIAS, textureBias);
	ConstantBuffer::SetVSRegister(5, CB_TYPE::CB_TEXTURE_BIAS);*/
}

void Shadow::Crear()
{
	//画面クリア
	float ClearColor[4] = { 0,0,0,0 };// クリア色作成　RGBAの順
	//画面クリア
	RendererSystem::GetDeviceContext()->ClearRenderTargetView(renderTargetView, ClearColor);
	//深度バッファクリア
	RendererSystem::GetDeviceContext()->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Shadow::SetViewPort()
{
	RendererSystem::GetDeviceContext()->RSSetViewports(1, viewPort);
}

void Shadow::SetRenderTarget()
{
	RendererSystem::GetDeviceContext()->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}

void Shadow::SetResources()
{
	RendererSystem::GetDeviceContext()->PSSetShaderResources(1, 1, &shaderResourceView);
}

void Shadow::Release()
{
	depthTexture->Release();
	depthTextureSTV->Release();
	shaderResourceView->Release();
	depthStencilView->Release();
	samplerState->Release();
}

#include "Texture.h"

using namespace MyDirectX;

void Texture::Unload()
{
	if(m_ShaderResourceView)
		m_ShaderResourceView->Release();
}

bool Texture::Load(std::string filename)
{
	HRESULT hr;
	D3DX11_IMAGE_INFO info = {};

	// テクスチャをロード
	hr = D3DX11CreateShaderResourceViewFromFileA(
	 RendererSystem::GetDevice(),
	 filename.c_str(),
	 NULL,
	 NULL,
	 &m_ShaderResourceView,
	 NULL);

	if (FAILED(hr))
	{
		MessageBox(NULL, (filename + std::string("の読み込みに失敗！！")).c_str(), "テクスチャ", MB_OK);
		return false;
	}

	//画像の幅高さを取得
	hr = D3DX11GetImageInfoFromFileA(filename.c_str(), nullptr, &info, nullptr);
	if (FAILED(hr))
	{
		MessageBox(NULL, "テクスチャの幅高さを取得に失敗！！", "", MB_OK);
		return false;
	}

	width = info.Width;
	height = info.Height;

	return SUCCEEDED(hr);

}


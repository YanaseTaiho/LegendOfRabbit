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

	// �e�N�X�`�������[�h
	hr = D3DX11CreateShaderResourceViewFromFileA(
	 RendererSystem::GetDevice(),
	 filename.c_str(),
	 NULL,
	 NULL,
	 &m_ShaderResourceView,
	 NULL);

	if (FAILED(hr))
	{
		MessageBox(NULL, (filename + std::string("�̓ǂݍ��݂Ɏ��s�I�I")).c_str(), "�e�N�X�`��", MB_OK);
		return false;
	}

	//�摜�̕��������擾
	hr = D3DX11GetImageInfoFromFileA(filename.c_str(), nullptr, &info, nullptr);
	if (FAILED(hr))
	{
		MessageBox(NULL, "�e�N�X�`���̕��������擾�Ɏ��s�I�I", "", MB_OK);
		return false;
	}

	width = info.Width;
	height = info.Height;

	return SUCCEEDED(hr);

}


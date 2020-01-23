//==================================================================================================
// �C���N���[�h
//==================================================================================================
#include "ShaderFormatManager.h"
#include "Infomation/VertexShaderInfo.h"
#include "Infomation/GeometryShaderInfo.h"
#include "Infomation/PixelShaderInfo.h"

using namespace MyDirectX;

ShaderFormatManager * ShaderFormatManager::instance = nullptr;

//--------------------------------------------------------------------------------------------------
// FBX�̌^�����Ǘ�����N���X
//--------------------------------------------------------------------------------------------------
// �R���X�g���N�^-----------------------------------------------------------------------------------
ShaderFormatManager::ShaderFormatManager()
{
    for (int i = 0; i < (int)VERTEX_SHADER_TYPE::MAX; i++)
    {
        pVertexFormats[i] = std::shared_ptr<VertexShaderInfo>( new VertexShaderInfo() );
    }
	for (int i = 0; i < (int)GEOMETRY_SHADER_TYPE::MAX; i++)
	{
		pGeometryFormats[i] = std::shared_ptr<GeometryShaderInfo>(new GeometryShaderInfo());
	}
	for (int i = 0; i < (int)PIXEL_SHADER_TYPE::MAX; i++)
	{
		pPixelFormats[i] = std::shared_ptr<PixelShaderInfo>(new PixelShaderInfo());
	}

	Initialize();
}

// �f�X�g���N�^-------------------------------------------------------------------------------------
ShaderFormatManager::~ShaderFormatManager()
{
    
}

// ������-------------------------------------------------------------------------------------------
void ShaderFormatManager::Initialize()
{
	// �X�L�����b�V���V�F�[�_�[�t�H�[�}�b�g�쐬
	CreateVertexShader();
	CreateGeometryShader();
	CreatePixelShader();
}

// ���_�V�F�[�_�[����
void ShaderFormatManager::CreateVertexShader()
{
	// ���C��
	{
		LAYOUT layout[] = {
				LAYOUT::POSITION,
		};
		pVertexFormats[(int)VERTEX_SHADER_TYPE::LINE]->CreateShader(
			FilePathShader(L"VertexShader/VS_Line.hlsl"),
			layout,
			ARRAYSIZE(layout)
		);
	}
	// ���b�V��
	{
		LAYOUT layout[] = {
				LAYOUT::POSITION,
				LAYOUT::TEXCOORD,
				LAYOUT::NORMAL,
		};
		pVertexFormats[(int)VERTEX_SHADER_TYPE::MESH]->CreateShader(
			FilePathShader(L"VertexShader/VS_Mesh.hlsl"),
			layout,
			ARRAYSIZE(layout)
		);
	}
	// �X�L�����b�V��
	{
		LAYOUT layout[] = {
				LAYOUT::POSITION,
				LAYOUT::TEXCOORD,
				LAYOUT::NORMAL,
				LAYOUT::BLENDINDICES,
				LAYOUT::BLENDWEIGHT,
		};
		pVertexFormats[(int)VERTEX_SHADER_TYPE::SKINMESH]->CreateShader(
			FilePathShader(L"VertexShader/VS_SkinMesh.hlsl"),
			layout,
			ARRAYSIZE(layout)
		);
	}
	// �V���h�E_���b�V��
	{
		LAYOUT layout[] = {
				LAYOUT::POSITION,
		};
		pVertexFormats[(int)VERTEX_SHADER_TYPE::MESH_SHADOW]->CreateShader(
			FilePathShader(L"VertexShader/VS_Mesh_Shadow.hlsl"),
			layout,
			ARRAYSIZE(layout)
		);
	}
	// �V���h�E_�X�L�����b�V��
	{
		LAYOUT layout[] = {
				LAYOUT::POSITION,
				LAYOUT::TEXCOORD,
				LAYOUT::NORMAL,
				LAYOUT::BLENDINDICES,
				LAYOUT::BLENDWEIGHT,
		};
		pVertexFormats[(int)VERTEX_SHADER_TYPE::SKINMESH_SHADOW]->CreateShader(
			FilePathShader(L"VertexShader/VS_SkinMesh_Shadow.hlsl"),
			layout,
			ARRAYSIZE(layout)
		);
	}
	// �m�[�}���e�N�X�`��_���b�V��
	{
		LAYOUT layout[] = {
				LAYOUT::POSITION,
				LAYOUT::TEXCOORD,
				LAYOUT::NORMAL,
				LAYOUT::TANGENT,
				LAYOUT::BINORMAL
		};
		pVertexFormats[(int)VERTEX_SHADER_TYPE::MESH_NORMAL_TEXTURE]->CreateShader(
			FilePathShader(L"VertexShader/VS_Mesh_NormalTexture.hlsl"),
			layout,
			ARRAYSIZE(layout)
		);
	}
	// �m�[�}���e�N�X�`��_�X�L�����b�V��
	{
		LAYOUT layout[] = {
				LAYOUT::POSITION,
				LAYOUT::TEXCOORD,
				LAYOUT::NORMAL,
				LAYOUT::BLENDINDICES,
				LAYOUT::BLENDWEIGHT,
				LAYOUT::TANGENT,
				LAYOUT::BINORMAL
		};
		pVertexFormats[(int)VERTEX_SHADER_TYPE::SKINMESH_NORMAL_TEXTURE]->CreateShader(
			FilePathShader(L"VertexShader/VS_SkinMesh_NormalTexture.hlsl"),
			layout,
			ARRAYSIZE(layout)
		);
	}
}

void ShaderFormatManager::CreateGeometryShader()
{
	pGeometryFormats[(int)GEOMETRY_SHADER_TYPE::LINE]->CreateShader(FilePathShader(L"GeometryShader/GS_Line.hlsl"));
}

// �s�N�Z���V�F�[�_�[����
void ShaderFormatManager::CreatePixelShader()
{
	// ���b�V���i�e�N�X�`���Ȃ��j
	{
		int type = (int)PIXEL_SHADER_TYPE::MESH_NONE_TEXTURE;
		pPixelFormats[type]->Initialize();
		pPixelFormats[type]->CreateRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_BACK);
		pPixelFormats[type]->CreateShader(FilePathShader(L"PixelShader/PS_NoneTexture.hlsl"));
	}
	// ���b�V���i�e�N�X�`������j
	{
		int type = (int)PIXEL_SHADER_TYPE::MESH_TEXTURE;
		pPixelFormats[type]->Initialize();
		pPixelFormats[type]->CreateRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_BACK);
		pPixelFormats[type]->CreateShader(FilePathShader(L"PixelShader/PS_Texture.hlsl"));
	}
	// ���b�V���i�e�N�X�`�����m�[�}���e�N�X�`���j
	{
		int type = (int)PIXEL_SHADER_TYPE::MESH_NORMAL_TEXTURE;
		pPixelFormats[type]->Initialize();
		pPixelFormats[type]->CreateRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_BACK);
		pPixelFormats[type]->CreateShader(FilePathShader(L"PixelShader/PS_NormalTexture.hlsl"));
	}
	// ���C��
	{
		int type = (int)PIXEL_SHADER_TYPE::LINE;
		pPixelFormats[type]->Initialize();
		pPixelFormats[type]->CreateRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_BACK);
		pPixelFormats[type]->CreateShader(FilePathShader(L"PixelShader/PS_Line.hlsl"));
	}
	// �V���h�E
	{
		int type = (int)PIXEL_SHADER_TYPE::SHADOW;
		pPixelFormats[type]->Initialize();
		pPixelFormats[type]->CreateRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_BACK);
		pPixelFormats[type]->CreateShader(FilePathShader(L"PixelShader/PS_Shadow.hlsl"));
	}
	// �X�J�C�h�[��
	{
		int type = (int)PIXEL_SHADER_TYPE::SKYDOME;
		pPixelFormats[type]->Initialize();
		pPixelFormats[type]->CreateRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_BACK);
		pPixelFormats[type]->CreateShader(FilePathShader(L"PixelShader/PS_SkyDome.hlsl"));
	}
}

ShaderFormatManager * ShaderFormatManager::Create()
{
	if(instance) return instance;
	
	instance = new ShaderFormatManager();

	return instance;
}

ShaderFormatManager * ShaderFormatManager::Instance()
{
	if (instance) return instance;
	// �܂�����Ă��Ȃ������琶��
	return Create();
}

void ShaderFormatManager::Destroy()
{
	if (instance) delete instance;
}

std::weak_ptr<VertexShaderInfo> ShaderFormatManager::GetVertexFormat(VERTEX_SHADER_TYPE type)
{
	return pVertexFormats[(int)type];
}

std::weak_ptr<GeometryShaderInfo> ShaderFormatManager::GetGeometryFormat(GEOMETRY_SHADER_TYPE type)
{
	return pGeometryFormats[(int)type];
}

std::weak_ptr<PixelShaderInfo> ShaderFormatManager::GetPixelFormat(PIXEL_SHADER_TYPE type)
{
	return  pPixelFormats[(int)type];
}

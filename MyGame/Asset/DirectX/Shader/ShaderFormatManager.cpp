//==================================================================================================
// インクルード
//==================================================================================================
#include "ShaderFormatManager.h"
#include "Infomation/VertexShaderInfo.h"
#include "Infomation/GeometryShaderInfo.h"
#include "Infomation/PixelShaderInfo.h"

using namespace MyDirectX;

ShaderFormatManager * ShaderFormatManager::instance = nullptr;

//--------------------------------------------------------------------------------------------------
// FBXの型式を管理するクラス
//--------------------------------------------------------------------------------------------------
// コンストラクタ-----------------------------------------------------------------------------------
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

// デストラクタ-------------------------------------------------------------------------------------
ShaderFormatManager::~ShaderFormatManager()
{
    
}

// 初期化-------------------------------------------------------------------------------------------
void ShaderFormatManager::Initialize()
{
	// スキンメッシュシェーダーフォーマット作成
	CreateVertexShader();
	CreateGeometryShader();
	CreatePixelShader();
}

// 頂点シェーダー生成
void ShaderFormatManager::CreateVertexShader()
{
	// ライン
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
	// メッシュ
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
	// スキンメッシュ
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
	// シャドウ_メッシュ
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
	// シャドウ_スキンメッシュ
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
	// ノーマルテクスチャ_メッシュ
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
	// ノーマルテクスチャ_スキンメッシュ
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

// ピクセルシェーダー生成
void ShaderFormatManager::CreatePixelShader()
{
	// メッシュ（テクスチャなし）
	{
		int type = (int)PIXEL_SHADER_TYPE::MESH_NONE_TEXTURE;
		pPixelFormats[type]->Initialize();
		pPixelFormats[type]->CreateRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_BACK);
		pPixelFormats[type]->CreateShader(FilePathShader(L"PixelShader/PS_NoneTexture.hlsl"));
	}
	// メッシュ（テクスチャあり）
	{
		int type = (int)PIXEL_SHADER_TYPE::MESH_TEXTURE;
		pPixelFormats[type]->Initialize();
		pPixelFormats[type]->CreateRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_BACK);
		pPixelFormats[type]->CreateShader(FilePathShader(L"PixelShader/PS_Texture.hlsl"));
	}
	// メッシュ（テクスチャ＆ノーマルテクスチャ）
	{
		int type = (int)PIXEL_SHADER_TYPE::MESH_NORMAL_TEXTURE;
		pPixelFormats[type]->Initialize();
		pPixelFormats[type]->CreateRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_BACK);
		pPixelFormats[type]->CreateShader(FilePathShader(L"PixelShader/PS_NormalTexture.hlsl"));
	}
	// メッシュ（トゥーン＆ノーマルテクスチャ）
	{
		int type = (int)PIXEL_SHADER_TYPE::MESH_TOON_TEXTURE;
		pPixelFormats[type]->Initialize();
		pPixelFormats[type]->CreateRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_BACK);
		pPixelFormats[type]->CreateShader(FilePathShader(L"PixelShader/PS_Toon.hlsl"));
	}
	// ライン
	{
		int type = (int)PIXEL_SHADER_TYPE::LINE;
		pPixelFormats[type]->Initialize();
		pPixelFormats[type]->CreateRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_BACK);
		pPixelFormats[type]->CreateShader(FilePathShader(L"PixelShader/PS_Line.hlsl"));
	}
	// シャドウ
	{
		int type = (int)PIXEL_SHADER_TYPE::SHADOW;
		pPixelFormats[type]->Initialize();
		pPixelFormats[type]->CreateRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_BACK);
		pPixelFormats[type]->CreateShader(FilePathShader(L"PixelShader/PS_Shadow.hlsl"));
	}
	// スカイドーム
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
	// まだ作られていなかったら生成
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

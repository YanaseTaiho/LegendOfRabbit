#ifndef _FBXFORMATMANAGER_H_
#define _FBXFORMATMANAGER_H_

//==================================================================================================
// インクルード
//==================================================================================================
#include "../Renderer/System/RendererSystem.h"
#include <vector>

namespace MyDirectX
{
	class VertexShaderInfo;
	class GeometryShaderInfo;
	class PixelShaderInfo;

	enum class VERTEX_SHADER_TYPE : int
	{
		LINE,
		MESH,
		MESH_SHADOW,
		MESH_NORMAL_TEXTURE,
		SKINMESH,
		SKINMESH_SHADOW,
		SKINMESH_NORMAL_TEXTURE,
		SKINMESH_FAR_NORMAL_TEXTURE,
		MAX
	};

	enum class GEOMETRY_SHADER_TYPE : int
	{
		LINE,
		MAX
	};

	enum class PIXEL_SHADER_TYPE : int
	{
		MESH_NONE_TEXTURE,		// テクスチャなし
		MESH_TEXTURE,			// テクスチャあり
		MESH_NORMAL_TEXTURE,	// 法線テクスチャあり
		LINE,
		SHADOW,
		SKYDOME,
		MESH_TOON_TEXTURE,
		MESH_ENVIROMENT_MAPPING_TEXTURE,
		MESH_FAR_NORMAL_TEXTURE,
		MAX
	};

	//==================================================================================================
	// クラス宣言
	//==================================================================================================
	// FBXの型式を管理するシングルトンクラス------------------------------------------------------------------------
	class ShaderFormatManager
	{
	public:

		static ShaderFormatManager * Create();
		static ShaderFormatManager * Instance();
		static void Destroy();

		// 型式の共有データを取得
		std::weak_ptr<VertexShaderInfo> GetVertexFormat(VERTEX_SHADER_TYPE type);
		std::weak_ptr<GeometryShaderInfo> GetGeometryFormat(GEOMETRY_SHADER_TYPE type);
		std::weak_ptr<PixelShaderInfo> GetPixelFormat(PIXEL_SHADER_TYPE type);

	private:
		ShaderFormatManager();
		~ShaderFormatManager();

		static ShaderFormatManager * instance;

		std::shared_ptr<VertexShaderInfo> pVertexFormats[(int)VERTEX_SHADER_TYPE::MAX];
		std::shared_ptr<GeometryShaderInfo> pGeometryFormats[(int)GEOMETRY_SHADER_TYPE::MAX];
		std::shared_ptr<PixelShaderInfo> pPixelFormats[(int)PIXEL_SHADER_TYPE::MAX];

		void Initialize();

		void CreateVertexShader();
		void CreateGeometryShader();
		void CreatePixelShader();
	};
}

#endif // !_FBXFORMATMANAGER_H_
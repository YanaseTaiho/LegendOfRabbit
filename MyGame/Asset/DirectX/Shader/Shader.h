#ifndef ___SHADER___H___
#define ___SHADER___H___

#include "ShaderFormatManager.h"
#include "../Shader/Infomation/VertexShaderInfo.h"
#include "../Shader/Infomation/GeometryShaderInfo.h"
#include "../Shader/Infomation/PixelShaderInfo.h"
#include "../MeshData/MeshData.h"

namespace MyDirectX
{
	class Material;
	struct VTX_MESH;
	struct VTX_SKIN_MESH;

	class Shader
	{
		friend cereal::access;
		template <typename Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			
		}
		template <typename Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			
		}
	public:
		Shader() {};
		virtual ~Shader() {};
		// メッシュデータごとに関数分けをする
		virtual void Draw(const Material * material, const MeshData<VTX_MESH> * mesh, unsigned short startIndex, unsigned short indexNum) {}
		virtual void Draw(const Material * material, const MeshData<VTX_SKIN_MESH> * mesh, unsigned short startIndex, unsigned short indexNum) {}
		virtual void DrawImGui(Material * material, int & id) {}

		static void SetShader(
			const std::weak_ptr<VertexShaderInfo> & vertexShader,
			const std::weak_ptr<PixelShaderInfo> & pixelShader,
			const std::weak_ptr<GeometryShaderInfo> & geometryShader = std::weak_ptr<GeometryShaderInfo>())
		{
			if (vertexShader.expired() || pixelShader.expired())
			{
				MessageBox(NULL, "シェーダーフォーマットが設定されてない！", "", MB_OK);
				return;
			}

			float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			RendererSystem::GetDeviceContext()->OMSetBlendState(pixelShader.lock()->m_pBlendState, blendFactor, 0xFFFFFFFF);

			// ラスタライザセット
			//RendererSystem::GetDeviceContext()->RSSetState(pixelShader.lock()->m_pRasterizerState);

			// 頂点シェーダーレイアウト設定
			RendererSystem::GetDeviceContext()->IASetInputLayout(vertexShader.lock()->m_pVtxLayout);

			// ジオメトリシェーダー
			if (geometryShader.expired())
				RendererSystem::GetDeviceContext()->GSSetShader(NULL, NULL, 0);
			else
				RendererSystem::GetDeviceContext()->GSSetShader(geometryShader.lock()->m_pGeomtryShader, NULL, 0);

			// シェーダーをセット
			RendererSystem::GetDeviceContext()->VSSetShader(vertexShader.lock()->m_pVtxShader, NULL, 0);
			RendererSystem::GetDeviceContext()->PSSetShader(pixelShader.lock()->m_pPixelShader, NULL, 0);
		}

	protected:
		std::weak_ptr<VertexShaderInfo> pVertexShaderInfo;
		std::weak_ptr<GeometryShaderInfo> pGeometryShaderInfo;
		std::weak_ptr<PixelShaderInfo> pPixelShaderInfo;

		void SetShader() const
		{
			if (pVertexShaderInfo.expired() || pPixelShaderInfo.expired())
			{
				MessageBox(NULL, "シェーダーフォーマットが設定されてない！", "", MB_OK);
				return;
			}

			float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			RendererSystem::GetDeviceContext()->OMSetBlendState(pPixelShaderInfo.lock()->m_pBlendState, blendFactor, 0xFFFFFFFF);

			// ラスタライザセット
			//RendererSystem::GetDeviceContext()->RSSetState(pPixelShaderInfo.lock()->m_pRasterizerState);

			// 頂点シェーダーレイアウト設定
			RendererSystem::GetDeviceContext()->IASetInputLayout(pVertexShaderInfo.lock()->m_pVtxLayout);

			// ジオメトリシェーダー
			if (pGeometryShaderInfo.expired())
				RendererSystem::GetDeviceContext()->GSSetShader(NULL, NULL, 0);
			else
				RendererSystem::GetDeviceContext()->GSSetShader(pGeometryShaderInfo.lock()->m_pGeomtryShader, NULL, 0);

			// シェーダーをセット
			RendererSystem::GetDeviceContext()->VSSetShader(pVertexShaderInfo.lock()->m_pVtxShader, NULL, 0);
			RendererSystem::GetDeviceContext()->PSSetShader(pPixelShaderInfo.lock()->m_pPixelShader, NULL, 0);
		}

		void SetVertexShaderFormat(VERTEX_SHADER_TYPE type)
		{
			this->pVertexShaderInfo = ShaderFormatManager::Instance()->GetVertexFormat(type);
		}
		void SetGeometryShaderFormat(GEOMETRY_SHADER_TYPE type)
		{
			this->pGeometryShaderInfo = ShaderFormatManager::Instance()->GetGeometryFormat(type);
		}
		void ResetGeometryShaderFormat() { this->pGeometryShaderInfo = std::weak_ptr<GeometryShaderInfo>(); }
		void SetPixelShaderFormat(PIXEL_SHADER_TYPE type)
		{
			this->pPixelShaderInfo = ShaderFormatManager::Instance()->GetPixelFormat(type);
		}
	};
}

CEREAL_CLASS_VERSION(MyDirectX::Shader, 0)

#endif // !___SHADER___H___


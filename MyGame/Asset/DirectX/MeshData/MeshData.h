#ifndef __MESH_DATA_H__
#define __MESH_DATA_H__

#include "../Shader/ShaderFormatManager.h"

#include "../Shader/Infomation/VertexShaderInfo.h"
#include "../Shader/Infomation/GeometryShaderInfo.h"
#include "../Shader/Infomation/PixelShaderInfo.h"

#include <vector>

namespace MyDirectX
{
	template<typename VTX>
	class MeshData
	{
	private:
		friend cereal::access;
		template <typename Archive>
		void save(Archive & archive) const
		{
			archive(CEREAL_NVP(vertex), CEREAL_NVP(index), CEREAL_NVP(meshScale));
		}
		template <typename Archive>
		void load(Archive & archive)
		{
			archive(CEREAL_NVP(vertex), CEREAL_NVP(index), CEREAL_NVP(meshScale));
			LoadBuffer();
		}

	public:
		MeshData();
		~MeshData();

		std::vector<VTX> vertex;
		std::vector<unsigned short> index;
		Vector3 meshScale;

		void CreateVertexBuffer(const std::vector<VTX> * vertex);
		void CreateIndexBuffer(const std::vector<unsigned short> * index);

		// バッファーセット
		void IASetBuffer();
		
		void Draw();
		void DrawIndexed(int indexCount, int startIndex);

		// 頂点情報を変更したときに呼ぶ
		void SetVertexBuffer();

		ID3D11Buffer* GetVertexBuffer() { return m_pVtxBuf; }
		ID3D11Buffer* GetIndexBuffer() { return m_pIdxBuf; }

	private:

		ID3D11Buffer *m_pVtxBuf = nullptr;
		ID3D11Buffer *m_pIdxBuf = nullptr;

		// 読み込み時データを元にバッファー作成
		void LoadBuffer();
	};

	template<typename VTX>
	MeshData<VTX>::MeshData() : m_pIdxBuf(nullptr), m_pVtxBuf(nullptr)
	{	
	}

	template<typename VTX>
	MeshData<VTX> ::~MeshData()
	{
		std::vector<VTX>().swap(vertex);
		std::vector<unsigned short>().swap(index);

		if (m_pVtxBuf)         m_pVtxBuf->Release();
		if (m_pIdxBuf)         m_pIdxBuf->Release();
	}

	// 頂点バッファの生成-------------------------------------------------------------------------------
	template<typename VTX>
	void MeshData<VTX> ::CreateVertexBuffer(const std::vector<VTX> * vertex)
	{
		if (m_pVtxBuf)
			MessageBox(NULL, "既に頂点情報が登録されています！！", "", MB_OK);

		this->vertex = std::vector<VTX>(*vertex);

		for (auto & ver : this->vertex)
		{
			// このメッシュの最大の大きさを調べる
			if (ver.pos.x > meshScale.x) meshScale.x = ver.pos.x;
			if (ver.pos.y > meshScale.y) meshScale.y = ver.pos.y;
			if (ver.pos.z > meshScale.z) meshScale.z = ver.pos.z;
		}

		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeof(VTX) * (UINT)this->vertex.size();
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = this->vertex.data();

		RendererSystem::GetDevice()->CreateBuffer(&desc, &data, &m_pVtxBuf);
	}

	// インデックスバッファの生成-----------------------------------------------------------------------
	template<typename VTX>
	void MeshData<VTX>::CreateIndexBuffer(const std::vector<unsigned short> * index)
	{
		if (m_pIdxBuf)
			MessageBox(NULL, "既に頂点番号が登録されています！！", "", MB_OK);

		this->index = std::vector<unsigned short>(*index);

		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = sizeof(unsigned short) * (UINT)this->index.size();
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = this->index.data();
		RendererSystem::GetDevice()->CreateBuffer(&desc, &data, &m_pIdxBuf);
	}

	template<typename VTX>
	inline void MeshData<VTX>::SetVertexBuffer()
	{
		// パラメータの受け渡し
		D3D11_MAPPED_SUBRESOURCE pdata;
		// パラメータの受け渡し(頂点)
		RendererSystem::GetDeviceContext()->Map(m_pVtxBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
		memcpy_s(pdata.pData, pdata.RowPitch, (void*)(this->vertex.data()), sizeof(VTX) * (UINT)this->vertex.size());
		RendererSystem::GetDeviceContext()->Unmap(m_pVtxBuf, 0);
	}
	

	template<typename VTX>
	inline void MeshData<VTX>::IASetBuffer()
	{
		UINT stride = sizeof(VTX);
		UINT offset = 0;
		RendererSystem::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pVtxBuf, &stride, &offset);
		if (m_pIdxBuf) 
			RendererSystem::GetDeviceContext()->IASetIndexBuffer(m_pIdxBuf, DXGI_FORMAT_R16_UINT, 0);
	}

	template<typename VTX>
	inline void MeshData<VTX>::LoadBuffer()
	{
		// 頂点バッファ作成
		{
			D3D11_BUFFER_DESC desc;
			desc.ByteWidth = sizeof(VTX) * (UINT)this->vertex.size();
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA data = { 0 };
			data.pSysMem = this->vertex.data();

			RendererSystem::GetDevice()->CreateBuffer(&desc, &data, &m_pVtxBuf);
		}
		// インデックスバッファ作成
		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.ByteWidth = sizeof(unsigned short) * (UINT)this->index.size();
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA data = { 0 };
			data.pSysMem = this->index.data();
			RendererSystem::GetDevice()->CreateBuffer(&desc, &data, &m_pIdxBuf);
		}
	}

	// 描画---------------------------------------------------------------------------------------------
	template<typename VTX>
	void MeshData<VTX>::Draw()
	{
		// 描画実行
		if (m_pIdxBuf == nullptr)
		{
			RendererSystem::GetDeviceContext()->Draw((UINT)vertex.size(), 0);
		}
		else
		{
			RendererSystem::GetDeviceContext()->DrawIndexed((UINT)index.size(), 0, 0);
		}
	}
	template<typename VTX>
	inline void MeshData<VTX>::DrawIndexed(int indexCount, int startIndex)
	{
		RendererSystem::GetDeviceContext()->DrawIndexed(indexCount, startIndex, 0);
	}
}
//--------------------------------------------------------------------------------------------------

#endif // !__MESH_DATA_H__
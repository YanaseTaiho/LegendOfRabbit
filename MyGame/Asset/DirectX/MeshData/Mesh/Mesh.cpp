#include "Mesh.h"
#include "../../Shader/ConstantBuffer.h"
#include "../../Shader/MeshShader.h"

using namespace MyDirectX;

void Mesh::Initialize()
{
	for (auto & node : meshNode)
	{
		for (auto& subset : node.subsetArray)
		{
			subset.material.SetShader(new MeshShader());
		}
	}
}

void Mesh::Draw(Transform * transform, std::vector<std::weak_ptr<Material>> & materialArray)
{
	// パラメータの受け渡し
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_WORLD, transform->GetWorldMatrix());
	ConstantBuffer::SetVSRegister(0, CB_TYPE::CB_WORLD);

	RendererSystem::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int materialCnt = 0;

	for (auto & node : meshNode)
	{
		MeshData<VTX_MESH> & meshData = (MeshData<VTX_MESH>&)node.meshData;

		for (const auto& subset : node.subsetArray)
		{
			const Material * material;
			if (!materialArray[materialCnt].expired())
				material = materialArray[materialCnt].lock().get();
			else
				material = &subset.material;

			meshData.IASetBuffer();
			material->SetOption();

			// 描画
			meshData.DrawIndexed(subset.indexNum, subset.startIndex);

			materialCnt++;
		}
	}
}

void Mesh::DrawShadow(Transform * transform)
{
	// パラメータの受け渡し
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_WORLD, transform->GetWorldMatrix());
	ConstantBuffer::SetVSRegister(0, CB_TYPE::CB_WORLD);

	RendererSystem::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto & node : meshNode)
	{
		MeshData<VTX_MESH> & meshData = (MeshData<VTX_MESH>&)node.meshData;
		const auto & vertex = ShaderFormatManager::Instance()->GetVertexFormat(VERTEX_SHADER_TYPE::MESH_SHADOW);
		const auto & pixel = ShaderFormatManager::Instance()->GetPixelFormat(PIXEL_SHADER_TYPE::SHADOW);
		// 深度テクスチャ用のシェーダーをセット
		Shader::SetShader(vertex, pixel);
		meshData.IASetBuffer();

		for (const auto& subset : node.subsetArray)
		{
			// 描画
			meshData.DrawIndexed(subset.indexNum, subset.startIndex);
		}
	}
}

int Mesh::GetMaterialNum()
{
	int num = 0;
	for (auto & mesh : meshNode)
	{
		for (auto & sub : mesh.subsetArray)
		{
			num++;
		}
	}
	return num;
}

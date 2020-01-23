#include "SkinMesh.h"
#include "FrameWork/Animation/AnimationClipManager.h"
#include "../../Shader/ConstantBuffer.h"
#include "../../Shader/SkinMeshShader.h"

using namespace MyDirectX;

#include "../../Texture/TextureManager.h"

void SkinMesh::Initialize()
{
	// アニメーションデータの生成
	Singleton<AnimationClipManager>::Instance()->RegisterAnimation(skeleton.get());

	// シェーダーセット
	for (auto & node : meshNode)
	{
		for (auto& subset : node.subsetArray)
		{
			subset.material.SetShader(new SkinMeshShader());
		}
	}
}

void SkinMesh::Draw(Transform * transform, std::vector<std::weak_ptr<Material>> & materialArray, const std::vector<std::weak_ptr<Transform>> & bones)
{
	// パラメータの受け渡し
	static CB_ARMATURE cbArm;

	RendererSystem::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ボーンの行列をセット
	for (UINT i = 0, size = (UINT)boneData.size();i < size;i++)
	{
		/// シェーダーには必ず転置してから渡す
		// Eigenはシェーダーと同じ並び順なのでそのまま渡す
		cbArm.boneArray[i].matrix = bones[i].lock()->GetWorldMatrix().matrix * boneData[i].offsetMatrix.matrix;
	}

	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_WORLD, transform->GetWorldMatrix());
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_ARMATURE, cbArm);

	ConstantBuffer::SetVSRegister(0, CB_TYPE::CB_WORLD);
	ConstantBuffer::SetVSRegister(3, CB_TYPE::CB_ARMATURE);

	int materialCnt = 0;

	for (auto & node : meshNode)
	{
		MeshData<VTX_SKIN_MESH> & meshData = (MeshData<VTX_SKIN_MESH>&)node.meshData;

		for (const auto& subset : node.subsetArray)
		{
			const Material * material;
			if (!materialArray[materialCnt].expired())
				material = materialArray[materialCnt].lock().get();
			else
				material = &subset.material;

			material->SetOption();
			meshData.IASetBuffer();

			// 描画
			meshData.DrawIndexed(subset.indexNum, subset.startIndex);

			materialCnt++;
		}
	}
}

void SkinMesh::DrawShadow(Transform * transform, const std::vector<std::weak_ptr<Transform>>& bones)
{
	// パラメータの受け渡し
	CB_ARMATURE cbArm;

	RendererSystem::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ボーンの行列をセット
	for (UINT i = 0, size = (UINT)boneData.size(); i < size; i++)
	{
		/// シェーダーには必ず転置してから渡す
		// Eigenはシェーダーと同じ並び順なのでそのまま渡す
		cbArm.boneArray[i] = bones[i].lock()->GetWorldMatrix() * boneData[i].offsetMatrix;	// 後でワールド行列で入れる
	}

	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_WORLD, transform->GetWorldMatrix());
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_ARMATURE, cbArm);

	ConstantBuffer::SetVSRegister(0, CB_TYPE::CB_WORLD);
	ConstantBuffer::SetVSRegister(3, CB_TYPE::CB_ARMATURE);

	for (auto & node : meshNode)
	{
		MeshData<VTX_SKIN_MESH> & meshData = (MeshData<VTX_SKIN_MESH>&)node.meshData;
		const auto & vertex = ShaderFormatManager::Instance()->GetVertexFormat(VERTEX_SHADER_TYPE::SKINMESH_SHADOW);
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

int SkinMesh::GetMaterialNum()
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

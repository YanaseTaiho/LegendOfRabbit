#include "MeshRenderer.h"
#include "../Light/DirectionalLight.h"
#include "../Texture/TextureManager.h"
#include "../../FrameWork/Component/Collision/Mesh/CollisionMeshInfoManager.h"

using namespace MyDirectX;

MeshRenderer::MeshRenderer(std::string fileName, bool useLight, bool useShadow, bool useCastShadow) 
	: useLight(useLight), useShadow(useShadow), useCastShadow(useCastShadow)
{
	model = Singleton<MeshManager>::Instance()->Load(fileName);
}

MeshRenderer::~MeshRenderer()
{
	
}

void MeshRenderer::DrawImGui(int id)
{
	std::string meshId = "##" +  std::to_string(id);

	if (Singleton<MeshManager>::Instance()->DropTargetImGuiMesh(this->model, meshId))
	{
		LoadModel();

		if (!model.expired())
		{
			auto colMesh = gameObject.lock()->GetComponent<CollisionMesh>();
			if (!colMesh.expired())
			{
				colMesh.lock()->meshInfo = CollisionMeshInfoManager::Load(this->model.lock().get());
			}
		}
	}

	ImGui::Checkbox(("UseLight" + meshId).c_str(), &useLight);
	ImGui::Checkbox(("UseShadow" + meshId).c_str(), &useShadow);
	ImGui::Checkbox(("UseCastShadow" + meshId).c_str(), &useCastShadow);
	ImGui::ColorEdit4(("BaseColor" + meshId).c_str(), baseColor);

	if (ImGui::TreeNode(("Materials" + meshId).c_str()))
	{
		int cnt = 0;
		for (auto & mat : materialArray)
		{
			++id;
			//ImGui::Text((std::to_string(cnt)).c_str()); ImGui::SameLine();
			ImGui::Bullet(); ImGui::SameLine();
			meshId = "##" + std::to_string(id);
			Singleton<MaterialManager>::Instance()->DropTargetImGuiMaterial(mat, meshId);
		}
		ImGui::TreePop();
	}
	
}

void MeshRenderer::Awake()
{
	if (model.expired()) return;

	LoadModel();
}

void MeshRenderer::Draw()
{
	if (model.expired()) return;

	RendererSystem::SetUseLightOption(useLight, useCastShadow);

	// ベースカラーをピクセルシェーダーにセット
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_COLOR, baseColor);
	ConstantBuffer::SetPSRegister(0, CB_TYPE::CB_COLOR);

	model.lock()->Draw(transform.lock().get(), materialArray);
}

void MeshRenderer::DrawShadow()
{
	if (model.expired()) return;
	if (!useShadow) return;
	model.lock()->DrawShadow(transform.lock().get());
}

void MeshRenderer::LoadModel()
{
	if (model.expired()) return;

	modelScale = Vector3::zero();

	// モデルの大きさを取得
	for (auto & node : model.lock()->meshNode)
	{
		if (node.meshData.meshScale.x > modelScale.x) modelScale.x = node.meshData.meshScale.x;
		if (node.meshData.meshScale.y > modelScale.y) modelScale.y = node.meshData.meshScale.y;
		if (node.meshData.meshScale.z > modelScale.z) modelScale.z = node.meshData.meshScale.z;
	}
	// マテリアルデータサイズ更新
	materialArray.clear();
	materialArray.resize(model.lock()->GetMaterialNum());
}

std::weak_ptr<Material> MeshRenderer::GetMaterial(int num)
{
	if (materialArray.size() > num)
	{
		return materialArray[num];
	}

	return std::weak_ptr<Material>();
}

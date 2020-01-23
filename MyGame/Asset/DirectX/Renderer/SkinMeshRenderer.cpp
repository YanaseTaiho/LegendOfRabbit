#include "SkinMeshRenderer.h"
#include "../../FrameWork/Animation/AnimationClipManager.h"
#include "../Texture/TextureManager.h"
#include "../Material/MaterialManager.h"

using namespace MyDirectX;

void SkinMeshRenderer::LoadSerialized(const std::shared_ptr<Component>& component)
{
	Renderer::LoadSerialized(component);
	transform.lock()->Update();
}

SkinMeshRenderer::SkinMeshRenderer(std::string fileName, bool isObject, bool useLight, bool useShadow, bool useCastShadow)
	: isObject(isObject), useLight(useLight), useShadow(useShadow), useCastShadow(useCastShadow)
{
	model = Singleton<SkinMeshManager>::Instance()->Load(fileName);
}

SkinMeshRenderer::~SkinMeshRenderer()
{
	std::vector<std::shared_ptr<Transform>>().swap(skeleton);
}

void SkinMeshRenderer::Awake()
{
	if (model.expired()) return;

	LoadModel();
}

void SkinMeshRenderer::OnDestroy()
{
	boneTransforms.clear();
	skeleton.clear();
}

void SkinMeshRenderer::LoadSkeleton(bool isDethObject)
{
	if (isDethObject)
	{
		for (auto & child : skeleton)
		{
			GameObject::Destroy(child->gameObject);
		}
	}
	else
	{
		for (auto & child : skeleton)
		{
			child->ReleaseParent();
			child->ReleaseChildAll();
		}
	}

	// �X�P���g�����폜
	OnDestroy();

	// �X�P���g���𐶐�
	const auto & bone = model.lock()->skeleton->mJoints;
	size_t bone_size = model.lock()->skeleton->mJoints.size();

	for (size_t b = 0; b < bone_size; b++)
	{
		if (bone[b].parentIndex == -1)
		{
			if (isObject)
			{
				// �{�[�����q�ɒǉ�����
				CreateSkeleton(gameObject.lock().get(), (int)b);
			}
			else
			{
				CreateSkeletonTransform(gameObject.lock()->transform.lock().get(), (int)b);
			}
		}
	}
}

// �{�[�����I�u�W�F�N�g�Ƃ��Đ�������
void SkinMeshRenderer::CreateSkeleton(GameObject * parent, int childsId)
{
	const auto & bone = model.lock()->skeleton->mJoints[childsId];

	std::weak_ptr<GameObject> obj = Singleton<GameObjectManager>::Instance()->CreateGameObject(new GameObject(bone.name, Tag::None, Layer::Default));
	obj.lock()->AddComponent(new Transform());
	obj.lock()->transform.lock()->SetLocalMatrix(bone.initLocalMatrix);	// �{�[���̏����p���Z�b�g
	parent->SetChild(obj, SetType::Local);
	// �K�w�f�[�^��ێ����邽�߂Ɋi�[
	skeleton.emplace_back(obj.lock()->transform.lock());

	// �{�[���̃f�[�^�̂ݔz��ɒǉ�
	for (BoneData & data : model.lock()->boneData)
	{
		if (data.name == bone.name)
		{
			boneTransforms.emplace_back(obj.lock()->transform);
			break;
		}
	}

	for (const int & id : bone.childsId)
	{
		CreateSkeleton(obj.lock().get(), id);
	}
}

// �{�[���f�[�^��������ăI�u�W�F�N�g�͍��Ȃ�
void SkinMeshRenderer::CreateSkeletonTransform(Transform * parent, int childsId)
{
	auto & bone = model.lock()->skeleton->mJoints[childsId];

	std::shared_ptr<Transform> trans = std::shared_ptr<Transform>(new Transform());
	trans->transform = trans;	// ���g�̎Q�Ƃ�ێ�
	trans->SetLocalMatrix(bone.initLocalMatrix);	// �{�[���̏����p���Z�b�g
	parent->SetChild((std::weak_ptr<Transform>&)trans, SetType::Local);
	// �K�w�f�[�^��ێ����邽�߂Ɋi�[
	skeleton.emplace_back(trans);

	// �{�[���̃f�[�^�̂ݔz��ɒǉ�
	for (BoneData & data : model.lock()->boneData)
	{
		if (data.name == bone.name)
		{
			boneTransforms.emplace_back(trans);
			break;
		}
	}

	for (int & id : bone.childsId)
	{
		CreateSkeletonTransform(trans.get(), id);
	}
}

void SkinMeshRenderer::LoadModel()
{
	if (model.expired()) return;

	modelScale = Vector3::zero();

	// ���f���̑傫�����擾
	for (auto & node : model.lock()->meshNode)
	{
		if (node.meshData.meshScale.x > modelScale.x) modelScale.x = node.meshData.meshScale.x;
		if (node.meshData.meshScale.y > modelScale.y) modelScale.y = node.meshData.meshScale.y;
		if (node.meshData.meshScale.z > modelScale.z) modelScale.z = node.meshData.meshScale.z;
	}
	// �}�e���A���f�[�^�T�C�Y�X�V
	materialArray.clear();
	materialArray.resize(model.lock()->GetMaterialNum());

	LoadSkeleton(isObject);
}

void SkinMeshRenderer::DrawImGui(int id)
{
	std::string meshId = "##" + std::to_string(id);

	if (Singleton<SkinMeshManager>::Instance()->DropTargetImGuiMesh(this->model, meshId))
	{
		LoadModel();
	}

	ImGui::Checkbox(("UseLight" + meshId).c_str(), &useLight);
	ImGui::Checkbox(("UseShadow" + meshId).c_str(), &useShadow);
	ImGui::Checkbox(("UseCastShadow" + meshId).c_str(), &useCastShadow);
	if (ImGui::Checkbox(("isSkeletonObject" + meshId).c_str(), &isObject))
	{
		LoadSkeleton(!isObject);
	}
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

void SkinMeshRenderer::Draw()
{
	if (model.expired()) return;

	// ���s�����Z�b�g ( �R���X�^���g�o�b�t�@�R�ɐݒ� )
	RendererSystem::SetUseLightOption(useLight, useCastShadow);

	// �x�[�X�J���[���s�N�Z���V�F�[�_�[�ɃZ�b�g
	ConstantBuffer::UpdateConstBuffer(CB_TYPE::CB_COLOR, baseColor);
	ConstantBuffer::SetPSRegister(0, CB_TYPE::CB_COLOR);

	model.lock()->Draw(transform.lock().get(), materialArray, boneTransforms);
}

void SkinMeshRenderer::DrawShadow()
{
	if (model.expired()) return;
	if (!useShadow) return;
	model.lock()->DrawShadow(transform.lock().get(), boneTransforms);
}

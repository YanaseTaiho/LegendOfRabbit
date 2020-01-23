#ifndef _SKINMESHMODEL_H_
#define _SKINMESHMODEL_H_

#include "../../FrameWork/Component/Renderer/Renderer.h"
#include "../MeshData/SkinMesh/SkinMeshManager.h"
#include "../Material/MaterialManager.h"

namespace MyDirectX
{
	class SkinMeshRenderer : public Renderer
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			std::string modelName;
			std::vector<std::string> matNameArray;
			if (!this->model.expired()) modelName = this->model.lock()->name;

			for (auto & mat : materialArray)
			{
				std::string matName = (mat.expired()) ? "" : mat.lock()->name;
				matNameArray.emplace_back(matName);
			}

			if (version == 0)
			{
				archive(cereal::make_nvp("Renderer", cereal::base_class<Renderer>(this)), CEREAL_NVP(useLight), CEREAL_NVP(isObject)
					, CEREAL_NVP(modelName), CEREAL_NVP(matNameArray), CEREAL_NVP(skeleton), CEREAL_NVP(boneTransforms));
			}
			else if (version == 1)
			{
				archive(cereal::make_nvp("Renderer", cereal::base_class<Renderer>(this)));
				archive(useLight, useShadow, useCastShadow, isObject, modelName, matNameArray, skeleton, boneTransforms);
			}
		}
		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			std::string modelName;
			std::vector<std::string> matNameArray;

			if (version == 0)
			{
				archive(cereal::make_nvp("Renderer", cereal::base_class<Renderer>(this)), CEREAL_NVP(useLight), CEREAL_NVP(isObject)
					, CEREAL_NVP(modelName), CEREAL_NVP(matNameArray), CEREAL_NVP(skeleton), CEREAL_NVP(boneTransforms));
			}
			else if (version == 1)
			{
				archive(cereal::make_nvp("Renderer", cereal::base_class<Renderer>(this)));
				archive(useLight, useShadow, useCastShadow, isObject, modelName, matNameArray, skeleton, boneTransforms);
			}

			model = Singleton<SkinMeshManager>::Instance()->GetMesh(modelName);
			
			for (auto & str : matNameArray)
			{
				auto mat = Singleton<MaterialManager>::Instance()->GetMaterial(str);
				materialArray.emplace_back(mat);
			}
			// ���f���̃}�e���A����������
			if (!model.expired() && materialArray.size() < model.lock()->GetMaterialNum())
			{
				materialArray.resize(model.lock()->GetMaterialNum());
			}
		}

		void LoadSerialized(const std::shared_ptr<Component> & component) override;

	public:
		// isObject : false  �{�[���I�u�W�F�N�g���쐬���Ȃ�( �{�[���ɓ����蔻��Ȃǂ���Ȃ��ꍇ )
		//			  true   �{�[���I�u�W�F�N�g���쐬����( �{�[���ɓ����蔻��Ȃǂ̏�����ǉ��������ꍇ )
		SkinMeshRenderer(std::string fileName, bool isObject, bool useLight = true, bool useShadow = true, bool useCastShadow = true);
		SkinMeshRenderer() {}
		~SkinMeshRenderer();

		void Awake() override;
		void OnDestroy() override;

		void DrawImGui(int id) override;
		void Draw() override;
		void DrawShadow() override;
		void LoadModel();	//�@���f�����ύX���ꂽ���ɌĂ�

		std::weak_ptr<SkinMesh> model;
		std::vector<std::weak_ptr<Material>> materialArray;
	private:
		
		std::vector<std::shared_ptr<Transform>> skeleton;	// �I�u�W�F�N�g�𐶐����Ȃ����̑���̃g�����X�t�H�[��	
		std::vector<std::weak_ptr<Transform>> boneTransforms;	// �{�[���̎Q�Ƃ�ێ�

		void LoadSkeleton(bool isDethObject);
		void CreateSkeleton(GameObject * parent, int childsId);
		void CreateSkeletonTransform(Transform * parent, int childsId);

		bool isObject = false;
		bool useLight = true;
		bool useShadow = true;
		bool useCastShadow = true;
	};
}

CEREAL_CLASS_VERSION(MyDirectX::SkinMeshRenderer, 1)
CEREAL_REGISTER_TYPE(MyDirectX::SkinMeshRenderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(FrameWork::Component, MyDirectX::SkinMeshRenderer)

#endif // !_SKINMESHMODEL_H_


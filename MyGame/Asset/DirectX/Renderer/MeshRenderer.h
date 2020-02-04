#ifndef _MESHRENDERER_H_
#define _MESHRENDERER_H_

#include "../../FrameWork/Component/Renderer/Renderer.h"
#include "../MeshData/Mesh/MeshManager.h"
#include "../Material/MaterialManager.h"

namespace MyDirectX
{
	class MeshRenderer : public Renderer
	{
	private:
		friend cereal::access;

		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			std::string modelName;
			if (!this->model.expired()) modelName = this->model.lock()->name;

			std::vector<std::string> matNameArray;
			for (auto & mat : materialArray)
			{
				if (mat.expired()) continue;
				matNameArray.emplace_back(mat.lock()->name);
			}

			archive(cereal::make_nvp("DXRenderer", cereal::base_class<Renderer>(this)),
				CEREAL_NVP(useLight), CEREAL_NVP(useShadow), CEREAL_NVP(useCastShadow),
				CEREAL_NVP(modelName), CEREAL_NVP(matNameArray));
		}
		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			std::string modelName;
			std::vector<std::string> matNameArray;

			archive(cereal::make_nvp("DXRenderer", cereal::base_class<Renderer>(this)),
				CEREAL_NVP(useLight), CEREAL_NVP(useShadow), CEREAL_NVP(useCastShadow),
				CEREAL_NVP(modelName), CEREAL_NVP(matNameArray));

			model = Singleton<MeshManager>::Instance()->GetMesh(modelName);

			for (auto & str : matNameArray)
			{
				auto mat = Singleton<MaterialManager>::Instance()->GetMaterial(str);
				materialArray.emplace_back(mat);
			}
			// モデルのマテリアル個数分生成
			if (!model.expired() && materialArray.size() < model.lock()->GetMaterialNum())
			{
				materialArray.resize(model.lock()->GetMaterialNum());
			}
		}

	public:
		MeshRenderer(std::string fileName, bool useLight = true, bool useShadow = true, bool useCastShadow = true);
		MeshRenderer() {}
		~MeshRenderer();
		
		void DrawImGui(int id) override;

		void Awake() override;
		void Draw() override;
		void DrawShadow() override;
		void LoadModel();	// モデルが変更された時に呼ぶ

		std::weak_ptr<Material> GetMaterial(int num);

		std::weak_ptr<Mesh> model;
		std::vector<std::weak_ptr<Material>> materialArray;
		bool useLight = true;
		bool useShadow = true;
		bool useCastShadow = true;
	};
}

CEREAL_CLASS_VERSION(MyDirectX::MeshRenderer, 0)
CEREAL_REGISTER_TYPE(MyDirectX::MeshRenderer)
//CEREAL_REGISTER_POLYMORPHIC_RELATION(FrameWork::Component, MyDirectX::MeshRenderer)

#endif // !_MESHRENDERER_H_


#ifndef _MATERIAL_MANAGER_H_
#define _MATERIAL_MANAGER_H_

#include "Material.h"
#include "../../FrameWork/Singleton.h"
#include "../Shader/Shader.h"
#include "../../FrameWork/BindClass.h"
#include <list>
#include <memory>

namespace MyDirectX
{
	class MaterialManager
	{
		friend FrameWork::Singleton<MaterialManager>;
		MaterialManager() {}
		~MaterialManager() { materialList.clear(); }
	public:
		void CreateShaderList(BindClass<Shader> sList);

		void SaveMaterial(std::string path);
		void LoadMaterial(std::string path);

		void DrawImGui();
		std::weak_ptr<Material> GetMaterial(std::string name);
		void BeginDragImGuiMaterial(std::shared_ptr<Material> & material, std::string id);
		bool DropTargetImGuiMaterial(std::weak_ptr<Material> & material, std::string id);

		bool IsMaterialClicked() { return isMaterialClick; }
		std::weak_ptr<Material> selectMaterial;
	private:
		void SetOriginalMaterialName(std::shared_ptr<Material> & material);
		bool CheckMaterialName(std::shared_ptr<Material> mine, std::string checkName);

		bool isMaterialClick;
		std::list<std::shared_ptr<Material>> materialList;
		std::list<std::string> deletePathList;
	};
}

#endif // !_MATERIAL_MANAGER_H_


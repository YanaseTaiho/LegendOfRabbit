#ifndef _SKINMESHMANAGER_H_
#define _SKINMESHMANAGER_H_

#include "SkinMesh.h"
#include <map>

namespace MyDirectX
{
	class SkinMeshManager
	{
		friend Singleton<SkinMeshManager>;
		SkinMeshManager() {}
		~SkinMeshManager() { skinMeshMap.clear(); }
	public:

		void SaveMesh(std::string path);
		void LoadMesh(std::string path);
		std::weak_ptr<SkinMesh> Load(std::string fileName);
		std::weak_ptr<SkinMesh> GetMesh(std::string fileName);

		void DrawImGui();
		void BeginDragImGuiMesh(std::weak_ptr<SkinMesh> mesh, std::string id);
		bool DropTargetImGuiMesh(std::weak_ptr<SkinMesh> & mesh, std::string id);
		
	private:

		std::map<std::string, std::shared_ptr<SkinMesh>> skinMeshMap;
		std::vector<std::string> loadSkinMeshNames;
	};
}

#endif // !_SKINMESHMANAGER_H_


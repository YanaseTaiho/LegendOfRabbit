#ifndef _MESHMANAGER_H_
#define _MESHMANAGER_H_

#include "Mesh.h"
#include <map>

namespace MyDirectX
{
	class MeshManager
	{
		friend Singleton<MeshManager>;
		MeshManager() {}
		~MeshManager() { meshMap.clear(); }
	public:

		void SaveMesh(std::string path);
		void LoadMesh(std::string path);
		std::weak_ptr<Mesh> Load(std::string fileName);
		std::weak_ptr<Mesh> GetMesh(std::string fileName);

		void DrawImGui();
		void BeginDragImGuiMesh(std::weak_ptr<Mesh> mesh, std::string id);
		bool DropTargetImGuiMesh(std::weak_ptr<Mesh> & mesh, std::string id);

	private:

		std::map<std::string, std::shared_ptr<Mesh>> meshMap;
	};
}

#endif // !_MESHMANAGER_H_
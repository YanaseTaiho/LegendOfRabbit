#ifndef _COLLISIONMESHINFOMANAGER_H_
#define _COLLISIONMESHINFOMANAGER_H_

#include "../../../../DirectX/MeshData/Mesh/Mesh.h"
#include "CollisionMeshInfo.h"
#include <unordered_map>

namespace FrameWork
{
	class CollisionMeshInfoManager
	{
	public:
		static std::weak_ptr<CollisionMeshInfo> Load(MyDirectX::Mesh * meshData);
		static std::weak_ptr<CollisionMeshInfo> Load(std::string name, CollisionMeshInfo * meshData);
		static void Release();
	private:
		CollisionMeshInfoManager() {}
		~CollisionMeshInfoManager() {}

		static std::unordered_map<std::string, std::shared_ptr<CollisionMeshInfo>> collisionMeshMap;
	};
}

#endif // !_COLLISIONMESHINFOMANAGER_H_


#include "CollisionMeshInfoManager.h"

using namespace FrameWork;
using namespace MyDirectX;

std::unordered_map<std::string, std::shared_ptr<CollisionMeshInfo>> CollisionMeshInfoManager::collisionMeshMap;

std::weak_ptr<CollisionMeshInfo> CollisionMeshInfoManager::Load(MyDirectX::Mesh * meshData)
{
	if(!meshData)
		return std::weak_ptr<CollisionMeshInfo>();

	if (collisionMeshMap.count(meshData->name) > 0) 
		return collisionMeshMap[meshData->name];

	CollisionMeshInfo * addInfo = new CollisionMeshInfo();
	Vector3 maxPos;

	for (const auto & node : meshData->meshNode)
	{
		const std::vector<VTX_MESH> & vertex = node.meshData.vertex;
		UINT vtxCnt = (UINT)vertex.size();
		UINT faceCnt = vtxCnt / 3;

		for (UINT i = 0; i < faceCnt; i++)
		{
			CollisionFaceInfo faceInfo;
			UINT v = i * 3;

			faceInfo.point[0] = vertex[v].pos;
			faceInfo.point[1] = vertex[v + 1].pos;
			faceInfo.point[2] = vertex[v + 2].pos;

			// ‚±‚ÌƒƒbƒVƒ…‚ÌÅ‘å‚Ì‘å‚«‚³‚ð’²‚×‚é
			
			for (auto point : faceInfo.point)
			{
				if (abs(point.x) > maxPos.x) maxPos.x = abs(point.x);
				if (abs(point.y) > maxPos.y) maxPos.y = abs(point.y);
				if (abs(point.z) > maxPos.z) maxPos.z = abs(point.z);
			}

			Vector3 vecAB = faceInfo.point[1] - faceInfo.point[0];
			Vector3 vecBC = faceInfo.point[2] - faceInfo.point[1];
			Vector3 normal = Vector3::Cross(vecAB, vecBC);
			faceInfo.normal = normal.Normalize();

			addInfo->faceInfoArray.emplace_back(faceInfo);
		}
	}
	addInfo->name = meshData->name;
	addInfo->scaleRadius = maxPos.Length() * 1.1f;	// ‚È‚º‚©‚¿‚å‚Á‚Æ‘å‚«‚³‚ª‘«‚è‚È‚©‚Á‚½‚Ì‚Å­‚µ‘å‚«‚ß‚ÉÝ’è

	return collisionMeshMap[meshData->name] = std::shared_ptr<CollisionMeshInfo>(addInfo);
}

std::weak_ptr<CollisionMeshInfo> CollisionMeshInfoManager::Load(std::string name, CollisionMeshInfo * meshData)
{
	if (collisionMeshMap.count(name) > 0) 
		return collisionMeshMap[name];

	return collisionMeshMap[name] = std::shared_ptr<CollisionMeshInfo>(meshData);
}

void CollisionMeshInfoManager::Release()
{
	collisionMeshMap.clear();
}

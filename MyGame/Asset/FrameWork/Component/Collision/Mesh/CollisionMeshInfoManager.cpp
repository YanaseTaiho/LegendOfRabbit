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
	float maxRad = 0.0f;

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
			if (faceInfo.point[0].x > maxRad) maxRad = faceInfo.point[0].x;
			if (faceInfo.point[0].y > maxRad) maxRad = faceInfo.point[0].y;
			if (faceInfo.point[0].z > maxRad) maxRad = faceInfo.point[0].z;
			if (faceInfo.point[1].x > maxRad) maxRad = faceInfo.point[1].x;
			if (faceInfo.point[1].y > maxRad) maxRad = faceInfo.point[1].y;
			if (faceInfo.point[1].z > maxRad) maxRad = faceInfo.point[1].z;
			if (faceInfo.point[2].x > maxRad) maxRad = faceInfo.point[2].x;
			if (faceInfo.point[2].y > maxRad) maxRad = faceInfo.point[2].y;
			if (faceInfo.point[2].z > maxRad) maxRad = faceInfo.point[2].z;

			Vector3 vecAB = faceInfo.point[1] - faceInfo.point[0];
			Vector3 vecBC = faceInfo.point[2] - faceInfo.point[1];
			Vector3 normal = Vector3::Cross(vecAB, vecBC);
			faceInfo.normal = normal.Normalize();

			addInfo->faceInfoArray.emplace_back(faceInfo);
		}
	}
	addInfo->name = meshData->name;
	addInfo->scaleRadius = maxRad;

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

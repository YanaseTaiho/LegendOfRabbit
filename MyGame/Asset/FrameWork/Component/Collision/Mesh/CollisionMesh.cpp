#include "CollisionMesh.h"
#include "CollisionMeshInfoManager.h"
#include "../CollisionJudge.h"

#include "../../../../DirectX/Renderer/MeshRenderer.h"
#include "../../../../DirectX/MeshData/LineMesh/DebugLine.h"

using namespace FrameWork;
using namespace MyDirectX;

void CollisionMesh::LoadSerialized(const std::shared_ptr<Component>& component)
{
	Collision::LoadSerialized(component);
	auto meshData = component->gameObject.lock()->GetComponent<MeshRenderer>();
	if (!meshData.expired())
	{
		if (!meshData.expired() && !meshData.lock()->model.expired())
			meshInfo = CollisionMeshInfoManager::Load(meshData.lock()->model.lock().get());
	}

	if (!meshInfo.expired())
	{
		// 当たり判定の表示用
		std::string lineMeshName = meshData.lock()->model.lock()->name;
		//if (!DebugLine::IsRegisterdLineData(lineMeshName))
		{
			LineMesh * line = new LineMesh();
			unsigned int vertexCnt = (unsigned int)meshInfo.lock()->faceInfoArray.size() * 6;
			std::vector<VTX_LINE> vertex(vertexCnt);

			unsigned int faceCnt = (unsigned int)meshInfo.lock()->faceInfoArray.size();
			for (unsigned int i = 0; i < faceCnt; i++)
			{
				unsigned int v = i * 6;
				vertex[v + 0].pos = meshInfo.lock()->faceInfoArray[i].point[0];
				vertex[v + 1].pos = meshInfo.lock()->faceInfoArray[i].point[1];

				vertex[v + 2].pos = meshInfo.lock()->faceInfoArray[i].point[1];
				vertex[v + 3].pos = meshInfo.lock()->faceInfoArray[i].point[2];

				vertex[v + 4].pos = meshInfo.lock()->faceInfoArray[i].point[2];
				vertex[v + 5].pos = meshInfo.lock()->faceInfoArray[i].point[0];
			}
			line->meshData.CreateVertexBuffer(&vertex);
			DebugLine::RegisterLineData(lineMeshName, line);
		}
	}
}

void CollisionMesh::DrawImGui(int id)
{
	std::string strId = "##ColliisonMesh" + std::to_string(id);
	float width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX();

	if (meshInfo.expired())
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Button(("None" + strId).c_str(), ImVec2(width, ImGui::GetFrameHeight()));
		ImGui::PopStyleColor(4);
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 1.0f, 0.6f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Button((meshInfo.lock()->name + strId).c_str(), ImVec2(width, ImGui::GetFrameHeight()));
		ImGui::PopStyleColor(4);
	}
}

void CollisionMesh::Awake()
{
	auto meshData = gameObject.lock()->GetComponent<MeshRenderer>();
	if (!meshData.expired())
	{
		if(!meshData.expired() && !meshData.lock()->model.expired())
			meshInfo = CollisionMeshInfoManager::Load(meshData.lock()->model.lock().get());
	}

	if (!meshInfo.expired())
	{
		// 当たり判定の表示用
		std::string lineMeshName = meshData.lock()->model.lock()->name;
		//if (!DebugLine::IsRegisterdLineData(lineMeshName))
		{
			LineMesh * line = new LineMesh();
			unsigned int vertexCnt = (unsigned int)meshInfo.lock()->faceInfoArray.size() * 6;
			std::vector<VTX_LINE> vertex(vertexCnt);

			unsigned int faceCnt = (unsigned int)meshInfo.lock()->faceInfoArray.size();
			for (unsigned int i = 0; i < faceCnt; i++)
			{
				unsigned int v = i * 6;
				vertex[v + 0].pos = meshInfo.lock()->faceInfoArray[i].point[0];
				vertex[v + 1].pos = meshInfo.lock()->faceInfoArray[i].point[1];

				vertex[v + 2].pos = meshInfo.lock()->faceInfoArray[i].point[1];
				vertex[v + 3].pos = meshInfo.lock()->faceInfoArray[i].point[2];

				vertex[v + 4].pos = meshInfo.lock()->faceInfoArray[i].point[2];
				vertex[v + 5].pos = meshInfo.lock()->faceInfoArray[i].point[0];
			}
			line->meshData.CreateVertexBuffer(&vertex);
			DebugLine::RegisterLineData(lineMeshName, line);
		}
	}

	this->Update();
}

void CollisionMesh::Update()
{
	worldMatrix = transform.lock()->GetWorldMatrix() * localMatrix;

	Vector3 scale = worldMatrix.scale();
	float radius = 0.0f;
	if (scale.x > radius) radius = scale.x;
	if (scale.y > radius) radius = scale.y;
	if (scale.z > radius) radius = scale.z;

	if (!meshInfo.expired())
	{
		radius *= meshInfo.lock()->scaleRadius;
	}

	this->SetScaleRadius(radius);
}

void CollisionMesh::Draw()
{
	auto meshData = gameObject.lock()->GetComponent<MeshRenderer>();
	if (!meshData.expired() && !meshData.lock()->model.expired())
		DebugLine::DrawLine(meshData.lock()->model.lock()->name, worldMatrix, Color::cyan());
}

bool CollisionMesh::CollisionJudge(Collision * other)
{
	return other->vsCollisionMesh(this);
}

bool CollisionMesh::vsCollisionSphere(CollisionSphere * other)
{
	return CollisionJudge::Sphere_VS_Mesh(other, this);
}

bool CollisionMesh::vsCollisionCapsule(CollisionCapsule * other)
{
	return false;
}

bool CollisionMesh::vsCollisionBox(CollisionBox * other)
{
	return false;
}

bool CollisionMesh::vsCollisionMesh(CollisionMesh * other)
{
	return false;
}

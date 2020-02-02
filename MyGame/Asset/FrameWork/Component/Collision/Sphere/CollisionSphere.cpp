#include "CollisionSphere.h"
#include "../CollisionJudge.h"
#include "DirectX/MeshData/LineMesh/DebugLine.h"

using namespace FrameWork;

CollisionSphere::CollisionSphere(Vector3 offset, float radius, bool isTrigger)
	: Collision("", isTrigger), radius(radius)
{
	localMatrix.SetMatrix(offset, Vector3(radius, radius, radius));
}

CollisionSphere::CollisionSphere(std::string name, Vector3 offset, float radius, bool isTrigger)
	: Collision(name, isTrigger), radius(radius)
{
	localMatrix.SetMatrix(offset, Vector3(radius, radius, radius));
}

CollisionSphere::~CollisionSphere()
{
}

void CollisionSphere::Awake()
{
	ChangeWorld();
}

void CollisionSphere::DrawImGui(int id)
{
	std::string strId = "##CollisionSphere" + std::to_string(id);
	ImGui::Checkbox(("IsTrigger" + strId).c_str(), &isTrigger);

	Vector3 offset = localMatrix.position();
	if (ImGui::DragFloat3(("Offset" + strId).c_str(), offset, 0.01f))
	{
		Vector3 scale = localMatrix.scale();
		localMatrix.SetMatrix(offset, scale);
		Update();
	}
	float rad = localMatrix.scale().x;
	if (ImGui::DragFloat(("Radius" + strId).c_str(), &rad, 0.01f))
	{
		SetRadius(rad);
	}
}

void CollisionSphere::Update()
{
	worldMatrix = transform.lock()->GetWorldMatrix() * localMatrix;

	Vector3 length = Vector3(worldMatrix.matrix(0, 0), worldMatrix.matrix(0, 1), worldMatrix.matrix(0, 2));
	this->radius = length.Length();

	// ‘å‚Ü‚©‚È‘å‚«‚³‚ðÝ’è
	SetScaleRadius(this->radius);
}

void CollisionSphere::SetRadius(float radius)
{
	localMatrix.SetMatrix(localMatrix.position(), Vector3(radius, radius, radius));
	this->Update();
}

float CollisionSphere::GetRadius()
{
	return radius;
}

void CollisionSphere::Draw()
{
	Matrix4 sMat(worldMatrix.position(), Vector3::one() * this->radius, worldMatrix.rotation());
	MyDirectX::DebugLine::DrawLine("Sphere", sMat, Color(0.0f, 1.0f, 1.0f, 1.0f));
}

bool CollisionSphere::CollisionJudge(Collision * other)
{
	return other->vsCollisionSphere(this);
}

bool CollisionSphere::vsCollisionSphere(CollisionSphere * other)
{
	return CollisionJudge::Sphere_VS_Sphere(this, other);
}

bool CollisionSphere::vsCollisionCapsule(CollisionCapsule * other)
{
	return CollisionJudge::Sphere_VS_Capsule(this, other);
}

bool CollisionSphere::vsCollisionBox(CollisionBox * other)
{
	return CollisionJudge::Sphere_VS_Box(this, other);
}

bool CollisionSphere::vsCollisionMesh(CollisionMesh * other)
{
	return CollisionJudge::Sphere_VS_Mesh(this, other);
}

#include "CollisionCapsule.h"
#include "../CollisionJudge.h"
#include "DirectX/MeshData/LineMesh/DebugLine.h"

using namespace FrameWork;

CollisionCapsule::CollisionCapsule(Vector3 offset, float radius, Vector3 start, Vector3 end, bool isTrigger) :
	Collision("", isTrigger), radius(radius), localStart(start), localEnd(end)
{
	localMatrix.SetMatrix(offset, Vector3(radius, radius, radius));
}

CollisionCapsule::CollisionCapsule(std::string name, Vector3 offset, float radius, Vector3 start, Vector3 end, bool isTrigger) :
	Collision(name, isTrigger), radius(radius), localStart(start), localEnd(end)
{
	localMatrix.SetMatrix(offset, Vector3(radius, radius, radius));
}

CollisionCapsule::~CollisionCapsule()
{
}

void CollisionCapsule::SetCapsule(float radius, Vector3 start, Vector3 end)
{
	localMatrix.SetMatrix(localMatrix.position(), Vector3(radius, radius, radius));
	localStart = start;
	localEnd = end;
	this->Update();
}

void CollisionCapsule::Awake()
{
	ChangeWorld();
}

void CollisionCapsule::Update()
{
	worldMatrix = transform.lock()->GetWorldMatrix() * localMatrix;

	Vector3 length = Vector3(worldMatrix.matrix(0, 0), worldMatrix.matrix(0, 1), worldMatrix.matrix(0, 2));
	this->radius = length.Length();
	// ƒ[ƒ‹ƒhÀ•W‚É•ÏŠ·
	worldStart = worldMatrix * localStart;
	worldEnd = worldMatrix * localEnd;
}

void CollisionCapsule::Draw()
{
	using namespace MyDirectX;

	Matrix4 matrix;
	matrix.SetMatrix(worldStart, Vector3(radius, radius, radius), worldMatrix.rotation());
	DebugLine::DrawLine("HalfSphere", matrix, Color::cyan());
	matrix.SetMatrix(worldEnd, Vector3(radius, radius, radius), Quaternion::AxisAngle(worldMatrix.right(), 180.0f) * worldMatrix.rotation());
	DebugLine::DrawLine("HalfSphere", matrix, Color::cyan());

	Vector3 offset;
	Vector3 dir = worldEnd - worldStart;
	float len = dir.Length();
	dir.Normalize();
	
	offset = worldMatrix.right() * this->radius;
	DebugLine::DrawRay(worldStart + offset, dir, len, Color::cyan());
	DebugLine::DrawRay(worldStart - offset, dir, len, Color::cyan());
	offset = worldMatrix.forward() * this->radius;
	DebugLine::DrawRay(worldStart + offset, dir, len, Color::cyan());
	DebugLine::DrawRay(worldStart - offset, dir, len, Color::cyan());
}

bool CollisionCapsule::CollisionJudge(Collision * other)
{
	return other->vsCollisionCapsule(this);
}

bool CollisionCapsule::vsCollisionSphere(CollisionSphere * other)
{
	return CollisionJudge::Sphere_VS_Capsule(other, this);
}

bool CollisionCapsule::vsCollisionCapsule(CollisionCapsule * other)
{
	return CollisionJudge::Capsule_VS_Capsule(this, other);
}

bool CollisionCapsule::vsCollisionBox(CollisionBox * other)
{
	return false;
}

bool CollisionCapsule::vsCollisionMesh(CollisionMesh * other)
{
	return false;
}

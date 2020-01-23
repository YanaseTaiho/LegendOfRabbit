#include "CollisionBox.h"
#include "../Sphere/CollisionSphere.h"

using namespace FrameWork;

CollisionBox::CollisionBox()
{
}

CollisionBox::~CollisionBox()
{
}

inline void CollisionBox::Update()
{
}

void CollisionBox::Draw()
{
}

inline bool CollisionBox::CollisionJudge(Collision * other)
{
	return other->vsCollisionBox(this);
}

inline bool CollisionBox::vsCollisionSphere(CollisionSphere * other)
{
	return false;
}

bool CollisionBox::vsCollisionCapsule(CollisionCapsule * other)
{
	return false;
}

inline bool CollisionBox::vsCollisionBox(CollisionBox * other)
{
	return false;
}

bool CollisionBox::vsCollisionMesh(CollisionMesh * other)
{
	return false;
}

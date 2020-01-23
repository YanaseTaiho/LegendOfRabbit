#include "Collision.h"
#include "../../Component/Rigidbody/Rigidbody.h"
#include "../../GameObject/GameObject.h"

using namespace FrameWork;

std::list<std::weak_ptr<Collision>> Collision::collisionLists[(int)Layer::MAX];
std::list<std::weak_ptr<Collision>> Collision::normalCollisionLists[(int)Layer::MAX];

void Collision::LoadSerialized(const std::shared_ptr<Component>& component)
{
	Component::LoadSerialized(component);
	layer = gameObject.lock()->GetLayer();
	AddComponentList<Collision>(collisionLists[(int)layer]);

	auto rigid = gameObject.lock()->GetComponent<Rigidbody>();
	if (rigid.expired())
		rigid = gameObject.lock()->GetComponentWithParent<Rigidbody>();

	if (rigid.expired())
		AddComponentList<Collision>(normalCollisionLists[(int)layer]);
}

void Collision::SetRigidbody(std::weak_ptr<Rigidbody> rigidbody)
{
	bool isDef = false;
	
	if (!this->rigidbody.expired())
		isDef = true;

	this->rigidbody = rigidbody;

	// リジッドボディが既にある時はコリジョンの操作をする必要がない
	if (isDef) return;

	// リジッドボディがセットされたらノーマルコリジョンリストから外す
	RemoveComponentList<Collision>(normalCollisionLists[(int)layer]);
}

void Collision::RemoveRigidbody()
{
	this->rigidbody.reset();
	// リジッドボディが外れたらノーマルコリジョンリストに登録
	for (auto & col : collisionLists[(int)layer])
	{
		if (col.lock().get() != this) continue;

		normalCollisionLists[(int)layer].emplace_back(col);
		return;
	}
}

void Collision::ChangeWorld()
{
	Vector3 offset = localMatrix.position();
	// オフセットだけ親から見たワールド座標で配置
	Vector3 worldPos = transform.lock()->GetWorldMatrix().position();

	localMatrix.matrix(0, 3) = worldPos.x + offset.x;
	localMatrix.matrix(1, 3) = worldPos.y + offset.y;
	localMatrix.matrix(2, 3) = worldPos.z + offset.z;

	Matrix4 invMat = transform.lock()->GetWorldMatrix().Inverse();
	localMatrix = invMat * localMatrix;

	this->Update();
}

void Collision::AddComponent()
{
	Component::AddComponent();
	layer = gameObject.lock()->GetLayer();
	AddComponentList<Collision>(collisionLists[(int)layer]);

	auto rigid = gameObject.lock()->GetComponent<Rigidbody>();
	if(rigid.expired())
		rigid = gameObject.lock()->GetComponentWithParent<Rigidbody>();

	if (rigid.expired())
	{
		AddComponentList<Collision>(normalCollisionLists[(int)layer]);
	}
	else
	{
		rigidbody = rigid;
		rigidbody.lock()->SetCollision(collisionLists[(int)layer].back());
	}

	Awake();
}

void Collision::RemoveComponent()
{
	RemoveComponentList<Collision>(collisionLists[(int)layer]);
	if (rigidbody.expired())
	{
		RemoveComponentList<Collision>(normalCollisionLists[(int)layer]);
	}
	else
	{
		rigidbody.lock()->RemoveCollision(this);
		rigidbody.reset();
	}
	Component::RemoveComponent();
}

void Collision::ChangeLayer(Layer layer)
{
	auto com = RemoveComponentList(collisionLists[(int)this->layer]);

	if (this->rigidbody.expired())
	{
		RemoveComponentList(normalCollisionLists[(int)this->layer]);
	}

	this->layer = layer;
	AddComponentList(collisionLists[(int)this->layer], com);

	if (this->rigidbody.expired())
	{
		AddComponentList(normalCollisionLists[(int)this->layer], com);
	}
}

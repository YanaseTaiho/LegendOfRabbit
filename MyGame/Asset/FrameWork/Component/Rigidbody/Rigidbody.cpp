#include "Rigidbody.h"
#include "../Collision/Collision.h"
#include "../../GameObject/GameObject.h"

using namespace FrameWork;

std::list<std::weak_ptr<Rigidbody>> Rigidbody::rigidbodyList;

Rigidbody::Rigidbody(float gravity, float resistance)
	: gravity(gravity), resistance(Mathf::Clamp01(resistance))
{

}

Rigidbody::~Rigidbody()
{

}


void Rigidbody::DrawImGui(int id)
{
	std::string strId = "##Rigid" + std::to_string(id);
	ImGui::DragFloat3(("Veclocity" + strId).c_str(), this->velocity, 0.001f);
	ImGui::DragFloat(("Gravity" + strId).c_str(), &this->gravity, 0.001f);
	ImGui::SliderFloat(("Resistance" + strId).c_str(), &this->resistance, 0.0f, 1.0f, "%.3f", 1.0f);
}

void Rigidbody::Update()
{
	resistance = Mathf::Clamp01(resistance);
	velocity *= (1.0f - resistance);
	velocity.y -= gravity;

	transform.lock()->SetWorldPosition(transform.lock()->GetWorldPosition() + velocity);
}

void Rigidbody::UpdateSet()
{
	Vector3 pos = transform.lock()->GetWorldPosition();
	transform.lock()->SetWorldPosition(pos + velocity);
}

void Rigidbody::AddForce(Vector3 force)
{
	velocity += force;
}

void Rigidbody::AddComponent()
{
	Component::AddComponent();
	AddComponentList(rigidbodyList);
	// コリジョンを登録
	ResetCollisions();
	if (collisions.size() == 0) return;

	// 親にもリジッドボディがあれば登録し直す
	auto rigid = gameObject.lock()->GetComponentWithParent<Rigidbody>();
	if (!rigid.expired())
		rigid.lock()->ResetCollisions();

	Awake();
}

void Rigidbody::RemoveComponent()
{
	RemoveCollisions();
	RemoveComponentList(rigidbodyList);
	Component::RemoveComponent();
}

void Rigidbody::SetCollision(std::weak_ptr<Collision> & collision)
{
	collisions.emplace_back(collision);
}

void Rigidbody::ResetCollisions()
{
	RemoveCollisions();

	std::list<std::weak_ptr<Collision>> cols;
	gameObject.lock()->GetComponents<Collision>(&cols);
	std::weak_ptr<Rigidbody> rigid;

	for (auto r : rigidbodyList) {
		if (this == r.lock().get()) { rigid = r; break; }
	}

	for (auto & col : cols)
	{
		SetCollision(col);
		col.lock()->SetRigidbody(rigid);
	}

	std::list<std::weak_ptr<Collision>>().swap(cols);

	for (auto & child : transform.lock()->GetChilds())
	{
		SetCollisions(rigid, child.lock()->gameObject);
	}
}

void Rigidbody::SetCollisions(std::weak_ptr<Rigidbody> & rigid, std::weak_ptr<GameObject> & object)
{
	if (object.expired()) return;
	// objectにリジッドボディがあれば何もしない
	if (!object.lock()->GetComponent<Rigidbody>().expired()) return;

	std::list<std::weak_ptr<Collision>> cols;
	object.lock()->GetComponents<Collision>(&cols);
	for (auto & col : cols)
	{
		rigid.lock()->SetCollision(col);
		col.lock()->SetRigidbody(rigid);
	}

	std::list<std::weak_ptr<Collision>>().swap(cols);

	for (auto & child : object.lock()->transform.lock()->GetChilds())
	{
		SetCollisions(rigid, child.lock()->gameObject);
	}
}

void Rigidbody::RemoveCollisions()
{
	for (auto & col : collisions)
	{
		col.lock()->RemoveRigidbody();
	}
	std::list<std::weak_ptr<Collision>>().swap(collisions);
}

void Rigidbody::RemoveCollision(Collision * collision)
{
	for (auto itr = collisions.begin(), end = collisions.end(); itr != end; ++itr)
	{
		if (itr->lock().get() == collision)
		{
			collisions.erase(itr);
			return;
		}
	}
}

void Rigidbody::JudgeNormalCollision()
{
	for (auto & rigidCol : this->collisions)
	{
		if (!rigidCol.lock()->IsEnable()) continue;

		for (int i = 0; i < (int)Layer::MAX; i++)
		{
			for (auto & normalCol : Collision::NormalCollisionList(i))
			{
				if (!normalCol.lock()->IsEnable()) continue;
				// 衝突判定
				if (rigidCol.lock()->CollisionJudge(normalCol.lock().get()))
					HitCollision(rigidCol, normalCol);
			}
		}
	}
}

void Rigidbody::JudgeRigidbody(std::weak_ptr<Rigidbody> rigidbody)
{
	for (auto & rigidCol1 : this->collisions)
	{
		if (!rigidCol1.lock()->IsEnable()) continue;
		for (auto & rigidCol2 : rigidbody.lock()->collisions)
		{
			if (!rigidCol2.lock()->IsEnable()) continue;
			// 衝突判定
			if (rigidCol1.lock()->CollisionJudge(rigidCol2.lock().get()))
			{
				HitCollision(rigidCol1, rigidCol2);
			}
		}
	}
}

void Rigidbody::HitCollision(std::weak_ptr<Collision> & a, std::weak_ptr<Collision> & b)
{
	bool myTrigger = a.lock()->isTrigger;
	bool otherTrigger = b.lock()->isTrigger;

	if (myTrigger)
	{
		a.lock()->gameObject.lock()->OnTrigger(a, b);
	}
	if (otherTrigger)
	{
		b.lock()->gameObject.lock()->OnTrigger(b, a);
		return; // この時点でどちらも物理判定ではないのでコンティニューする
	}
	// どちらも物理判定の場合のみ呼ぶ
	if (!myTrigger && !otherTrigger)
	{
		// 衝突した時の位置をコリジョンに反映 //

		// 一つ目のコリジョンは必ずリジッドボディを持っているので更新
		{
			auto & allCol = a.lock()->rigidbody.lock()->collisions;
			for (auto & col : allCol)
			{
				col.lock()->Update();
			}
		}

		// 二つ目のコリジョンはリジッドボディを持っていない時もあるので確認して更新
		if (!b.lock()->rigidbody.expired())
		{
			auto & allCol = b.lock()->rigidbody.lock()->collisions;
			for (auto & col : allCol)
			{
				col.lock()->Update();
			}
		}

		a.lock()->gameObject.lock()->OnCollision(a, b);
		b.lock()->gameObject.lock()->OnCollision(b, a);
	}
}

void Rigidbody::UpdateCollisionJudge()
{
	for (auto rigidItrA = rigidbodyList.begin(), rigidEnd = rigidbodyList.end(); rigidItrA != rigidEnd; ++rigidItrA)
	{
		if (!rigidItrA->lock()->IsEnable()) continue;
		// リジッドボディがない全てのコリジョンと判定
		rigidItrA->lock()->JudgeNormalCollision();

		// リジッドボディ同士の判定
		auto rigidItrB = rigidItrA;
		++rigidItrB;
		for (; rigidItrB != rigidEnd; ++rigidItrB)
		{
			if (!rigidItrB->lock()->IsEnable()) continue;
			rigidItrA->lock()->JudgeRigidbody(*rigidItrB);
		}
	}
}

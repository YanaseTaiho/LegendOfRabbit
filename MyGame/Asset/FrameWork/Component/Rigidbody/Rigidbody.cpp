#include "Rigidbody.h"
#include "../Collision/Collision.h"
#include "../Collision/Sphere/CollisionSphere.h"
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
	transform.lock()->SetWorldPosition(transform.lock()->GetWorldPosition() + velocity);

	resistance = Mathf::Clamp01(resistance);
	velocity *= (1.0f - resistance);
	velocity.y -= gravity;
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
	// �R���W������o�^
	ResetCollisions();
	if (collisions.size() == 0) return;

	// �e�ɂ����W�b�h�{�f�B������Γo�^������
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
	// object�Ƀ��W�b�h�{�f�B������Ή������Ȃ�
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

				// ��܂��Ȕ�������� ( �����m�̔���ȊO )
				if (rigidCol.lock()->GetType() != typeid(CollisionSphere)
					|| normalCol.lock()->GetType() != typeid(CollisionSphere))
				{
					float distSq = (rigidCol.lock()->worldMatrix.position()- normalCol.lock()->worldMatrix.position()).LengthSq();
					float radSq = rigidCol.lock()->scaleRadius * rigidCol.lock()->scaleRadius + normalCol.lock()->scaleRadius * normalCol.lock()->scaleRadius;
					// �Փ˂̉\�������������画��I��
					if (distSq > radSq) continue;
				}

				// �Փ˔���
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

			// ��܂��Ȕ�������� ( �����m�̔���ȊO )
			if (rigidCol1.lock()->GetType() != typeid(CollisionSphere)
				|| rigidCol2.lock()->GetType() != typeid(CollisionSphere))
			{
				float distSq = (rigidCol1.lock()->worldMatrix.position() - rigidCol2.lock()->worldMatrix.position()).LengthSq();
				float radSq = rigidCol1.lock()->scaleRadius * rigidCol1.lock()->scaleRadius + rigidCol2.lock()->scaleRadius * rigidCol2.lock()->scaleRadius;
				// �Փ˂̉\�������������画��I��
				if (distSq > radSq) continue;
			}

			// �Փ˔���
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
		return; // ���̎��_�łǂ������������ł͂Ȃ��̂ŃR���e�B�j���[����
	}
	// �ǂ������������̏ꍇ�̂݌Ă�
	if (!myTrigger && !otherTrigger)
	{
		// �Փ˂������̈ʒu���R���W�����ɔ��f //

		// ��ڂ̃R���W�����͕K�����W�b�h�{�f�B�������Ă���̂ōX�V
		{
			auto & allCol = a.lock()->rigidbody.lock()->collisions;
			for (auto & col : allCol)
			{
				col.lock()->Update();
			}
		}

		// ��ڂ̃R���W�����̓��W�b�h�{�f�B�������Ă��Ȃ���������̂Ŋm�F���čX�V
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
		// ���W�b�h�{�f�B���Ȃ��S�ẴR���W�����Ɣ���
		rigidItrA->lock()->JudgeNormalCollision();

		// ���W�b�h�{�f�B���m�̔���
		auto rigidItrB = rigidItrA;
		++rigidItrB;
		for (; rigidItrB != rigidEnd; ++rigidItrB)
		{
			if (!rigidItrB->lock()->IsEnable()) continue;
			rigidItrA->lock()->JudgeRigidbody(*rigidItrB);
		}
	}
}

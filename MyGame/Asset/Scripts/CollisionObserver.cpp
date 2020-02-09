#include "CollisionObserver.h"

void CollisionObserver::DrawImGui(int id)
{
	std::string strId = "##CollisionObserver" + std::to_string(id);
	ImGui::Text("Collision");
	ImGui::Indent();
	for (auto & c : collisionList)
	{
		ImGui::Text(c.lock()->gameObject.lock()->name.c_str());
	}
	ImGui::Unindent();

	ImGui::Text("Trigger");
	ImGui::Indent();
	for (auto & c : collisionTriggerList)
	{
		ImGui::Text(c.lock()->gameObject.lock()->name.c_str());
	}
	ImGui::Unindent();
}

void CollisionObserver::Update()
{
	// �t���[���P�ʂŃR���W�����̃��X�g�����Z�b�g
	addCollisionList.clear();
	addCollisionTriggerList.clear();
}

void CollisionObserver::LateUpdate()
{
	// ���ݕێ����Ă���R���W�����ƃt���[���P�ʂ̃��X�g���r
	for (auto itr = collisionList.begin(), end = collisionList.end(); itr != end; ++itr)
	{
		bool isHit = false;
		for (auto addItr = addCollisionList.begin(), addEnd = addCollisionList.end(); addItr != addEnd; ++addItr)
		{
			if (itr->lock() == addItr->lock())
			{
				addCollisionList.erase(addItr);
				isHit = true;
				break;
			}
		}
		// �R���W���������ꂽ���O�̊֐����Ă�
		if (!isHit)
		{
			if (collisionExit && !itr->expired()) collisionExit(*itr);
			collisionList.erase(itr);
		}
	}

	// ���ݕێ����Ă���R���W�����ƃt���[���P�ʂ̃��X�g���r
	for (auto itr = collisionTriggerList.begin(), end = collisionTriggerList.end(); itr != end; ++itr)
	{
		bool isHit = false;
		for (auto addItr = addCollisionTriggerList.begin(), addEnd = addCollisionTriggerList.end(); addItr != addEnd; ++addItr)
		{
			if (itr->lock() == addItr->lock())
			{
				addCollisionTriggerList.erase(addItr);
				isHit = true;
				break;
			}
		}
		// �R���W���������ꂽ���O�̊֐����Ă�
		if (!isHit)
		{
			if (triggerExit && !itr->expired()) triggerExit(*itr);
			collisionTriggerList.erase(itr);
		}
	}
}

void CollisionObserver::OnCollisionStay(std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other)
{
	for (auto itr = collisionList.begin(), end = collisionList.end(); itr != end; ++itr)
	{
		// �O�̃t���[���ŏՓ˂��Ă�����
		if (itr->lock() == other.lock())
		{
			addCollisionList.emplace_back(other); 
			return;
		}
	}

	// �R���W�������Փ˂����u�Ԃ̊֐����Ă�
	if(collisionEnter) collisionEnter(other);

	addCollisionList.emplace_back(other);
	collisionList.emplace_back(other);
}

void CollisionObserver::OnTriggerStay(std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other)
{
	for (auto itr = collisionTriggerList.begin(), end = collisionTriggerList.end(); itr != end; ++itr)
	{
		if (itr->lock() == other.lock())
		{
			addCollisionTriggerList.emplace_back(other);
			return;
		}
	}

	// �R���W�������Փ˂����u�Ԃ̊֐����Ă�
	if (triggerEnter) triggerEnter(other);

	addCollisionTriggerList.emplace_back(other);
	collisionTriggerList.emplace_back(other);
}
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
	// フレーム単位でコリジョンのリストをリセット
	addCollisionList.clear();
	addCollisionTriggerList.clear();
}

void CollisionObserver::LateUpdate()
{
	// 現在保持しているコリジョンとフレーム単位のリストを比較
	for (auto itr = collisionList.begin(), end = collisionList.end(); itr != end;)
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
		// コリジョンが離れた直前の関数を呼ぶ
		if (!isHit)
		{
			if (collisionExit && !itr->expired()) collisionExit(*itr);
			itr = collisionList.erase(itr);
			continue;
		}
		++itr;
	}

	// 現在保持しているコリジョンとフレーム単位のリストを比較
	for (auto itr = collisionTriggerList.begin(), end = collisionTriggerList.end(); itr != end;)
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
		// コリジョンが離れた直前の関数を呼ぶ
		if (!isHit)
		{
			if (triggerExit && !itr->expired()) triggerExit(*itr);
			itr = collisionTriggerList.erase(itr);
			continue;
		}
		++itr;
	}
}

void CollisionObserver::OnCollisionStay(std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other)
{
	for (auto itr = collisionList.begin(), end = collisionList.end(); itr != end; ++itr)
	{
		// 前のフレームで衝突していたら
		if (itr->lock() == other.lock())
		{
			addCollisionList.emplace_back(other); 
			return;
		}
	}

	// コリジョンが衝突した瞬間の関数を呼ぶ
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

	// コリジョンが衝突した瞬間の関数を呼ぶ
	if (triggerEnter) triggerEnter(other);

	addCollisionTriggerList.emplace_back(other);
	collisionTriggerList.emplace_back(other);
}

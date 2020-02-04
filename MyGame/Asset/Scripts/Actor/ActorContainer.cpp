#include "ActorContainer.h"

void ActorContainer::DrawImGui(int id)
{
}

void ActorContainer::OnStart()
{
	// 子のアクターを全て取得
	gameObject.lock()->GetComponentsWithChild<BaseActor>(&actorList);

	for (auto actor : actorList)
	{
		actor.lock()->OnStart();
	}
}

void ActorContainer::OnUpdate()
{
	for (auto itr = actorList.begin(),end = actorList.end(); itr != end;)
	{
		// 途中で死んだ場合はリストから外す
		if (itr->expired())
		{
			itr = actorList.erase(itr);
			continue;
		}
		itr->lock()->OnUpdate();
		++itr;
	}
}

void ActorContainer::OnLateUpdate()
{
	for (auto itr = actorList.begin(), end = actorList.end(); itr != end;)
	{
		// 途中で死んだ場合はリストから外す
		if (itr->expired())
		{
			itr = actorList.erase(itr);
			continue;
		}
		itr->lock()->OnLateUpdate();
		++itr;
	}
}

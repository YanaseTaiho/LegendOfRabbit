#include "ActorContainer.h"

void ActorContainer::DrawImGui(int id)
{
}

void ActorContainer::OnStart()
{
	// 子のアクターを全て取得
	gameObject.lock()->GetComponentsWithChild<BaseActor>(&actorList);
}

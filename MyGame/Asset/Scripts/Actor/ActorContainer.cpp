#include "ActorContainer.h"

void ActorContainer::DrawImGui(int id)
{
}

void ActorContainer::OnStart()
{
	// �q�̃A�N�^�[��S�Ď擾
	gameObject.lock()->GetComponentsWithChild<BaseActor>(&actorList);
}

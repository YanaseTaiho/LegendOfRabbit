#include "ActorContainer.h"

void ActorContainer::DrawImGui(int id)
{
}

void ActorContainer::OnStart()
{
	// �q�̃A�N�^�[��S�Ď擾
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
		// �r���Ŏ��񂾏ꍇ�̓��X�g����O��
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
		// �r���Ŏ��񂾏ꍇ�̓��X�g����O��
		if (itr->expired())
		{
			itr = actorList.erase(itr);
			continue;
		}
		itr->lock()->OnLateUpdate();
		++itr;
	}
}

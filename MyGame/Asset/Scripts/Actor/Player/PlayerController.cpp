#include "PlayerController.h"

PlayerController::PlayerController()
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::OnStart()
{
	this->player.lock()->OnStart();
}

void PlayerController::OnUpdate()
{
	this->player.lock()->OnUpdate();
}

void PlayerController::OnLateUpdate()
{
	this->player.lock()->OnLateUpdate();
}

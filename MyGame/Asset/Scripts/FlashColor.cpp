#include "FlashColor.h"

void FlashColor::SetRenderer(std::weak_ptr<Renderer> target)
{
	this->targetRenderer = target;
}

void FlashColor::StartFlash(const Color & from, const Color & to, float speed, float time)
{
	this->defaultColor = from;
	this->targetColor = to;

	this->flashSpeed = speed;
	this->flashTime = time;
	this->colorWeight = 0.0f;
	this->isPlus = true;
}

void FlashColor::OnUpdate()
{
	if (flashTime <= 0.0f || targetRenderer.expired()) return;

	Color & color = targetRenderer.lock()->baseColor;
	color.r = Mathf::Lerp(defaultColor.r, targetColor.r, colorWeight);
	color.g = Mathf::Lerp(defaultColor.g, targetColor.g, colorWeight);
	color.b = Mathf::Lerp(defaultColor.b, targetColor.b, colorWeight);
	color.a = Mathf::Lerp(defaultColor.a, targetColor.a, colorWeight);

	if (isPlus)
	{
		colorWeight += flashSpeed * Time::DeltaTime();
		if (colorWeight >= 1.0f)
		{
			isPlus = false;
			colorWeight = 1.0f;
		}
	}
	else
	{
		colorWeight -= flashSpeed * Time::DeltaTime();
		if (colorWeight <= 0.0f)
		{
			isPlus = true;
			colorWeight = 0.0f;
		}
	}

	flashTime -= Time::DeltaTime();
	if (flashTime <= 0.0f)
	{
		flashTime = 0.0f;
		targetRenderer.lock()->SetColor(defaultColor);	// 点滅時間終了時元のカラーに戻す
	}
}

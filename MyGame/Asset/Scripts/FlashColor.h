#ifndef __FlashColor__H__
#define __FlashColor__H__

#include "FrameWork/Common.h"

// 設定したカラーを行き来させるクラス

class FlashColor
{
public:
	FlashColor() {}
	~FlashColor() {}

	void SetRenderer(std::weak_ptr<Renderer> target);
	void OnUpdate();
	void StartFlash(const Color & from, const Color & to, float speed, float time);
	bool IsFlash() { return flashTime > 0.0f; }

private:
	std::weak_ptr<Renderer> targetRenderer;

	Color defaultColor;
	Color targetColor;

	bool isPlus;
	float colorWeight;  // 二つのカラーの重み
	float flashSpeed;	// 点滅速度

	float flashTime = 0.0f;		// 点滅時間
};

#endif // !__FlashColor__H__
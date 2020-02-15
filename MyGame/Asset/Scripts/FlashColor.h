#ifndef __FlashColor__H__
#define __FlashColor__H__

#include "FrameWork/Common.h"

// �ݒ肵���J���[���s����������N���X

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
	float colorWeight;  // ��̃J���[�̏d��
	float flashSpeed;	// �_�ő��x

	float flashTime = 0.0f;		// �_�Ŏ���
};

#endif // !__FlashColor__H__
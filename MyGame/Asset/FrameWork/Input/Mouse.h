#ifndef _MOUSE_H_
#define _MOUSE_H_

#include "../Vector.h"

namespace Input
{
	class Mouse
	{
	public:

		enum class Button : int
		{
			Left,
			Right,
			Center,
			Max
		};

		static void Initialize();
		static void Update();
		static void SetButtonDown(Button button);
		static void SetButtonUp(Button button);
		static void SetWheel(float value);
		static void GetPosition(FrameWork::Vector2 * pos);
		static FrameWork::Vector2 & GetPosition();
		static bool IsPress(Button button);
		static bool IsPressAny();
		static bool IsTriggerAny();
		static bool IsTrigger(Button button);
		static bool IsRelease(Button button);
		static void GetMove(FrameWork::Vector2 * move);
		static FrameWork::Vector2 & GetMove();
		static float GetHwheel();

	private:
		static FrameWork::Vector2 curPos;
		static FrameWork::Vector2 prePos;
		static FrameWork::Vector2 clientPos;
		static FrameWork::Vector2 moveVec;
		static float wheelValue;
		static bool isWheel;

		static bool curState[(int)Button::Max];
		static bool mouseState[(int)Button::Max];
		static bool mouseStateTrigger[(int)Button::Max];
		static bool mouseStateRelease[(int)Button::Max];
	};
}

#endif // !_MOUSE_H_
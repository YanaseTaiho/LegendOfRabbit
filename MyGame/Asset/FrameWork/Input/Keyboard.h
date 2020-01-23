#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <windows.h>

namespace Input
{
#define	NUM_KEY_MAX			(256)	// キーの最大数

	class Keyboad
	{
	public:
		static void Update();

		static bool IsPress(int nKey);
		static bool IsTrigger(int nKey);
		static bool IsRelease(int nKey);

	public:
		static BYTE keyState[NUM_KEY_MAX];			// キーボードの入力情報ワーク
		static BYTE keyStateTrigger[NUM_KEY_MAX];	// キーボードのトリガー情報ワーク
		static BYTE keyStateRelease[NUM_KEY_MAX];	// キーボードのリリース情報ワーク
	};
}

#endif // !_KEYBOARD_H_

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <windows.h>

namespace Input
{
#define	NUM_KEY_MAX			(256)	// �L�[�̍ő吔

	class Keyboad
	{
	public:
		static void Update();

		static bool IsPress(int nKey);
		static bool IsTrigger(int nKey);
		static bool IsRelease(int nKey);

	public:
		static BYTE keyState[NUM_KEY_MAX];			// �L�[�{�[�h�̓��͏�񃏁[�N
		static BYTE keyStateTrigger[NUM_KEY_MAX];	// �L�[�{�[�h�̃g���K�[��񃏁[�N
		static BYTE keyStateRelease[NUM_KEY_MAX];	// �L�[�{�[�h�̃����[�X��񃏁[�N
	};
}

#endif // !_KEYBOARD_H_

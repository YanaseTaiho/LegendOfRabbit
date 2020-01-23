#include "Keyboard.h"

using namespace Input;

BYTE Keyboad::keyState[NUM_KEY_MAX];
BYTE Keyboad::keyStateTrigger[NUM_KEY_MAX];
BYTE Keyboad::keyStateRelease[NUM_KEY_MAX];

void Keyboad::Update()
{
	BYTE aKeyState[NUM_KEY_MAX];

	// デバイスからデータを取得
	if (GetKeyboardState(aKeyState))
	{
		for (int nCnKey = 0; nCnKey < NUM_KEY_MAX; nCnKey++)
		{
			// キートリガー・リリース情報を生成
			keyStateTrigger[nCnKey] = (keyState[nCnKey] ^ aKeyState[nCnKey]) & aKeyState[nCnKey];
			keyStateRelease[nCnKey] = (keyState[nCnKey] ^ aKeyState[nCnKey]) & keyState[nCnKey];

			// キープレス情報を保存
			keyState[nCnKey] = aKeyState[nCnKey];
		}
	}
}

bool Keyboad::IsPress(int nKey)
{
	return (keyState[nKey] & 0x80) != 0;
}

bool Keyboad::IsTrigger(int nKey)
{
	return (keyStateTrigger[nKey] & 0x80) != 0;
}

bool Keyboad::IsRelease(int nKey)
{
	return (keyStateRelease[nKey] & 0x80) != 0;
}

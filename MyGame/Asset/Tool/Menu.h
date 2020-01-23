#ifndef _MENU_H_
#define _MENU_H_

#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
//#include "../resource.h"
#include <windows.h>

#include "../FrameWork/Singleton.h"

namespace Tool
{
	class Menu
	{
	private:
		friend FrameWork::Singleton<Menu>;

		Menu();
		~Menu();
	public:

		void WinMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		//void Command(WORD param) const;
		void ImGui() const;
	};
}

#endif // !MENU_H_


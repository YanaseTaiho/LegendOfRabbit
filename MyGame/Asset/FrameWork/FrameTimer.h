#ifndef __FRAME_TIMER__H__
#define __FRAME_TIMER__H__

#include <vector>
#include <functional>
#include "Time.h"

namespace FrameWork
{
	class FrameTimer
	{
	public:
		// frame : ‚±‚ÌƒtƒŒ[ƒ€”Œã‚ÉŠÖ”‚ğŒÄ‚Ño‚·
		static void SetFuncTimer(int frame, std::function<void(void)> func);

		static void Update();
		static void Release();

	private:
		struct FrameFunc
		{
			FrameFunc(int frameCnt, std::function<void(void)> func)
				: frameCnt(frameCnt), func(func) {};
			int frameCnt;
			std::function<void(void)> func;
		};

		static std::vector<FrameFunc> frameFuncArray;
	};
}

#endif // !__FRAME_TIMER__H__


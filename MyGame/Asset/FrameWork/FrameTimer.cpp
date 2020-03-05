#include "FrameTimer.h"
#include <algorithm>

using namespace FrameWork;

std::vector<FrameTimer::FrameFunc> FrameTimer::frameFuncArray;

void FrameTimer::SetFuncTimer(int frame, std::function<void(void)> func)
{
	frameFuncArray.emplace_back(frame, func);
}

void FrameTimer::Update()
{
	auto eraseEnd = std::remove_if(frameFuncArray.begin(), frameFuncArray.end(), [](FrameFunc & f)->bool
	{
		if (f.frameCnt <= 0)
		{
			f.func();
			return true;
		}
		f.frameCnt--;
		return false;
	});

	frameFuncArray.erase(eraseEnd, frameFuncArray.end());
}

void FrameTimer::Release()
{
	std::vector<FrameFunc>().swap(frameFuncArray);
}

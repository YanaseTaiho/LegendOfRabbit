#ifndef _LAYER_H_
#define _LAYER_H_

#include <string>

namespace FrameWork
{

#define BITMASK(num) (1 << (num))
#define LAYER_COMPARE(a, b) (a & b) != 0

	enum class Layer : int
	{
		Default = 0,
		Camera,
		Player,
		Ground,
		Canvas,
		MAX // Н≈Се31
	};

	static std::string enum_to_string(Layer layer)
	{
		switch (layer)
		{
		case Layer::Default: return "Default";
		case Layer::Camera:  return "Camera";
		case Layer::Player:  return "Player";
		case Layer::Ground:  return "Ground";
		case Layer::Canvas:  return "Canvas";
		}
		return "";
	}
}

#endif // !_LAYER_H_

#ifndef _TAG_H_
#define _TAG_H_

#include <string>

namespace FrameWork
{
	enum class Tag : int
	{
		None = 0,
		MainCamera,
		Player,
		Bullet,
		MAX
	};

	static std::string enum_to_string(Tag tag)
	{
		switch (tag)
		{
		case Tag::None:			return "None";
		case Tag::MainCamera:	return "MainCamera";
		case Tag::Player:		return "Player";
		case Tag::Bullet:		return "Bullet";
		}
		return "";
	}
}

#endif // !_TAG_H_


#ifndef _TAKENODE__H__
#define _TAKENODE__H__

#include "AnimationTake.h"
#include <memory>

namespace FrameWork
{
	struct TakeNode
	{
		~TakeNode()
		{
			std::vector<TakeNode>().swap(childTakes);
		}
		// w’è‚µ‚½ŠK‘w‚Ü‚Å’H‚é( ŠK‘w‚ª‘«‚è‚È‚©‚Á‚½‚ç¶¬‚·‚é )
		TakeNode * GetIndexArrayToTakeNode(int * indexArray, int length)
		{
			// ‘«‚è‚È‚¢•ª‚Ì”z—ñ‚ğ’Ç‰Á
			int n = (*indexArray + 1) - (int)childTakes.size();
			for (int c = 0; c < n; c++)
			{
				childTakes.emplace_back(TakeNode());
			}

			if (length <= 1)
				return &childTakes[*indexArray];
			else
			{
				int num = *indexArray;	// ˆê’U”’l‚ğ“ü‚ê‚Ä‚©‚ç‚Å‚È‚¢‚ÆŠëŒ¯II
				return childTakes[num].GetIndexArrayToTakeNode(++indexArray, --length);
			}
		}
		// w’è‚µ‚½ŠK‘w‚É‚ ‚é‚©‚Ç‚¤‚©’T‚·( ‚½‚Ç‚è‚Â‚¯‚È‚©‚Á‚½‚ç‹ó‚ğ•Ô‚· )
		TakeNode * SearchIndexArrayToTakeNode(int * indexArray, int length)
		{
			// ŠK‘w‚ª‚È‚©‚Á‚½‚çnull‚ğ•Ô‚·
			if (*indexArray > (int)childTakes.size())
				return nullptr;

			if (length <= 1)
				return &childTakes[*indexArray];
			else
			{
				int num = *indexArray;	// ˆê’U”’l‚ğ“ü‚ê‚Ä‚©‚ç‚Å‚È‚¢‚ÆŠëŒ¯II
				return childTakes[num].GetIndexArrayToTakeNode(++indexArray, --length);
			}
		}

		std::shared_ptr<AnimationTake> animTake;
		std::vector<TakeNode> childTakes;

	private:
		friend cereal::access;
		template<class Archive>
		void serialize(Archive & archive, std::uint32_t const version)
		{
			archive(CEREAL_NVP(animTake), CEREAL_NVP(childTakes));
		}
	};
}

CEREAL_CLASS_VERSION(FrameWork::TakeNode, 0)

#endif // !_TAKENODE__H__
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
		// 指定した階層まで辿る( 階層が足りなかったら生成する )
		TakeNode * GetIndexArrayToTakeNode(int * indexArray, int length)
		{
			// 足りない分の配列を追加
			int n = (*indexArray + 1) - (int)childTakes.size();
			for (int c = 0; c < n; c++)
			{
				childTakes.emplace_back(TakeNode());
			}

			if (length <= 1)
				return &childTakes[*indexArray];
			else
			{
				int num = *indexArray;	// 一旦数値を入れてからでないと危険！！
				return childTakes[num].GetIndexArrayToTakeNode(++indexArray, --length);
			}
		}
		// 指定した階層にあるかどうか探す( たどりつけなかったら空を返す )
		TakeNode * SearchIndexArrayToTakeNode(int * indexArray, int length)
		{
			// 階層がなかったらnullを返す
			if (*indexArray > (int)childTakes.size())
				return nullptr;

			if (length <= 1)
				return &childTakes[*indexArray];
			else
			{
				int num = *indexArray;	// 一旦数値を入れてからでないと危険！！
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
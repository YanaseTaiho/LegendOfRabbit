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
		// �w�肵���K�w�܂ŒH��( �K�w������Ȃ������琶������ )
		TakeNode * GetIndexArrayToTakeNode(int * indexArray, int length)
		{
			// ����Ȃ����̔z���ǉ�
			int n = (*indexArray + 1) - (int)childTakes.size();
			for (int c = 0; c < n; c++)
			{
				childTakes.emplace_back(TakeNode());
			}

			if (length <= 1)
				return &childTakes[*indexArray];
			else
			{
				int num = *indexArray;	// ��U���l�����Ă���łȂ��Ɗ댯�I�I
				return childTakes[num].GetIndexArrayToTakeNode(++indexArray, --length);
			}
		}
		// �w�肵���K�w�ɂ��邩�ǂ����T��( ���ǂ���Ȃ���������Ԃ� )
		TakeNode * SearchIndexArrayToTakeNode(int * indexArray, int length)
		{
			// �K�w���Ȃ�������null��Ԃ�
			if (*indexArray > (int)childTakes.size())
				return nullptr;

			if (length <= 1)
				return &childTakes[*indexArray];
			else
			{
				int num = *indexArray;	// ��U���l�����Ă���łȂ��Ɗ댯�I�I
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
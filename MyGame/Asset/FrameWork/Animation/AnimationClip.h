#ifndef _ANIMATION__H__
#define _ANIMATION__H__

#include "TakeNode.h"
#include "../Matrix.h"
#include <unordered_map>
#include <string>
#include <functional>

namespace FrameWork
{
	class Transform;
	class AnimationClipManager;

	class AnimationClip
	{
		friend AnimationClipManager;

	public:
		AnimationClip();
		~AnimationClip();

		void DrawImGui(int id);

		// hierarchy : ���̊K�w�Ɠ����ꏊ�ɑ}��
		// frame : �}���������t���[��
		void SetAnimationTake(Transform * hierarchy, int frame, Matrix4 & matrix);
		// �z��ŊK�w���w�肵�đ}��
		void SetAnimationTake(std::vector<int> * indexArray, int frame, Matrix4 & matrix);
		// �w�肵���K�w�̃t���[�����폜
		void DeleteAnimationTake(Transform * hierarchy, int frame);
		// �w�肵���K�w�̃t���[�����폜
		void DeleteAnimationTake(std::vector<int> indexArray, int frame);
		// �w�肵���t���[�����ɃR�[���o�b�N�֐����Ă�
		void SetCallBackFrame(int frameCnt, std::function<void(void)> callBackFunc);

		void OnStart();
		void UpdateAnimation(Transform * transform);
		void UpdateFrame();

		static void UpdateBlendAnimation(Transform * transform, AnimationClip * anim1, AnimationClip * anim2, const float weight);

		void SetTakeNode(std::weak_ptr<TakeNode> takeNode) { this->takeNode = takeNode; }
		void SetLoop(bool onLoop) { this->isLoop = onLoop; }
		void SetFrameCount(float frameCnt) { this->frameCnt = frameCnt; }
		void SetSpeed(float speed) { this->speed = speed; }
		void SetMaxFrame(int maxFrame) { this->maxFrame = maxFrame; }
		
		bool IsLoop() { return this->isLoop; }
		int GetFrameCount() { return (int)this->frameCnt; }
		float GetCurrentPercent() { return (maxFrame <= 0.0f) ? 1.0f : Mathf::Clamp01(frameCnt / maxFrame); }
		int GetMaxFrame() { return this->maxFrame; }
		float GetSpeed() { return this->speed; }
		bool IsEnd() { return isEnd; }
		bool IsTakeNode() { return !this->takeNode.expired(); }
		std::string GetName() { return name; }

		AnimationClip * CreateClone();

	private:

		// �{�̂����Ă�
		void Initialize(std::string name, std::weak_ptr<TakeNode> takeNode);

		// �ړI�̊K�w���������� indexArray �ɊK�w�f�[�^������
		bool SearchHeirarchy(Transform * hierarchy, Transform * parent, std::vector<int> * indexArray);

		static inline void HierarchyUpdate(Transform * transform, TakeNode * takeNode, const float frameCnt);

		static inline void HierarchyBlendUpdate(Transform * transform, TakeNode * takeNode1, TakeNode * takeNode2, const float frameCnt1, const float frameCnt2, const float weight);

		std::weak_ptr<TakeNode> takeNode;	// �Q��
		std::string name;
		float frameCnt;
		int maxFrame;
		float speed;
		bool isLoop;
		bool isEnd;

		int oldFrameCnt;
		std::unordered_map<int, std::function<void(void)>> callBackFuncMap;

	private:
		friend cereal::access;
		template<class Archive>
		void serialize(Archive & archive, std::uint32_t const version)
		{
			archive(name, takeNode, frameCnt, maxFrame, speed, isLoop, isEnd);
		}
	};
}

CEREAL_CLASS_VERSION(FrameWork::AnimationClip, 0)

#endif // !_ANIMATION__H__
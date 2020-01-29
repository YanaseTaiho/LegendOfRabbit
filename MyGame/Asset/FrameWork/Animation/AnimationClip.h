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

		// hierarchy : この階層と同じ場所に挿入
		// frame : 挿入したいフレーム
		void SetAnimationTake(Transform * const hierarchy, int frame, Matrix4 & matrix);
		// 配列で階層を指定して挿入
		void SetAnimationTake(std::vector<int> * indexArray, int frame, Matrix4 & matrix);
		// 指定した階層のフレームを削除
		void DeleteAnimationTake(Transform * const hierarchy, int frame);
		// 指定した階層のフレームを削除
		void DeleteAnimationTake(std::vector<int> indexArray, int frame);
		// 指定したフレーム時にコールバック関数を呼ぶ
		void SetCallBackFrame(int frameCnt, std::function<void(void)> callBackFunc);

		void OnStart();
		void UpdateAnimation(Transform * transform);
		void UpdateFrame();

		static void UpdateBlendAnimation(Transform * const transform, AnimationClip * const anim1, AnimationClip * const anim2, const float weight);

		void SetTakeNode(std::weak_ptr<TakeNode> takeNode) { this->takeNode = takeNode; }
		void SetLoop(bool onLoop) { this->isLoop = onLoop; }
		void SetFrameCount(float frameCnt) { this->frameCnt = frameCnt; }
		void SetSpeed(float speed) { this->speed = speed; }
		void SetMaxFrame(int maxFrame) { this->maxFrame = maxFrame; }
		
		bool IsLoop() { return this->isLoop; }
		int GetFrameCount() { return (int)this->frameCnt; }
		float GetCurrentPercent() { return (maxFrame <= 0.0f) ? 1.0f : Mathf::Clamp01(Mathf::Absf(frameCnt / maxFrame)); }
		int GetMaxFrame() { return this->maxFrame; }
		float GetSpeed() { return this->speed; }
		bool IsEnd() { return isEnd; }
		bool IsTakeNode() { return !this->takeNode.expired(); }
		std::string GetName() { return name; }

		AnimationClip * CreateClone();

	private:

		// 本体だけ呼ぶ
		void Initialize(std::string name, std::weak_ptr<TakeNode> takeNode);

		// 目的の階層を検索して indexArray に階層データを入れる
		bool SearchHeirarchy(Transform * const hierarchy, Transform * const parent, std::vector<int> * indexArray);

		static inline void HierarchyUpdate(Transform * const transform, TakeNode * const takeNode, const float frameCnt);

		static inline void HierarchyBlendUpdate(Transform * const transform, TakeNode * const takeNode1, TakeNode * const takeNode2, const float frameCnt1, const float frameCnt2, const float weight);

		std::weak_ptr<TakeNode> takeNode;	// 参照
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
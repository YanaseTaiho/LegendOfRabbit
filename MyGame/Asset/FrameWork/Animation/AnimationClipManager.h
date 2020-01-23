#ifndef ANIMATIONMANAGER_H_
#define ANIMATIONMANAGER_H_

#include "AnimationClip.h"
#include "SkeletonData.h"
#include "../Singleton.h"
#include <map>

namespace FrameWork
{
	class AnimationClipManager
	{
		friend Singleton<AnimationClipManager>;
		AnimationClipManager();
		~AnimationClipManager();
	public:

		void DrawImGui();
		void BeginDragImGui(std::unique_ptr<AnimationClip> & animClip, std::string id);
		bool DropTargetImGui(std::shared_ptr<AnimationClip> & animClip, std::string id);

		// アニメーションデータを登録する
		void RegisterAnimation(Skeleton * skeleton);
		// アニメーションデータを削除
		void DeleteAnimation(Skeleton * skeleton);
		// 指定した名前のアニメーションデータのクローンを取得する
		AnimationClip * GetClone(std::string animName);
		std::weak_ptr<TakeNode> GetTakeNode(std::string name);
		// アニメーションデータの名前が登録しているのかを確かめる
		// 登録済み：true、未登録：false
		bool IsRegistered(std::string animName);

	private:

		std::map<std::string, std::unique_ptr<AnimationClip>> animationMap;
		// テイクデータは共通なのでここで管理する
		std::unordered_map<std::string, std::shared_ptr<TakeNode>> takeNodeMap;
	};
}

#endif // !ANIMATIONMANAGER_H_

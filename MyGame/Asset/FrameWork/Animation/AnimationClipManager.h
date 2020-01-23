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

		// �A�j���[�V�����f�[�^��o�^����
		void RegisterAnimation(Skeleton * skeleton);
		// �A�j���[�V�����f�[�^���폜
		void DeleteAnimation(Skeleton * skeleton);
		// �w�肵�����O�̃A�j���[�V�����f�[�^�̃N���[�����擾����
		AnimationClip * GetClone(std::string animName);
		std::weak_ptr<TakeNode> GetTakeNode(std::string name);
		// �A�j���[�V�����f�[�^�̖��O���o�^���Ă���̂����m���߂�
		// �o�^�ς݁Ftrue�A���o�^�Ffalse
		bool IsRegistered(std::string animName);

	private:

		std::map<std::string, std::unique_ptr<AnimationClip>> animationMap;
		// �e�C�N�f�[�^�͋��ʂȂ̂ł����ŊǗ�����
		std::unordered_map<std::string, std::shared_ptr<TakeNode>> takeNodeMap;
	};
}

#endif // !ANIMATIONMANAGER_H_

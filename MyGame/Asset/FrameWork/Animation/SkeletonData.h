#ifndef __SKELETON_DATA_H__
#define __SKELETON_DATA_H__

#include <string>
#include <vector>
#include "../Matrix.h"

namespace FrameWork
{
	struct Keyframe
	{
		int frameNum;
		Matrix4 localMatrix;
	private:
		friend cereal::access;
		template<typename Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(frameNum), CEREAL_NVP(localMatrix));
		}
	};

	struct Anim
	{
		std::vector<Keyframe> keyFrames;
	private:
		friend cereal::access;
		template<typename Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(keyFrames));
		}
	};

	struct AnimInfo
	{
		std::string name;
		unsigned int maxFrame = 0;
	private:
		friend cereal::access;
		template<typename Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(name), CEREAL_NVP(maxFrame));
		}
	};

	struct Joint {
		std::vector<int> hierarchy;
		std::vector<int> childsId;
		Matrix4 initLocalMatrix;	// ���[�J����Ԃ̏����p��
		int parentIndex;
		std::string name;
		std::vector<Anim> animations;

		Joint()
		{
			parentIndex = -1;
		}
	private:
		friend cereal::access;
		template<typename Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(hierarchy), CEREAL_NVP(childsId), CEREAL_NVP(initLocalMatrix), CEREAL_NVP(parentIndex),
				 CEREAL_NVP(name), CEREAL_NVP(animations));
		}
	};

	struct BoneData
	{
		std::string name;
		Matrix4 offsetMatrix;
	private:
		friend cereal::access;
		template<typename Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(name), CEREAL_NVP(offsetMatrix));
		}
	};

	struct Skeleton {
		std::vector<Joint> mJoints;
		std::vector<AnimInfo> animInfo;

	private:
		friend cereal::access;
		template<typename Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(mJoints), CEREAL_NVP(animInfo));
		}
	public:

		Joint * GetJoint(std::string name)
		{
			for (auto& bone : mJoints)
			{
				if (bone.name == name)
				{
					return &bone;
				}
			}
			return nullptr;
		}

		Matrix4 GetKeyFrameWorldMatrix(Joint * joint, const unsigned int takeNum, const unsigned int keyFrame)
		{
			// �L�[�t���[��������Ă��Ȃ��ꍇ�͍Ō�̃L�[�t���[���̍s���I��
			Matrix4 keyMat = ((unsigned int)joint->animations[takeNum].keyFrames.size() > keyFrame) ?
				joint->animations[takeNum].keyFrames[keyFrame].localMatrix :
				joint->animations[takeNum].keyFrames.back().localMatrix;

			// �A�j���[�V�����f�[�^���Ȃ��ꍇ�͏����p����I��
			if (joint->animations.size() < takeNum)
			{
				keyMat = joint->initLocalMatrix;
			}

			if (joint->parentIndex != -1)
			{
				Joint * j = &mJoints[joint->parentIndex];
				Matrix4 mat = GetKeyFrameWorldMatrix(j, takeNum, keyFrame);

				return mat * keyMat;
			}
			else
			{
				// ��e�������ꍇ���̂܂܂̍s���Ԃ�
				return keyMat;
			}
		}
	};
}

#endif // !__SKELETON_DATA_H__


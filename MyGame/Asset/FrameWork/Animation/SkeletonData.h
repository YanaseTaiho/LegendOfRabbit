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
		Matrix4 initLocalMatrix;	// ローカル空間の初期姿勢
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
			// キーフレームが足りていない場合は最後のキーフレームの行列を選択
			Matrix4 keyMat = ((unsigned int)joint->animations[takeNum].keyFrames.size() > keyFrame) ?
				joint->animations[takeNum].keyFrames[keyFrame].localMatrix :
				joint->animations[takeNum].keyFrames.back().localMatrix;

			// アニメーションデータがない場合は初期姿勢を選択
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
				// 大親だった場合そのままの行列を返す
				return keyMat;
			}
		}
	};
}

#endif // !__SKELETON_DATA_H__


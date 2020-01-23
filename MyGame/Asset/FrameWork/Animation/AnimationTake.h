#ifndef _ANIMATIONTAKE_H_
#define _ANIMATIONTAKE_H_

#include "Takes/TakesVector3.h"
#include "Takes/TakesQuaternion.h"

namespace FrameWork
{
	// ��̃A�j���[�V�������Ǘ�����N���X
	class AnimationTake
	{
	public:
		AnimationTake();
		~AnimationTake();

		// �w�肵���t���[���̏ꏊ�ɍs����i�[
		void AddTakePosition(int frame, const Vector3 & position);
		void AddTakeScale(int frame, const Vector3 & scale);
		void AddTakeRotation(int frame, const Quaternion & rotation);

		// �w�肵���t���[�����폜
		void DeleteTakePosition(int frame);
		void DeleteTakeScale(int frame);
		void DeleteTakeRotation(int frame);

		// �J�E���g��i�߂�
		void UpdateTake(float frame, Vector3 * outPosition, Vector3 * outscale, Quaternion * outRotation);
		// �Ō�̃t���[���̎p�����擾����( ���[�v�������X���[�Y�ɂ��邽�߂Ɏg�� )
		void GetMaxTake(Vector3 * outPosition, Vector3 * outscale, Quaternion * outRotation);

	private:

		TakesVector3 takesPosition;	// �e�C�N�ԍ����̍��W
		TakesVector3 takesScale;	// �e�C�N�ԍ����̃X�P�[��
		TakesQuaternion takesRotation;	// �e�C�N�ԍ����̉�]

	private:
		friend cereal::access;
		template<class Archive>
		void serialize(Archive & archive, std::uint32_t const version)
		{
			archive(CEREAL_NVP(takesPosition), CEREAL_NVP(takesScale), CEREAL_NVP(takesRotation));
		}
	};
}

CEREAL_CLASS_VERSION(FrameWork::AnimationTake, 0)

#endif // !_ANIMATIONTAKE_H_


#ifndef _TAKES_H__
#define _TAKES_H__

#include <vector>
#include "Cereal/Common.h"

namespace FrameWork
{
	template<typename T>
	class Takes
	{
	private:
		friend cereal::access;
		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(CEREAL_NVP(takes), CEREAL_NVP(curTakeNum));
		}

	private:
		virtual T BlendTake(const T & a, const T & b, float weight) = 0;

	public:
		struct Take
		{
			Take() {}
			Take(float frame, T value) : frame(frame), value(value) {};
			T value;	// ���̃e�C�N�ł̒l
			float frame;	// �X�̃t���[��( ���݂̃t���[�����������A���̃e�C�N�Ɉڍs )

		private:
			friend cereal::access;
			template<class Archive>
			void serialize(Archive & archive)
			{
				archive(CEREAL_NVP(value), CEREAL_NVP(frame));
			}
		};

		Takes() {};
		virtual ~Takes() { std::vector<Take>().swap(takes); }

		std::vector<Take> takes;
		int curTakeNum = 0;


		// �w�肵���t���[���̏ꏊ�ɍs����i�[
		void AddTake(int frame, T value)
		{
			if (frame < 0) return;

			if (takes.size() == 0)
				takes.emplace_back(Take((float)frame, value));

			auto itr = takes.begin();
			// �w�肳�ꂽ�t���[���̏��Ԓʂ�̏ꏊ�ɓ����
			for (size_t i = 0, takeSize = takes.size(); i < takeSize; i++)
			{
				if (takes[i].frame == frame)
				{
					// �����t���[���Ȃ�㏑��
					takes[i] = Take((float)frame, value);
					return;
				}
				if (takes[i].frame > frame)
				{
					if (i > 0) { --itr; }
					break;
				}
				++itr;
			}

			// �ړI�̈ʒu�ɑ}��
			takes.insert(itr, Take((float)frame, value));
		}
		// �w�肵���t���[�����폜
		void DeleteTake(int frame)
		{
			if (takes.size() == 0) return;

			auto end = takes.end();
			for (auto itr = takes.begin(); itr != end; ++itr)
			{
				if (itr->frame == frame)
				{
					takes.erase(itr);
					return;
				}
			}
		}

		// �J�E���g��i�߂�
		void UpdateTake(float frameCnt, T * value)
		{
			int takeMax = (int)takes.size() - 1;
			if (takeMax < 0) return;

			// ���݂̃J�E���g�ɂ���e�C�N���`�F�b�N
			CheckFrame(frameCnt);

			T blend;
			if (takeMax > curTakeNum)					// ���̃e�C�N�����݂���ꍇ
			{
				const Take * curTake = &takes[curTakeNum];
				const Take * nextTake = &takes[curTakeNum + 1];
				// ���݂̃e�C�N�Ǝ��̃e�C�N�̃u�����h���v�Z
				float curCnt = frameCnt - curTake->frame;
				float diffFrame = nextTake->frame - curTake->frame;
				float weight = curCnt / diffFrame;
				blend = BlendTake(curTake->value, nextTake->value, weight);
			}
			// ���̃t���[�������݂��Ȃ��ꍇ
			else
			{
				blend = takes[curTakeNum].value;
			}

			*value = blend;
		}
		// �Ō���̃f�[�^���擾
		void GetMaxTake(T * value)
		{
			*value = takes.back().value;
		}

	private:
		void CheckFrame(float frameCnt)
		{
			if (takes[curTakeNum].frame > frameCnt)
			{
				curTakeNum--;
				CheckFrame(frameCnt);
			}

			// �e�C�N�Ɏ����Ȃ��ꍇ
			int takeMax = (int)takes.size() - 1;
			if (takeMax <= curTakeNum)
			{
				curTakeNum = takeMax;
				return;
			}

			// �J�E���g�����̃t���[���ɒB�����玟�̃e�C�N��
			if (takes[curTakeNum + 1].frame <= frameCnt)
			{
				curTakeNum++;
				// ���̃e�C�N�Ɉڍs�ł��邩�`�F�b�N���邽�ߍċN
				CheckFrame(frameCnt);
			}
		}
	};
}

#endif // !_TAKES_H__


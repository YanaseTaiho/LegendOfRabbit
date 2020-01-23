#ifndef _TIME_H_
#define _TIME_H_

#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <list>

namespace FrameWork
{

	class Time
	{
	private:
		static LARGE_INTEGER m_Counter;           // �N���b�N�J�E���g��
		static double m_dFreq;                    // 1�b������N���b�N�J�E���g���i���g���j
		static LONGLONG m_OldLongCount;           // �ȑO�̃N���b�N�J�E���g��
		static DWORD m_dwTGTOldCount;             // �ȑO�̎����i�~���b�j
		static std::list<double> m_dwDefTimeLst;       // ���ԃ��X�g
		static UINT m_uiNum;                      // �ړ����όv�Z���̃f�[�^��
		static double m_dwSumTimes;               // ���ʕ����̍��v�l

		static float timeScale; // �^�C���̑�����Ȃǂ̏���

		static double FPS;
		static float deltaTime;
		static float fixedDeltaTime;

		static int targetFPS; // ���̒l��FPS�ɂ��낦��

		static double timer;

	public:
		// �T���v������ݒ�( �����������x���オ�� )
		static void Initialize(unsigned int smp);
		static void Finalize();
		static void SetTargetFPS(unsigned int fps);

		// FPS�l���擾
		static double GetFPS();
		// �Q�[���N�����Ă���̃^�C�}�[���擾
		static double GetTime();
		// �b�Ԃ̒l��Ԃ�
		static float DeltaTime();
		// �����蔻��Ȃǂ̕����n�̏����Ɏg��
		static double FixedDeltaTime();
		// deltaTime()�ŕԂ��l�̔{����ݒ�( ��{�� 1.0f )
		static void SetTimeScale(float scale);
		// Update�Ȃǂ̊Ԋu�����낦�邽�߂֖̊�
		static bool Update();

	private:
		// �b�Ԃ̒l��Ԃ�(double�^)
		static double dDeltaTime();
		// FPS���X�V
		static void UpdateFPS(double Def);
		// �T���v������ύX
		static void SetSampleNum(unsigned int smp);
		// ���݂̎������擾
		static double GetCurDefTime();
	};
}

#endif // !_TIME_H_


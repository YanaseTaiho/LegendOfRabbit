#include "Time.h"
#include "debugPrintf.h"

using namespace FrameWork;

LARGE_INTEGER Time::m_Counter;           // �N���b�N�J�E���g��
double Time::m_dFreq;                    // 1�b������N���b�N�J�E���g���i���g���j
LONGLONG Time::m_OldLongCount;           // �ȑO�̃N���b�N�J�E���g��
DWORD Time::m_dwTGTOldCount;             // �ȑO�̎����i�~���b�j
std::list<double> Time::m_dwDefTimeLst;  // ���ԃ��X�g
UINT Time::m_uiNum;                      // �ړ����όv�Z���̃f�[�^��
double Time::m_dwSumTimes;               // ���ʕ����̍��v�l

float Time::deltaTime = 0.0f;
float Time::fixedDeltaTime = 0.0f;
float Time::timeScale = 1.0f;

double Time::FPS;
int Time::targetFPS = 60;
double Time::timer = 0.0;

void Time::Initialize(unsigned int smp)
{
	// �T���v�����̐ݒ�
	SetSampleNum(smp);

	// ���x���グ��
	timeBeginPeriod(1);

	// �������̎����i�~���b�j���擾
	m_dwTGTOldCount = timeGetTime();

	// �v��
	UpdateFPS(GetCurDefTime());

	timer = 0.0;
}

void Time::Finalize()
{
	timeEndPeriod(1);    // �^�C�}�[�̐��x��߂�
}

void Time::SetTargetFPS(unsigned int fps)
{
	targetFPS = fps;
}

// FPS�l���擾
double Time::GetFPS()
{
	return FPS;
}

double FrameWork::Time::GetTime()
{
	return timer;
}


bool Time::Update()
{
	// timeGetTime�֐��ɂ��v��
	DWORD CurTime = timeGetTime();
	DWORD CurDef = CurTime - m_dwTGTOldCount;         // �����J�E���g�����Z�o����

	if (CurDef >= (DWORD)(1000.0f / targetFPS))
	{
		UpdateFPS(CurDef);
		timer += dDeltaTime();
		m_dwTGTOldCount = CurTime; // ���݂̎�����ێ�

		return true;
	}
	return false;
}

double Time::dDeltaTime()
{
	if (FPS == 0.0) return 0.0;
	return (1.0 / FPS) * (double)timeScale;
}

// FPS���X�V
void Time::UpdateFPS(double Def)
{
	if (Def == 0) {
		// �v�Z�ł��Ȃ��̂ł�Ԃ�
		return;
	}

	// �ł��Â��f�[�^������
	m_dwDefTimeLst.pop_front();

	// �V�����f�[�^��ǉ�
	m_dwDefTimeLst.emplace_back(Def);

	// ���ʉ��Z�����̍X�V
	m_dwSumTimes += Def - *m_dwDefTimeLst.begin();

	// FPS�Z�o
	double AveDef = (m_dwSumTimes + Def) / m_uiNum;
	if (AveDef != 0)
	{
		FPS = 1000.0 / AveDef;
		if (FPS != 0.0)
		{
			deltaTime = (float)(1.0 / FPS) * timeScale;
			fixedDeltaTime = (float)(1.0 / (FPS * 0.5)) * timeScale;
		}
	}
}

// ���݂̍����������~���b�P�ʂŎ擾
double Time::GetCurDefTime()
{
	// timeGetTime�֐��ɂ��v��
	DWORD CurTime = timeGetTime();
	DWORD CurDef = CurTime - m_dwTGTOldCount;         // �����J�E���g�����Z�o����
	return CurDef;
}

// �T���v������ύX
void Time::SetSampleNum(unsigned int smp)
{
	m_uiNum = smp;    // ���ς��v�Z�����
	m_dwDefTimeLst.resize(m_uiNum, 0.0);    // ���X�g������
	m_dwSumTimes = 0;
}

float Time::DeltaTime()
{
	return deltaTime;
}

double Time::FixedDeltaTime()
{
	return fixedDeltaTime;
}

void Time::SetTimeScale(float scale)
{
	timeScale = scale;
}

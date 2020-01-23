#include "Time.h"
#include "debugPrintf.h"

using namespace FrameWork;

LARGE_INTEGER Time::m_Counter;           // クロックカウント数
double Time::m_dFreq;                    // 1秒当たりクロックカウント数（周波数）
LONGLONG Time::m_OldLongCount;           // 以前のクロックカウント数
DWORD Time::m_dwTGTOldCount;             // 以前の時刻（ミリ秒）
std::list<double> Time::m_dwDefTimeLst;  // 時間リスト
UINT Time::m_uiNum;                      // 移動平均計算時のデータ数
double Time::m_dwSumTimes;               // 共通部分の合計値

float Time::deltaTime = 0.0f;
float Time::fixedDeltaTime = 0.0f;
float Time::timeScale = 1.0f;

double Time::FPS;
int Time::targetFPS = 60;
double Time::timer = 0.0;

void Time::Initialize(unsigned int smp)
{
	// サンプル数の設定
	SetSampleNum(smp);

	// 精度を上げる
	timeBeginPeriod(1);

	// 生成時の時刻（ミリ秒）を取得
	m_dwTGTOldCount = timeGetTime();

	// 計測
	UpdateFPS(GetCurDefTime());

	timer = 0.0;
}

void Time::Finalize()
{
	timeEndPeriod(1);    // タイマーの精度を戻す
}

void Time::SetTargetFPS(unsigned int fps)
{
	targetFPS = fps;
}

// FPS値を取得
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
	// timeGetTime関数による計測
	DWORD CurTime = timeGetTime();
	DWORD CurDef = CurTime - m_dwTGTOldCount;         // 差分カウント数を算出する

	if (CurDef >= (DWORD)(1000.0f / targetFPS))
	{
		UpdateFPS(CurDef);
		timer += dDeltaTime();
		m_dwTGTOldCount = CurTime; // 現在の時刻を保持

		return true;
	}
	return false;
}

double Time::dDeltaTime()
{
	if (FPS == 0.0) return 0.0;
	return (1.0 / FPS) * (double)timeScale;
}

// FPSを更新
void Time::UpdateFPS(double Def)
{
	if (Def == 0) {
		// 計算できないのでを返す
		return;
	}

	// 最も古いデータを消去
	m_dwDefTimeLst.pop_front();

	// 新しいデータを追加
	m_dwDefTimeLst.emplace_back(Def);

	// 共通加算部分の更新
	m_dwSumTimes += Def - *m_dwDefTimeLst.begin();

	// FPS算出
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

// 現在の差分時刻をミリ秒単位で取得
double Time::GetCurDefTime()
{
	// timeGetTime関数による計測
	DWORD CurTime = timeGetTime();
	DWORD CurDef = CurTime - m_dwTGTOldCount;         // 差分カウント数を算出する
	return CurDef;
}

// サンプル数を変更
void Time::SetSampleNum(unsigned int smp)
{
	m_uiNum = smp;    // 平均を計算する個数
	m_dwDefTimeLst.resize(m_uiNum, 0.0);    // リスト初期化
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

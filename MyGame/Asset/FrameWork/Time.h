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
		static LARGE_INTEGER m_Counter;           // クロックカウント数
		static double m_dFreq;                    // 1秒当たりクロックカウント数（周波数）
		static LONGLONG m_OldLongCount;           // 以前のクロックカウント数
		static DWORD m_dwTGTOldCount;             // 以前の時刻（ミリ秒）
		static std::list<double> m_dwDefTimeLst;       // 時間リスト
		static UINT m_uiNum;                      // 移動平均計算時のデータ数
		static double m_dwSumTimes;               // 共通部分の合計値

		static float timeScale; // タイムの早送りなどの処理

		static double FPS;
		static float deltaTime;
		static float fixedDeltaTime;

		static int targetFPS; // この値のFPSにそろえる

		static double timer;

	public:
		// サンプル数を設定( 多い方が精度が上がる )
		static void Initialize(unsigned int smp);
		static void Finalize();
		static void SetTargetFPS(unsigned int fps);

		// FPS値を取得
		static double GetFPS();
		// ゲーム起動してからのタイマーを取得
		static double GetTime();
		// 秒間の値を返す
		static float DeltaTime();
		// 当たり判定などの物理系の処理に使う
		static double FixedDeltaTime();
		// deltaTime()で返す値の倍数を設定( 基本は 1.0f )
		static void SetTimeScale(float scale);
		// Updateなどの間隔をそろえるための関門
		static bool Update();

	private:
		// 秒間の値を返す(double型)
		static double dDeltaTime();
		// FPSを更新
		static void UpdateFPS(double Def);
		// サンプル数を変更
		static void SetSampleNum(unsigned int smp);
		// 現在の時刻を取得
		static double GetCurDefTime();
	};
}

#endif // !_TIME_H_


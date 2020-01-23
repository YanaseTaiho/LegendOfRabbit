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
			T value;	// このテイクでの値
			float frame;	// 個々のフレーム( 現在のフレーム超えた時、次のテイクに移行 )

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


		// 指定したフレームの場所に行列を格納
		void AddTake(int frame, T value)
		{
			if (frame < 0) return;

			if (takes.size() == 0)
				takes.emplace_back(Take((float)frame, value));

			auto itr = takes.begin();
			// 指定されたフレームの順番通りの場所に入れる
			for (size_t i = 0, takeSize = takes.size(); i < takeSize; i++)
			{
				if (takes[i].frame == frame)
				{
					// 同じフレームなら上書き
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

			// 目的の位置に挿入
			takes.insert(itr, Take((float)frame, value));
		}
		// 指定したフレームを削除
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

		// カウントを進める
		void UpdateTake(float frameCnt, T * value)
		{
			int takeMax = (int)takes.size() - 1;
			if (takeMax < 0) return;

			// 現在のカウントにあるテイクかチェック
			CheckFrame(frameCnt);

			T blend;
			if (takeMax > curTakeNum)					// 次のテイクが存在する場合
			{
				const Take * curTake = &takes[curTakeNum];
				const Take * nextTake = &takes[curTakeNum + 1];
				// 現在のテイクと次のテイクのブレンドを計算
				float curCnt = frameCnt - curTake->frame;
				float diffFrame = nextTake->frame - curTake->frame;
				float weight = curCnt / diffFrame;
				blend = BlendTake(curTake->value, nextTake->value, weight);
			}
			// 次のフレームが存在しない場合
			else
			{
				blend = takes[curTakeNum].value;
			}

			*value = blend;
		}
		// 最後尾のデータを取得
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

			// テイクに次がない場合
			int takeMax = (int)takes.size() - 1;
			if (takeMax <= curTakeNum)
			{
				curTakeNum = takeMax;
				return;
			}

			// カウントが次のフレームに達したら次のテイクへ
			if (takes[curTakeNum + 1].frame <= frameCnt)
			{
				curTakeNum++;
				// 次のテイクに移行できるかチェックするため再起
				CheckFrame(frameCnt);
			}
		}
	};
}

#endif // !_TAKES_H__


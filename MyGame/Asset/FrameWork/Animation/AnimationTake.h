#ifndef _ANIMATIONTAKE_H_
#define _ANIMATIONTAKE_H_

#include "Takes/TakesVector3.h"
#include "Takes/TakesQuaternion.h"

namespace FrameWork
{
	// 一つのアニメーションを管理するクラス
	class AnimationTake
	{
	public:
		AnimationTake();
		~AnimationTake();

		// 指定したフレームの場所に行列を格納
		void AddTakePosition(int frame, const Vector3 & position);
		void AddTakeScale(int frame, const Vector3 & scale);
		void AddTakeRotation(int frame, const Quaternion & rotation);

		// 指定したフレームを削除
		void DeleteTakePosition(int frame);
		void DeleteTakeScale(int frame);
		void DeleteTakeRotation(int frame);

		// カウントを進める
		void UpdateTake(float frame, Vector3 * outPosition, Vector3 * outscale, Quaternion * outRotation);
		// 最後のフレームの姿勢を取得する( ループ処理をスムーズにするために使う )
		void GetMaxTake(Vector3 * outPosition, Vector3 * outscale, Quaternion * outRotation);

	private:

		TakesVector3 takesPosition;	// テイク番号毎の座標
		TakesVector3 takesScale;	// テイク番号毎のスケール
		TakesQuaternion takesRotation;	// テイク番号毎の回転

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


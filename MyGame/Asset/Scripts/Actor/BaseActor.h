#ifndef _BASEACTOR_H_
#define _BASEACTOR_H_

#include "FrameWork/Common.h"
#include "BaseFSM.h"

enum class DamageType : int
{
	None,
	Hit,
	Guard,
	Deth
};

class BaseActor : public MonoBehaviour
{
public:
	BaseActor() {}
	virtual ~BaseActor() {}

	// ダメージを受けた時に呼ばれる ( ガードされた時は false を返す )
	// mine : ダメージ判定のあったコリジョン
	// other : ダメージを与えるコリジョン
	virtual DamageType Damage(int damage, std::weak_ptr<Collision>& mine, std::weak_ptr<Collision>& other, Vector3 force = Vector3::zero()) { return DamageType::None; }
private:
	
};

#endif // !_BASEACTOR_H_


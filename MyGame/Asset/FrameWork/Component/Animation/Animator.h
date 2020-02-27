#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

#include "../Component.h"
#include "../../Animation/AnimationController.h"
#include "../../BindClass.h"

namespace FrameWork
{
	class Animator : public Component
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive) const
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
			archive(CEREAL_NVP(animController));
		}
		template<class Archive>
		void load(Archive & archive)
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
			archive(CEREAL_NVP(animController));
		}

		void LoadSerialized(const std::shared_ptr<Component> & component) override;

		void DrawImGui(int id) override;

	public:
		Animator() {}
		Animator(AnimationController * animController);
		~Animator();

		static std::list<std::weak_ptr<Animator>> & ComponentList() { return animatorList; }

		static void CreateAnimationControllerList(BindClass<AnimationController> animControList);

		void Start();
		void Update();

		void SetAnimationCallBack(std::string name, int frame, std::function<void(void)> callBack);
		void ChangeAnimation(std::string name);
		void SetFloat(std::string name, float value);
		void SetInt(std::string name, int value);
		void SetBool(std::string name, bool value);
		void SetTrigger(std::string name, bool param = true);

		float GetFloat(std::string name);
		int GetInt(std::string name);
		bool GetBool(std::string name);
		bool GetTrigger(std::string name);

		float GetCurrentPercent();
		bool IsCurrentAnimation(std::string name);	// AnimationControllerのステート名
	protected:

		void AddComponent() override;

		void RemoveComponent() override
		{
			RemoveComponentList(animatorList);
			Component::RemoveComponent();
		}
	private:
		static std::list<std::weak_ptr<Animator>> animatorList;

		std::shared_ptr<AnimationController> animController;
	};
}

CEREAL_REGISTER_TYPE(FrameWork::Animator)

#endif // !_ANIMATOR_H_


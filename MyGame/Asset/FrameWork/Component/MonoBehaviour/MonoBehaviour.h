#ifndef MONOBEHAVIOUR_H_
#define MONOBEHAVIOUR_H_

#include "../Component.h"

namespace FrameWork
{
	class Collision;

	class MonoBehaviour : public Component
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive, std::uint32_t const version) const
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
		}

		template<class Archive>
		void load(Archive & archive, std::uint32_t const version)
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
		}

		void LoadSerialized(const std::shared_ptr<Component> & component) override;

	public:
		MonoBehaviour() = default;
		virtual ~MonoBehaviour() = default;
		
		static std::list<std::weak_ptr<MonoBehaviour>> & ComponentList() { return behaviourList; }

		// �X�V�̍ŏ��Ɉ�x�����Ă΂��
		virtual void Start() {}

		// �ʏ�X�V����
		inline virtual void Update() {}
		// �x���X�V����( ���̑��̍X�V����������ɌĂ΂��( �J�����̈ړ��Ȃ� ) )
		inline virtual void LateUpdate() {}

		// �f�o�b�O�p
		virtual void Draw() {};

		// �����I�ɓ������Ă���Ԃ͌Ă΂��
		inline virtual void OnCollisionStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) {}	 
		inline virtual void OnTriggerStay(std::weak_ptr<Collision> & mine, std::weak_ptr<Collision> & other) {}

	private:
		void AddComponent() override final;
		void RemoveComponent() override final;

		static std::list<std::weak_ptr<MonoBehaviour>> behaviourList;
	};
}

CEREAL_CLASS_VERSION(FrameWork::MonoBehaviour, 0)
CEREAL_REGISTER_TYPE(FrameWork::MonoBehaviour)

using namespace FrameWork;


#endif // !MONOBEHAVIOUR_H_


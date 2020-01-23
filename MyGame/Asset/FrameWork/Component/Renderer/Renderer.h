#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../Component.h"
#include "../../Color.h"
#include "../../Layer.h"
#include "../../Vector.h"

namespace FrameWork
{
	class Renderer : public Component
	{

	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive) const
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)), CEREAL_NVP(baseColor));
		}
		template<class Archive>
		void load(Archive & archive)
		{
			archive(cereal::make_nvp("Component", cereal::base_class<Component>(this)),
				CEREAL_NVP(baseColor));
		}


	protected:

		void LoadSerialized(const std::shared_ptr<Component> & component) override;
		Vector3 modelScale;

	public:
		Renderer() {};
		virtual ~Renderer() {};

		virtual void DrawShadow() {};
		virtual void Draw() {};
		Vector3 GetModelScale() { return modelScale; };

		static std::list<std::weak_ptr<Renderer>> & ComponentList(int layer) { return rendererList[layer]; }

		void SetColor(Color color) { baseColor = color; }

		Color baseColor;

	private:
		void AddComponent() override;
		void RemoveComponent() override;
		void ChangeLayer(Layer layer) override;

		static std::list<std::weak_ptr<Renderer>> rendererList[(int)Layer::MAX];
	};
}

CEREAL_REGISTER_TYPE(FrameWork::Renderer)

#endif // !_RENDERER_H_


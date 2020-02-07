#ifndef _DX_CAMERA_H_
#define _DX_CAMERA_H_

#include "FrameWork/Component/Camera/Camera.h"
#include "FrameWork/Matrix.h"
#include "DirectX/Common.h"

namespace MyDirectX
{
	class DXCamera : public FrameWork::Camera
	{
	private:
		friend cereal::access;
		template<class Archive>
		void save(Archive & archive) const
		{
			archive(cereal::make_nvp("Camera", cereal::base_class<FrameWork::Camera>(this)),
				CEREAL_NVP(viewport), CEREAL_NVP(nearDistance), CEREAL_NVP(farDistance));
		}
		template<class Archive>
		void load(Archive & archive)
		{
			archive(cereal::make_nvp("Camera", cereal::base_class<FrameWork::Camera>(this)),
				CEREAL_NVP(viewport), CEREAL_NVP(nearDistance), CEREAL_NVP(farDistance));
		}

	public:
		DXCamera();
		DXCamera(int left, int right, int top, int bottom, float nearDistance = 1.0f, float farDistance = 1000.0f);
		~DXCamera() {}

		void DrawImGui(int id) override;

		bool IsVisiblity(FrameWork::Vector3 position) override;
		void Draw() override;
		// カメラのスクリーン座標からワールド座標へ変換( depth : 0.0f ～ 1.0f )
		void ScreenToWorldPoint(Vector3 & outPos, const Vector2 & screenPos, float depth) override;

		Rect viewport;
		FrameWork::Matrix4 viewMatrix;
		FrameWork::Matrix4 projectionMatrix;
	private:
		
		float nearDistance;
		float farDistance;
	};
}

CEREAL_REGISTER_TYPE(MyDirectX::DXCamera)
CEREAL_REGISTER_POLYMORPHIC_RELATION(FrameWork::Component, MyDirectX::DXCamera)


#endif // !_DX_CAMERA_H_

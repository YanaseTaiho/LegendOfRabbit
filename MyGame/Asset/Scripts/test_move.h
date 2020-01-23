#ifndef _TEST_MOVE_H_
#define _TEST_MOVE_H_

#include "FrameWork/Component/MonoBehaviour/MonoBehaviour.h"
#include "Scene/TitleScene.h"
#include "Main/audio_clip.h"
#include "DirectX/Common.h"

class TestMove : public MonoBehaviour
{
public:
	//std::weak_ptr<Transform> camera;
	//CAudioClip * clip;
	//std::weak_ptr<Animator> anim;
	//std::weak_ptr<Rigidbody> rb;

	//Ray downRay;
	Ray forwardRay;
	//Ray rightRay;
	//Ray leftRay;

	//float horizontal = 0.0f;
	//float vertical = 0.0f;

	//TestMove() {};

	//~TestMove()
	//{
	//	clip->Stop();
	//	clip->Unload();
	//	delete clip;
	//}

	//void Start() override
	//{
	//	std::weak_ptr<GameObject> obj = FindGameObject(Tag::MainCamera);
	//	if (!obj.expired())
	//	{
	//		camera = obj.lock()->transform;
	//	}

	//	rb = gameObject.lock()->GetRigidbody();
	//	anim = gameObject.lock()->GetAnimator();

	//	clip = new CAudioClip();
	//	clip->Load("Asset/Battle2.wav");
	//	clip->Play(true);
	//}

	void Update() override
	{
	//	if (camera.expired()) return;

	//	vertical *= 0.80f;
	//	horizontal *= 0.80f;

	//	if (Input::Keyboad::IsPress(VK_UP))
	//		vertical += 0.3f;
	//	if (Input::Keyboad::IsPress(VK_DOWN))
	//		vertical -= 0.3f;
	//	if (Input::Keyboad::IsPress(VK_LEFT))
	//		horizontal -= 0.3f;
	//	if (Input::Keyboad::IsPress(VK_RIGHT))
	//		horizontal += 0.3f;
	//	
	//	if (vertical < 0.01f && vertical > -0.01f) vertical = 0.0f;
	//	if (horizontal < 0.01f && horizontal > -0.01f) horizontal = 0.0f;

	//	//Vector3 pos = transform.lock()->GetWorldPosition();
	//	Vector3 force;

	//	Vector3 forward = camera.lock()->forward(); forward.y = 0.0f; forward.Normalize();
	//	Vector3 right = camera.lock()->right(); right.y = 0.0f; right.Normalize();

	//	Vector3 moveDir = forward * vertical + right * horizontal;
	//	if (moveDir != Vector3::zero())
	//		moveDir.Normalize();

	//	float v = Mathf::Clamp(vertical, -1.0f, 1.0f);
	//	float h = Mathf::Clamp(horizontal, -1.0f, 1.0f);

	//	float moveAmount = Mathf::Clamp01(Mathf::Absf(v) + Mathf::Absf(h));
	//	float speed = 2.0f * Time::DeltaTime();

	//	force += moveDir * moveAmount * speed;

	//	if (Input::Keyboad::IsTrigger(VK_SPACE))
	//	{
	//		force += Vector3::up() * 2.0f;
	//	}
	//	if (Input::Keyboad::IsPress(VK_BACK))
	//	{
	//		force -= Vector3::up() * 0.1f;
	//	}

	//	rb.lock()->AddForce(force);
	//	//transform.lock()->SetWorldPosition(pos);

	//	float forceMax = 5.0f;
	//	if (rb.lock()->velocity.Length() > forceMax)
	//		rb.lock()->velocity = rb.lock()->velocity.Normalize() * forceMax;

		Vector3 pos = transform.lock()->GetWorldPosition();
		Quaternion q1 = transform.lock()->GetWorldRotation();
		if (Input::Keyboad::IsPress('G'))
		{
			Quaternion look = Quaternion::AxisAngle(Vector3::right(), -100.0f * Time::DeltaTime());
			q1 = look * q1;
			transform.lock()->SetWorldRotation(q1);
		}
		if (Input::Keyboad::IsPress('T'))
		{
			Quaternion look = Quaternion::AxisAngle(Vector3::right(), 100.0f * Time::DeltaTime());
			q1 = look * q1;
			transform.lock()->SetWorldRotation(q1);
		}
		if (Input::Keyboad::IsPress('F'))
		{
			Quaternion look = Quaternion::AxisAngle(Vector3::forward(), -100.0f * Time::DeltaTime());
			q1 = look * q1;
			transform.lock()->SetWorldRotation(q1);
		}
		if (Input::Keyboad::IsPress('H'))
		{
			Quaternion look = Quaternion::AxisAngle(Vector3::forward(), 100.0f * Time::DeltaTime());
			q1 = look * q1;
			transform.lock()->SetWorldRotation(q1);
		}
		float moveSpeed = 1.0f;
		if (Input::Keyboad::IsPress('X'))
			pos.y += moveSpeed;
		if (Input::Keyboad::IsPress('Z'))
			pos.y -= moveSpeed;
		if (Input::Keyboad::IsPress(VK_UP))
			pos.z += moveSpeed;
		if (Input::Keyboad::IsPress(VK_DOWN))
			pos.z -= moveSpeed;
		if (Input::Keyboad::IsPress(VK_LEFT))
			pos.x -= moveSpeed;
		if (Input::Keyboad::IsPress(VK_RIGHT))
			pos.x += moveSpeed;

		transform.lock()->SetWorldPosition(pos);

	//	// レイキャストの判定
	//	{
	//		RayCastInfo info;
	//		std::weak_ptr<Collision> hitCollision;
	//		Vector3 pos = transform.lock()->GetWorldPosition();
	//		downRay.Set(pos + Vector3(0.0f, 1.0f, 0.0f), Vector3::down(), 5.0f);
	//		if (RayCast::JudgeAllCollision(&downRay, &info, &hitCollision))
	//		{
	//			float height = info.point.y + 2.0f;
	//			if (height + 3.0f >= pos.y)
	//			{
	//				pos.y = /*height - 0.1f;*/ Mathf::Lerp(pos.y, height - 0.1f, 0.3f);
	//		
	//				transform.lock()->SetWorldPosition(pos);
	//				if (rb.lock()->velocity.y < 0.0f)
	//					rb.lock()->velocity.y = 0.0f;
	//			}
	//		}
	//		
	//	//	Vector3 vec = rb.lock()->velocity; vec.y = 0.0f;
	//	////	if (vec.Length() > 0.01f)
	//	//	{
	//	//		forwardRay.Set(pos, vec, 5.0f);
	//	//		Vector3 r = Vector3::Cross(transform.lock()->up(), vec);
	//	//		rightRay.Set(pos, r, 3.0f);
	//	//		leftRay.Set(pos, -r, 3.0f);
	//	//		if (RayCast::JudgeAllCollision(&forwardRay, &info))
	//	//		{
	//	//			float dist = Vector3::Dot(info.normal, vec.Normalize()) * info.distance;
	//	//			dist = Mathf::Absf(dist);
	//	//			if (dist < 2.5f)
	//	//			{
	//	//				Vector3 pushPoint = info.normal * (2.4f - dist);

	//	//				pos = Vector3::Lerp(pos, pos + pushPoint, 0.5f);//pushPoint;
	//	//				transform.lock()->SetWorldPosition(pos);
	//	//			}
	//	//		}
	//	//		else if (RayCast::JudgeAllCollision(&rightRay, &info))
	//	//		{
	//	//			float dist = Vector3::Dot(info.normal, r.Normalize()) * info.distance;
	//	//			dist = Mathf::Absf(dist);
	//	//			if (dist < 2.0f)
	//	//			{
	//	//				Vector3 pushPoint = info.normal * (1.9f - dist);

	//	//				pos = Vector3::Lerp(pos, pos + pushPoint, 0.5f);
	//	//				transform.lock()->SetWorldPosition(pos);
	//	//			}
	//	//		}
	//	//		else if (RayCast::JudgeAllCollision(&leftRay, &info))
	//	//		{
	//	//			float dist = Vector3::Dot(info.normal, -r.Normalize()) * info.distance;
	//	//			dist = Mathf::Absf(dist);
	//	//			if (dist < 2.0f)
	//	//			{
	//	//				Vector3 pushPoint = info.normal * (1.9f - dist);

	//	//				pos = Vector3::Lerp(pos, pos + pushPoint, 0.5f);
	//	//				transform.lock()->SetWorldPosition(pos);
	//	//			}
	//	//		}
	//	//	}
	//	}

	//	if (!anim.expired())
	//	{
	//		anim.lock()->SetFloat("WalkValue", moveAmount);

	//		if (Input::Keyboad::IsTrigger(VK_SPACE))
	//		{
	//			anim.lock()->SetTrigger("HappyTrigger");
	//		}
	//	}
	}

	void Draw() override
	{
		forwardRay.Set(transform.lock()->GetWorldPosition(), transform.lock()->forward(), 20.0f);
		//DebugLine::DrawRay(downRay.start, downRay.end, Color::red());
		DebugLine::DrawRay(forwardRay.start, forwardRay.end, Color::green());
		DebugLine::DrawLine("Sphere", transform.lock()->GetWorldMatrix(), Color::green());
		//DebugLine::DrawRay(rightRay.start, rightRay.end, Color::green());
		//DebugLine::DrawRay(leftRay.start, leftRay.end, Color::blue());
	}

	//void LateUpdate() override
	//{
	//	if (Input::Keyboad::IsTrigger(VK_DELETE))
	//	{
	//		this->gameObject.lock()->Destroy();
	//	}

	//	if (Input::Keyboad::IsTrigger(VK_RETURN))
	//	{
	//		SceneManager::Instance()->SceneChange(new TitleScene());
	//	}
	//}
};

#endif // !1

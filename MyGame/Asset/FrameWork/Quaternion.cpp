#include "Quaternion.h"

using namespace FrameWork;

Quaternion::Quaternion()
{
	quaternion.Identity();
}

Quaternion::Quaternion(float x, float y, float z, float w)
{
	quaternion = Eigen::Quaternionf(w, x, y, z);
}

Quaternion::Quaternion(float x, float y, float z)
{
	SetEulerAngles(x, y, z);
}

Quaternion::Quaternion(Vector3 v)
{
	SetEulerAngles(v.x, v.y, v.z);
}

Quaternion::Quaternion(Vector3 axis, float angle)
{
	SetAxisAngle(axis, angle);
}

Quaternion & Quaternion::operator*=(const Quaternion & q)
{
	quaternion *= q.quaternion;
	return *this;
}

Quaternion Quaternion::operator*(const Quaternion & q)
{
	Quaternion q1(*this); 
	return q1 *= q;
}

bool Quaternion::operator==(const Quaternion & q)
{
	return (quaternion.x() == q.quaternion.x() &&
			quaternion.y() == q.quaternion.y() &&
			quaternion.z() == q.quaternion.z() &&
			quaternion.w() == q.quaternion.w());
}

bool Quaternion::operator!=(const Quaternion & q)
{
	return !(*this == q);
}

Quaternion Quaternion::Set(float x, float y, float z, float w)
{
	quaternion = Eigen::Quaternionf(w, x, y, z);
	return *this;
	quaternion.angularDistance(quaternion);
}

Quaternion Quaternion::SetEulerAngles(float x, float y, float z)
{
	quaternion = Eigen::AngleAxisf(Mathf::DegToRad(x), Eigen::Vector3f::UnitX()) *
				 Eigen::AngleAxisf(Mathf::DegToRad(y), Eigen::Vector3f::UnitY()) *
				 Eigen::AngleAxisf(Mathf::DegToRad(z), Eigen::Vector3f::UnitZ());

	return *this;
}

Quaternion Quaternion::SetEulerAngles(Vector3 v)
{
	quaternion = Eigen::AngleAxisf(Mathf::DegToRad(v.x), Eigen::Vector3f::UnitX()) *
		Eigen::AngleAxisf(Mathf::DegToRad(v.y), Eigen::Vector3f::UnitY()) *
		Eigen::AngleAxisf(Mathf::DegToRad(v.z), Eigen::Vector3f::UnitZ());
	
	return *this;
}

Vector3 Quaternion::GetEulerAngles()
{
	Eigen::Vector3f v = quaternion.matrix().eulerAngles(0, 1, 2);	// roll pitch yaw
	return Vector3(v.x(), v.y(), v.z()) * Mathf::RadToDeg();
}

Vector3 Quaternion::GetEulerAnglesToRadian()
{
	Eigen::Vector3f v = quaternion.matrix().eulerAngles(0, 1, 2);	// roll pitch yaw
	return Vector3(v.x(), v.y(), v.z());
}

Quaternion Quaternion::SetAxisAngle(Vector3 axis, float angle)
{
	Eigen::Vector3f v(axis.x, axis.y, axis.z);
	quaternion = Eigen::AngleAxisf(Mathf::DegToRad(angle), v);
	return *this;
}

Quaternion Quaternion::SetIdentity()
{
	quaternion = Eigen::Quaternionf::Identity();
	return *this;
}

Quaternion Quaternion::Slerp(const Quaternion & q, float t) const
{
	Quaternion q1;
	q1.quaternion = quaternion.slerp(t, q.quaternion);
	return q1;
}

Quaternion Quaternion::SetFormToRotation(Vector3 from, Vector3 to)
{
	this->quaternion = FromToRotation(from, to).quaternion;
	return *this;
}

Quaternion Quaternion::Normalize()
{
	quaternion.normalize();
	return *this;
}

Quaternion Quaternion::Normalized() const
{
	return Normalize(*this);
}

Quaternion Quaternion::Inverse()
{
	quaternion = quaternion.inverse();
	return *this;
}

Quaternion Quaternion::Inversed() const
{
	return Inverse(*this);
}

Quaternion Quaternion::Conjugate()
{
	quaternion = quaternion.conjugate();
	return *this;
}

Quaternion Quaternion::Conjugated() const
{
	return Conjugate(*this);
}

Quaternion Quaternion::FromToRotation(Vector3 from, Vector3 to)
{
	from.Normalize();
	to.Normalize();
	Eigen::Vector3f v1(from.x, from.y, from.z);
	Eigen::Vector3f v2(to.x, to.y, to.z);
	Quaternion q;
	q.quaternion.setFromTwoVectors(v1, v2);
	return q;
}

Quaternion Quaternion::LookRotation(Vector3 lookAt, Vector3 upDirection)
{
	Vector3 forward = lookAt.Normalize();
	Vector3 right = Vector3::Cross(upDirection, forward).Normalize();
	Vector3 up = Vector3::Cross(forward, right);

	float m00 = right.x;   float m01 = right.y;   float m02 = right.z;
	float m10 = up.x;	   float m11 = up.y;      float m12 = up.z;
	float m20 = forward.x; float m21 = forward.y; float m22 = forward.z;

	float num8 = (m00 + m11) + m22;
	Quaternion quaternion;

	if (num8 > 0.0f)
	{
		float num = Mathf::Sqrtf(num8 + 1.0f);
		quaternion.w() = num * 0.5f;
		num = 0.5f / num;
		quaternion.x() = (m12 - m21) * num;
		quaternion.y() = (m20 - m02) * num;
		quaternion.z() = (m01 - m10) * num;
		return quaternion;
	}
	if ((m00 >= m11) && (m00 >= m22))
	{
		float num7 = Mathf::Sqrtf(((1.0f + m00) - m11) - m22);
		float num4 = 0.5f / num7;
		quaternion.x() = 0.5f * num7;
		quaternion.y() = (m01 + m10) * num4;
		quaternion.z() = (m02 + m20) * num4;
		quaternion.w() = (m12 - m21) * num4;
		return quaternion;
	}
	if (m11 > m22)
	{
		float num6 = Mathf::Sqrtf(((1.0f + m11) - m00) - m22);
		float num3 = 0.5f / num6;
		quaternion.x() = (m10 + m01) * num3;
		quaternion.y() = 0.5f * num6;
		quaternion.z() = (m21 + m12) * num3;
		quaternion.w() = (m20 - m02) * num3;
		return quaternion;
	}

	float num5 = Mathf::Sqrtf(((1.0f + m22) - m00) - m11);
	float num2 = 0.5f / num5;
	quaternion.x() = (m20 + m02) * num2;
	quaternion.y() = (m21 + m12) * num2;
	quaternion.z() = 0.5f * num5;
	quaternion.w() = (m01 - m10) * num2;
	return quaternion;
}

float Quaternion::AngularDistance(Quaternion q1, Quaternion q2)
{
	return q1.quaternion.angularDistance(q2.quaternion);
}

Quaternion Quaternion::EulerAngles(float x, float y, float z)
{
	return Quaternion(x, y, z);
}

Quaternion Quaternion::EulerAngles(Vector3 v)
{
	return Quaternion(v);
}

Quaternion Quaternion::AxisAngle(Vector3 axis, float angle)
{
	return Quaternion(axis, angle);
}

Quaternion Quaternion::Slerp(Quaternion from, Quaternion to, float t)
{
	Quaternion q;
	q.quaternion = from.quaternion.slerp(t, to.quaternion);
	return q;
}

Quaternion Quaternion::Inverse(Quaternion q)
{
	Quaternion q1(q);
	q1.quaternion = q1.quaternion.inverse();
	return q1;
}	

Quaternion Quaternion::Identity()
{
	Quaternion q;
	q.quaternion = Eigen::Quaternionf::Identity();
	return q;
}

Quaternion Quaternion::Normalize(Quaternion q)
{
	Quaternion q1(q);
	q1.quaternion.normalize();
	return q1;
}

Quaternion Quaternion::Conjugate(Quaternion q)
{
	Quaternion q1(q);
	q1.quaternion = q1.quaternion.conjugate();
	return q1;
}

float Quaternion::Dot(Quaternion q)
{
	return quaternion.dot(q.quaternion);
}

float Quaternion::Dot(Quaternion q1, Quaternion q2)
{
	return q1.Dot(q2);
}

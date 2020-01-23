#include "Matrix.h"

using namespace FrameWork;

Matrix4::Matrix4(const XMMATRIX4 & matrix)
{
	*this = matrix; 
}

Matrix4& Matrix4::operator= (const _matrix& m)
{
	matrix = m;
	return *this;
}

Matrix4& Matrix4::operator= (const XMMATRIX& m)
{
	matrix(0, 0) = m.r[0].m128_f32[0];
	matrix(1, 0) = m.r[0].m128_f32[1];
	matrix(2, 0) = m.r[0].m128_f32[2];
	matrix(3, 0) = m.r[0].m128_f32[3];

	matrix(0, 1) = m.r[1].m128_f32[0];
	matrix(1, 1) = m.r[1].m128_f32[1];
	matrix(2, 1) = m.r[1].m128_f32[2];
	matrix(3, 1) = m.r[1].m128_f32[3];

	matrix(0, 2) = m.r[2].m128_f32[0];
	matrix(1, 2) = m.r[2].m128_f32[1];
	matrix(2, 2) = m.r[2].m128_f32[2];
	matrix(3, 2) = m.r[2].m128_f32[3];

	matrix(0, 3) = m.r[3].m128_f32[0];
	matrix(1, 3) = m.r[3].m128_f32[1];
	matrix(2, 3) = m.r[3].m128_f32[2];
	matrix(3, 3) = m.r[3].m128_f32[3];

	return *this;
}

Matrix4& Matrix4::operator= (const XMMATRIX4& m)
{
	*this = m.matrix;
	return *this;
}

Matrix4 & Matrix4::operator*=(const Matrix4 & m)
{
	matrix *= m.matrix;
	return *this;
}

Matrix4 Matrix4::operator*(const Matrix4& m) const 
{
	Matrix4 m1 = *this;
	m1 *= m;
	return m1;
}

Vector3 Matrix4::operator* (const Vector3& v) const
{
	Eigen::Vector3f vec = Eigen::Vector3f(v.x, v.y, v.z);
	Eigen::Affine3f affine;
	affine = matrix;
	vec = affine * vec;
	return Vector3(vec.x(), vec.y(), vec.z());
}

Vector3 Matrix4::forward() const
{
	return Vector3(matrix(0, 2), matrix(1, 2), matrix(2, 2)).Normalize();
}

Vector3 Matrix4::right() const
{
	return Vector3(matrix(0, 0), matrix(1, 0), matrix(2, 0)).Normalize();
}

Vector3 Matrix4::up() const
{
	return Vector3(matrix(0, 1), matrix(1, 1), matrix(2, 1)).Normalize();
}

Vector3 Matrix4::position() const
{
	return Vector3(matrix(0, 3), matrix(1, 3), matrix(2, 3));
}

Vector3 Matrix4::scale() const
{
	Vector3 x(matrix(0, 0), matrix(0, 1), matrix(0, 2));
	Vector3 y(matrix(1, 0), matrix(1, 1), matrix(1, 2));
	Vector3 z(matrix(2, 0), matrix(2, 1), matrix(2, 2));
	return Vector3(x.Length(), y.Length(), z.Length());
}

Quaternion Matrix4::rotation() const
{
	Eigen::Affine3f affine;
	affine.matrix() = matrix;
	Eigen::Matrix3f rot_ = affine.rotation();
	Quaternion q;
	q.quaternion = rot_;
	return q;
}

void Matrix4::SetMatrix(const Vector3 & position, const Vector3 & scale, const Quaternion & quaternion)
{
	Translation3f translation(position.x, position.y, position.z);
	Scaling3f scaling(scale.x, scale.y, scale.z);

	Eigen::Affine3f affine = translation * scaling * quaternion.quaternion;
	matrix = affine.matrix();
}

Matrix4 Matrix4::Inverse() const
{
	Matrix4 mat;
	mat.matrix = matrix.inverse();
	return mat;
}

Matrix4 Matrix4::Transpose() const
{
	Matrix4 mat;
	mat.matrix = matrix.transpose();
	return mat;
}

Matrix4 Matrix4::BlendMatrix(const Matrix4 & a, const Matrix4 & b, const float & weight)
{
	Vector3 position = Vector3::Lerp(a.position(), b.position(), weight);
	Vector3 scale = Vector3::Lerp(a.scale(), b.scale(), weight);
	Quaternion rotation = Quaternion::Slerp(a.rotation(), b.rotation(), weight);

	Translation3f translation(position.x, position.y, position.z);
	Scaling3f scaling(scale.x, scale.y, scale.z);

	Eigen::Affine3f affine = translation * scaling * rotation.quaternion;
	Matrix4 matrix1;
	matrix1.matrix = affine.matrix();

	return matrix1;
}



XMMATRIX4& XMMATRIX4::operator= (const XMMATRIX& m)
{
	matrix.r[0].m128_f32[0] = m.r[0].m128_f32[0];
	matrix.r[0].m128_f32[1] = m.r[0].m128_f32[1];
	matrix.r[0].m128_f32[2] = m.r[0].m128_f32[2];
	matrix.r[0].m128_f32[3] = m.r[0].m128_f32[3];

	matrix.r[1].m128_f32[0] = m.r[1].m128_f32[0];
	matrix.r[1].m128_f32[1] = m.r[1].m128_f32[1];
	matrix.r[1].m128_f32[2] = m.r[1].m128_f32[2];
	matrix.r[1].m128_f32[3] = m.r[1].m128_f32[3];

	matrix.r[2].m128_f32[0] = m.r[2].m128_f32[0];
	matrix.r[2].m128_f32[1] = m.r[2].m128_f32[1];
	matrix.r[2].m128_f32[2] = m.r[2].m128_f32[2];
	matrix.r[2].m128_f32[3] = m.r[2].m128_f32[3];

	matrix.r[3].m128_f32[0] = m.r[3].m128_f32[0];
	matrix.r[3].m128_f32[1] = m.r[3].m128_f32[1];
	matrix.r[3].m128_f32[2] = m.r[3].m128_f32[2];
	matrix.r[3].m128_f32[3] = m.r[3].m128_f32[3];

	return *this;
}

XMMATRIX4& XMMATRIX4::operator= (const Matrix4& m)
{
	matrix.r[0].m128_f32[0] = m.matrix(0,0);
	matrix.r[0].m128_f32[1] = m.matrix(1,0);
	matrix.r[0].m128_f32[2] = m.matrix(2,0);
	matrix.r[0].m128_f32[3] = m.matrix(3,0);
										 
	matrix.r[1].m128_f32[0] = m.matrix(0,1);
	matrix.r[1].m128_f32[1] = m.matrix(1,1);
	matrix.r[1].m128_f32[2] = m.matrix(2,1);
	matrix.r[1].m128_f32[3] = m.matrix(3,1);
										 
	matrix.r[2].m128_f32[0] = m.matrix(0,2);
	matrix.r[2].m128_f32[1] = m.matrix(1,2);
	matrix.r[2].m128_f32[2] = m.matrix(2,2);
	matrix.r[2].m128_f32[3] = m.matrix(3,2);
										 
	matrix.r[3].m128_f32[0] = m.matrix(0,3);
	matrix.r[3].m128_f32[1] = m.matrix(1,3);
	matrix.r[3].m128_f32[2] = m.matrix(2,3);
	matrix.r[3].m128_f32[3] = m.matrix(3,3);

	return *this;
}
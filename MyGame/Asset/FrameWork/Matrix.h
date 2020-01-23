#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <Eigen/Geometry>
#include <DirectXMath.h>

#include "Vector.h"
#include "Quaternion.h"

using namespace DirectX;

namespace FrameWork
{
	class Vector3;
	class Quaternion;
	class XMMATRIX4;

	typedef Eigen::Translation<float, 3> Translation3f;
	typedef Eigen::DiagonalMatrix<float, 3> Scaling3f;

	class Matrix4
	{
	private:
		friend cereal::access;
		template<class Archive>
		void serialize(Archive & archive)
		{
			archive(cereal::make_nvp("a1", matrix(0, 0)),cereal::make_nvp("a2", matrix(1, 0)),cereal::make_nvp("a3", matrix(2, 0)),cereal::make_nvp("a4", matrix(3, 0)),
				cereal::make_nvp("b1", matrix(0, 1)), cereal::make_nvp("b2", matrix(1, 1)), cereal::make_nvp("b3", matrix(2, 1)), cereal::make_nvp("b4", matrix(3, 1)),
				cereal::make_nvp("c1", matrix(0, 2)), cereal::make_nvp("c2", matrix(1, 2)), cereal::make_nvp("c3", matrix(2, 2)), cereal::make_nvp("c4", matrix(3, 2)),
				cereal::make_nvp("d1", matrix(0, 3)), cereal::make_nvp("d2", matrix(1, 3)), cereal::make_nvp("d3", matrix(2, 3)), cereal::make_nvp("d4", matrix(3, 3)));
		}

	public:
		typedef Eigen::Matrix<float, 4, 4, Eigen::RowMajor> _matrix;
		_matrix matrix;

		Matrix4() { this->matrix = matrix.Identity(); }
		Matrix4(const _matrix & matrix) : matrix(matrix) {}
		Matrix4(const XMMATRIX & matrix) { *this = matrix; }
		Matrix4(const XMMATRIX4 & matrix);
		Matrix4(const Vector3 & position, const Vector3 & scale, const Quaternion & quaternion) { this->SetMatrix(position, scale, quaternion); }

		Matrix4& operator= (const _matrix& m);
		Matrix4& operator= (const XMMATRIX& m);
		Matrix4& operator= (const XMMATRIX4& m);
		Matrix4& operator*= (const Matrix4& m);
		Matrix4 operator* (const Matrix4& m) const;
		Vector3 operator* (const Vector3& v) const;

		// 自身の前方を指すベクトルを取得
		Vector3 forward() const;
		// 自身の右方を指すベクトルを取得
		Vector3 right() const;
		// 自身の上方を指すベクトルを取得
		Vector3 up() const;

		Vector3 position() const;
		Vector3 scale() const;
		Quaternion rotation() const;
		
		void SetMatrix(const Vector3 & position, const Vector3 & scale = Vector3::one(), const Quaternion & quaternion = Quaternion::Identity());

		Matrix4 Inverse() const;
		Matrix4 Transpose() const;

		static Matrix4 Identity() { return (_matrix)_matrix::Identity(); }

		// ２つの行列の補間（weight : 0.0f ～ 1.0f）
		static Matrix4 BlendMatrix(const Matrix4 & a, const Matrix4 & b, const float & weight);
	private:
	};

	class XMMATRIX4
	{
	public:
		XMMATRIX matrix;

		XMMATRIX4() {};
		XMMATRIX4(const XMMATRIX& m)
		{
			matrix = m;
		};
		XMMATRIX4(const Matrix4& m)
		{
			*this = m;
		};

		XMMATRIX4& operator= (const Matrix4& m);
		XMMATRIX4& operator= (const XMMATRIX& m);
	};
}

// 定義された名前空間でやる必要がある
namespace Eigen
{
	template<class Archive>
	void serialize(Archive & archive, FrameWork::Translation3f &translation)
	{
		archive(cereal::make_nvp("x", translation.x()), cereal::make_nvp("y", translation.y()), cereal::make_nvp("z", translation.z()));
	}

	template<class Archive>
	void serialize(Archive & archive, FrameWork::Scaling3f &scaling)
	{
		archive(cereal::make_nvp("x", scaling.diagonal().x()), cereal::make_nvp("y", scaling.diagonal().y()), cereal::make_nvp("z", scaling.diagonal().z()));
	}
}

#endif // !_MATRIX_H_


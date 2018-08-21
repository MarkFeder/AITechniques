#pragma once

#include <math.h>
#include <vector>
#include "Common/Public/Misc/Utils.h"
#include "Common/Public/2D/Vector2D.h"

class C2DMatrix
{
private:

	struct Matrix
	{
		double _11, _12, _13;
		double _21, _22, _23;
		double _31, _32, _33;

		Matrix()
		{
			_11 = 0, _12 = 0, _13 = 0;
			_21 = 0, _22 = 0, _23 = 0;
			_31 = 0, _32 = 0, _33 = 0;
		}
	};

	Matrix m_Matrix;

	// Multiplies the matrix with min
	inline void MatrixMultiply(Matrix &mIn);

public:

	C2DMatrix();

	// Create an identity matrix
	inline void Identity();

	// Create a transformation matrix
	inline void Translate(double x, double y);

	// Create a scale matrix
	inline void Scale(double x, double y);

	// Create a rotation matrix
	inline void Rotate(double rotation);

	// Create a rotation matrix from a fwd and side 2D vector
	inline void Rotate(const Vector2D& fwd, const Vector2D& side);

	// Applys a transformation matrix to a std::vector of points
	inline void TransformVector2Ds(std::vector<Vector2D>& vPoints);

	// Applys a transformation matrix to a point
	inline void TransformVector2Ds(Vector2D& vPoint);

	// Accessors to the matrix elements
	void _11(double val) { m_Matrix._11 = val; }
	void _12(double val) { m_Matrix._12 = val; }
	void _13(double val) { m_Matrix._13 = val; }

	void _21(double val) { m_Matrix._21 = val; }
	void _22(double val) { m_Matrix._22 = val; }
	void _23(double val) { m_Matrix._23 = val; }
	
	void _31(double val) { m_Matrix._31 = val; }
	void _32(double val) { m_Matrix._32 = val; }
	void _33(double val) { m_Matrix._33 = val; }
};
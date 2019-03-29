#pragma once

#include <math.h>
#include <vector>
#include "Public/Misc/Utils.h"
#include "Public/2D/Vector2D.h"

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

	Matrix m_matrix;

	//----------------------- MatrixMultiply ---------------------------------------
	// Multiplies the matrix with min
	// -----------------------------------------------------------------------------

	inline void MatrixMultiply(Matrix& mIn)
	{
		Matrix mat_temp;

		// First row
		mat_temp._11 = (m_matrix._11 * mIn._11) + (m_matrix._12 * mIn._21) + (m_matrix._13 * mIn._31);
		mat_temp._12 = (m_matrix._11 * mIn._12) + (m_matrix._12 * mIn._22) + (m_matrix._13 * mIn._32);
		mat_temp._13 = (m_matrix._11 * mIn._13) + (m_matrix._12 * mIn._23) + (m_matrix._13 * mIn._33);

		// Second row										      						   	 
		mat_temp._21 = (m_matrix._21 * mIn._11) + (m_matrix._22 * mIn._21) + (m_matrix._23 * mIn._31);
		mat_temp._22 = (m_matrix._21 * mIn._12) + (m_matrix._22 * mIn._22) + (m_matrix._23 * mIn._32);
		mat_temp._23 = (m_matrix._21 * mIn._13) + (m_matrix._22 * mIn._23) + (m_matrix._23 * mIn._33);

		// Third row										      						   	 
		mat_temp._31 = (m_matrix._31 * mIn._11) + (m_matrix._32 * mIn._21) + (m_matrix._33 * mIn._31);
		mat_temp._32 = (m_matrix._31 * mIn._12) + (m_matrix._32 * mIn._22) + (m_matrix._33 * mIn._32);
		mat_temp._33 = (m_matrix._31 * mIn._13) + (m_matrix._32 * mIn._23) + (m_matrix._33 * mIn._33);

		m_matrix = mat_temp;
	}

public:

	C2DMatrix()
	{
		// Initialize the matrix to an identity matrix
		Identity();
	}

	//----------------------- Identity ---------------------------------------
	// Create an identity matrix
	// -----------------------------------------------------------------------

	inline void Identity()
	{
		m_matrix._11 = 1; m_matrix._12 = 0; m_matrix._13 = 0;
		m_matrix._21 = 0; m_matrix._22 = 1; m_matrix._23 = 0;
		m_matrix._31 = 0; m_matrix._32 = 0; m_matrix._33 = 1;
	}

	//----------------------- Translate ----------------------------------
	// Create a transformation matrix
	// -------------------------------------------------------------------

	inline void Translate(double x, double y)
	{
		Matrix mat;

		mat._11 = 1; mat._12 = 0; mat._13 = 0;
		mat._21 = 0; mat._22 = 1; mat._23 = 0;
		mat._31 = x; mat._32 = y; mat._33 = 1;

		// and multiply
		MatrixMultiply(mat);
	}

	//----------------------- Scale ----------------------------------
	// Create a scale matrix
	// ---------------------------------------------------------------

	inline void Scale(double x, double y)
	{
		Matrix mat;

		mat._11 = x; mat._12 = 0; mat._13 = 0;
		mat._21 = 0; mat._22 = y; mat._23 = 0;
		mat._31 = 0; mat._32 = 0; mat._33 = 1;

		// and multiply
		MatrixMultiply(mat);
	}

	//----------------------- Rotate ---------------------------------
	// Create a rotation matrix
	// ---------------------------------------------------------------

	inline void Rotate(double rotation)
	{
		Matrix mat;

		double sin = std::sin(rotation);
		double cos = std::cos(rotation);

		mat._11 = cos; mat._12 = sin; mat._13 = 0;
		mat._21 = -sin; mat._22 = cos; mat._23 = 0;
		mat._31 = 0; mat._32 = 0; mat._33 = 1;

		// and multiply
		MatrixMultiply(mat);
	}

	//----------------------- Rotate ---------------------------------
	// Create a rotation matrix from a fwd and side 2D vector
	// ---------------------------------------------------------------

	inline void Rotate(const Vector2D& fwd, const Vector2D& side)
	{
		Matrix mat;

		mat._11 = fwd.x; mat._12 = fwd.y; mat._13 = 0;
		mat._21 = side.x; mat._22 = side.y; mat._23 = 0;
		mat._31 = 0; mat._32 = 0; mat._33 = 1;

		// and multiply
		MatrixMultiply(mat);
	}

	//----------------------- TransformVector2Ds ---------------------------------
	// Apply a transformation matrix to a std::vector of points
	// ---------------------------------------------------------------------------

	inline void TransformVector2Ds(std::vector<Vector2D>& vPoints)
	{
		for (unsigned int i = 0; i < vPoints.size(); ++i)
		{
			double tempX = (m_matrix._11*vPoints[i].x) + (m_matrix._21*vPoints[i].y) + (m_matrix._31);

			double tempY = (m_matrix._12*vPoints[i].x) + (m_matrix._22*vPoints[i].y) + (m_matrix._32);

			vPoints[i].x = tempX;

			vPoints[i].y = tempY;

			// Would be the same to call the aux method
			// TransformVector2Ds(vPoints[i]);
		}
	}

	//----------------------- TransformVector2Ds ---------------------------------
	// Apply a transformation matrix to a point
	// ---------------------------------------------------------------------------

	inline void TransformVector2Ds(Vector2D& vPoint)
	{
		double tempX = (m_matrix._11*vPoint.x) + (m_matrix._21*vPoint.y) + (m_matrix._31);

		double tempY = (m_matrix._12*vPoint.x) + (m_matrix._22*vPoint.y) + (m_matrix._32);

		vPoint.x = tempX;

		vPoint.y = tempY;
	}

	// Accessors to the matrix elements
	void _11(double val) { m_matrix._11 = val; }
	void _12(double val) { m_matrix._12 = val; }
	void _13(double val) { m_matrix._13 = val; }

	void _21(double val) { m_matrix._21 = val; }
	void _22(double val) { m_matrix._22 = val; }
	void _23(double val) { m_matrix._23 = val; }
	
	void _31(double val) { m_matrix._31 = val; }
	void _32(double val) { m_matrix._32 = val; }
	void _33(double val) { m_matrix._33 = val; }
};
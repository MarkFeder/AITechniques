#include "Common/Public/2D/C2DMatrix.h"

inline C2DMatrix::C2DMatrix()
{
	// Initialize the matrix to an identity matrix
	Identity();
}

inline void C2DMatrix::Identity()
{
	m_Matrix._11 = 1; m_Matrix._12 = 0; m_Matrix._13 = 0;
	m_Matrix._21 = 0; m_Matrix._22 = 1; m_Matrix._23 = 0;
	m_Matrix._31 = 0; m_Matrix._32 = 0; m_Matrix._33 = 1;
}

inline void C2DMatrix::MatrixMultiply(Matrix& mIn)
{
	Matrix mat_temp;

	// First row
	mat_temp._11 = (m_Matrix._11 * mIn._11) + (m_Matrix._12 * mIn._21) + (m_Matrix._13 * mIn._31);
	mat_temp._12 = (m_Matrix._11 * mIn._12) + (m_Matrix._12 * mIn._22) + (m_Matrix._13 * mIn._32);
	mat_temp._13 = (m_Matrix._11 * mIn._13) + (m_Matrix._12 * mIn._23) + (m_Matrix._13 * mIn._33);

	// Second row										      						   	 
	mat_temp._21 = (m_Matrix._21 * mIn._11) + (m_Matrix._22 * mIn._21) + (m_Matrix._23 * mIn._31);
	mat_temp._22 = (m_Matrix._21 * mIn._12) + (m_Matrix._22 * mIn._22) + (m_Matrix._23 * mIn._32);
	mat_temp._23 = (m_Matrix._21 * mIn._13) + (m_Matrix._22 * mIn._23) + (m_Matrix._23 * mIn._33);

	// Third row										      						   	 
	mat_temp._31 = (m_Matrix._31 * mIn._11) + (m_Matrix._32 * mIn._21) + (m_Matrix._33 * mIn._31);
	mat_temp._32 = (m_Matrix._31 * mIn._12) + (m_Matrix._32 * mIn._22) + (m_Matrix._33 * mIn._32);
	mat_temp._33 = (m_Matrix._31 * mIn._13) + (m_Matrix._32 * mIn._23) + (m_Matrix._33 * mIn._33);

	m_Matrix = mat_temp;
}

inline void C2DMatrix::Translate(double x, double y)
{
	Matrix mat;

	mat._11 = 1; mat._12 = 0; mat._13 = 0;
	mat._21 = 0; mat._22 = 1; mat._23 = 0;
	mat._31 = x; mat._32 = y; mat._33 = 1;

	// and multiply
	MatrixMultiply(mat);
}


inline void C2DMatrix::Scale(double x, double y)
{
	Matrix mat;

	mat._11 = x; mat._12 = 0; mat._13 = 0;
	mat._21 = 0; mat._22 = y; mat._23 = 0;
	mat._31 = 0; mat._32 = 0; mat._33 = 1;

	// and multiply
	MatrixMultiply(mat);
}

inline void C2DMatrix::Rotate(double rotation)
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

inline void C2DMatrix::Rotate(const Vector2D& fwd, const Vector2D& side)
{
	Matrix mat;

	mat._11 = fwd.x; mat._12 = fwd.y; mat._13 = 0;
	mat._21 = side.x; mat._22 = side.y; mat._23 = 0;
	mat._31 = 0; mat._32 = 0; mat._33 = 1;

	// and multiply
	MatrixMultiply(mat);
}

inline void C2DMatrix::TransformVector2Ds(std::vector<Vector2D>& vPoints)
{
	for (unsigned int i = 0; i < vPoints.size(); ++i)
	{
		double tempX = (m_Matrix._11*vPoints[i].x) + (m_Matrix._21*vPoints[i].y) + (m_Matrix._31);

		double tempY = (m_Matrix._12*vPoints[i].x) + (m_Matrix._22*vPoints[i].y) + (m_Matrix._32);

		vPoints[i].x = tempX;

		vPoints[i].y = tempY;

		// Would be the same to call the aux method
		// TransformVector2Ds(vPoints[i]);
	}
}

inline void C2DMatrix::TransformVector2Ds(Vector2D& vPoint)
{
	double tempX = (m_Matrix._11*vPoint.x) + (m_Matrix._21*vPoint.y) + (m_Matrix._31);

	double tempY = (m_Matrix._12*vPoint.x) + (m_Matrix._22*vPoint.y) + (m_Matrix._32);

	vPoint.x = tempX;

	vPoint.y = tempY;
}

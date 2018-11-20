#include "Common/Public/2D/Vector2D.h"

//----------------------- Length -----------------------------------
// Returns the length of a 2D Vector
//------------------------------------------------------------------

inline double Vector2D::Length() const
{
	return sqrt(x * x + y * y);
}

//----------------------- LengthSq -----------------------------------
// Returns the squared length of a 2D Vector
//--------------------------------------------------------------------

inline double Vector2D::LengthSq() const
{
	return (x * x + y * y);
}

//------------------------- Normalize ------------------------------------
// Normalizes a 2D Vector
//------------------------------------------------------------------------

inline void Vector2D::Normalize()
{
	double vector_length = this->Length();

	if (vector_length > std::numeric_limits<double>::epsilon())
	{
		this->x /= vector_length;
		this->y /= vector_length;
	}
}

//----------------------- Length -----------------------------------
// Calculates the dot product
//------------------------------------------------------------------

inline double Vector2D::Dot(const Vector2D& v2) const
{
	return x*v2.x + y*v2.y;
}

//----------------------- Sign ---------------------------------------
// Returns positive if v2 is clockwise of this vector, 
// minus if anticlockwise /Y axis pointing down, x axis to right)
//--------------------------------------------------------------------

enum {clockwise = 1, anticlockwise = -1};

inline int Vector2D::Sign(const Vector2D& v2) const
{
	return (y*v2.x > x*v2.y) ? anticlockwise : clockwise;
}

//----------------------- Perp -----------------------------------
// Returns a vector perpendicular to this vector
//------------------------------------------------------------------

inline Vector2D Vector2D::Perp() const
{
	return Vector2D(-y, x);
}

//----------------------- Truncate -----------------------------------
// Truncates a vector so that its length does not exceed max
//--------------------------------------------------------------------

inline void Vector2D::Truncate(double max)
{
	if (this->Length() > max)
	{
		this->Normalize();
		*this = *this * max;
	}
}

//----------------------- Distance -----------------------------------
// Calculates the euclidean distance between two vectors
//--------------------------------------------------------------------

inline double Vector2D::Distance(const Vector2D& v2) const
{
	double xSeparation = v2.x - x;
	double ySeparation = v2.y - y;


	return sqrt(ySeparation * ySeparation + xSeparation * xSeparation);
}

//----------------------- DistanceSq -----------------------------------
// Calculates the euclidean distance squared between two vectors
//----------------------------------------------------------------------

inline double Vector2D::DistanceSq(const Vector2D& v2) const
{
	double xSeparation = v2.x - x;
	double ySeparation = v2.y - y;


	return (ySeparation * ySeparation + xSeparation * xSeparation);
}

//----------------------- Reflect -----------------------------------
// Given a normalized vector this method reflects the vector it
// is operating upon. (like the path of a ball bouncing off a wall)
//-------------------------------------------------------------------

inline void Vector2D::Reflect(const Vector2D& norm)
{
	*this += 2.0 * this->Dot(norm) * norm.GetReverse();
}

//----------------------- GetReverse ----------------------------------------
// Returns the vector that is the reverse of this vector
//---------------------------------------------------------------------------

inline Vector2D Vector2D::GetReverse() const
{
	return Vector2D(-this->x, -this->y);
}

//--------------------- Basic Operator overloads ----------------------------

const Vector2D& Vector2D::operator+=(const Vector2D& rhs)
{
	x += rhs.x;
	y += rhs.y;

	return *this;
}

const Vector2D& Vector2D::operator-=(const Vector2D& rhs)
{
	x -= rhs.x;
	y -= rhs.y;

	return *this;
}

const Vector2D& Vector2D::operator*=(const double& rhs)
{
	x *= rhs;
	y *= rhs;

	return *this;
}

const Vector2D& Vector2D::operator/=(const double& rhs)
{
	x /= rhs;
	y /= rhs;

	return *this;
}

const Vector2D& Vector2D::operator/=(const Vector2D& rhs)
{
	x /= rhs.x;
	y /= rhs.y;

	return *this;
}

const Vector2D& Vector2D::operator*=(const Vector2D& rhs)
{
	x *= rhs.x;
	y *= rhs.y;

	return *this;
}

bool Vector2D::operator==(const Vector2D& rhs)
{
	return IsEqual(x, rhs.x) && IsEqual(y, rhs.y);
}

bool Vector2D::operator!=(const Vector2D& rhs)
{
	return (x != rhs.x) && (y != rhs.y);
}

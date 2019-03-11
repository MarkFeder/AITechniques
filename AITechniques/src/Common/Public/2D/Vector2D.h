#pragma once

#include <math.h>
#include <windows.h>
#include <iosfwd>
#include <limits>
#include <fstream>

#include "Common/Public/Misc/Utils.h"

struct Vector2D
{
	double x;
	double y;

	Vector2D() :x(0.0), y(0.0) {}
	Vector2D(double a, double b) :x(a), y(b) {}

	// Sets x and y to zero
	void Zero() { x = 0.0; y = 0.0; }
	
	// Returns true if both x and y are zero
	bool IsZero() const { return (x*x + y*y) < MinDouble; }

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

	//----------------------- Dot -----------------------------------
	// Calculates the dot product
	//---------------------------------------------------------------

	inline double Vector2D::Dot(const Vector2D& v2) const
	{
		return x * v2.x + y * v2.y;
	}

	//----------------------- Sign ---------------------------------------
	// Returns positive if v2 is clockwise of this vector, 
	// minus if anticlockwise /Y axis pointing down, x axis to right)
	//--------------------------------------------------------------------

	enum { clockwise = 1, anticlockwise = -1 };

	inline int Vector2D::Sign(const Vector2D& v2) const
	{
		return (y*v2.x > x*v2.y) ? anticlockwise : clockwise;
	}

	//----------------------- Perp -----------------------------------
	// Returns a vector perpendicular to this vector
	//----------------------------------------------------------------

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

	// We need some basic operator overloads
	const Vector2D& operator+=(const Vector2D& rhs);
	const Vector2D& operator-=(const Vector2D& rhs);
	const Vector2D& operator*=(const double& rhs);
	const Vector2D& operator/=(const double& rhs);
	const Vector2D& operator/=(const Vector2D& rhs);
	const Vector2D& operator*=(const Vector2D& rhs);

	bool operator==(const Vector2D& rhs);
	bool operator!=(const Vector2D& rhs);

	// More operator overloads
	friend inline Vector2D operator*(const Vector2D& lhs, double rhs)
	{
		Vector2D result(lhs);
		result *= rhs;

		return result;
	}

	friend inline Vector2D operator*(double lhs, const Vector2D& rhs)
	{
		Vector2D result(rhs);
		result *= lhs;

		return result;
	}

	friend inline Vector2D operator-(const Vector2D& lhs, const Vector2D& rhs)
	{
		Vector2D result(lhs);
		result.x -= rhs.x;
		result.y -= rhs.y;

		return result;
	}

	friend inline Vector2D operator+(const Vector2D& lhs, const Vector2D& rhs)
	{
		Vector2D result(lhs);
		result.x += rhs.x;
		result.y += rhs.y;

		return result;
	}

	friend inline Vector2D operator/(const Vector2D& lhs, double val)
	{
		Vector2D result(lhs);
		result.x /= val;
		result.y /= val;

		return result;
	}
	
	// Vector2D friend functions

	friend inline Vector2D Vec2DNormalize(const Vector2D& v)
	{
		Vector2D vec = v;

		double vLength = vec.Length();

		if (vLength > std::numeric_limits<double>::epsilon())
		{
			vec.x /= vLength;
			vec.y /= vLength;
		}

		return vec;
	}

	friend inline void Vec2DNormalizeInPlace(Vector2D& v)
	{
		double vLength = v.Length();

		if (vLength > std::numeric_limits<double>::epsilon())
		{
			v.x /= vLength;
			v.y /= vLength;
		}
	}
	
	friend inline double Vec2DDistance(const Vector2D& v1, const Vector2D& v2)
	{
		double ySeparation = v2.y - v1.y;
		double xSeparation = v2.x - v1.x;

		return sqrt(ySeparation * ySeparation + xSeparation * xSeparation);
	}

	friend inline double Vec2DDistanceSq(const Vector2D& v1, const Vector2D& v2)
	{
		double ySeparation = v2.y - v1.y;
		double xSeparation = v2.x - v1.x;

		return ySeparation * ySeparation + xSeparation * xSeparation;
	}

	friend inline Vector2D POINTtoVector(const POINT& p)
	{
		return Vector2D(p.x, p.y);
	}

	friend inline Vector2D POINTStoVector(const POINTS& p)
	{
		return Vector2D((double)p.x, (double)p.y);
	}

	friend inline POINT VectorToPOINT(const Vector2D& v)
	{
		POINT p;
		p.x = (long)v.x;
		p.y = (long)v.y;

		return p;
	}

	friend inline POINTS VectorToPOINTS(const Vector2D& v)
	{
		POINTS p;
		p.x = (short)v.x;
		p.y = (short)v.y;

		return p;
	}

	friend inline std::ostream& operator<<(std::ostream& os, const Vector2D& rhs)
	{
		os << " " << rhs.x << " " << rhs.y;

		return os;
	}

	//std::ifstream& operator>>(std::ifstream& is, Vector2D& lhs)
	//{
	//	is >> lhs.x >> lhs.y;
	//
	//	return is;
	//}
};

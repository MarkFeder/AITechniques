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

	// Returns the length of the vector
	inline double Length() const;

	// Returns the squared length of the vector (thereby avoiding the sqrt)
	inline double LengthSq() const;

	// Returns the normalized vector
	inline void Normalize();

	// Returns the dot product
	inline double Dot(const Vector2D& v2) const;

	// Returns positive if v2 is clockwise of this vector,
	// negative if anticlockwise (assuming the Y axis is pointing down, 
	// X axis to right like a Window app)
	inline int Sign(const Vector2D& v2) const;

	// Returns the vector that is perpendicular to this one
	inline Vector2D Perp() const;

	// Adjusts x and y so that the length of the vector does not exceed max
	inline void Truncate(double max);

	// Returns the distance between this vector and the one passed as a parameter
	inline double Distance(const Vector2D &v2) const;

	// Squared version of the above
	inline double DistanceSq(const Vector2D &v2) const;

	inline void Reflect(const Vector2D& norm);

	// Returns the vector that is the reverse of this vector
	inline Vector2D GetReverse() const;

	// We need some basic operator overloads
	const Vector2D& operator+=(const Vector2D& rhs);
	const Vector2D& operator-=(const Vector2D& rhs);
	const Vector2D& operator*=(const double& rhs);
	const Vector2D& operator/=(const double& rhs);
	const Vector2D& operator/=(const Vector2D& rhs);
	const Vector2D& operator*=(const Vector2D& rhs);

	bool operator==(const Vector2D& rhs);
	bool operator!=(const Vector2D& rhs);

	// Some more operator overloads
	friend inline Vector2D operator*(const Vector2D& lhs, double rhs);
	friend inline Vector2D operator*(double lhs, const Vector2D& rhs);
	friend inline Vector2D operator-(const Vector2D& lhs, const Vector2D& rhs);
	friend inline Vector2D operator+(const Vector2D& lhs, const Vector2D& rhs);
	friend inline Vector2D operator/(const Vector2D& lhs, double val);
	
	friend inline Vector2D Vec2DNormalize(const Vector2D &v);
	
	friend inline double Vec2DDistance(const Vector2D &v1, const Vector2D& v2);
	friend inline double Vec2DDistanceSq(const Vector2D& v1, const Vector2D& v2);

	friend inline Vector2D POINTtoVector(const POINT& p);
	friend inline Vector2D POINTStoVector(const POINTS& p);
	friend inline POINT VectorToPOINT(const Vector2D& v);
	friend inline POINTS VectorToPOINTS(const Vector2D& v);

	friend inline std::ostream& operator<<(std::ostream& os, const Vector2D& rhs);
	//friend inline std::ifstream& operator>>(std::ifstream& is, Vector2D& lhs);
};

//--------------------- More Operator overloads ----------------------------

inline Vector2D operator*(const Vector2D& lhs, double rhs)
{
	Vector2D result(lhs);
	result *= rhs;

	return result;
}

inline Vector2D operator*(double lhs, const Vector2D& rhs)
{
	Vector2D result(rhs);
	result *= lhs;

	return result;
}

inline Vector2D operator-(const Vector2D& lhs, const Vector2D& rhs)
{
	Vector2D result(lhs);
	result.x -= rhs.x;
	result.y -= rhs.y;

	return result;
}

inline Vector2D operator+(const Vector2D& lhs, const Vector2D& rhs)
{
	Vector2D result(lhs);
	result.x += rhs.x;
	result.y += rhs.y;

	return result;
}

inline Vector2D operator/(const Vector2D& lhs, double val)
{
	Vector2D result(lhs);
	result.x /= val;
	result.y /= val;

	return result;
}

std::ostream& operator<<(std::ostream& os, const Vector2D& rhs)
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

//--------------------- Vector2D Friend Functions ----------------------------

inline Vector2D Vec2DNormalize(const Vector2D& v)
{
	Vector2D vec = v;

	double vector_length = vec.Length();

	if (vector_length > std::numeric_limits<double>::epsilon())
	{
		vec.x /= vector_length;
		vec.y /= vector_length;
	}

	return vec;
}

inline double Vec2DDistance(const Vector2D& v1, const Vector2D& v2)
{
	double ySeparation = v2.y - v1.y;
	double xSeparation = v2.x - v1.x;

	return sqrt(ySeparation * ySeparation + xSeparation * xSeparation);
}

inline double Vec2DDistanceSq(const Vector2D& v1, const Vector2D& v2)
{
	double ySeparation = v2.y - v1.y;
	double xSeparation = v2.x - v1.x;

	return ySeparation * ySeparation + xSeparation * xSeparation;
}

inline Vector2D POINTtoVector(const POINT& p)
{
	return Vector2D(p.x, p.y);
}

inline Vector2D POINTStoVector(const POINTS& p)
{
	return Vector2D((double)p.x, (double)p.y);
}

inline POINT VectorToPOINT(const Vector2D& v)
{
	POINT p;
	p.x = (long)v.x;
	p.y = (long)v.y;

	return p;
}

inline POINTS VectorToPOINTS(const Vector2D& v)
{
	POINTS p;
	p.x = (short)v.x;
	p.y = (short)v.y;

	return p;
}

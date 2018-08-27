#pragma once

#include <math.h>
#include <windows.h>
#include <iosfwd>
#include <limits>

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

	inline void Normalize();

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

	friend std::ostream& operator<<(std::ostream& os, const Vector2D& rhs);
	friend std::ifstream& operator>>(std::ifstream& is, Vector2D& lhs);
};
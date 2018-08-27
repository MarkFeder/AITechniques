#include "Common/Public/2D/Vector2D.h"
#include <fstream>

// Member operations

inline double Vector2D::Length() const
{
	return sqrt(x * x + y * y);
}

inline double Vector2D::LengthSq() const
{
	return (x * x + y * y);
}

inline void Vector2D::Normalize()
{
	double vector_length = this->Length();

	if (vector_length > std::numeric_limits<double>::epsilon())
	{
		this->x /= vector_length;
		this->y /= vector_length;
	}
}

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

inline double Vector2D::Dot(const Vector2D& v2) const
{
	return x*v2.x + y*v2.y;
}

// Returns positive if v2 is clockwise of this vector, 
// minus if anticlockwise /Y axis pointing down, x axis to right)
enum {clockwise = 1, anticlockwise = -1};

inline int Vector2D::Sign(const Vector2D& v2) const
{
	return (y*v2.x > x*v2.y) ? anticlockwise : clockwise;
}

inline Vector2D Vector2D::Perp() const
{
	return Vector2D(-y, x);
}

inline void Vector2D::Truncate(double max)
{
	if (this->Length() > max)
	{
		this->Normalize();
		*this = *this * max;
	}
}

inline double Vector2D::Distance(const Vector2D& v2) const
{
	double xSeparation = v2.x - x;
	double ySeparation = v2.y - y;


	return sqrt(ySeparation * ySeparation + xSeparation * xSeparation);
}

inline double Vector2D::DistanceSq(const Vector2D& v2) const
{
	double xSeparation = v2.x - x;
	double ySeparation = v2.y - y;


	return (ySeparation * ySeparation + xSeparation * xSeparation);
}

inline void Vector2D::Reflect(const Vector2D& norm)
{
	*this += 2.0 * this->Dot(norm) * norm.GetReverse();
}

inline Vector2D Vector2D::GetReverse() const
{
	return Vector2D(-this->x, -this->y);
}

// Basic operator overloads

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

// More operator overloads

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

std::ifstream& operator>>(std::ifstream& is, Vector2D& lhs)
{
	is >> lhs.x >> lhs.y;

	return is;
}

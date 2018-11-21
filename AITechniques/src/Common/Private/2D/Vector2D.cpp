#include "Common/Public/2D/Vector2D.h"

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

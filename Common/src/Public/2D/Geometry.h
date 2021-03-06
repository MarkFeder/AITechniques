#pragma once

#include "Public/Misc/Utils.h"
#include "Public/2D/Vector2D.h"
#include "Public/2D/C2DMatrix.h"
#include "Transformations.h"

#include <math.h>
#include <vector>

const double PI = 3.14159;

// --------------------- DistanceToRayPlaneIntersection ---------------------------------
// Given a plane and a ray this function determins how far along the ray an intersection
// occurs. Returns a negative if the ray is parallel
// --------------------------------------------------------------------------------------

inline double DistanceToRayPlaneIntersection(Vector2D rayOrigin, Vector2D rayHeading, Vector2D planePoint, Vector2D planeNormal)
{
	double d = -planeNormal.Dot(planePoint);
	double numer = planeNormal.Dot(rayOrigin) + d;
	double denom = planeNormal.Dot(rayHeading);

	// normal is parallel to vector
	return ((denom < 0.000001) && (denom > -0.000001)) ? -1.0 : -(numer / denom);
}

// --------------------- DistToLineSegment  ---------------------------------
//  Given a line segment AB and a point P, this function calculates the 
//  perpendicular distance between them
//---------------------------------------------------------------------------

inline double DistToLineSegment(Vector2D a, Vector2D b, Vector2D p)
{
	// If the angle is obtuse between PA and AB (is obtuse when the closest
	// vertex must be A)
	double dotA = (p.x - a.x) * (b.x - a.x) + (p.y - a.y)* (b.y - a.y);

	if (dotA <= 0) return Vec2DDistance(a, p);

	// If the angle is obtuse between PB and AB (is obstuse when the closest
	// vertext must be B
	double dotB = (p.x - b.x) * (a.x - b.x) + (p.y - b.y) * (a.y - b.y);

	if (dotB <= 0) return Vec2DDistance(b, p);

	// Calculate the point along AB that is the closest to P
	Vector2D point = a + ((b - a) * dotA) / (dotA + dotB);

	// Calculate the distance P-Point
	return Vec2DDistance(p, point);
}

// --------------------- LinesIntersection2D  ---------------------------------
// Given 2 lines in 2D space AB, CD this returns true if an intersection occurs
//-----------------------------------------------------------------------------

inline bool LineIntersection2D(const Vector2D& a, const Vector2D& b, const Vector2D& c, const Vector2D& d)
{
	double rTop = (a.y - c.y) * (d.x - c.x) - (a.x - c.x) * (d.y - c.y);
	double sTop = (a.y - c.y) * (b.x - a.x) - (a.x - c.x) * (b.y - a.y);

	double bot = (b.x - a.x) * (d.y - c.y) - (b.y - a.y) * (d.x - c.x);

	if (bot == 0) // parallel
	{
		return false;
	}

	double r = rTop / bot;
	double s = sTop / bot;

	if ((r > 0) && (r < 1) && (s > 0) && (s < 1))
	{
		// Lines intersect
		return true;
	}

	// Lines do not intersect
	return false;
}

// --------------------- LinesIntersection2D  ---------------------------------
// Given 2 lines in 2D space AB, CD this returns true if an intersection occurs
// and sets dist to the distance the intersection occurs along AB
//-----------------------------------------------------------------------------

inline bool LineIntersection2D(const Vector2D& a, const Vector2D& b, const Vector2D& c, const Vector2D& d, double& dist)
{
	double rTop = (a.y - c.y) * (d.x - c.x) - (a.x - c.x) * (d.y - c.y);
	double sTop = (a.y - c.y) * (b.x - a.x) - (a.x - c.x) * (b.y - a.y);

	double bot = (b.x - a.x) * (d.y - c.y) - (b.y - a.y) * (d.x - c.x);
	
	// Check if rects are parallel
	if (bot == 0)
	{
		if (IsEqual(rTop, 0) && IsEqual(sTop, 0))
		{
			return true;
		}

		return false;
	}

	double r = rTop / bot;
	double s = sTop / bot;

	if ((r > 0) && (r < 1) && (s > 0) && (s < 1))
	{
		dist = Vec2DDistance(a, b) * r;
		return true;
	}
	else
	{
		dist = 0;
		return false;
	}
}

// --------------------- LinesIntersection2D  ---------------------------------
// Given 2 lines in 2D space AB, CD this returns true if an intersection occurs
// and sets dist to the distance the intersection occurs along AB. Also sets the
// 2d vector point to the point of intersection
//-----------------------------------------------------------------------------

inline bool LineIntersection2D(const Vector2D& a, const Vector2D& b, const Vector2D& c,const  Vector2D& d, double& dist, Vector2D& point)
{
	double rTop = (a.y - c.y) * (d.x - c.x) - (a.x - c.x) * (d.y - c.y);
	double rBot = (b.x - a.x) * (d.y - c.y) - (b.y - a.y) * (d.x - c.x);

	double sTop = (a.y - c.y) * (b.x - a.x) - (a.x - c.x) * (b.y - a.y);
	double sBot = (b.x - a.x) * (d.y - c.y) - (b.y - a.y) * (d.x - c.x);

	if ((rBot == 0) || (sBot == 0))
	{
		// lines are parallel
		return false;
	}

	double r = rTop / rBot;
	double s = sTop / sBot;

	if ((r > 0) && (r < 1) && (s > 0) && (s < 1))
	{
		dist = Vec2DDistance(a, b) * r;
		point = a + r * (b - a);
		return true;
	}
	else
	{
		dist = 0;
		return false;
	}
}

// --------------------- WhereIsPoint -----------------------------------------
// Check whether a 2D point is or not on the backside, 
// front or on a given plane
//-----------------------------------------------------------------------------

enum span_type { ST_Backside, ST_Front, ST_OnPlane };
inline span_type WhereIsPoint(Vector2D point, Vector2D pointOnPlane, Vector2D planeNormal)
{
	Vector2D dir = pointOnPlane - point;

	double d = dir.Dot(planeNormal);

	if (d < -0.000001)
	{
		return ST_Front;
	}
	else if (d > 0.000001)
	{
		return ST_Backside;
	}

	return ST_OnPlane;
}

// --------------------- GetRayCircleIntersec -----------------------------------
// Check whether a ray intersects with a circle with a given radius
//-------------------------------------------------------------------------------

inline double GetRayCircleIntersect(Vector2D rayOrigin, Vector2D rayHeading, Vector2D circleOrigin, double radius)
{
	Vector2D toCircle = circleOrigin - rayOrigin;
	double length = toCircle.Length();
	double v = toCircle.Dot(rayHeading);
	double d = radius * radius - (length * length* - v * v);

	// If there was no intersection, return -1
	if (d < 0.0) return (-1.0);

	// Return the distance to the [first] intersecting point
	return (v - sqrt(d));
}

//----------------------------- DoRayCircleIntersect --------------------------

inline bool DoRayCircleIntersect(Vector2D rayOrigin, Vector2D rayHeading, Vector2D circleOrigin, double radius)
{
	Vector2D toCircle = circleOrigin - rayOrigin;
	double length = toCircle.Length();
	double v = toCircle.Dot(rayHeading);
	double d = radius * radius - ((length * length) * (-v * v));

	// If there was no intersection, return -1
	return (d < 0.0);
}

//----------------------------- GetTangentPoints  -----------------------------
// Given a point P and a circle of radius R centered at C this function 
// determines the two points on the circle that intersect with the tangents from
// P to the circle. Returns false if P is within the circle.
//-----------------------------------------------------------------------------

inline bool GetTangentPoints(Vector2D circle, double radius, Vector2D point, Vector2D& t1, Vector2D& t2)
{
	Vector2D PC = point - circle;
	double sqrLen = PC.LengthSq();
	double rSqr = radius * radius;

	if (sqrLen <= rSqr)
	{
		// Point is inside or on the circle
		return false;
	}

	double invSqrLen = 1 / sqrLen;
	double root = sqrt(fabs(sqrLen - rSqr));

	t1.x = circle.x + radius * (radius * PC.x - PC.y * root) * invSqrLen;
	t1.y = circle.y + radius * (radius * PC.y + PC.x * root) * invSqrLen;

	t2.x = circle.x + radius * (radius * PC.x + PC.y * root) * invSqrLen;
	t1.y = circle.y + radius * (radius * PC.y - PC.x * root) * invSqrLen;

	return true;
}

//----------------------------- TwoCirclesOverlapped -----------------------------
// Returns true if the two circles overlap
//--------------------------------------------------------------------------------

inline bool TwoCirclesOverlapped(double x1, double y1, double r1, double x2, double y2, double r2)
{
	double distBetweenCenters = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

	return (distBetweenCenters < (r1 + r2)) || (distBetweenCenters < fabs(r1 - r2)) ? true : false;
}

//----------------------------- TwoCirclesOverlapped -----------------------------
// Returns true if the two circles overlap
//--------------------------------------------------------------------------------

inline bool TwoCirclesOverlapped(Vector2D c1, double r1, Vector2D c2, double r2)
{
	double distBetweenCenters = sqrt((c1.x - c2.x) * (c1.x - c2.x) + (c1.y - c2.y) * (c1.y - c2.y));

	return (distBetweenCenters < (r1 + r2)) || (distBetweenCenters < fabs(r1 - r2)) ? true : false;
}

//----------------------- PointInCircle ----------------------------------
//  Returns true if the point p is within the radius of the given circle
//------------------------------------------------------------------------
inline bool PointInCircle(Vector2D pos, double radius, Vector2D p)
{
	double distFromCenterSquared = (p - pos).LengthSq();

	return (distFromCenterSquared < (radius*radius)) ? true : false;
}
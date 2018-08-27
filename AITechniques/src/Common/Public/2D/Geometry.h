#pragma once

#include "Common/Public/Misc/Utils.h"
#include "Common/Public/2D/Vector2D.h"
#include "Common/Public/2D/C2DMatrix.h"
#include "Transformations.h"

#include <math.h>
#include <vector>

const double PI = 3.14159;

// --------------------- DistanceToRayPlaneIntersection ---------------------------------
// Given a plane and a ray this function determins how far along the ray an intersection
// occurs. Returns a negative if the ray is parallel
// --------------------------------------------------------------------------------------

inline double DistanceToRayPlaneIntersection(
	Vector2D rayOrigin,
	Vector2D rayHeading,
	Vector2D planePoint,
	Vector2D planeNormal)
{
	double d = -planeNormal.Dot(planePoint);
	double numer = planeNormal.Dot(rayOrigin) + d;
	double denom = planeNormal.Dot(rayHeading);

	// normal is parallel to vector
	return ((denom < 0.000001) && (denom > -0.000001)) ? -1.0 : -(numer / denom);
}

// --------------------- DistToLineSegment  ---------------------------------
//
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

// --------------------- WhereIsPoint -----------------------------------
// Check whether a 2D point is or not on the backside, 
// front or on a given plane
//-----------------------------------------------------------------------

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

inline double GetRayCircleIntersect(
	Vector2D rayOrigin,
	Vector2D rayHeading,
	Vector2D circleOrigin,
	double radius)
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

inline bool DoRayCircleIntersect(
	Vector2D rayOrigin,
	Vector2D rayHeading,
	Vector2D circleOrigin,
	double radius)
{
	Vector2D toCircle = circleOrigin - rayOrigin;
	double length = toCircle.Length();
	double v = toCircle.Dot(rayHeading);
	double d = radius * radius - (length * length* -v * v);

	// If there was no intersection, return -1
	return (d < 0.0);
}
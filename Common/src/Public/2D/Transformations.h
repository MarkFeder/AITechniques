#pragma once

#include <vector>

#include "Public/2D/Vector2D.h"
#include "Public/2D/C2DMatrix.h"

// ----------------------------- WorldTransform -----------------------------
// given a std::vector of 2D vectors, a position, orientation and scale,
// this function transforms the 2D vectors into the object's world space
// --------------------------------------------------------------------------

inline std::vector<Vector2D> WorldTransform(const std::vector<Vector2D>& points, 
	const Vector2D& pos, 
	const Vector2D& forward, 
	const Vector2D& side, 
	const Vector2D& scale)
{
	// Copy the original vertices into the buffer about to be transformed
	std::vector<Vector2D> tranVector2Ds = points;

	// Create a transform matrix
	C2DMatrix matTransform;

	// Scale
	if ((scale.x != 1.0f) || (scale.y != 1.0f))
	{
		matTransform.Scale(scale.x, scale.y);
	}

	// Rotate
	matTransform.Rotate(forward, side);

	// Translate
	matTransform.Translate(pos.x, pos.y);

	// Now transforms the object's vertices
	matTransform.TransformVector2Ds(tranVector2Ds);

	return tranVector2Ds;
}

inline void InPlaceWorldTransform(std::vector<Vector2D>& points,
	const Vector2D& pos,
	const Vector2D& forward,
	const Vector2D& side,
	const Vector2D& scale)
{
	// Create a transform matrix
	C2DMatrix matTransform;

	// Scale
	if ((scale.x != 1.0f) || (scale.y != 1.0f))
	{
		matTransform.Scale(scale.x, scale.y);
	}

	// Rotate
	matTransform.Rotate(forward, side);

	// Translate
	matTransform.Translate(pos.x, pos.y);

	// Now transforms the object's vertices
	matTransform.TransformVector2Ds(points);
}

// ----------------------------- WorldTransform -----------------------------
// given a std::vector of 2D vectors, a position and orientation,
// this function transforms the 2D vectors into the object's world space
// --------------------------------------------------------------------------

inline std::vector<Vector2D> WorldTransform(const std::vector<Vector2D>& points,
	const Vector2D& pos,
	const Vector2D& forward,
	const Vector2D& side)
{
	// Copy the original vertices into the buffer about to be transformed
	std::vector<Vector2D> tranVector2Ds = points;

	// Create a transformation matrix
	C2DMatrix matTransform;

	// Rotate
	matTransform.Rotate(forward, side);

	// Translate
	matTransform.Translate(pos.x, pos.y);

	// Now transforms the object's vertices
	matTransform.TransformVector2Ds(tranVector2Ds);

	return tranVector2Ds;
}

inline void InPlaceWorldTransform(std::vector<Vector2D>& points,
	const Vector2D& pos,
	const Vector2D& forward,
	const Vector2D& side)
{
	// Create a transformation matrix
	C2DMatrix matTransform;

	// Rotate
	matTransform.Rotate(forward, side);

	// Translate
	matTransform.Translate(pos.x, pos.y);

	// Now transforms the object's vertices
	matTransform.TransformVector2Ds(points);
}

// ----------------------------- PointToWorldSpace --------------------------
// Transform a point from the agent's local space into world space
// --------------------------------------------------------------------------

inline Vector2D PointToWorldSpace(const Vector2D& point,
	const Vector2D& agentHeading,
	const Vector2D& agentSide,
	const Vector2D& agentPosition)
{
	// Make a copy of the point
	Vector2D transPoint = point;

	// Create a transformation matrix
	C2DMatrix matTransform;

	// Rotate
	matTransform.Rotate(agentHeading, agentSide);

	// Translate
	matTransform.Translate(agentPosition.x, agentPosition.y);

	// Now transform the vertice
	matTransform.TransformVector2Ds(transPoint);

	return transPoint;
}

inline void InPlacePointToWorldSpace(Vector2D& point,
	const Vector2D& agentHeading,
	const Vector2D& agentSide,
	const Vector2D& agentPosition)
{
	// Create a transformation matrix
	C2DMatrix matTransform;

	// Rotate
	matTransform.Rotate(agentHeading, agentSide);

	// Translate
	matTransform.Translate(agentPosition.x, agentPosition.y);

	// Now transform the input vertice
	matTransform.TransformVector2Ds(point);
}

// ----------------------------- VectorToWorldSpace  ------------------------
// Transforms a vector from the agent's local space into world space
// --------------------------------------------------------------------------

inline Vector2D VectorToWorldSpace(const Vector2D& vec,
	const Vector2D& agentHeading,
	const Vector2D& agentSide)
{
	// Make a copy of the point
	Vector2D transVec = vec;

	// Create a transformation matrix
	C2DMatrix matTransform;

	// Rotate
	matTransform.Rotate(agentHeading, agentSide);

	// Now transform the vertices
	matTransform.TransformVector2Ds(transVec);

	return transVec;
}

inline void InPlaceVectorToWorldSpace(Vector2D& vec,
	const Vector2D& agentHeading,
	const Vector2D& agentSide)
{
	// Create a transformation matrix
	C2DMatrix matTransform;

	// Rotate
	matTransform.Rotate(agentHeading, agentSide);

	// Now transform the vertices
	matTransform.TransformVector2Ds(vec);
}

// --------------------- PointToLocalSpace ----------------------------------
//	Transforms a point from world space into agent's local space
// --------------------------------------------------------------------------

inline Vector2D PointToLocalSpace(const Vector2D& point,
	Vector2D& agentHeading,
	Vector2D& agentSide,
	Vector2D& agentPosition)
{
	// Make a copy of the point
	Vector2D transPoint = point;

	// Create a transformation matrix
	C2DMatrix matTransform;

	double Tx = -agentPosition.Dot(agentHeading);
	double Ty = -agentPosition.Dot(agentSide);

	// Create the transformation matrix
	matTransform._11(agentHeading.x); matTransform._12(agentSide.x);
	matTransform._21(agentHeading.y); matTransform._22(agentSide.y);
	matTransform._31(Tx); matTransform._32(Ty);

	// Now transform the vertices
	matTransform.TransformVector2Ds(transPoint);

	return transPoint;
}

// --------------------- VectorToLocalSpace ---------------------------------
// Transforms a vector from world space into agent's local space
// --------------------------------------------------------------------------

inline Vector2D VectorToLocalSpace(const Vector2D& vec,
	const Vector2D& agentHeading,
	const Vector2D& agentSide)
{
	// Make a copy of the vector
	Vector2D transPoint = vec;

	// Create a transformation matrix
	C2DMatrix matTrasform;

	// Create the transformation matrix
	matTrasform._11(agentHeading.x); matTrasform._12(agentSide.x);
	matTrasform._21(agentHeading.y); matTrasform._22(agentSide.y);

	// now transform the vertices
	matTrasform.TransformVector2Ds(transPoint);

	return transPoint;
}

// --------------------- Vec2DRotateAroundOrigin  ---------------------------------
// Rotates a vector ang rads around the origin
// --------------------------------------------------------------------------------

inline void Vec2DRotateAroundOrigin(Vector2D& v, double ang)
{
	// Create a transformation matrix
	C2DMatrix matTransform;

	// Rotate
	matTransform.Rotate(ang);

	// Now transform the object's vertices
	matTransform.TransformVector2Ds(v);
}

//------------------------ CreateWhiskers ------------------------------------
//
//  Given an origin, a facing direction, a 'field of view' describing the 
//  limit of the outer whiskers, a whisker length and the number of whiskers
//  this method returns a vector containing the end positions of a series
//  of whiskers radiating away from the origin and with equal distance between
//  them. (like the spokes of a wheel clipped to a specific segment size)
//----------------------------------------------------------------------------

inline std::vector<Vector2D> CreateWhiskers(
	unsigned int numWhiskers,
	double whiskerLength,
	double fov,
	Vector2D facing,
	Vector2D origin)
{
	// This is the magnitude of the angle separating each whisker
	double sectorSize = fov / (double)(numWhiskers - 1);

	std::vector<Vector2D> whiskers;
	Vector2D temp;
	double angle = -fov * 0.5;

	for (unsigned int w = 0; w < numWhiskers; ++w)
	{
		// Create the whisker extending outwards at this angle
		temp = facing;
		Vec2DRotateAroundOrigin(temp, angle);
		whiskers.push_back(origin + whiskerLength * temp);

		angle += sectorSize;
	}

	return whiskers;
}
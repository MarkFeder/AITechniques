#include <cassert>
#include "Common/Public/Entities/MovingEntity.h"
#include "Common/Public/2D/Vector2D.h"

inline void MovingEntity::SetHeading(Vector2D newHeading)
{
	assert((newHeading.LengthSq() - 1.0) < 0.00001);

	m_vHeading = newHeading;

	// The side vector must always be perpendicular to the heading
	m_vSide = m_vHeading.Perp();
}

inline bool MovingEntity::RotateHeadingToFacePosition(Vector2D target)
{
	Vector2D toTarget = Vec2DNormalize(target - m_vPos);

	// First determine the angle between the heading vector and the target
	double angle = acos(m_vHeading.Dot(toTarget));
}

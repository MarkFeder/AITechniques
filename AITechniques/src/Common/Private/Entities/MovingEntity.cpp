#include <cassert>

#include "Common/Public/Entities/MovingEntity.h"
#include "Common/Public/2D/Vector2D.h"
#include "Common/Public/2D/C2DMatrix.h"

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

	// Return true if the player is facing the target
	if (angle < 0.00001) 
		return true;

	// Clamp the amount to turn to the max turn rate
	if (angle > m_dMaxTurnRate)
		angle = m_dMaxTurnRate;

	// The next few lines use a rotation matrix to rotate the player's
	// heading vector accordingly
	C2DMatrix rotationMatrix;

	// Notice how the direction of rotation has to be detemrined when creating
	// the rotation matrix
	rotationMatrix.Rotate(angle * m_vHeading.Sign(toTarget));
	rotationMatrix.TransformVector2Ds(m_vHeading);
	rotationMatrix.TransformVector2Ds(m_vVelocity);

	// Finally recreate m_vSide
	m_vSide = m_vHeading.Perp();

	return false;
}

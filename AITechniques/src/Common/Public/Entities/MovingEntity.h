#pragma once

#include <cassert>

#include "Common/Public/2D/Vector2D.h"
#include "BaseGameEntity.h"

class MovingEntity : public BaseGameEntity
{
protected:

	// A vector that represents the entity's velocity
	Vector2D m_vVelocity;

	// A normalized vector pointing in the direction the entity is heading
	Vector2D m_vHeading;

	// A vector which is perpendicular to the heading vector
	Vector2D m_vSide;

	// The mass of this entity
	double m_dMass;

	// The maximum speed this entity may travel at
	double m_dMaxSpeed;

	// The maximum force this entity can produce to power itself
	double m_dMaxForce;

	// The maximum rate (radians per second) this vehicle can rotate
	double m_dMaxTurnRate;

public:

	// Only one constructor needed for `MovingEntity` class
	MovingEntity(
		Vector2D position, 
		double radius, 
		Vector2D velocity, 
		double maxSpeed,
		Vector2D heading, 
		double mass, 
		Vector2D scale, 
		double turnRate, 
		double maxForce)
		: BaseGameEntity(0, position, radius, scale),
		m_vVelocity(velocity),
		m_vHeading(heading),
		m_vSide(m_vHeading.Perp()),
		m_dMass(mass),
		m_dMaxSpeed(maxSpeed),
		m_dMaxTurnRate(turnRate),
		m_dMaxForce(maxForce)
	{}

	// Default virtual destructor
	virtual ~MovingEntity() = default;

	virtual void Update(double timeElapsed) {};

	virtual void Render() {};

	// Get/Set velocity of/to this entity
	Vector2D Velocity() const { return m_vVelocity; }
	void SetVelocity(Vector2D newVel) { m_vVelocity = newVel; }

	// Get/Set mass of/to this entity
	double Mass() const { return m_dMass; }
	void SetMass(double newMass) { m_dMass = newMass; }

	Vector2D Side() const { return m_vSide; }

	// Get/Set max speed of/to this entity
	double MaxSpeed() const { return m_dMaxSpeed; }
	void SetMaxSpeed(double newMaxSpeed) { m_dMaxSpeed = newMaxSpeed; }

	// Get/Set max force of/to this entity
	double MaxForce() const { return m_dMaxForce; }
	void SetMaxForce(double maxForce) { m_dMaxForce = maxForce; }

	bool IsSpeedMaxedOut() const { return (m_dMaxSpeed * m_dMaxSpeed) >= m_vVelocity.LengthSq(); }
	// Get/Set speed of/to this entity
	double Speed() const { return m_vVelocity.Length(); }
	double SpeedSq() const { return m_vVelocity.LengthSq(); }

	// Get/Set heading vector of/to this entity
	Vector2D Heading() const { return m_vHeading; }
	inline void SetHeading(Vector2D newHeading);
	
	// Get/Set max turn rate of/to this entity
	double MaxTurnRate() const { return m_dMaxTurnRate; }
	void SetMaxTurnRate(double val) { m_dMaxTurnRate = val; }

	// Given a target position, this method rotates the entity's heading and
	// side vectors by an amount not greater than m_dMaxTurnRate until it directly
	// faces the target
	inline bool RotateHeadingToFacePosition(Vector2D target);
};

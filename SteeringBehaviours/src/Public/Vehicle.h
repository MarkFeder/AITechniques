#pragma once

#include "Public/Entities/MovingEntity.h"
#include "Public/Misc/Smoother.h"
#include "Public/2D/Vector2D.h"

#include <vector>
#include <list>
#include <string>

class GameWorld;
class SteeringBehavior;

class Vehicle : public MovingEntity
{
private:

	// A pointer to the world data enabling a vehicle to access any obstacle
	// path, wall, or agent data
	GameWorld* m_pWorld;

	// The steering behaviour class
	SteeringBehavior* m_pSteering;

	// Some steering behaviors give jerkly looking movement. The
	// following members are used to smooth the vehicle's heading
	Smoother<Vector2D>* m_pHeadingSmoother;

	// This vector represents the average of the vehicle's heading
	// vector smoothed over the last few frames
	Vector2D m_vSmoothedHeading;

	// When true, smoothing is active
	bool m_bSmoothingOn;

	// Keeps a track of the most recent update time. (some of the 
	// steering behaviors make use of this - see Wander)
	double m_dTimeElapsed;

	// Buffer to the vehicle shape
	std::vector<Vector2D> m_vecVehicleVB;

	// Fills the buffer with vertex data
	void InitializeBuffer();

public:

	Vehicle() = default;
	Vehicle(GameWorld* world, 
			Vector2D position, 
			double rotation, 
			Vector2D velocity, 
			double mass, 
			double maxForce, 
			double maxSpeed, 
			double maxTurnRate, 
			double scale);

	~Vehicle();

	// Disallow the copying of Vehicle types
	Vehicle(const Vehicle&) = delete;
	Vehicle& operator=(const Vehicle&) = delete;

	// Updates the vehicle's position and orientation
	void Update(double timeElapsed) override;
	
	// Do not handle any specific message between vehicles
	bool HandleMessage(const Telegram& msg) override { return false; };

	// Renders the vehicle
	void Render() override;

	// Accessors methods
	SteeringBehavior* const Steering() const { return m_pSteering; }
	GameWorld* const World() const { return m_pWorld; }

	Vector2D SmoothedHeading() const { return m_vSmoothedHeading; }
	double TimeElapsed() const { return m_dTimeElapsed; }

	bool IsSmoothingOn() const { return m_bSmoothingOn; }
	void SmoothingOn() { m_bSmoothingOn = true; }
	void SmoothingOff() { m_bSmoothingOn = false; }
	void ToggleSmoothing() { m_bSmoothingOn = !m_bSmoothingOn; }
};

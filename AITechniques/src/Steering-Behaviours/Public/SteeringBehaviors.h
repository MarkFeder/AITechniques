#pragma once

#include <vector>
#include <Windows.h>
#include <string>
#include <list>

#include "Common/Public/2D/Vector2D.h"
#include "ParamLoader.h"
#include "Constants.h"
#include "Path.h"

class Vehicle;
class CController;
class Wall2D;
class BaseGameEntity;

//--------------------------- Constants ------------------------------------
//--------------------------------------------------------------------------

// The radius of the constraining circle for the wander behavior
const double wanderRad = 1.2;

// Distance the wander circle is projected in front of the agent
const double wanderDist = 2.0;

// The maxium amount of displacement along the circle each frame
const double wanderJitterPerSec = 80.0;

// Used in path following
const double waypointSeekDist = 20;

//--------------------------- SeteeringBehavior ----------------------------
// Class to encapsulate steering behaviors for a vehicle
//--------------------------------------------------------------------------

class SeteeringBehavior
{
public:

	enum SummingMethod
	{
		WeightedAverage,
		Prioritized,
		Dithered
	};

private:

	enum BehaviorTyoe
	{
		None = 0x00000,
		Seek = 0x00002,
		Flee = 0x00004,
		Arrive = 0x00008,
		Wander = 0x00010,
		Cohesion = 0x00020,
		Separation = 0x00040,
		Allignment = 0x00080,
		ObstacleAvoidance = 0x00100,
		FollowPath = 0x00400,
		Pursuit = 0x00800,
		Evade = 0x01000,
		Interpose = 0x02000,
		Hide = 0x04000,
		Flock = 0x08000,
		OffsetPursuit = 0x10000
	};

	// A pointer to the owner of this instance
	Vehicle* m_pVehicle;

	// The steering force created by the combined 
	// effect of all the selected behaviors
	Vector2D m_vSteeringForce;

	// These can be used to keep track of friends, pursuers or prey
	Vehicle* m_pTargetAgent1;
	Vehicle* m_pTargetAgent2;

	// The current target
	Vector2D m_vTarget;

	// Length of the 'detection box' utilized in obstacle avoidance
	double m_dDBoxLength;

	// A vertex buffer to contain the feelers required for wall avoidance
	std::vector<Vector2D> m_feelers;

	// The length of the 'feeler/s' used in wall detection
	double m_dWallDetectionFeelerLength;

	// The current position on the wander circle the agent is
	// attempting to steer towards
	Vector2D m_vWanderTarget;

	// Explained above
	double m_dWanderJitter;
	double m_dWanderRadius;
	double m_dWanderDistance;

	// Multipliers. These can be adjusted to effect strength of the appropiate
	// behavoir. Useful to get flocking the wya you require for example
	double m_dWeightSeparation;
	double m_dWeightCohesion;
	double m_dWeightAlignment;
	double m_dWeightWander;
	double m_dWeightObstacleAvoidance;
	double m_dWeightWallAvoidance;
	double m_dWeightSeek;
	double m_dWeightFlee;
	double m_dWeightArrive;
	double m_dWeightPursuit;
	double m_dWeightOffsetPursuit;
	double m_dWeightInterpose;
	double m_dWeightHide;
	double m_dWeightEvade;
	double m_dWeightFollowPath;

	// How far the agent can 'see'
	double m_dViewDistance;


};
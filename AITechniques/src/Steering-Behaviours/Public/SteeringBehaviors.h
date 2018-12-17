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

	enum BehaviorType
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

	// Pointer to any current path
	Path* m_pPath;

	// The distance (squared) a vehicle has to be from a path waypoint before
	// it starts seeking to the next waypoint
	double m_dWaypointSeekDistSq;

	// Any offset used for formations or offset pursuit
	Vector2D m_vOffset;

	// Binary flags to indicate whether or not a behavior should be active
	int m_iFlags;

	// Arrive makes use of these to determine how quickly a vehicle 
	// should decelerate to its target
	enum Deceleration { slow = 3, normal = 2, fast = 1};

	// Default deceleration
	Deceleration m_Deceleration;

	// Is cell space partitioning to be used or not?
	bool m_bCellSpaceOn;

	// Which type of method is used to sum any active behavior
	SummingMethod m_SummingMethod;

	// This function tests if a specific bit of m_iFlags is set
	bool On(const BehaviorType& bt) { return (m_iFlags & bt) == bt; }

	bool AccumulateForce(Vector2D& sf, Vector2D& forceToAdd);

	// Creates the antenna utilized by the wall avoidance behavior
	void CreateFeelers();

	//--------------------------------------------------------------------------
	// Behaviour declarations
	//--------------------------------------------------------------------------

	// This behaviour moves the agent towards a target position
	Vector2D Seek(Vector2D& targetPos);

	// This behaviour returns a vector that moves the agent away
	// from a target position
	Vector2D Flee(Vector2D& targetPos);

	// This behaviour is similar to seek but it attempts to arrive at
	// the target position with a zero velocity
	Vector2D Arrive(const Vector2D& targetPos, const Deceleration& deceleration);

	// This behavior predicts where an agent will be in time T and seeks towards
	// that point to intercept it
	Vector2D Pursuit(const Vehicle* agent);

	// This behaviour mantains a position, in the direction of offset from
	// the target vehicle
	Vector2D OffsetPursuit(const Vehicle* agent, const Vector2D& offset);

	// This behavior attempts to evade a pursuer
	Vector2D Evade(const Vehicle* agent);

	// This behavior makes the agent wander about randomly
	Vector2D Wander();
};
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

class SteeringBehavior
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
		Alignment = 0x00080,
		ObstacleAvoidance = 0x00100,
		WallAvoidance = 0x00200,
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

	// Calculates and sums the steering forces from any active behaviors
	Vector2D CalculateWeightedSum();
	Vector2D CalculatePrioritized();
	Vector2D CalculateDithered();

	// Helper method for hide. Returns a position located on the other
	// side of an obstacle to the pursuer
	Vector2D GetHidingPosition(const Vector2D& posObstacle, const double radiusObstacle, const Vector2D& posHunter);

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

	// This returns a steering force which will attempt to keep the agent away from
	// any obstacles it may encounter
	Vector2D ObstacleAvoidance(const std::vector<BaseGameEntity*>& obstacles);

	// This returns a steering force which will keep the agent away from any
	// walls it may encounter
	Vector2D WallAvoidance(const std::vector<Wall2D>& walls);

	// Given a series of Vector2Ds, this method produces a force that will move
	// the agent along the waypoints in order
	Vector2D FollowPath();

	// This results in a steering force that attemps to steer the vehicle
	// to the center of the vector connecting two moving agents
	Vector2D Interpose(const Vehicle* vehicleA, const Vehicle* vehicleB);

	// Given another agent position to hide from and a list of BaseGameEntities this
	// method attemps to put an obstacle between itself and its opponent
	Vector2D Hide(const Vehicle* hunter, const std::vector<BaseGameEntity*>& obstacles);

	//--------------------------------------------------------------------------
	// Group Behaviours
	//--------------------------------------------------------------------------

	Vector2D Cohesion(const std::vector<Vehicle*>& agents);
	
	Vector2D Separation(const std::vector<Vehicle>& agents);

	Vector2D Alignment(const std::vector<Vehicle*>& agents);

	// The following three are the same as above but they use cell-space
	// partitioning to find the neighbors
	Vector2D CohesionPlus(const std::vector<Vehicle*>& agents);
	Vector2D SeparationPlus(const std::vector<Vehicle*>& agents);
	Vector2D AlignmentPlus(const std::vector<Vehicle*>& agents);

public:

	SteeringBehavior(Vehicle* agent);

	virtual ~SteeringBehavior();

	// Calculates and sums the steering forces from any active behaviors
	Vector2D Calculate();

	// Calculates the component of the steering force that is parallel
	// with the vehicle heading
	double ForwardComponent();

	// Calculates the component of the steering force that is perpendicular
	// with the vehicle heading
	double SideComponent();

	// Renders visual aids and info for seeing how each behavior is calculated
	void RenderAids();

	void SetTarget(const Vector2D t) { m_vTarget = t; }

	void SetTargetAgent1(Vehicle* agent) { m_pTargetAgent1 = agent; }
	void SetTargetAgent2(Vehicle* agent) { m_pTargetAgent2 = agent; }

	void SetOffset(const Vector2D offset) { m_vOffset = offset; }
	Vector2D GetOffset() const { return m_vOffset; }

	void SetPath(std::list<Vector2D> newPath) { m_pPath->Set(newPath); }
	void CreateRandomPath(int numWaypoints, int mx, int my, int cx, int cy) const { m_pPath->CreateRandomPath(numWaypoints, mx, my, cx, cy); }

	Vector2D Force() const { return m_vSteeringForce; }

	void ToggleSpacePartitioningOnOff() { m_bCellSpaceOn = !m_bCellSpaceOn; }
	bool IsSpacePartitioningOn() const { return m_bCellSpaceOn; }

	void SetSummingMethod(SummingMethod sm) { m_SummingMethod = sm; }

	void FleeOn() { m_iFlags |= Flee; }
	void SeekOn() { m_iFlags |= Seek; }
	void ArriveOn() { m_iFlags |= Arrive; }
	void WanderOn() { m_iFlags |= Wander; }
	void PursuitOn(Vehicle* v) { m_iFlags |= Pursuit; m_pTargetAgent1 = v; }
	void EvadeOn(Vehicle* v) { m_iFlags |= Evade; m_pTargetAgent1 = v; }
	void CohesionOn() { m_iFlags |= Cohesion; }
	void SeparationOn() { m_iFlags |= Separation; }
	void AlignmentOn() { m_iFlags |= Alignment; }
	void ObstacleAvoidanceOn() { m_iFlags |= ObstacleAvoidance; }
	void WallAvoidanceOn() { m_iFlags |= WallAvoidance; }
	void FollowPathOn() { m_iFlags |= FollowPath; }
	void InterposeOn(Vehicle* v1, Vehicle* v2) { m_iFlags |= Interpose; m_pTargetAgent1 = v1; m_pTargetAgent2 = v2; }
	void HideOn(Vehicle* v) { m_iFlags |= Hide; m_pTargetAgent1 = v; }
	void OffsetPursuitOn(Vehicle* v1, const Vector2D offset) { m_iFlags |= OffsetPursuit; m_vOffset = offset; m_pTargetAgent1 = v1; }
	void FlockingOn() { CohesionOn(); AlignmentOn(); SeparationOn(); WanderOn(); }


};
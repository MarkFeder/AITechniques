#include "Steering-Behaviours/Public/SteeringBehaviors.h"
#include "Steering-Behaviours/Public/Vehicle.h"
#include "Steering-Behaviours/Public/GameWorld.h"

#include "Common/Public/2D/Wall2D.h"
#include "Common/Public/2D/Transformations.h"
#include "Common/Public/2D/Geometry.h"
#include "Common/Public/Misc/Utils.h"
#include "Common/Public/Misc/Cgdi.h"
#include "Common/Public/Misc/CellSpacePartition.h"
#include "Common/Public/Misc/StreamUtils.h"
#include "Common/Public/Entities/BaseGameEntity.h"
#include "Common/Public/Entities/EntityTemplates.h"

#include <cassert>

using std::string;
using std::vector;

//--------------------------- ctor ------------------------------------
//---------------------------------------------------------------------

SteeringBehavior::SteeringBehavior(Vehicle* agent)
	: m_pVehicle(agent),
	m_iFlags(0),
	m_dDBoxLength(Prm.MinDetectionBoxLength()),
	m_dWeightCohesion(Prm.CohesionWeight()),
	m_dWeightAlignment(Prm.AlignmentWeight()),
	m_dWeightSeparation(Prm.SeparationWeight()),
	m_dWeightObstacleAvoidance(Prm.ObstacleAvoidanceWeight()),
	m_dWeightWander(Prm.WanderWeight()),
	m_dWeightWallAvoidance(Prm.WallAvoidanceWeight()),
	m_dViewDistance(Prm.ViewDistance()),
	m_dWallDetectionFeelerLength(Prm.WallDetectionFeelerLength()),
	m_feelers(3),
	m_Deceleration(normal),
	m_pTargetAgent1(nullptr),
	m_pTargetAgent2(nullptr),
	m_dWanderDistance(wanderDist),
	m_dWanderJitter(wanderJitterPerSec),
	m_dWanderRadius(wanderRad),
	m_dWaypointSeekDistSq(waypointSeekDist * waypointSeekDist),
	m_dWeightSeek(Prm.SeekWeight()),
	m_dWeightFlee(Prm.FleeWeight()),
	m_dWeightArrive(Prm.ArriveWeight()),
	m_dWeightPursuit(Prm.PursuitWeight()),
	m_dWeightOffsetPursuit(Prm.OffsetPursuitWeight()),
	m_dWeightInterpose(Prm.InterposeWeight()),
	m_dWeightHide(Prm.HideWeight()),
	m_dWeightEvade(Prm.EvadeWeight()),
	m_dWeightFollowPath(Prm.FollowPathWeight()),
	m_bCellSpaceOn(false),
	m_SummingMethod(Prioritized)
{
	// stuff for the wander behavior
	double theta = RandFloat() * TwoPi;

	// Create a vector to a target position on the wander circle
	m_vWanderTarget = Vector2D(m_dWanderRadius * cos(theta), m_dWanderRadius * sin(theta));

	// Create a path
	m_pPath = new Path();
	m_pPath->LoopOn();
}

//--------------------------- dtor ------------------------------------
//---------------------------------------------------------------------

SteeringBehavior::~SteeringBehavior()
{
	delete m_pPath;
}

//--------------------------- Calculate ------------------------------------
// Calculates the accumulated steering force according to the method set
// in m_summingMethod
//--------------------------------------------------------------------------

Vector2D SteeringBehavior::Calculate()
{
	// Reset the steering force
	m_vSteeringForce.Zero();

	// Use space partitioning to calculate the neighbours of this vehicle
	// if switched on. If not, use the standard tagging system
	if (On(BT_Separation) || On(BT_Alignment) || On(BT_Cohesion))
	{
		if (!IsSpacePartitioningOn())
		{
			m_pVehicle->World()->TagVehiclesWithinViewRange(m_pVehicle, m_dViewDistance);
		}
		else
		{
			m_pVehicle->World()->CellSpace()->CalculateNeighbors(m_pVehicle->Pos(), m_dViewDistance);
		}
	}

	switch(m_SummingMethod)
	{
		case WeightedAverage:
			m_vSteeringForce = CalculateWeightedSum();
			break;
		
		case Prioritized:
			m_vSteeringForce = CalculatePrioritized();
			break;

		case Dithered:
			m_vSteeringForce = CalculateDithered();
			break;

		default:
			m_vSteeringForce = Vector2D(0,0);
	}

	return m_vSteeringForce;
}

//--------------------------- ForwardComponent ------------------------------------
// Returns the forward component of the steering force
//---------------------------------------------------------------------------------

double SteeringBehavior::ForwardComponent()
{
	return m_pVehicle->Heading().Dot(m_vSteeringForce);
}

//--------------------------- SideComponent ---------------------------------------
// Returns the side component of the steering force
//---------------------------------------------------------------------------------

double SteeringBehavior::SideComponent()
{
	return m_pVehicle->Side().Dot(m_vSteeringForce);
}


void SteeringBehavior::RenderAids()
{
}

//--------------------------- AccumulateForce -------------------------------------
// This function calculates how much of its max steering force the vehicle
// has left to apply and then applies that amount of the force to add.
//---------------------------------------------------------------------------------

bool SteeringBehavior::AccumulateForce(Vector2D& sf, Vector2D& forceToAdd)
{
	// Calculate how much steering force the vehicle has used so far
	double magnitudeSoFar = sf.Length();

	// Calculate how much steering force remains to be used by this vehicle
	double magnitudeRemaining = m_pVehicle->MaxForce() - magnitudeSoFar;

	// Returns false if there is no more force left to use
	if (magnitudeRemaining <= 0.0) return false;

	// Calculate the magnitude of the force we want to add
	double magnitudeToAdd = forceToAdd.Length();

	// If the magnitude of the sum of forceToAdd and the running total does not
	// exceed the maximum force available to this vehicle, just add together.
	// Otherwise add as much of the forceToAdd vector is possible without going over the max.
	if (magnitudeToAdd < magnitudeRemaining)
	{
		sf += forceToAdd;
	}
	else
	{
		// Add it to the steering force
		sf += (Vec2DNormalize(forceToAdd) * magnitudeRemaining);
	}

	return true;
}

//--------------------------- CreateFeelers ---------------------------------------
// Creates the antenna utilized by WallAvoidance
//---------------------------------------------------------------------------------

void SteeringBehavior::CreateFeelers()
{
	// Feeler pointing straight in front
	m_feelers[0] = m_pVehicle->Pos() + m_dWallDetectionFeelerLength * m_pVehicle->Heading();

	// Feeler to left
	Vector2D temp = m_pVehicle->Heading();
	Vec2DRotateAroundOrigin(temp, HalfPi * 3.5f);
	m_feelers[1] = m_pVehicle->Pos() + m_dWallDetectionFeelerLength / 2.0f * temp;

	// Feeler to right
	temp = m_pVehicle->Heading();
	Vec2DRotateAroundOrigin(temp, HalfPi * 0.5f);
	m_feelers[2] = m_pVehicle->Pos() + m_dWallDetectionFeelerLength / 2.0f * temp;
}

//--------------------------- CalculateWeightedSum --------------------------------
// This simply sums up all the active behaviors X their weights and truncates
// the result to the max available steering force before returning
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::CalculateWeightedSum()
{
	return Vector2D();
}

//--------------------------- CalculatePrioritized --------------------------------
// This method calls each active steering behavior in order of priority and
// accumulates their forces until the max steering force magnitude is reached,
// at which time the function returns the steering force accumulated to that point
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::CalculatePrioritized()
{
	Vector2D force;

	if (On(BT_WallAvoidance))
	{
		force = WallAvoidance(m_pVehicle->World()->Walls()) * m_dWeightWallAvoidance;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(BT_ObstacleAvoidance))
	{
		force = ObstacleAvoidance(m_pVehicle->World()->Obstacles()) * m_dWeightObstacleAvoidance;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(BT_Evade))
	{
		assert(m_pTargetAgent1 && "Evade target not assigned");

		force = Evade(m_pTargetAgent1) * m_dWeightEvade;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(BT_Flee))
	{
		force = Flee(m_pVehicle->World()->Crosshair()) * m_dWeightFlee;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	// These next three can be combined for flocking behavior (wander is also a good behavior to add into this mix)
	if (!IsSpacePartitioningOn())
	{
		if (On(BT_Separation))
		{
			force = Separation(m_pVehicle->World()->Agents()) * m_dWeightSeparation;

			if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
		}

		if (On(BT_Alignment))
		{
			force = Alignment(m_pVehicle->World()->Agents()) * m_dWeightAlignment;

			if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
		}

		if (On(BT_Cohesion))
		{
			force = Cohesion(m_pVehicle->World()->Agents()) * m_dWeightCohesion;

			if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
		}
	}
	else
	{
		if (On(BT_Separation))
		{
			force = SeparationPlus(m_pVehicle->World()->Agents()) * m_dWeightSeparation;

			if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
		}

		if (On(BT_Alignment))
		{
			force = AlignmentPlus(m_pVehicle->World()->Agents()) * m_dWeightAlignment;

			if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
		}

		if (On(BT_Cohesion))
		{
			force = CohesionPlus(m_pVehicle->World()->Agents()) * m_dWeightCohesion;

			if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
		}
	}

	if (On(BT_Seek))
	{
		force = Seek(m_pVehicle->World()->Crosshair()) * m_dWeightSeek;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(BT_Arrive))
	{
		force = Arrive(m_pVehicle->World()->Crosshair(), m_Deceleration) * m_dWeightArrive;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(BT_Wander))
	{
		force = Wander() * m_dWeightWander;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(BT_Pursuit))
	{
		assert(m_pTargetAgent1 && "Pursuit target not assigned");

		force = Pursuit(m_pTargetAgent1) * m_dWeightPursuit;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(BT_OffsetPursuit))
	{
		assert(m_pTargetAgent1 && "Pursuit target not assigned");
		assert(!m_vOffset.IsZero() && "No offset assigned");

		force = OffsetPursuit(m_pTargetAgent1, m_vOffset);

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(BT_Interpose))
	{
		assert(m_pTargetAgent1 && m_pTargetAgent2 && "Interpose agents not assigned");

		force = Interpose(m_pTargetAgent1, m_pTargetAgent2) * m_dWeightInterpose;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(BT_Hide))
	{
		assert(m_pTargetAgent1 && "Hide target not assigned");

		force = Hide(m_pTargetAgent1, m_pVehicle->World()->Obstacles()) * m_dWeightHide;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(BT_FollowPath))
	{
		force = FollowPath() * m_dWeightFollowPath;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	return m_vSteeringForce;
}

Vector2D SteeringBehavior::CalculateDithered()
{
	return Vector2D();
}

Vector2D SteeringBehavior::GetHidingPosition(const Vector2D& posObstacle, const double radiusObstacle, const Vector2D& posHunter)
{
	return Vector2D();
}

//--------------------------- Seek ------------------------------------------------
// Given a target, this behavior returns a steering force which will direct
// the agent towards the target
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::Seek(Vector2D& targetPos)
{
	Vector2D desiredVelocity = Vec2DNormalize(targetPos - m_pVehicle->Pos()) * m_pVehicle->MaxSpeed();
	return (desiredVelocity - m_pVehicle->Velocity());
}

//--------------------------- Flee ------------------------------------------------
// Does the opposite of Seek
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::Flee(Vector2D& targetPos)
{
	Vector2D desiredVelocity = Vec2DNormalize(m_pVehicle->Pos() - targetPos) * m_pVehicle->MaxSpeed();
	return (desiredVelocity - m_pVehicle->Velocity());
}

//--------------------------- Arrive ----------------------------------------------
// This behavior is similar to seek but it attempts to arrive at the target with
// a zero velocity
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::Arrive(const Vector2D& targetPos, const Deceleration& deceleration)
{
	Vector2D toTarget = targetPos - m_pVehicle->Pos();

	// Calculate the distance to the target
	double dist = toTarget.Length();

	if (dist > 0)
	{
		// Because Deceleration is enumerated as an int, this value is required to provide
		// fine tweaking of the deceleration ...
		const double decelerationTweaker = 0.3;

		// Calculate the speed required to reach the target given the desired
		// deceleration
		double speed = dist / ((double)deceleration * decelerationTweaker);

		// Make sure the velocity does not exceed the max
		speed = min(speed, m_pVehicle->MaxSpeed());

		// From here proceed just like Seek except we don't need to normalize the
		// toTarget vector because we have already gone to the trouble of calculating
		// its length: dist
		Vector2D desiredVelocity = toTarget * speed / dist;

		return (desiredVelocity - m_pVehicle->Velocity());
	}

	return Vector2D(0,0);
}

//--------------------------- Pursuit ---------------------------------------------
// This behavior creates a force that steers the agent towards the evader
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::Pursuit(const Vehicle* evader)
{
	// If the evader is ahead and facing the agent then we can just seek for
	// the evader's current position
	Vector2D toEvader = evader->Pos() - m_pVehicle->Pos();

	double relativeHeading = m_pVehicle->Heading().Dot(evader->Heading());

	if ((toEvader.Dot(m_pVehicle->Heading()) > 0) && (relativeHeading < acos(0.95))) // = 18 degs
	{
		return Seek(evader->Pos());
	}

	// Not considered ahead so we predict where the evader will be

	// The lookahead time is proportional to the distance between the evader and the pursuer; 
	// and is inversely proportional to the sum of the agent's velocities
	double lookAheadTime = toEvader.Length() / (m_pVehicle->MaxSpeed() + evader->Speed());

	// Now seek to the predicted future position of the evader
	return Seek(evader->Pos() + evader->Velocity() * lookAheadTime);
}

Vector2D SteeringBehavior::OffsetPursuit(const Vehicle* agent, const Vector2D& offset)
{
	return Vector2D();
}

//--------------------------- Evade -----------------------------------------------
// Similar to Pursuit except the agent flees from the estimated future position
// of the pursuer
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::Evade(const Vehicle* pursuer)
{
	// Not necessary to include the check for facing direction this time

	Vector2D toPursuer = pursuer->Pos() - m_pVehicle->Pos();

	// Uncomment the following two lines to have Evade only consider pursuers
	// within a 'threat range'
	const double threatRange = 100.0;
	if (toPursuer.LengthSq() > threatRange * threatRange) return Vector2D();

	// The lookahead time is proportional to the distance between the pursuer nand the evader; 
	// and is inversely proportional to the sum of the agents' velocities
	double lookAheadTime = toPursuer.Length() / (m_pVehicle->MaxSpeed() + pursuer->Speed());

	// Now flee away from predicted future position of the pursuer
	return Flee(pursuer->Pos() + (pursuer->Velocity() * lookAheadTime));
}

//--------------------------- Wander ----------------------------------------------
// This behavior makes the agent wander about randomly
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::Wander()
{
	// This behavior is dependent on the update rate, so this line must be included
	// when using time independent framerate
	double jitterThisTimeSlice = m_dWanderJitter * m_pVehicle->TimeElapsed();

	// First, add a small random vector to the target's position
	m_vWanderTarget += Vector2D(RandomClamped() * jitterThisTimeSlice, RandomClamped() * jitterThisTimeSlice);

	// Reproject this new vector back on to a unit circle
	m_vWanderTarget.Normalize();

	// Increases the length of the vector to the same as the radius of the wander circle
	m_vWanderTarget *= m_dWanderRadius;

	// Move the target into a position WanderDist in front of the agent
	Vector2D target = m_vWanderTarget + Vector2D(m_dWanderDistance, 0);

	// Project the target into world space
	Vector2D projectedTarget = PointToWorldSpace(target, m_pVehicle->Heading(), m_pVehicle->Side(), m_pVehicle->Pos());

	// And steer towards it
	return projectedTarget - m_pVehicle->Pos();
}

Vector2D SteeringBehavior::ObstacleAvoidance(const std::vector<BaseGameEntity*>& obstacles)
{
	return Vector2D();
}

Vector2D SteeringBehavior::WallAvoidance(const std::vector<Wall2D>& walls)
{
	return Vector2D();
}

Vector2D SteeringBehavior::FollowPath()
{
	return Vector2D();
}

Vector2D SteeringBehavior::Interpose(const Vehicle* vehicleA, const Vehicle * vehicleB)
{
	return Vector2D();
}

Vector2D SteeringBehavior::Hide(const Vehicle * hunter, const std::vector<BaseGameEntity*>& obstacles)
{
	return Vector2D();
}

Vector2D SteeringBehavior::Cohesion(const std::vector<Vehicle*>& agents)
{
	return Vector2D();
}

Vector2D SteeringBehavior::Separation(const std::vector<Vehicle*>& agents)
{
	return Vector2D();
}

Vector2D SteeringBehavior::Alignment(const std::vector<Vehicle*>& agents)
{
	return Vector2D();
}

Vector2D SteeringBehavior::CohesionPlus(const std::vector<Vehicle*>& agents)
{
	return Vector2D();
}

Vector2D SteeringBehavior::SeparationPlus(const std::vector<Vehicle*>& agents)
{
	return Vector2D();
}

Vector2D SteeringBehavior::AlignmentPlus(const std::vector<Vehicle*>& agents)
{
	return Vector2D();
}

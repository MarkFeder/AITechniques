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
	m_dWeightAlignment(Prm.Align_dmentWeight()),
	m_dWeightSeparation(Prm.SeparationWeight()),
	m_dWeightObstacleAvoidance(Prm.ObstacleAvoidance()),
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
	m_SummingMethod(Prioritized);
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
			m_vSteeringForce = Vector2D(0.0);
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

void SteeringBehavior::CreateFeelers()
{
}

Vector2D SteeringBehavior::CalculateWeightedSum()
{
	return Vector2D();
}

Vector2D SteeringBehavior::CalculatePrioritized()
{
	return Vector2D();
}

Vector2D SteeringBehavior::CalculateDithered()
{
	return Vector2D();
}

Vector2D SteeringBehavior::GetHidingPosition(const Vector2D & posObstacle, const double radiusObstacle, const Vector2D & posHunter)
{
	return Vector2D();
}

Vector2D SteeringBehavior::Seek(Vector2D & targetPos)
{
	return Vector2D();
}

Vector2D SteeringBehavior::Flee(Vector2D & targetPos)
{
	return Vector2D();
}

Vector2D SteeringBehavior::Arrive(const Vector2D & targetPos, const Deceleration & deceleration)
{
	return Vector2D();
}

Vector2D SteeringBehavior::Pursuit(const Vehicle * agent)
{
	return Vector2D();
}

Vector2D SteeringBehavior::OffsetPursuit(const Vehicle * agent, const Vector2D & offset)
{
	return Vector2D();
}

Vector2D SteeringBehavior::Evade(const Vehicle * agent)
{
	return Vector2D();
}

Vector2D SteeringBehavior::Wander()
{
	return Vector2D();
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

Vector2D SteeringBehavior::Interpose(const Vehicle * vehicleA, const Vehicle * vehicleB)
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

Vector2D SteeringBehavior::Separation(const std::vector<Vehicle>& agents)
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

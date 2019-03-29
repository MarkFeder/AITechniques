#include "Public/SteeringBehaviors.h"
#include "Public/Vehicle.h"
#include "Public/GameWorld.h"

#include "Public/2D/Wall2D.h"
#include "Public/2D/Transformations.h"
#include "Public/2D/Geometry.h"
#include "Public/Misc/Utils.h"
#include "Public/Misc/Cgdi.h"
#include "Public/Misc/CellSpacePartition.h"
#include "Public/Misc/StreamUtils.h"
#include "Public/Entities/BaseGameEntity.h"
#include "Public/Entities/EntityTemplates.h"

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

	switch (m_SummingMethod)
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
		m_vSteeringForce = Vector2D(0, 0);
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
	if (On(BT_WallAvoidance))
	{
		m_vSteeringForce += WallAvoidance(m_pVehicle->World()->Walls()) * m_dWeightWallAvoidance;
	}

	if (On(BT_ObstacleAvoidance))
	{
		m_vSteeringForce += ObstacleAvoidance(m_pVehicle->World()->Obstacles()) * m_dWeightObstacleAvoidance;
	}

	if (On(BT_Evade))
	{
		assert(m_pTargetAgent1 && "Evade target not assigned");

		m_vSteeringForce += Evade(m_pTargetAgent1) * m_dWeightEvade;
	}

	// These next three can be combined for flocking behavior (wander is
	// also a good behavior to add into this mix
	if (!IsSpacePartitioningOn())
	{
		if (On(BT_Separation))
		{
			m_vSteeringForce += Separation(m_pVehicle->World()->Agents()) * m_dWeightSeparation;
		}

		if (On(BT_Alignment))
		{
			m_vSteeringForce += Alignment(m_pVehicle->World()->Agents()) * m_dWeightAlignment;
		}

		if (On(BT_Cohesion))
		{
			m_vSteeringForce += Cohesion(m_pVehicle->World()->Agents()) * m_dWeightCohesion;
		}
	}
	else
	{
		if (On(BT_Separation))
		{
			m_vSteeringForce += SeparationPlus(m_pVehicle->World()->Agents()) * m_dWeightSeparation;
		}

		if (On(BT_Alignment))
		{
			m_vSteeringForce += AlignmentPlus(m_pVehicle->World()->Agents()) * m_dWeightAlignment;
		}

		if (On(BT_Cohesion))
		{
			m_vSteeringForce += CohesionPlus(m_pVehicle->World()->Agents()) * m_dWeightCohesion;
		}
	}

	if (On(BT_Wander))
	{
		m_vSteeringForce += Wander() * m_dWeightWander;
	}

	if (On(BT_Seek))
	{
		m_vSteeringForce += Seek(m_pVehicle->World()->Crosshair()) * m_dWeightSeek;
	}

	if (On(BT_Arrive))
	{
		m_vSteeringForce += Arrive(m_pVehicle->World()->Crosshair(), m_Deceleration) * m_dWeightArrive;
	}

	if (On(BT_Flee))
	{
		m_vSteeringForce += Flee(m_pVehicle->World()->Crosshair()) * m_dWeightFlee;
	}

	if (On(BT_Pursuit))
	{
		assert(m_pTargetAgent1 && "Pursuit target not assigned");

		m_vSteeringForce += Pursuit(m_pTargetAgent1) * m_dWeightPursuit;
	}

	if (On(BT_OffsetPursuit))
	{
		assert(m_pTargetAgent1 && "Pursuit target not assigned");
		assert(!m_vOffset.IsZero() && "No offset assigned");

		m_vSteeringForce += OffsetPursuit(m_pTargetAgent1, m_vOffset) * m_dWeightOffsetPursuit;
	}

	if (On(BT_Interpose))
	{
		assert(m_pTargetAgent1 && m_pTargetAgent2 && "Interpose agents not assigned");

		m_vSteeringForce += Interpose(m_pTargetAgent1, m_pTargetAgent2) * m_dWeightInterpose;
	}

	if (On(BT_Hide))
	{
		assert(m_pTargetAgent1 && "Hide target not assigned");

		m_vSteeringForce += Hide(m_pTargetAgent1, m_pVehicle->World()->Obstacles()) * m_dWeightHide;
	}

	if (On(BT_FollowPath))
	{
		m_vSteeringForce += FollowPath() * m_dWeightFollowPath;
	}

	m_vSteeringForce.Truncate(m_pVehicle->MaxForce());

	return m_vSteeringForce;
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

//--------------------------- CalculateDithered -----------------------------------
// This method sums up the active behaviors by assigning a probability of being
// calculated to each behavior. It then tests the first priority to see if it
// should be calculated this simulation-step. If so, it calculates the steering
// force resulting from this behavior. If it is more than zero it returns the force.
// If zero, or if the behavior is skipped it continues onto the next priority, and
// so on.
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::CalculateDithered()
{
	// Reset the steering force
	m_vSteeringForce.Zero();

	if (On(BT_WallAvoidance) && RandFloat() < Prm.PrWallAvoidance())
	{
		m_vSteeringForce = WallAvoidance(m_pVehicle->World()->Walls()) * (m_dWeightWallAvoidance / Prm.PrWallAvoidance());

		if (!m_vSteeringForce.IsZero())
		{
			m_vSteeringForce.Truncate(m_pVehicle->MaxForce());

			return m_vSteeringForce;
		}
	}

	if (On(BT_ObstacleAvoidance) && RandFloat() < Prm.PrObstacleAvoidance())
	{
		m_vSteeringForce = ObstacleAvoidance(m_pVehicle->World()->Obstacles()) * (m_dWeightObstacleAvoidance / Prm.PrObstacleAvoidance());

		if (!m_vSteeringForce.IsZero())
		{
			m_vSteeringForce.Truncate(m_pVehicle->MaxForce());

			return m_vSteeringForce;
		}
	}

	if (!IsSpacePartitioningOn())
	{
		if (On(BT_Separation) && RandFloat() < Prm.PrSeparation())
		{
			m_vSteeringForce += Separation(m_pVehicle->World()->Agents()) * m_dWeightSeparation / Prm.PrSeparation();

			if (!m_vSteeringForce.IsZero())
			{
				m_vSteeringForce.Truncate(m_pVehicle->MaxForce());

				return m_vSteeringForce;
			}
		}
	}
	else
	{
		if (On(BT_Separation) && RandFloat() < Prm.PrSeparation())
		{
			m_vSteeringForce += SeparationPlus(m_pVehicle->World()->Agents()) * m_dWeightSeparation / Prm.PrSeparation();

			if (!m_vSteeringForce.IsZero())
			{
				m_vSteeringForce.Truncate(m_pVehicle->MaxForce());

				return m_vSteeringForce;
			}
		}
	}

	if (On(BT_Flee) && RandFloat() < Prm.PrFlee())
	{
		m_vSteeringForce += Flee(m_pVehicle->World()->Crosshair()) * m_dWeightFlee / Prm.PrFlee();

		if (!m_vSteeringForce.IsZero())
		{
			m_vSteeringForce.Truncate(m_pVehicle->MaxForce());

			return m_vSteeringForce;
		}
	}

	if (On(BT_Evade) && RandFloat() < Prm.PrEvade())
	{
		assert(m_pTargetAgent1 && "Evade target not assigned");

		m_vSteeringForce += Evade(m_pTargetAgent1) * m_dWeightEvade / Prm.PrEvade();

		if (!m_vSteeringForce.IsZero())
		{
			m_vSteeringForce.Truncate(m_pVehicle->MaxForce());

			return m_vSteeringForce;
		}
	}

	if (!IsSpacePartitioningOn())
	{
		if (On(BT_Alignment) && RandFloat() < Prm.PrAlignment())
		{
			m_vSteeringForce += Alignment(m_pVehicle->World()->Agents()) * m_dWeightAlignment / Prm.PrAlignment();

			if (!m_vSteeringForce.IsZero())
			{
				m_vSteeringForce.Truncate(m_pVehicle->MaxForce());

				return m_vSteeringForce;
			}
		}

		if (On(BT_Cohesion) && RandFloat() < Prm.PrCohesion())
		{
			m_vSteeringForce += Cohesion(m_pVehicle->World()->Agents()) * m_dWeightCohesion / Prm.PrCohesion();

			if (!m_vSteeringForce.IsZero())
			{
				m_vSteeringForce.Truncate(m_pVehicle->MaxForce());

				return m_vSteeringForce;
			}
		}
	}
	else
	{
		if (On(BT_Alignment) && RandFloat() < Prm.PrAlignment())
		{
			m_vSteeringForce += AlignmentPlus(m_pVehicle->World()->Agents()) * m_dWeightAlignment / Prm.PrAlignment();

			if (!m_vSteeringForce.IsZero())
			{
				m_vSteeringForce.Truncate(m_pVehicle->MaxForce());

				return m_vSteeringForce;
			}
		}

		if (On(BT_Cohesion) && RandFloat() < Prm.PrCohesion())
		{
			m_vSteeringForce += CohesionPlus(m_pVehicle->World()->Agents()) * m_dWeightCohesion / Prm.PrCohesion();

			if (!m_vSteeringForce.IsZero())
			{
				m_vSteeringForce.Truncate(m_pVehicle->MaxForce());

				return m_vSteeringForce;
			}
		}
	}

	if (On(BT_Wander) && RandFloat() < Prm.PrWander())
	{
		m_vSteeringForce += Wander() * m_dWeightWander / Prm.PrWander();

		if (!m_vSteeringForce.IsZero())
		{
			m_vSteeringForce.Truncate(m_pVehicle->MaxForce());

			return m_vSteeringForce;
		}
	}

	if (On(BT_Seek) && RandFloat() < Prm.PrSeek())
	{
		m_vSteeringForce += Seek(m_pVehicle->World()->Crosshair()) * m_dWeightSeek / Prm.PrSeek();

		if (!m_vSteeringForce.IsZero())
		{
			m_vSteeringForce.Truncate(m_pVehicle->MaxForce());

			return m_vSteeringForce;
		}
	}

	if (On(BT_Arrive) && RandFloat() < Prm.PrArrive())
	{
		m_vSteeringForce += Arrive(m_pVehicle->World()->Crosshair(), m_Deceleration) * m_dWeightArrive / Prm.PrArrive();

		if (!m_vSteeringForce.IsZero())
		{
			m_vSteeringForce.Truncate(m_pVehicle->MaxForce());

			return m_vSteeringForce;
		}
	}

	return m_vSteeringForce;
}

//--------------------------- GetHidingPosition -----------------------------------
// Given the position of a hunter, and the position and radius of an obstacle, this
// method calculates a position DistanceFromBoundary away from its bounding radius
// and directly opposite the hunter
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::GetHidingPosition(const Vector2D& posObstacle, const double radiusObstacle, const Vector2D& posHunter)
{
	// Calculate how far away the agent is to be from the chosen obstacle's bounding radius
	const double distanceFromBoundary = 30.0;
	double distAway = radiusObstacle + distanceFromBoundary;

	// Calculate the heading toward the object from the hunter
	Vector2D toOb = Vec2DNormalize(posObstacle - posHunter);

	// Scale it to size and add to the obstacles position to get the hiding spot
	return (toOb * distAway) + posObstacle;
}

//--------------------------- Seek ------------------------------------------------
// Given a target, this behavior returns a steering force which will direct
// the agent towards the target
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::Seek(const Vector2D& targetPos)
{
	Vector2D desiredVelocity = Vec2DNormalize(targetPos - m_pVehicle->Pos()) * m_pVehicle->MaxSpeed();
	return (desiredVelocity - m_pVehicle->Velocity());
}

//--------------------------- Flee ------------------------------------------------
// Does the opposite of Seek
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::Flee(const Vector2D& targetPos)
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

	return Vector2D(0, 0);
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

//--------------------------- OffsetPursuit ---------------------------------------
// Produces a steering force that keeps a vehicle at a specified offset from a
// leader vehicle
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::OffsetPursuit(const Vehicle* leader, const Vector2D& offset)
{
	// Calculate the offset's position in world space
	Vector2D worldOffsetPos = PointToWorldSpace(offset, leader->Heading(), leader->Side(), leader->Pos());

	Vector2D toOffset = worldOffsetPos - m_pVehicle->Pos();

	// The lookahead time is proportional to the distance between the leader and the pursuer;
	// and is inversely proportional to the sum of both agent's velocities
	double lookAheadTime = toOffset.Length() / (m_pVehicle->MaxSpeed() + leader->Speed());

	// Now Arrive at the predicted future position of the offset
	return Arrive(worldOffsetPos + leader->Velocity() * lookAheadTime, fast);
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

//--------------------------- ObstacleAvoidance -----------------------------------
// Given a vector of obstacles, this method returns a steering force that will
// prevent the agent colliding with the closest obstacle
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::ObstacleAvoidance(const std::vector<BaseGameEntity*>& obstacles)
{
	// The detection box length is proportional to the agent's velocity
	m_dDBoxLength = Prm.MinDetectionBoxLength() +
		(m_pVehicle->Speed() / m_pVehicle->MaxSpeed()) * Prm.MinDetectionBoxLength();

	// Tag all obstacles within range of the box for processing
	m_pVehicle->World()->TagObstaclesWithingViewRange(m_pVehicle, m_dDBoxLength);

	// This will keep track of the closest intersecting obstacle (CIB)
	BaseGameEntity* closestIntersectingObstacle = nullptr;

	// This will be used to track the distance to the CIB
	double distToClosestIP = MaxDouble;

	// This will record the transformed local coordinates of the CIB
	Vector2D localPosOfClosestObstacles;

	std::vector<BaseGameEntity*>::const_iterator curOb = obstacles.begin();

	while (curOb != obstacles.end())
	{
		// If the obstacle has been tagged within range proceed
		if ((*curOb)->IsTagged())
		{
			// Calculate this obstacle's position in local space
			Vector2D localPos = PointToLocalSpace((*curOb)->Pos(),
				m_pVehicle->Heading(), m_pVehicle->Side(), m_pVehicle->Pos());

			// If the local position has a negative x value then it must lay behind
			// the agent. (in which case it can be ignored)
			if (localPos.x >= 0)
			{
				// If the distance from the x axis to the object's position is less than its radius + 
				// half the width of the detection box, then there is a potential intersection
				double expandedRadius = (*curOb)->BRadius() + m_pVehicle->BRadius();

				if (fabs(localPos.y) < expandedRadius)
				{
					// Now to do a line/circle intersection test. The center of the circle is represented by (cx, cy).
					// The intersection points are given by the formula x = cX +/- sqrt(r^2 - cY^2) for y=0. We only
					// need to look at the smallest positive value of x because that will be the closest point of
					// intersection.
					double cX = localPos.x;
					double cY = localPos.y;

					// We only need to calculate the sqrt part of the above equation once
					double sqrtPart = sqrt(expandedRadius * expandedRadius - cY * cY);

					double ip = cX - sqrtPart;

					if (ip <= 0.0)
					{
						ip = cX + sqrtPart;
					}

					// Test to see if this is the closest so far. If it is keep a record of the obstacle and its 
					// local coordinates
					if (ip < distToClosestIP)
					{
						distToClosestIP = ip;

						closestIntersectingObstacle = *curOb;

						localPosOfClosestObstacles = localPos;
					}
				}
			}
		}

		++curOb;
	}

	// If we have found an intersecting obstacle, calculate a steering force away from it
	Vector2D steeringForce;

	if (closestIntersectingObstacle)
	{
		// The closer the agent is to an object, the stronger the steering force should be
		double multiplier = 1.0 + (m_dDBoxLength - localPosOfClosestObstacles.x) / m_dDBoxLength;

		// Calculate the lateral force
		steeringForce.y = (closestIntersectingObstacle->BRadius() - localPosOfClosestObstacles.y) * multiplier;

		// Apply a braking force proportional to the obstacles distance from the vehicle
		const double brakingWeight = 0.2;

		steeringForce.x = (closestIntersectingObstacle->BRadius() - localPosOfClosestObstacles.x) * brakingWeight;
	}

	// Finally, convert the steering vector from local to world space
	return VectorToWorldSpace(steeringForce, m_pVehicle->Heading(), m_pVehicle->Side());
}

//--------------------------- WallAvoidance ---------------------------------------
// This returns a steering force that will keep the agent away from any walls
// it may encounter
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::WallAvoidance(const std::vector<Wall2D>& walls)
{
	// The feelers are contained in a std::vector, m_feelers
	CreateFeelers();

	double distToThisIP = 0.0;
	double distToClosestIP = MaxDouble;

	// This will hold an index into the vector of walls
	int closestWall = -1;

	Vector2D steeringForce,
		point, // Used for storing temporary info
		closestPoint; // Holds the closest intersection point

	// Examine each feelere in turn
	for (unsigned int flr = 0; flr < m_feelers.size(); ++flr)
	{
		// Run through each wall checking for any intersection points
		for (unsigned int w = 0; w < walls.size(); ++w)
		{
			if (LineIntersection2D(m_pVehicle->Pos(), m_feelers[flr],
				walls[w].From(), walls[w].To(), distToThisIP, point))
			{
				// Is this the closest found so far? If so, keep a record
				if (distToThisIP < distToClosestIP)
				{
					distToClosestIP = distToThisIP;

					closestWall = w;

					closestPoint = point;
				}
			}
		} // Next Wall

		// If an intersection point has been detected, calculate a force that will direct
		// the agent away
		if (closestWall >= 0)
		{
			// calculate by what distance the projected position of the agent will overshoot the wall
			Vector2D overshoot = m_feelers[flr] - closestPoint;

			// Create a force in the direction of the wall normal, with a magnitude of the overshoot
			steeringForce = walls[closestWall].Normal() * overshoot.Length();
		}
	} // Next feeler

	return steeringForce;
}

//--------------------------- FollowPath ------------------------------------------
// Given a series of Vector2Ds, this method produces a force that will move the
// agent along the waypoints in order. The agent uses the 'Seek' behavior to move to
// the next waypoint - unless it is the last waypoint, in which case it 'Arrives'
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::FollowPath()
{
	// Move to next target if close enough to current target (working in distance squared space)
	if (Vec2DDistanceSq(m_pPath->CurrentWaypoint(), m_pVehicle->Pos()) < m_dWaypointSeekDistSq)
	{
		m_pPath->SetNextWaypoint();
	}

	if (!m_pPath->Finished())
	{
		return Seek(m_pPath->CurrentWaypoint());
	}
	else
	{
		return Arrive(m_pPath->CurrentWaypoint(), normal);
	}
}

//--------------------------- Interpose -------------------------------------------
// Given two agents, this method returns a force that attempts to position the
// vehicle between them
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::Interpose(const Vehicle* vehicleA, const Vehicle* vehicleB)
{
	// First we need to figure out where the two agents are going to be at time T in the future
	// This is approximated by determining the time taken to reach the mid way point at 
	// the current time at a max speed
	Vector2D midPoint = (vehicleA->Pos() + vehicleB->Pos()) / 2.0;

	double timeToReachMidPoint = Vec2DDistance(m_pVehicle->Pos(), midPoint) / m_pVehicle->MaxSpeed();

	// Now that we have T, we assume that agent A and agent B will continue on a straight
	// trajectory and extrapolate to get their future positions
	Vector2D aPos = vehicleA->Pos() + vehicleA->Velocity() * timeToReachMidPoint;
	Vector2D bPos = vehicleB->Pos() + vehicleB->Velocity() * timeToReachMidPoint;

	// Calculate the mid point of these predicted positions
	midPoint = (aPos + bPos) / 2.0;

	// Then steer to arrive at it
	return Arrive(midPoint, fast);
}

//--------------------------- Hide -----------------------------------------------
// 
//--------------------------------------------------------------------------------

Vector2D SteeringBehavior::Hide(const Vehicle* hunter, const std::vector<BaseGameEntity*>& obstacles)
{
	double distToClosest = MaxDouble;
	Vector2D bestHidingSpot;

	std::vector<BaseGameEntity*>::const_iterator curOb = obstacles.begin();
	std::vector<BaseGameEntity*>::const_iterator closest;

	while (curOb != obstacles.end())
	{
		// Calculate the position of the hiding spot for this obstacle
		Vector2D hidingSpot = GetHidingPosition((*curOb)->Pos(), (*curOb)->BRadius(), hunter->Pos());

		// Work in distance-squared space to find the closest hiding spot to the agent
		double dist = Vec2DDistanceSq(hidingSpot, m_pVehicle->Pos());

		if (dist < distToClosest)
		{
			distToClosest = dist;
			bestHidingSpot = hidingSpot;
			closest = curOb;
		}

		++curOb;
	}

	// If no suitable obstacles found then Evade the hunter
	if (distToClosest == MaxFloat)
	{
		return Evade(hunter);
	}

	// Use Arrive on the hiding spot
	return Arrive(bestHidingSpot, fast);
}

//--------------------------- Cohesion -------------------------------------------
// Returns a steering force that attempts to move the agent towards the center of
// mass of the agents in its immediate area
//--------------------------------------------------------------------------------

Vector2D SteeringBehavior::Cohesion(const std::vector<Vehicle*>& agents)
{
	// First find the center of mass of all the agents
	Vector2D centerOfMass, steeringForce;
	int neighborCount = 0;

	// Iterate through the neighbors and sum up all the position vectors
	for (unsigned int a = 0; a < agents.size(); ++a)
	{
		// Make sure this agent is not included in the calculations and that the
		// agent being examined is close enough; also make sure it doesn't include
		// the evade target
		if ((agents[a] != m_pVehicle) && agents[a]->IsTagged() && (agents[a] != m_pTargetAgent1))
		{
			centerOfMass += agents[a]->Pos();
			++neighborCount;
		}
	}

	if (neighborCount > 0)
	{
		// The center of mass is the average of the sum of positions
		centerOfMass /= (double)neighborCount;

		// Now seek towards that position
		steeringForce = Seek(centerOfMass);
	}

	// The magnitude of cohesion is usually much larger than separation or allignment so it usually helps
	// to normalize it
	return Vec2DNormalize(steeringForce);
}

//--------------------------- Separation -------------------------------------------
// This calculates a force repelling from the other neighbours
//----------------------------------------------------------------------------------

Vector2D SteeringBehavior::Separation(const std::vector<Vehicle*>& agents)
{
	Vector2D steeringForce;

	for (unsigned int a = 0; a < agents.size(); ++a)
	{
		// Make sure this agent isn't included in the calculations and that the
		// agent being examined is close enough. Also, make sure it doesn't include
		// the evade target
		if (agents[a] != m_pVehicle && agents[a]->IsTagged() && agents[a] != m_pTargetAgent1)
		{
			Vector2D toAgent = m_pVehicle->Pos() - agents[a]->Pos();

			// Scale the force inversely proportional to the agents
			// distance from its neighbour
			steeringForce += Vec2DNormalize(toAgent) / toAgent.Length();
		}
	}

	return steeringForce;
}

//--------------------------- Alignment -----------------------------------------
// Returns a force that attempts to align these agents heading with that of its
// neighbours
//-------------------------------------------------------------------------------

Vector2D SteeringBehavior::Alignment(const std::vector<Vehicle*>& agents)
{
	// Used to record the average heading of the neighbours
	Vector2D averageHeading;

	// Used to count the number of vehicles in the neighborhood
	int neighbourCount = 0;

	// Iterate through all the tagged vehicles and sum their heading vectors
	for (unsigned int a = 0; a < agents.size(); ++a)
	{
		// Make sure this agent isn't included in the calculations and that the agent
		// being examined is close enough. Also, make sure it doesn't include any
		// evade target
		if (agents[a] != m_pVehicle && agents[a]->IsTagged() && agents[a] != m_pTargetAgent1)
		{
			averageHeading += agents[a]->Heading();

			++neighbourCount;
		}
	}

	// If the neighborhood contained one or more vehicles, average their heading
	// vectors
	if (neighbourCount > 0)
	{
		averageHeading /= (double)neighbourCount;

		averageHeading -= m_pVehicle->Heading();
	}

	return averageHeading;
}

//--------------------------- CohesionPlus -----------------------------------------
// Returns a steering force that attempts to move the agent towards the center of 
// mass of the agents in its immediate area
// It uses spatial partitioning
//----------------------------------------------------------------------------------

Vector2D SteeringBehavior::CohesionPlus(const std::vector<Vehicle*>& agents)
{
	// First, find the center of mass of all agents
	Vector2D centerOfMass, steeringForce;

	int neighbourCount = 0;

	// Iterate through the neighbours and sum up all the position vectors
	for (BaseGameEntity* pV = m_pVehicle->World()->CellSpace()->Begin();
		!m_pVehicle->World()->CellSpace()->End();
		pV = m_pVehicle->World()->CellSpace()->Next())
	{
		// Make sure this agent isn't included in the calculations and that the
		// agent being examined is close enough
		if (pV != m_pVehicle)
		{
			centerOfMass += pV->Pos();

			++neighbourCount;
		}
	}

	if (neighbourCount > 0)
	{
		// The center of mass is the average of the sum of positions
		centerOfMass /= (double)neighbourCount;

		// Now, seek towards that position
		steeringForce = Seek(centerOfMass);
	}

	// The magnitude of cohesion is usually much larger than separation or alignment so
	// it usually helps to normalize it
	return Vec2DNormalize(steeringForce);
}

//--------------------------- SeparationPlus ---------------------------------------
// This calculates a force repelling from the other neighbours
// It uses spatial partitioning
//----------------------------------------------------------------------------------

Vector2D SteeringBehavior::SeparationPlus(const std::vector<Vehicle*>& agents)
{
	Vector2D steeringForce;

	// Iterate through the neighbours and sum up all the position vectors
	for (BaseGameEntity* pV = m_pVehicle->World()->CellSpace()->Begin();
		!m_pVehicle->World()->CellSpace()->End();
		pV = m_pVehicle->World()->CellSpace()->Next())
	{
		// Make sure this agent isn't included in the calculations and that the
		// agent being examined is close enough
		if (pV != m_pVehicle)
		{
			Vector2D toAgent = m_pVehicle->Pos() - pV->Pos();

			// Scale the force inversely proportional to the agents distance from
			// its neighbour
			steeringForce += Vec2DNormalize(toAgent) / toAgent.Length();
		}
	}

	return steeringForce;
}

//--------------------------- AlignmentPlus ---------------------------------------
// Returns a force that attempts to align this agents with that of its neighbours
// It uses spatial partitioning
//---------------------------------------------------------------------------------

Vector2D SteeringBehavior::AlignmentPlus(const std::vector<Vehicle*>& agents)
{
	// This will record the average heading of the neighbours
	Vector2D averageHeading;

	// This count the number of vehicles in the neighbourhood
	double neighbourCount = 0.0;

	// Iterate through the neighbours and sum up all the position vectors
	for (MovingEntity* pV = m_pVehicle->World()->CellSpace()->Begin();
		!m_pVehicle->World()->CellSpace()->End();
		pV = m_pVehicle->World()->CellSpace()->Next())
	{
		// Make sure this agent isn't included in the calculations and that the agent
		// being examined is close enough
		if (pV != m_pVehicle)
		{
			averageHeading += pV->Heading();

			++neighbourCount;
		}
	}

	// If the neighbourhood contained one or more vehicles, average their
	// heading vectors.
	if (neighbourCount > 0.0)
	{
		averageHeading /= neighbourCount;
		averageHeading -= m_pVehicle->Heading();
	}

	return averageHeading;
}

// For receiving keyboard input from user
#define KEYDOWN(vkCode) ((GetAsyncKeyState(vkCode) & 0x8000) ? 1 : 0)

//--------------------------- RenderAids ---------------------------------------
//------------------------------------------------------------------------------

void SteeringBehavior::RenderAids()
{
	gdi->TransparentText();
	gdi->TextColor(Cgdi::grey);

	int nextSlot = 0;
	int slotSize = 20;

	if (KEYDOWN(VK_INSERT)) { m_pVehicle->SetMaxForce(m_pVehicle->MaxForce() + 1000.0f * m_pVehicle->TimeElapsed()); }
	if (KEYDOWN(VK_DELETE)) { if (m_pVehicle->MaxForce() > 0.2f) m_pVehicle->SetMaxForce(m_pVehicle->MaxForce() - 1000.0f * m_pVehicle->TimeElapsed()); }
	if (KEYDOWN(VK_HOME)) { m_pVehicle->SetMaxSpeed(m_pVehicle->MaxSpeed() + 50.0f * m_pVehicle->TimeElapsed()); }
	if (KEYDOWN(VK_END)) { if (m_pVehicle->MaxSpeed() > 0.2f) m_pVehicle->SetMaxSpeed(m_pVehicle->MaxSpeed() - 50.0f * m_pVehicle->TimeElapsed()); }

	if (m_pVehicle->MaxForce() < 0) m_pVehicle->SetMaxForce(0.0f);
	if (m_pVehicle->MaxSpeed() < 0) m_pVehicle->SetMaxSpeed(0.0f);

	if (m_pVehicle->ID() == 0) { gdi->TextAtPos(5, nextSlot, "MaxForce(Ins/Del):"); gdi->TextAtPos(160, nextSlot, ttos(m_pVehicle->MaxForce() / Prm.SteeringForceTweaker())); nextSlot += slotSize; }
	if (m_pVehicle->ID() == 0) { gdi->TextAtPos(5, nextSlot, "MaxSpeed(Home/End):"); gdi->TextAtPos(160, nextSlot, ttos(m_pVehicle->MaxSpeed())); nextSlot += slotSize; }

	// Render the steering force
	if (m_pVehicle->World()->RenderSteeringForce())
	{
		gdi->RedPen();
		Vector2D f = (m_vSteeringForce / Prm.SteeringForceTweaker()) * Prm.VehicleScale();
		gdi->Line(m_pVehicle->Pos(), m_pVehicle->Pos() + f);
	}

	// Render wander stuff if relevant
	if (On(BT_Wander) && m_pVehicle->World()->RenderWanderCircle())
	{
		if (KEYDOWN('F')) { m_dWanderJitter += 1.0f * m_pVehicle->TimeElapsed(); Clamp(m_dWanderJitter, 0.0f, 100.0f); }
		if (KEYDOWN('V')) { m_dWanderJitter -= 1.0f * m_pVehicle->TimeElapsed(); Clamp(m_dWanderJitter, 0.0f, 100.0f); }

		if (KEYDOWN('G')) { m_dWanderDistance += 2.0f * m_pVehicle->TimeElapsed(); Clamp(m_dWanderDistance, 0.0f, 50.0f); }
		if (KEYDOWN('B')) { m_dWanderDistance -= 2.0f * m_pVehicle->TimeElapsed(); Clamp(m_dWanderDistance, 0.0f, 50.0f); }

		if (KEYDOWN('H')) { m_dWanderRadius += 2.0f * m_pVehicle->TimeElapsed(); Clamp(m_dWanderRadius, 0.0f, 100.0f); }
		if (KEYDOWN('N')) { m_dWanderRadius += 2.0f * m_pVehicle->TimeElapsed(); Clamp(m_dWanderRadius, 0.0f, 100.0f); }

		if (m_pVehicle->ID() == 0) { gdi->TextAtPos(5, nextSlot, "Jitter(F/V):"); gdi->TextAtPos(160, nextSlot, ttos(m_dWanderJitter)); nextSlot += slotSize; }
		if (m_pVehicle->ID() == 0) { gdi->TextAtPos(5, nextSlot, "Distance(G/B):"); gdi->TextAtPos(160, nextSlot, ttos(m_dWanderDistance)); nextSlot += slotSize; }
		if (m_pVehicle->ID() == 0) { gdi->TextAtPos(5, nextSlot, "Radius(H/N):"); gdi->TextAtPos(160, nextSlot, ttos(m_dWanderRadius)); nextSlot += slotSize; }

		// Calculate the center of the wander circle
		Vector2D m_vTCC = PointToWorldSpace(Vector2D(m_dWanderDistance * m_pVehicle->BRadius(), 0), m_pVehicle->Heading(), m_pVehicle->Side(), m_pVehicle->Pos());

		// Draw the wander circle
		gdi->GreenPen();
		gdi->HollowBrush();
		gdi->Circle(m_vTCC, m_dWanderRadius * m_pVehicle->BRadius());

		// Draw the wander target
		gdi->RedPen();
		gdi->Circle(PointToWorldSpace((m_vWanderTarget + Vector2D(m_dWanderDistance, 0)) * m_pVehicle->BRadius(),
			m_pVehicle->Heading(),
			m_pVehicle->Side(),
			m_pVehicle->Pos()), 3);
	}

	//------------------------------------------------------------------------------------------------------------------------
	// Render the detection box if relevant
	//------------------------------------------------------------------------------------------------------------------------
	if (m_pVehicle->World()->RenderDetectionBox())
	{
		gdi->GreyPen();

		// A vertex buffer required for drawing the detection box
		static std::vector<Vector2D> box(4);

		double length = Prm.MinDetectionBoxLength() + (m_pVehicle->Speed() / m_pVehicle->MaxSpeed()) * Prm.MinDetectionBoxLength();

		// Verts for the detection box buffer
		box[0] = Vector2D(0, m_pVehicle->BRadius());
		box[1] = Vector2D(length, m_pVehicle->BRadius());
		box[2] = Vector2D(length, -m_pVehicle->BRadius());
		box[3] = Vector2D(0, -m_pVehicle->BRadius());

		if (!m_pVehicle->IsSmoothingOn())
		{
			box = WorldTransform(box, m_pVehicle->Pos(), m_pVehicle->Heading(), m_pVehicle->Side());
			gdi->ClosedShape(box);
		}
		else
		{
			box = WorldTransform(box, m_pVehicle->Pos(), m_pVehicle->SmoothedHeading(), m_pVehicle->SmoothedHeading().Perp());
			gdi->ClosedShape(box);
		}

		// The detection box length is proportional to the agent's velocity
		m_dDBoxLength = Prm.MinDetectionBoxLength() + (m_pVehicle->Speed() / m_pVehicle->MaxSpeed()) * Prm.MinDetectionBoxLength();

		// Tag all obstacles within range of the box for processing
		m_pVehicle->World()->TagObstaclesWithingViewRange(m_pVehicle, m_dDBoxLength);

		// This will keep track of the closest intersecting obstacle (CIB)
		BaseGameEntity* closestIntersectingObstacle = nullptr;

		// This will be used to track the distance to the CIB
		double distToClosestIP = MaxDouble;

		// This will record the transformed local coordinates of the CIB
		Vector2D localPosOfClosestObstacle;

		std::vector<BaseGameEntity*>::const_iterator curOb = m_pVehicle->World()->Obstacles().begin();

		while (curOb != m_pVehicle->World()->Obstacles().end())
		{
			// If the obstacle has been tagged within range proceed
			if ((*curOb)->IsTagged())
			{
				// Calculate this obstacle's position in local space
				Vector2D localPos = PointToLocalSpace((*curOb)->Pos(), m_pVehicle->Heading(), m_pVehicle->Side(), m_pVehicle->Pos());

				// If the local position has a negative x value then it must lay behind the agent. (in which case it can be ignored)
				if (localPos.x >= 0)
				{
					// If the distance from the x axis to the object's position is less than its radius + half the width
					// of the detection box then there is a potential intersection.
					if (fabs(localPos.y) < ((*curOb)->BRadius() + m_pVehicle->BRadius()))
					{
						gdi->ThickRedPen();
						gdi->ClosedShape(box);
					}
				}
			}

			++curOb;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	// Render the wall avoidance feelers
	//------------------------------------------------------------------------------------------------------------------------

	if (On(BT_WallAvoidance) && m_pVehicle->World()->RenderFeelers())
	{
		gdi->OrangePen();

		for (unsigned int flr = 0; flr < m_feelers.size(); ++flr)
		{
			gdi->Line(m_pVehicle->Pos(), m_feelers[flr]);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	// Render the path info
	//------------------------------------------------------------------------------------------------------------------------

	if (On(BT_FollowPath) && m_pVehicle->World()->RenderPath())
	{
		m_pPath->Render();
	}

	if (On(BT_Separation))
	{
		if (m_pVehicle->ID() == 0) { gdi->TextAtPos(5, nextSlot, "Separation(S/X):"); gdi->TextAtPos(160, nextSlot, ttos(m_dWeightSeparation / Prm.SteeringForceTweaker())); nextSlot += slotSize; }

		if (KEYDOWN('S')) { m_dWeightSeparation += 200 * m_pVehicle->TimeElapsed(); Clamp(m_dWeightSeparation, 0.0f, 50.0f * Prm.SteeringForceTweaker()); }
		if (KEYDOWN('X')) { m_dWeightSeparation -= 200 * m_pVehicle->TimeElapsed(); Clamp(m_dWeightSeparation, 0.0f, 50.0f * Prm.SteeringForceTweaker()); }
	}

	if (On(BT_Alignment))
	{
		if (m_pVehicle->ID() == 0) { gdi->TextAtPos(5, nextSlot, "Alignment(A/Z):"); gdi->TextAtPos(160, nextSlot, ttos(m_dWeightAlignment / Prm.SteeringForceTweaker())); nextSlot += slotSize; }

		if (KEYDOWN('A')) { m_dWeightAlignment += 200 * m_pVehicle->TimeElapsed(); Clamp(m_dWeightAlignment, 0.0f, 50.0f * Prm.SteeringForceTweaker()); }
		if (KEYDOWN('Z')) { m_dWeightAlignment -= 200 * m_pVehicle->TimeElapsed(); Clamp(m_dWeightAlignment, 0.0f, 50.0f * Prm.SteeringForceTweaker()); }
	}

	if (On(BT_Cohesion))
	{
		if (m_pVehicle->ID() == 0) { gdi->TextAtPos(5, nextSlot, "Cohesion(D/C):"); gdi->TextAtPos(160, nextSlot, ttos(m_dWeightCohesion / Prm.SteeringForceTweaker())); nextSlot += slotSize; }

		if (KEYDOWN('D')) { m_dWeightCohesion += 200 * m_pVehicle->TimeElapsed(); Clamp(m_dWeightSeparation, 0.0f, 50.0f * Prm.SteeringForceTweaker()); }
		if (KEYDOWN('C')) { m_dWeightCohesion -= 200 * m_pVehicle->TimeElapsed(); Clamp(m_dWeightSeparation, 0.0f, 50.0f * Prm.SteeringForceTweaker()); }
	}

	if (On(BT_FollowPath))
	{
		double sd = sqrt(m_dWaypointSeekDistSq);
		if (m_pVehicle->ID() == 0) { gdi->TextAtPos(5, nextSlot, "SeekDistance(D/C):"); gdi->TextAtPos(160, nextSlot, ttos(sd)); nextSlot += slotSize; }

		if (KEYDOWN('D')) { m_dWaypointSeekDistSq += 1.0; }
		if (KEYDOWN('C')) { m_dWaypointSeekDistSq -= 1.0; Clamp(m_dWaypointSeekDistSq, 0.0f, 400.0f); }
	}
}

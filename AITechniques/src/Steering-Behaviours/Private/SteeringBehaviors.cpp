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

SteeringBehavior::SteeringBehavior(Vehicle * agent)
{
}

SteeringBehavior::~SteeringBehavior()
{
}

Vector2D SteeringBehavior::Calculate()
{
	return Vector2D();
}

double SteeringBehavior::ForwardComponent()
{
	return 0.0;
}

double SteeringBehavior::SideComponent()
{
	return 0.0;
}

void SteeringBehavior::RenderAids()
{
}

bool SteeringBehavior::AccumulateForce(Vector2D & sf, Vector2D & forceToAdd)
{
	return false;
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

#pragma once

#include <list>

#include "BaseGameEntity.h"
#include "Common/Public/2D/Geometry.h"

// --------------------- Overlapped  -------------------------------------
// Tests to see if an entity is overlapping any of a number of entities
// stored in a std container
// -----------------------------------------------------------------------

template<class T, class conT>
bool Overlapped(const T* ob, const conT& conOb, double minDistBetweenObstacles = 40.0)
{
	typename conT::const_iterator it;

	for (it = conOb.begin(); it != conOb.end(); ++it)
	{
if (TwoCirclesOverlapped(
	ob->Pos(),
	ob->BRadius() + minDistBetweenObstacles,
	(*it)->Pos(),
	(*it)->BRadius()))
{
	return true;
}
	}

	return false;
}

// --------------------- TagNeighbors   -------------------------------------
// Tags any entites contained in a std container that are within the radius
// of the single entity parameter
// --------------------------------------------------------------------------

template<class T, class conT>
void TagNeighbors(const T& entity, conT& containerOfEntities, double radius)
{
	// Iterate through all entities checking for range
	for (typename conT::iterator curEntity = containerOfEntities.begin();
		curEntity != containerOfEntities.end();
		++curEntity)
	{
		// First clear any current tag
		(*curEntity)->UnTag();

		Vector2D to = (*curEntity)->Pos() - entity->Pos();

		// The bounding radius of the other is taken into account by adding it
		// to the range
		double range = radius + (*curEntity)->BRadius();

		// If entity is within range, tag for further consideration
		if (((*curEntity) != entity) && (to.LengthSq() < range*range))
		{
			(*curEntity)->Tag();
		}
	}
}

// --------------------- EnforceNonPenetrationConstraint --------------------
// Given a pointer to an entity and a std container of pointers to nearby
// entities, this function checks to see if there is an overlap between entities.
// If there is, then the entities are moved away from each other
// --------------------------------------------------------------------------

template<class T, class conT>
void EnforceNonPenetrationConstraint(const T& entity, const conT& containerOfEntities)
{
	// Iterate through all entities checking for any overlap of bounding radius
	for (typename conT::const_iterator curEntity = containerOfEntities.begin();
		curEntity != containerOfEntities.end();
		++curEntity)
	{
		// Make sure we don't check against the individual
		if (*curEntity == entity) continue;

		// Calculate the distance between the positions of the entities
		Vector2D toEntity = entity->Pos() - (*curEntity)->Pos();

		double distFromEachOther = toEntity.Length();

		// If this distance is smaller than the sum of their radius then this
		// entity must be moved away in the direction parallel to the toEntity
		// vector
		double amountOfOverlap = (*curEntity)->BRadius() + entity->BRadius() - distFromEachOther;

		if (amountOfOverlap >= 0)
		{
			// Move the entity a distance away equivalent to the amount of overlap
			entity->SetPos(entity->Pos() + (toEntity / distFromEachOther) * amountOfOverlap);
		}
	}
}

// --------------------- GetEntityLineSegmentIntersections  --------------------
// Tests a line segment AB against a container of entities. First of all, a test
// is made to confirm that the entity is within a specified range of the 
// one_to_ignore (positioned at A). If within range the intersection test is made.
// 
// Returns a list of all the entities that tested positive for intersection
// -----------------------------------------------------------------------------

template<class T, class conT>
std::list<T> GetEntityLineSegmentIntersections(const conT& entities, int toIgnore,
	Vector2D a, Vector2D b, double range = MaxDouble)
{
	typename conT::const_iterator it;

	std::list<T> hits;

	// Iterate through all entities checking against the line segment AB
	for (it = entities.begin(); it != entities.end(); ++it)
	{
		// If not within range or the entity being checked is the one to ignore
		// just continue with the next entity
		bool sameId = (*it)->ID() == toIgnore;
		bool withinRange = Vec2DDistanceSq((*it)->Pos(), a) <= range * range;

		if (sameId || !withinRange) continue;

		// If the distance to AB is less than the entities bounding radius then
		// There is an intersection so add it to hits
		if (DistToLineSegment(a, b, (*it)->Pos()) < (*it)->BRadius())
		{
			hits.push_back(*it);
		}
	}

	return hits;
}
#pragma once

#include <vector>
#include <list>
#include <cassert>

#include "Common/Public/2D/Vector2D.h"
#include "Common/Public/2D/InvertedAABox2D.h"
#include "Common/Public/Misc/Utils.h"

//--------------------------------------------------------------------------
// Defines a cell containing a list of pointers to entities
//--------------------------------------------------------------------------

template<class Entity>
struct Cell
{
	// All the entities inhabiting this cell
	std::list<Entity> m_members;

	// The cell's bounding box (it's inverted because the Window's default 
	// co-ordinate system has a y axis that increases as it descends)
	InvertedAABox2D m_bBox;

	Cell(Vector2D topLeft, Vector2D botRight) : m_bBox(InvertedAABox2D(topLeft, botRight)) {}
};

//--------------------------------------------------------------------------
// The subdivision class. Divides a 2D Space into a grid of cells each of which
// may contain a number of entities. Once created and initialized with 
// entities, fast proximity querys can be made by calling the CalculateNeighbours
// method with a position and proximity radius.
//
// If an entity is capable of moving, and therefore capable of moving between
// cells, the Update method should be called each update-cycle to synchronize
// the entity and the cell space it occupies.
//--------------------------------------------------------------------------

template<class Entity>
class CellSpacePartition
{

};

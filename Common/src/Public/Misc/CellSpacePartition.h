#pragma once

#include <vector>
#include <list>
#include <iterator>
#include <cassert>

#include "Public/2D/Vector2D.h"
#include "Public/2D/InvertedAABox2D.h"
#include "Public/Misc/Utils.h"

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
private:

	// The required amount of cells in the space
	std::vector<Cell<Entity>> m_cells;

	// This is used to store any valid neighbors when an agent searches
	// its neighboring space
	std::vector<Entity> m_neighbors;

	// This iterator will be used by the methods next and begin to traverse
	// through the above vector of neighbors
	typename std::vector<Entity>::iterator m_curNeighbor;

	// The width and height of the world space the entities inhabit
	double m_dSpaceWidth;
	double m_dSpaceHeight;

	// The number of cells the space is going to be divided up into
	int m_iNumCellsX;
	int m_iNumCellsY;

	double m_dCellSizeX;
	double m_dCellSizeY;

	//----------------------- PositionToIndex -----------------------------------
	// Given a 2D vector representing a position within the game world, this
	// method calculates an index into its appropriate cell
	//---------------------------------------------------------------------------

	inline size_t PositionToIndex(const Vector2D& pos) const
	{
		size_t idx =
			(int)(m_iNumCellsX * pos.x / m_dSpaceWidth) +
			((int)(m_iNumCellsY * pos.y / m_dSpaceHeight) * m_iNumCellsX);

		// If the entity's position is equal to vector2d(m_dSpaceWidth, m_dSpaceHeight)
		// then the index will overshoot. We need to check for this and adjust
		if (idx > m_cells.size() - 1) idx = m_cells.size() - 1;

		return idx;
	}

public:

	// Main ctor
	CellSpacePartition(double width, double height, int cellsX, int cellsY, int maxEntities)
		:m_dSpaceWidth(width),
		m_dSpaceHeight(height),
		m_iNumCellsX(cellsX),
		m_iNumCellsY(cellsY),
		m_neighbors(maxEntities, Entity())
	{
		// Calculate bounds of each cell
		m_dCellSizeX = width / cellsX;
		m_dCellSizeY = height / cellsY;

		// Create the cells
		for (int y = 0; y < m_iNumCellsY; ++y)
		{
			for (int x = 0; x < m_iNumCellsX; ++x)
			{
				double left = x * m_dCellSizeX;
				double right = left + m_dCellSizeX;
				double top = y * m_dCellSizeY;
				double bottom = top + m_dCellSizeY;

				m_cells.push_back(Cell<Entity>(Vector2D(left, top), Vector2D(right, bottom)));
			}
		}
	}

	//----------------------- AddEntity --------------------------------------
	// Used to add the entities into the data structure
	//------------------------------------------------------------------------

	inline void AddEntity(const Entity& entity)
	{
		assert(entity);

		size_t sz = m_cells.size();
		size_t idx = PositionToIndex(entity->Pos());

		m_cells[idx].m_members.push_back(entity);
	}

	//----------------------- UpdateEntity -----------------------------------
	// Checks to see if an entity has moved cells. If so, the data structure
	// is updated accordingly
	//------------------------------------------------------------------------

	inline void UpdateEntity(const Entity& entity, Vector2D& oldPos)
	{
		// If the index for the old pos and the new pos are not equal then the entity
		// has moved to another cell
		size_t oldIdx = PositionToIndex(oldPos);
		size_t newIdx = PositionToIndex(entity->Pos());

		if (newIdx == oldIdx) return;

		// The entity has moved into another cell so delete from current cell and
		// add to new one
		m_cells[oldIdx].m_members.remove(entity);
		m_cells[newIdx].m_members.push_back(entity);
	}

	//----------------------- CalculateNeighbors -----------------------------------
	// This must be called to create the vector of neighbors. This method
	// examines each cell within range of the target, if the cells contain
	// entities then they're tested to see if they're situated within the 
	// target's neighborhood region. If they are added to neighbor list
	//----------------------------------------------------------------------

	inline void CalculateNeighbors(Vector2D& targetPos, double queryRadius)
	{
		// Create an iterator and set it to the beginning of the neighbor vector
		std::vector<Entity>::iterator curNeighbor = m_neighbors.begin();

		// Create the query box that is the bounding box of the target's query area
		InvertedAABox2D queryBox(
			targetPos - Vector2D(queryRadius, queryRadius),
			targetPos + Vector2D(queryRadius, queryRadius)
		);

		// Iterate through each cell and test to see if its bounding box overlaps with the query box. 
		// If it does and it also contains entities then make further proximity tests
		std::vector<Cell<Entity>>::iterator curCell;
		for (curCell = m_cells.begin(); curCell != m_cells.end(); ++curCell)
		{
			// Test to see if this cell contains members and if it overlaps the query box
			if ((*curCell).m_bBox.IsOverlappedWith(queryBox) && !(*curCell).m_members.empty())
			{
				// Add any entities found within query radius to the neighbor list
				std::list<Entity>::iterator it = curCell->m_members.begin();
				for (it; it != curCell->m_members.end(); ++it)
				{
					if (Vec2DDistanceSq((*it)->Pos(), targetPos) < (queryRadius * queryRadius))
					{
						*curNeighbor++ = *it;
					}
				}
			}
		}

		// Mark the end of the list with a zero-null
		*curNeighbor = nullptr;
	}

	//----------------------- EmptyCells -----------------------------------
	// Clears the cells of all entities
	//----------------------------------------------------------------------

	inline void EmptyCells()
	{
		std::vector<Cell<Entity>>::iterator it = m_cells.begin();

		for (it; it != m_cells.end(); ++it)
		{
			(*it).m_members.clear();
		}
	}

	//----------------------- RenderCells -----------------------------------
	//-----------------------------------------------------------------------

	inline void RenderCells() const
	{
		std::vector<Cell<Entity>>::const_iterator curCell;
		for (curCell = m_cells.cbegin(); curCell != m_cells.cend(); ++curCell)
		{
			(*curCell).m_bBox.Render(false);
		}
	}

	// Returns a reference to the entity at the front of the neighbor vector
	inline Entity& Begin() { m_curNeighbor = m_neighbors.begin(); return *m_curNeighbor; }

	// Returns the next entity in the neighbor vector
	inline Entity& Next() { ++m_curNeighbor; return *m_curNeighbor; }

	// Returns true if the end of the vector is found (a zero value marks the end)
	inline bool End() { return (m_curNeighbor == m_neighbors.end()) || (*m_curNeighbor == nullptr); }
};

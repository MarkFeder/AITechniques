#include "Common/Public/Misc/CellSpacePartition.h"

//----------------------- Constructor -----------------------------------

template<class Entity>
inline CellSpacePartition<Entity>::CellSpacePartition(double width, double height, int cellsX, int cellsY, int maxEntities)
	:m_dSpaceWidth(width),
	m_dSpaceHeight(height),
	m_dCellSizeX(cellsX),
	m_dCellSizeY(cellsY),
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

//----------------------- PositionToIndex -----------------------------------
// Given a 2D vector representing a position within the game world, this
// method calculates an index into its appropriate cell
//---------------------------------------------------------------------------

template<class Entity>
inline int CellSpacePartition<Entity>::PositionToIndex(const Vector2D& pos) const
{
	int idx = 
		(int)(m_iNumCellsX * pos.x / m_dSpaceWidth) +
		((int)(m_iNumCellsY * pos.y / m_dSpaceHeight) * m_iNumCellsX);

	// If the entity's position is equal to vector2d(m_dSpaceWidth, m_dSpaceHeight)
	// then the index will overshoot. We need to check for this and adjust
	if (idx > m_cells.size() - 1) idx = m_cells.size() - 1;

	return idx;
}

//----------------------- AddEntity --------------------------------------
// Used to add the entities into the data structure
//------------------------------------------------------------------------

template<class Entity>
inline void CellSpacePartition<Entity>::AddEntity(const Entity& entity)
{
	assert(entity);

	int sz = m_cells.size();
	int idx = PositionToIndex(entity->Pos());

	m_cells[idx].m_members.push_back(entity);
}

//----------------------- UpdateEntity -----------------------------------
// Checks to see if an entity has moved cells. If so, the data structure
// is updated accordingly
//------------------------------------------------------------------------

template<class Entity>
inline void CellSpacePartition<Entity>::UpdateEntity(const Entity& entity, Vector2D& oldPos)
{
	// If the index for the old pos and the new pos are not equal then the entity
	// has moved to another cell
	int oldIdx = PositionToIndex(oldPos);
	int newIdx = PositionToIndex(entity->Pos());

	if (newIdx == oldIdx) return;

	// The entity has moved into another cell so delete from current cell and
	// add to new one
	m_cells[oldIdx].m_members.remove(entity);
	m_cells[newIdx].m_members.push_back(entity);
}

//----------------------- EmptyCells -----------------------------------
// This must be called to create the vector of neighbors. This method
// examines each cell within range of the target, if the cells contain
// entities then they're tested to see if they're situated within the 
// target's neighborhood region. If they are added to neighbor list
//----------------------------------------------------------------------

template<class Entity>
inline void CellSpacePartition<Entity>::CalculateNeighbors(Vector2D& targetPos, double queryRadius)
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
		if (curCell->BBox.isOverlappedWith(queryBox) && !curCell->m_members.empty())
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

template<class Entity>
inline void CellSpacePartition<Entity>::EmptyCells()
{
	std::vector<Cell<Entity>>::iterator it = m_cells.begin();

	for (it; it != m_cells.end(); ++it)
	{
		(*it)->m_members.clear();
	}
}

//----------------------- RenderCells -----------------------------------
//-----------------------------------------------------------------------

template<class Entity>
inline void CellSpacePartition<Entity>::RenderCells() const
{
	std::vector<Cell<Entity>>::const_iterator curCell;
	for (curCell = m_cells.begin(); curCell != m_cells.end(); ++curCell)
	{
		(*curCell)->BBox.Render(false);
	}
}

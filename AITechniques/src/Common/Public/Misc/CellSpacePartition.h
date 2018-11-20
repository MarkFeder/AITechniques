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

	// Given a position in the game space this method determines the
	// relevant cell's index
	inline int PositionToIndex(const Vector2D& pos) const;

public:

	// Main ctor
	CellSpacePartition(
		double width, // width of the environment
		double height, // height of the environment
		int cellsX, // number of cells horizontally
		int cellsY, // number of cells vertically
		int maxEntities); // maximum number of entities to add

	// Adds entities to the class by allocating them to the appropriate cell
	inline void AddEntity(const Entity& entity);

	// Update an entity's cell by calling this from your entity's update method
	inline void UpdateEntity(const Entity& entity, Vector2D& oldPos);

	// This method calculates all a target's neighbors and stores them in
	// the neighbor vector. After you have called this method use the begin, next
	// and end methods to iterate through the vector
	inline void CalculateNeighbors(Vector2D& targetPos, double queryRadius);

	// Returns a reference to the entity at the front of the neighbor vector
	inline Entity& Begin() { m_curNeighbor = m_neighbors.begin(); return *m_curNeighbor; }

	// Returns the next entity in the neighbor vector
	inline Entity& Next() { ++m_curNeighbor; return *m_curNeighbor; }

	// Returns true if the end of the vector is found (a zero value marks the end)
	inline bool End() { return (m_curNeighbor == m_neighbors.end()) || (*m_curNeighbor == nullptr); }

	// Empties the cells of entities
	void EmptyCells();

	// Call this to use the gdi to render the cell edges
	inline void RenderCells() const;
};

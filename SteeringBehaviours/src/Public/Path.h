#pragma once

#include <list>
#include <cassert>

#include "Common/Public/2D/Vector2D.h"

class Path
{
private:

	std::list<Vector2D> m_wayPoints;

	// Points to the current waypoint
	std::list<Vector2D>::iterator m_curWaypoint;

	// Flag to indicate if the path should be looped
	// (the last waypoint connected to the first)
	bool m_bLooped;

public:

	Path() = default;

	// Constructor for creating a path with initial random waypoints. 
	// MinX/Y & MaxX/Y define the bounding box of the path.
	Path(int numWaypoints, double minX, double minY, double maxX, double maxY, bool looped)
		: m_bLooped(looped)
	{
		CreateRandomPath(numWaypoints, minX, minY, maxX, maxY);
	}

	// Creates a random path which is bound by rectangle described by the min/max values
	void CreateRandomPath(int numWaypoints, double minX, double minY, double maxX, double maxY);

	// Adds a waypoint to the end of the path
	void AddWayPoint(Vector2D newPoint) { m_wayPoints.push_back(newPoint); }

	void SetNextWaypoint();

	// Methods for setting the path with either another Path or a list of vectors
	void Set(std::list<Vector2D> newPath) { m_wayPoints = newPath; m_curWaypoint = m_wayPoints.begin(); }
	void Set(const Path& path) { m_wayPoints = path.GetPath(); m_curWaypoint = m_wayPoints.begin(); }

	// Returns the current waypoint
	Vector2D CurrentWaypoint() const { return *m_curWaypoint; }

	// Returns true if the end of the list has been reached
	bool Finished() const { return !(m_curWaypoint != m_wayPoints.end()); }

	std::list<Vector2D> GetPath() const { return m_wayPoints; }

	void LoopOn() { m_bLooped = true; }
	void LoopOff() { m_bLooped = false; }

	void Clear() { m_wayPoints.clear(); }

	// Renders the path in orange
	void Render() const;
};
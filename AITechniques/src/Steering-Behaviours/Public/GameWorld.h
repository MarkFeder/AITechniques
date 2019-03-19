#pragma once

#include <windows.h>
#include <vector>

#include "Common/Public/2D/Vector2D.h"
#include "Common/Public/Time/PrecisionTimer.h"
#include "Common/Public/Misc/CellSpacePartition.h"
#include "Common/Public/Entities/BaseGameEntity.h"
#include "Common/Public/Entities/EntityTemplates.h"
#include "Vehicle.h"

class Obstacle;
class Wall2D;
class Path;

typedef std::vector<BaseGameEntity*>::iterator ObIt;

class GameWorld
{
private:

	// A container of all the moving entities
	std::vector<Vehicle*> m_vehicles;

	// Any obstacles
	std::vector<BaseGameEntity*> m_obstacles;

	// Container containing any walls in the environment
	std::vector<Wall2D> m_Walls;

	CellSpacePartition<Vehicle*>* m_pCellSpace;

	// Any path we may create for the vehicles to follow
	Path* m_pPath;

	// Set true to pause the motion
	bool m_bPaused;

	// Local copy of client window dimensions
	int m_cxClient, m_cyClient;

	// The position of the crosshair
	Vector2D m_vCrosshair;

	// Keeps track of the average FPS
	double m_dAvFrameTime;

	// Flags to turn aids and obstacles, etc. on/off
	bool m_bShowWalls;
	bool m_bShowObstacles;
	bool m_bShowPath;
	bool m_bShowDetectionBox;
	bool m_bShowWanderCircle;
	bool m_bShowFeelers;
	bool m_bShowSteeringForce;
	bool m_bShowFPS;
	bool m_bRenderNeighbors;
	bool m_bViewKeys;
	bool m_bShowCellSpaceInfo;

	void CreateObstacles();

	void CreateWalls();

public:

	GameWorld(int cx, int cy);

	~GameWorld();

	void Update(double timeElapsed);

	void Render();

	void NonPenetrationConstraint(Vehicle* v) { EnforceNonPenetrationConstraint(v, m_vehicles); }
	
	void TagVehiclesWithinViewRange(BaseGameEntity* pVehicle, double range)
	{
		TagNeighbors(pVehicle, m_vehicles, range);
	}

	void TagObstaclesWithingViewRange(BaseGameEntity* pVehicle, double range)
	{
		TagNeighbors(pVehicle, m_obstacles, range);
	}

	const std::vector<Wall2D>& Walls() const { return m_Walls; }
	CellSpacePartition<Vehicle*>* CellSpace() const { return m_pCellSpace; }
	const std::vector<BaseGameEntity*>& Obstacles() const { return m_obstacles; }
	const std::vector<Vehicle*>& Agents() const { return m_vehicles; }

	// Handle WM_COMMAND messages
	void HandleYKey();
	void HandleUKey();
	void HandleIKey();
	void HandleKeyPresses(WPARAM wParam);

	void ToggleObstacles();
	void ToggleWalls();
	void ToggleMenuSmoothing();
	void ToggleSpacePartitioning();
	void ToggleCellSpacePartitioning();
	void ToggleSumming();
	void TogglePrioritizeSummingMethod();
	void HandleMenuItems(WPARAM wParam, HWND hwnd);

	void TogglePause() { m_bPaused = !m_bPaused; }
	bool Paused() const { return m_bPaused; }

	Vector2D Crosshair() const { return m_vCrosshair; }
	void SetCrosshair(POINTS p);
	void SetCrosshair(Vector2D v) { m_vCrosshair = v; }

	// Accesors
	int cxClient() const { return m_cxClient; }
	int cyClient() const { return m_cyClient; }

	bool RenderWalls() const { return m_bShowWalls; }
	bool RenderObstacles() const { return m_bShowObstacles; }
	bool RenderPath() const { return m_bShowPath; }
	bool RenderDetectionBox() const { return m_bShowDetectionBox; }
	bool RenderWanderCircle() const { return m_bShowWanderCircle; }
	bool RenderFeelers() const { return m_bShowFeelers; }
	bool RenderSteeringForce() const { return m_bShowSteeringForce; }

	bool RenderFPS() const { return m_bShowFPS; }
	void ToogleShowFPS() { m_bShowFPS = !m_bShowFPS; }

	void ToggleRenderNeighbors() { m_bRenderNeighbors = !m_bRenderNeighbors; }
	bool RenderNeighbors() const { return m_bRenderNeighbors; }

	void ToggleViewKeys() { m_bViewKeys = !m_bViewKeys; }
	bool ViewKeys() const { return m_bViewKeys; }
};
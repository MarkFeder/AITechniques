#include <list>

#include "Steering-Behaviours/Public/GameWorld.h"
#include "Steering-Behaviours/Public/Vehicle.h"
#include "Steering-Behaviours/Public/ParamLoader.h"
// #include "Steering-Behaviours/Public/SteeringBehaviors.h"
#include "Common/Public/2D/Geometry.h"
#include "Common/Public/2D/Wall2D.h"
#include "Common/Public/2D/Transformations.h"
#include "Common/Public/2D/Geometry.h"
#include "Common/Public/Misc/Smoother.h"
// #include "Common/Public/Misc/WindowsUtils.h"
// #include "Common/Public/Misc/StreamUtils.h"

//------------------------------- ctor -----------------------------------
//------------------------------------------------------------------------

GameWorld::GameWorld(int cx, int cy)
	:m_cxClient(cx),
	m_cyClient(cy),
	m_bPaused(false),
	m_vCrosshair(Vector2D(cxClient() / 2.0, cyClient() / 2.0)),
	m_bShowWalls(false),
	m_bShowObstacles(false),
	m_bShowPath(false),
	m_bShowWanderCircle(false),
	m_bShowSteeringForce(false),
	m_bShowFeelers(false),
	m_bShowDetectionBox(false),
	m_bShowFPS(false),
	m_dAvFrameTime(0),
	m_pPath(nullptr),
	m_bRenderNeighbors(false),
	m_bViewKeys(false),
	m_bShowCellSpaceInfo(false)
{
	// Setup the spatial subdivision class
	m_pCellSpace = new CellSpacePartition<Vehicle*>((double)cx, (double)cy, Prm.NumCellsX(), Prm.NumCellsY(), Prm.NumAgents());

	double border = 30;
	// m_pPath = new Path(5, border, border, cx - border, cy - border, true);

	// Setup the agents
	// for (int a = 0; a < )
}

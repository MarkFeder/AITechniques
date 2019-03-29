#include "Public/GameWorld.h"
#include "Public/Vehicle.h"
#include "Public/Constants.h"
#include "Public/ParamLoader.h"
#include "Public/Path.h"
#include "Public/Obstacle.h"
#include "Public/SteeringBehaviors.h"
#include "Public/Resource.h"
#include "Public/2D/Geometry.h"
#include "Public/2D/Wall2D.h"
#include "Public/2D/Transformations.h"
#include "Public/2D/Geometry.h"
#include "Public/Misc/Smoother.h"
#include "Public/Misc/StreamUtils.h"
#include "Public/Misc/WindowsUtils.h"

#include <list>

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
	m_pPath = new Path(5, border, border, cx - border, cy - border, true);

	// Setup the agents
	for (int a = 0; a < Prm.NumAgents(); ++a)
	{
		// Determine a random starting position
		Vector2D spawnPos = Vector2D(cx / 2.0 + RandomClamped() * cx / 2.0, cy / 2.0 + RandomClamped() * cy / 2.0);

		Vehicle* pVehicle = new Vehicle(
			this,
			spawnPos, // initial position
			RandFloat() * TwoPi, // start rotation
			Vector2D(0, 0), // velocity
			Prm.VehicleMass(), // mass
			Prm.MaxSteeringForce(), // max force
			Prm.MaxSpeed(), // max velocity
			Prm.MaxTurnRatePerSecond(), // max turn rate
			Prm.VehicleScale() // scale
		);

		pVehicle->Steering()->FlockingOn();

		m_vehicles.push_back(pVehicle);

		// Add it to the cell subdivision
		m_pCellSpace->AddEntity(pVehicle);
	}

#define SHOAL
#ifdef SHOAL
		m_vehicles[Prm.NumAgents() - 1]->Steering()->FlockingOff();
		m_vehicles[Prm.NumAgents() - 1]->SetScale(Vector2D(10, 10));
		m_vehicles[Prm.NumAgents() - 1]->Steering()->WanderOn();
		m_vehicles[Prm.NumAgents() - 1]->SetMaxSpeed(70);

		for (int i = 0; i < Prm.NumAgents(); ++i)
		{
			m_vehicles[i]->Steering()->EvadeOn(m_vehicles[Prm.NumAgents() - 1]);
		}
#endif

		// Create any obstacles or walls
		 CreateObstacles();
		 CreateWalls();
}

//------------------------------- dtor -----------------------------------
//------------------------------------------------------------------------

GameWorld::~GameWorld()
{
	for (unsigned int a = 0; a < m_vehicles.size(); ++a)
	{
		delete m_vehicles[a];
	}

	for (unsigned int ob = 0; ob < m_obstacles.size(); ++ob)
	{
		delete m_obstacles[ob];
	}

	delete m_pCellSpace;

	delete m_pPath;
}

//------------------------------- Update  -----------------------------------
//---------------------------------------------------------------------------

void GameWorld::Update(double timeElapsed)
{
	if (m_bPaused) return;

	// Create a smoother to smooth the framerate
	const int sampleRate = 10;
	static Smoother<double> frameRateSmoother(sampleRate, 0.0);

	m_dAvFrameTime = frameRateSmoother.Update(timeElapsed);

	// Update the vehicles
	for (unsigned int a = 0; a < m_vehicles.size(); ++a)
	{
		m_vehicles[a]->Update(timeElapsed);
	}
}

//------------------------------- CreateWalls  -----------------------------------
// Creates some walls that form an enclosure for the steering agents.
// Used to demonstrate several of the steering behaviors
//--------------------------------------------------------------------------------

void GameWorld::CreateWalls()
{
	// Create the walls
	double borderSize = 20.0;
	double cornerSize = 0.2;

	double vDist = m_cyClient - 2 * borderSize;
	double hDist = m_cxClient - 2 * borderSize;

	const int numWallVerts = 8;

	Vector2D walls[numWallVerts] = {
		Vector2D(hDist * cornerSize + borderSize, borderSize),
		Vector2D(m_cxClient - borderSize - hDist * cornerSize, borderSize),
		Vector2D(m_cxClient - borderSize, borderSize + vDist * cornerSize),
		Vector2D(m_cxClient - borderSize, m_cyClient - borderSize - vDist * cornerSize),
		Vector2D(m_cxClient - borderSize - hDist * cornerSize, m_cyClient - borderSize),
		Vector2D(hDist * cornerSize + borderSize, m_cyClient - borderSize),
		Vector2D(borderSize, m_cyClient - borderSize - vDist * cornerSize),
		Vector2D(borderSize, borderSize + vDist * cornerSize)
	};

	for (int w = 0; w < numWallVerts - 1; ++w)
	{
		m_Walls.push_back(Wall2D(walls[w], walls[w + 1]));
	}

	m_Walls.push_back(Wall2D(walls[numWallVerts - 1], walls[0]));
}

//------------------------------- CreateObstacles  -----------------------------------
// Sets up the vector of obstacles with random positions and sizes. Makes sure the
// obstacles do not overlap
//------------------------------------------------------------------------------------

void GameWorld::CreateObstacles()
{
	// Create a number of randomly sized tiddylywinks
	for (int o = 0; o < Prm.NumObstacles(); ++o)
	{
		bool bOverlapped = true;

		// Keep creating tiddlywinks until we find one that does not overlap any others.
		// Sometimes this can get into an endless loop because the obstacle has nowhere to fit.
		// We test for this case and exit accordingly.

		int numTries = 0;
		int numAllowableTries = 2000;

		while (bOverlapped)
		{
			numTries++;

			if (numTries > numAllowableTries) return;

			int radius = RandInt((int)Prm.MinObstacleRadius(), (int)Prm.MaxObstacleRadius());

			const int border = 10;
			const int minGapBetweenObstacles = 20;

			Obstacle* ob = new Obstacle(
				RandInt(radius + border, m_cxClient - radius - border),
				RandInt(radius + border, m_cyClient - radius - 30 - border),
				radius
			);

			if (!Overlapped(ob, m_obstacles, minGapBetweenObstacles))
			{
				// It's not overlapped so we can add it
				m_obstacles.push_back(ob);

				bOverlapped = false;
			}
			else
			{
				delete ob;
			}
		}
	}
}

//------------------------------- Set Crosshair -----------------------------------
// The user can set the position of the crosshair by right clicking the mouse.
// This method makes sure the click is not inside any enabled obstacles and sets
// the position appropriately
//---------------------------------------------------------------------------------

void GameWorld::SetCrosshair(POINTS p)
{
	Vector2D proposedPosition((double)p.x, (double)p.y);

	// Make sure it's not inside an obstacle
	for (ObIt curOb = m_obstacles.begin(); curOb != m_obstacles.end(); ++curOb)
	{
		if (PointInCircle((*curOb)->Pos(), (*curOb)->BRadius(), proposedPosition)) return;
	}

	m_vCrosshair.x = (double)p.x;
	m_vCrosshair.y = (double)p.y;
}

//------------------------------- HandleKeyPresses -----------------------------------
//------------------------------------------------------------------------------------

void GameWorld::HandleYKey()
{
	m_bShowObstacles = !m_bShowObstacles;

	if (!m_bShowObstacles)
	{
		m_obstacles.clear();

		for (unsigned int i = 0; i < m_vehicles.size(); ++i)
		{
			m_vehicles[i]->Steering()->ObstacleAvoidanceOff();
		}
	}
	else
	{
		CreateObstacles();

		for (unsigned int i = 0; i < m_vehicles.size(); ++i)
		{
			m_vehicles[i]->Steering()->ObstacleAvoidanceOn();
		}
	}
}

void GameWorld::HandleUKey()
{
	if (m_pPath)
	{
		delete m_pPath;
		double border = 60;

		m_pPath = new Path(RandInt(3, 7), border, border, cxClient() - border, cyClient() - border, true);
		m_bShowPath = true;

		for (unsigned int i = 0; i < m_vehicles.size(); ++i)
		{
			m_vehicles[i]->Steering()->SetPath(m_pPath->GetPath());
		}
	}
}

void GameWorld::HandleIKey()
{
	for (unsigned int i = 0; i < m_vehicles.size(); ++i)
	{
		m_vehicles[i]->ToggleSmoothing();
	}
}

void GameWorld::HandleKeyPresses(WPARAM wParam)
{
	switch (wParam)
	{
		case 'U':
			HandleUKey();
			break;

		case 'P':
			TogglePause();
			break;

		case 'O':
			ToggleRenderNeighbors();
			break;

		case 'I':
			HandleIKey();
			break;

		case 'Y':
			HandleYKey();
			break;

		default: 
			break;
	}
}

//------------------------------- HandleMenuItems -----------------------------------
//-----------------------------------------------------------------------------------

void GameWorld::ToggleObstacles(HWND hwnd)
{
	m_bShowObstacles = !m_bShowObstacles;

	if (!m_bShowObstacles)
	{
		m_obstacles.clear();

		for (unsigned int i = 0; i < m_vehicles.size(); ++i)
		{
			m_vehicles[i]->Steering()->ObstacleAvoidanceOff();
		}

		// Uncheck the menu
		ChangeMenuState(hwnd, ID_OB_OBSTACLES, MFS_UNCHECKED);
	}
	else
	{
		CreateObstacles();

		for (unsigned int i = 0; i < m_vehicles.size(); ++i)
		{
			m_vehicles[i]->Steering()->ObstacleAvoidanceOn();
		}

		// Check the menu
		ChangeMenuState(hwnd, ID_OB_OBSTACLES, MFS_CHECKED);
	}
}

void GameWorld::ToggleWalls(HWND hwnd)
{
	m_bShowWalls = !m_bShowWalls;

	if (m_bShowWalls)
	{
		CreateWalls();

		for (unsigned int i = 0; i < m_vehicles.size(); ++i)
		{
			m_vehicles[i]->Steering()->WallAvoidanceOn();
		}

		// Check the menu
		ChangeMenuState(hwnd, ID_OB_WALLS, MFS_CHECKED);
	}
	else
	{
		m_Walls.clear();

		for (unsigned int i = 0; i < m_vehicles.size(); ++i)
		{
			m_vehicles[i]->Steering()->WallAvoidanceOff();
		}

		// Uncheck the menu
		ChangeMenuState(hwnd, ID_OB_WALLS, MFS_UNCHECKED);
	}
}

void GameWorld::ToggleMenuSmoothing(HWND hwnd)
{
	if (m_vehicles.size() > 0)
	{
		for (unsigned int i = 0; i < m_vehicles.size(); ++i)
		{
			m_vehicles[i]->ToggleSmoothing();
		}

		CheckMenuItemAppropriately(hwnd, ID_MENU_SMOOTHING, m_vehicles[0]->IsSmoothingOn());
	}
}

void GameWorld::ToggleSpacePartitioning(HWND hwnd)
{
	for (unsigned int i = 0; i < m_vehicles.size(); ++i)
	{
		m_vehicles[i]->Steering()->ToggleSpacePartitioningOnOff();
	}

	// If toggled on, empty the cell space and then re-add all the vehicles
	if (m_vehicles[0]->Steering()->IsSpacePartitioningOn())
	{
		m_pCellSpace->EmptyCells();

		for (unsigned int i = 0; i < m_vehicles.size(); ++i)
		{
			m_pCellSpace->AddEntity(m_vehicles[i]);
		}
	}
	else
	{
		ChangeMenuState(hwnd, IDR_PARTITIONING, MFS_UNCHECKED);
		ChangeMenuState(hwnd, IDM_PARTITION_VIEW_NEIGHBORS, MFS_UNCHECKED);
		m_bShowCellSpaceInfo = false;
	}
}

void GameWorld::ToggleCellSpacePartitioning(HWND hwnd)
{
	m_bShowCellSpaceInfo = !m_bShowCellSpaceInfo;

	if (m_bShowCellSpaceInfo)
	{
		ChangeMenuState(hwnd, IDM_PARTITION_VIEW_NEIGHBORS, MFS_CHECKED);

		if (!m_vehicles[0]->Steering()->IsSpacePartitioningOn())
		{
			SendMessage(hwnd, WM_COMMAND, IDR_PARTITIONING, NULL);
		}
	}
	else
	{
		ChangeMenuState(hwnd, IDM_PARTITION_VIEW_NEIGHBORS, MFS_UNCHECKED);
	}
}

void GameWorld::ToggleSumming(HWND hwnd)
{
	ChangeMenuState(hwnd, IDR_WEIGHTED_SUM, MFS_CHECKED);
	ChangeMenuState(hwnd, IDR_PRIORITIZED, MFS_UNCHECKED);
	ChangeMenuState(hwnd, IDR_DITHERED, MFS_UNCHECKED);

	for (unsigned int i = 0; i < m_vehicles.size(); ++i)
	{
		m_vehicles[i]->Steering()->SetSummingMethod(SteeringBehavior::WeightedAverage);
	}
}

void GameWorld::TogglePrioritizeSummingMethod(HWND hwnd)
{
	ChangeMenuState(hwnd, IDR_WEIGHTED_SUM, MFS_UNCHECKED);
	ChangeMenuState(hwnd, IDR_PRIORITIZED, MFS_CHECKED);
	ChangeMenuState(hwnd, IDR_DITHERED, MFS_UNCHECKED);

	for (unsigned int i = 0; i < m_vehicles.size(); ++i)
	{
		m_vehicles[i]->Steering()->SetSummingMethod(SteeringBehavior::Prioritized);
	}
}

void GameWorld::ToggleDithered(HWND hwnd)
{
	ChangeMenuState(hwnd, IDR_WEIGHTED_SUM, MFS_UNCHECKED);
	ChangeMenuState(hwnd, IDR_PRIORITIZED, MFS_UNCHECKED);
	ChangeMenuState(hwnd, IDR_DITHERED, MFS_CHECKED);

	for (unsigned int i = 0; i < m_vehicles.size(); ++i)
	{
		m_vehicles[i]->Steering()->SetSummingMethod(SteeringBehavior::Dithered);
	}
}

void GameWorld::HandleMenuItems(WPARAM wParam, HWND hwnd)
{
	switch (wParam)
	{
		case ID_OB_OBSTACLES:
			ToggleObstacles(hwnd);
			break;

		case ID_OB_WALLS:
			ToggleWalls(hwnd);
			break;

		case IDR_PARTITIONING:
			ToggleSpacePartitioning(hwnd);
			break;

		case IDM_PARTITION_VIEW_NEIGHBORS:
			ToggleCellSpacePartitioning(hwnd);
			break;

		case IDR_WEIGHTED_SUM:
			ToggleSumming(hwnd);
			break;

		case IDR_PRIORITIZED:
			TogglePrioritizeSummingMethod(hwnd);
			break;

		case IDR_DITHERED:

			break;

		case ID_VIEW_KEYS:
			ToggleViewKeys();
			CheckMenuItemAppropriately(hwnd, ID_VIEW_FPS, RenderFPS());
			break;

		case ID_VIEW_FPS:
			ToogleShowFPS();
			CheckMenuItemAppropriately(hwnd, ID_VIEW_FPS, RenderFPS());
			break;

		case ID_MENU_SMOOTHING:
			ToggleMenuSmoothing(hwnd);
			break;

		default:
			break;
	}
}

//------------------------------- Render -----------------------------------
//--------------------------------------------------------------------------

void GameWorld::Render()
{
	gdi->TransparentText();

	// Render any walls
	gdi->BlackPen();
	
	for (unsigned int w = 0; w < m_Walls.size(); ++w)
	{
		// true flag shows normals
		m_Walls[w].Render(true);
	}

	// Render any obstacles
	gdi->BlackPen();

	for (unsigned int ob = 0; ob < m_obstacles.size(); ++ob)
	{
		gdi->Circle(m_obstacles[ob]->Pos(), m_obstacles[ob]->BRadius());
	}

	// Render the agents
	for (unsigned int a = 0; a < m_vehicles.size(); ++a)
	{
		m_vehicles[a]->Render();

		// Render cell partitioning stuff
		if (m_bShowCellSpaceInfo && a == 0)
		{
			gdi->HollowBrush();
			InvertedAABox2D box(
				m_vehicles[a]->Pos() - Vector2D(Prm.ViewDistance(), Prm.ViewDistance()),
				m_vehicles[a]->Pos() + Vector2D(Prm.ViewDistance(), Prm.ViewDistance())
			);

			box.Render();

			gdi->RedPen();

			CellSpace()->CalculateNeighbors(m_vehicles[a]->Pos(), Prm.ViewDistance());
			for (BaseGameEntity* pV = CellSpace()->Begin(); !CellSpace()->End(); pV = CellSpace()->Next())
			{
				gdi->Circle(pV->Pos(), pV->BRadius());
			}

			gdi->GreenPen();
			gdi->Circle(m_vehicles[a]->Pos(), Prm.ViewDistance());
		}
	}

// #define CROSSHAIR
#ifdef CROSSHAIR
	// And finally the crosshair ...
	gdi->RedPen();
	gdi->Circle(m_vCrosshair, 4);
	gdi->Line(m_vCrosshair.x - 8, m_vCrosshair.y, m_vCrosshair.x + 8, m_vCrosshair.y);
	gdi->Line(m_vCrosshair.x, m_vCrosshair.y - 8, m_vCrosshair.x, m_vCrosshair.y + 8);
	gdi->TextAtPos(5, cyClient() - 20, "Click to move crosshair");
#endif

	gdi->TextColor(Cgdi::grey);

	if (RenderPath())
	{
		gdi->TextAtPos((int)(cxClient() / 2.0f - 80), cyClient() - 20, "Press 'U' for random path");

		m_pPath->Render();
	}

	if (RenderFPS())
	{
		gdi->TextColor(Cgdi::grey);
		gdi->TextAtPos(5, cyClient() - 20, ttos(1.0 / m_dAvFrameTime));
	}

	if (m_bShowCellSpaceInfo) 
		m_pCellSpace->RenderCells();
}
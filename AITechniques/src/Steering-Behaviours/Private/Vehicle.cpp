#include "Steering-Behaviours/Public/Vehicle.h"
#include "Common/Public/2D/C2DMatrix.h"
#include "Common/Public/2D/Transformations.h"
// #include "GameWorld.h"
// #include "Common/Public/Misc/CellSpacePartion.h"
// #include "Common/Public/Misc/Cgdi.h"

Vehicle::Vehicle(GameWorld* world,
	Vector2D position,
	double rotation,
	Vector2D velocity,
	double mass,
	double maxForce,
	double maxSpeed,
	double maxTurnRate,
	double scale)
	: MovingEntity(
		position,
		scale,
		velocity,
		maxSpeed,
		Vector2D(sin(rotation), -cos(rotation)),
		mass,
		Vector2D(scale, scale),
		maxTurnRate,
		maxForce),

	m_pWorld(world),
	m_vSmoothedHeading(Vector2D(0, 0)),
	m_bSmoothingOn(false),
	m_dTimeElapsed(0.0f)
{
	InitializeBuffer();

	// Sets up the steering behavior class
	// m_pSteering = new SteeringBehaviour(this);

	// Sets up the smoother
	m_pHeadingSmoother = new Smoother<Vector2D>(0, Vector2D(0.0f, 0.0f));
}

// --------------------- Destructor  ---------------------------------
// -------------------------------------------------------------------

Vehicle::~Vehicle()
{
	// delete m_pSteering;
	delete m_pHeadingSmoother;
}

// --------------------- Update  -------------------------------------
// Fills the vehicle's shape buffer with its vertices
// -------------------------------------------------------------------

void Vehicle::InitializeBuffer()
{
	const int numVehicleVerts = 3;

	Vector2D vehicle[numVehicleVerts] = { 
		Vector2D(-1.0f, 0.6f), 
		Vector2D(1.0f, 0.0f), 
		Vector2D(-1.0f, -0.6f) 
	};

	// Setup the vertex buffers and calculate the bounding radius
	for (int vtx = 0; vtx < numVehicleVerts; ++vtx)
	{
		m_vecVehicleVB.push_back(vehicle[vtx]);
	}
}

// --------------------- Update  -------------------------------------
// Updates the vehicle's position from a series of steering behaviours
// -------------------------------------------------------------------

void Vehicle::Update(double timeElapsed)
{
	// Update the time elapsed
	m_dTimeElapsed = timeElapsed;

	// Keep a record of its old position so we can update its cell later
	Vector2D oldPos = Pos();

	Vector2D steeringForce;

	// Calculate the combined force from each steering behaviour in the
	// vehicle's list
	// steeringForce = m_pSteering->Calculate();

	// Acceleration = Force / Mass
	Vector2D acceleration = steeringForce / m_dMass;

	// Update velocity
	m_vVelocity += acceleration * timeElapsed;

	// Make sure vehicle does not exceed maximum velocity
	m_vVelocity.Truncate(m_dMaxSpeed);

	// Update the position
	m_vPos += m_vVelocity * timeElapsed;

	// Update the heading if the vehicle has a non zero velocity
	if (m_vVelocity.LengthSq() > 0.00000001)
	{
		m_vHeading = Vec2DNormalize(m_vVelocity);
		m_vSide = m_vHeading.Perp();
	}

	// Treat the screen as a toroid
	// WrapAround(m_vPos, m_pWorld->cxClient(), m_pWorld->cyClient());

	// Update the vehicle's current cell if SP is turned on
	//if (Steering()->isSpacePartitionOn())
	//{
	//	World()->CellSpace()->UpdateEntity(this, oldPos);
	//}

	if (IsSmoothingOn())
	{
		m_vSmoothedHeading = m_pHeadingSmoother->Update(Heading());
	}
}

// --------------------- Render  -------------------------------------
// -------------------------------------------------------------------

void Vehicle::Render()
{

}

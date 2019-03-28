#pragma once

#include "Constants.h"
#include "Public/Misc/IniFileLoaderBase.h"
#include "Public/Misc/Utils.h"

#define Prm (*ParamLoader::Instance())

class ParamLoader : public IniFileLoaderBase
{
private:

	int m_iNumAgents;
	int m_iNumObstacles;
	double m_dMinObstacleRadius;
	double m_dMaxObstacleRadius;

	// Number of horizontal cells used for spatial partitioning
	int m_iNumCellsX;
	// Number of vertical cells used for spatial partitioning
	int m_iNumCellsY;

	// How many samples the smoother will use to average a value
	int m_iNumSamplesForSmoothing;

	// Used to tweak the combined steering force (simply altering the MaxSteeringForce
	// will not work. This tweaker affects all the steering force multipliers too).
	double m_dSteeringForceTweaker;

	double m_dMaxSteeringForce;
	double m_dMaxSpeed;
	double m_dVehicleMass;

	double m_dVehicleScale;
	double m_dMaxTurnRatePerSecond;

	double m_dSeparationWeight;
	double m_dAlignmentWeight;
	double m_dCohesionWeight;
	double m_dObstacleAvoidanceWeight;
	double m_dWallAvoidanceWeight;
	double m_dWanderWeight;
	double m_dSeekWeight;
	double m_dFleeWeight;
	double m_dArriveWeight;
	double m_dPursuitWeight;
	double m_dOffsetPursuitWeight;
	double m_dInterposeWeight;
	double m_dHideWeight;
	double m_dEvadeWeight;
	double m_dFollowPathWeight;

	// How to close a neighbour must be before an agent perceives it (considers it
	// to be within its neighbourhood)
	double m_dViewDistance;

	// Used in obstacle avoidance
	double m_dMinDetectionBoxLength;

	// Used in wall avoidance
	double m_dWallDetectionFeelerLength;

	// These are probabilities that a steering behaviour will be used when the
	// prioritized dither calculate method is used
	double m_dPrWallAvoidance;
	double m_dPrObstacleAvoidance;
	double m_dPrSeparation;
	double m_dPrAlignment;
	double m_dPrCohesion;
	double m_dPrWander;
	double m_dPrSeek;
	double m_dPrFlee;
	double m_dPrEvade;
	double m_dPrHide;
	double m_dPrArrive;

	ParamLoader()
		:IniFileLoaderBase("params.ini"),
		m_iNumAgents(GetNextParameterInt()),
		m_iNumObstacles(GetNextParameterInt()),
		m_dMinObstacleRadius(GetNextParameterDouble()),
		m_dMaxObstacleRadius(GetNextParameterDouble()),
		// Spatial partitioning
		m_iNumCellsX(GetNextParameterInt()),
		m_iNumCellsY(GetNextParameterInt()),
		// Smoothing samples
		m_iNumSamplesForSmoothing(GetNextParameterInt()),
		// SteeringForce tweaked properties
		m_dSteeringForceTweaker(GetNextParameterFloat()),
		m_dMaxSteeringForce(GetNextParameterFloat() * m_dSteeringForceTweaker),
		m_dMaxSpeed(GetNextParameterFloat()),
		m_dVehicleMass(GetNextParameterFloat()),
		m_dVehicleScale(GetNextParameterFloat()),
		m_dMaxTurnRatePerSecond(Pi),
		m_dSeparationWeight(GetNextParameterFloat() * m_dSteeringForceTweaker),
		m_dAlignmentWeight(GetNextParameterFloat() * m_dSteeringForceTweaker),
		m_dCohesionWeight(GetNextParameterFloat() * m_dSteeringForceTweaker),
		m_dObstacleAvoidanceWeight(GetNextParameterFloat() * m_dSteeringForceTweaker),
		m_dWallAvoidanceWeight(GetNextParameterFloat() * m_dSteeringForceTweaker),
		m_dWanderWeight(GetNextParameterFloat() * m_dSteeringForceTweaker),
		m_dSeekWeight(GetNextParameterFloat() * m_dSteeringForceTweaker),
		m_dFleeWeight(GetNextParameterFloat() * m_dSteeringForceTweaker),
		m_dArriveWeight(GetNextParameterFloat() * m_dSteeringForceTweaker),
		m_dPursuitWeight(GetNextParameterFloat() * m_dSteeringForceTweaker),
		m_dOffsetPursuitWeight(GetNextParameterFloat() * m_dSteeringForceTweaker),
		m_dInterposeWeight(GetNextParameterFloat() * m_dSteeringForceTweaker),
		m_dHideWeight(GetNextParameterFloat() * m_dSteeringForceTweaker),
		m_dEvadeWeight(GetNextParameterFloat() * m_dSteeringForceTweaker),
		m_dFollowPathWeight(GetNextParameterFloat() * m_dSteeringForceTweaker),
		// Agent perception
		m_dViewDistance(GetNextParameterFloat()),
		// Obstacle avoidance
		m_dMinDetectionBoxLength(GetNextParameterFloat()),
		m_dWallDetectionFeelerLength(GetNextParameterFloat()),
		// Probabilities
		m_dPrWallAvoidance(GetNextParameterFloat()),
		m_dPrObstacleAvoidance(GetNextParameterFloat()),
		m_dPrSeparation(GetNextParameterFloat()),
		m_dPrAlignment(GetNextParameterFloat()),
		m_dPrCohesion(GetNextParameterFloat()),
		m_dPrWander(GetNextParameterFloat()),
		m_dPrSeek(GetNextParameterFloat()),
		m_dPrFlee(GetNextParameterFloat()),
		m_dPrEvade(GetNextParameterFloat()),
		m_dPrHide(GetNextParameterFloat()),
		m_dPrArrive(GetNextParameterFloat())
	{}

public:

	static ParamLoader* Instance();

	// Getters to access loaded parameters
	inline int NumAgents() const { return m_iNumAgents; }
	inline int NumObstacles() const { return m_iNumObstacles; }

	inline double MinObstacleRadius() const { return m_dMinObstacleRadius; }
	inline double MaxObstacleRadius() const { return m_dMaxObstacleRadius; }

	inline int NumCellsX() const { return m_iNumCellsX; }
	inline int NumCellsY() const { return m_iNumCellsY; }

	inline int NumSamplesForSmoothing() const { return m_iNumSamplesForSmoothing; }

	inline double SteeringForceTweaker() const { return m_dSteeringForceTweaker; }

	inline double MaxSteeringForce() const { return m_dMaxSteeringForce; }
	inline double MaxSpeed() const { return m_dMaxSpeed; }
	inline double VehicleMass() const { return m_dVehicleMass; }
	inline double VehicleScale() const { return m_dVehicleScale; }

	inline double MaxTurnRatePerSecond() const { return m_dMaxTurnRatePerSecond; }
	inline double SeparationWeight() const { return m_dSeparationWeight; }

	inline double AlignmentWeight() const { return m_dAlignmentWeight; }
	inline double CohesionWeight() const { return m_dCohesionWeight; }
	inline double ObstacleAvoidanceWeight() const { return m_dObstacleAvoidanceWeight; }
	inline double WallAvoidanceWeight() const { return m_dWallAvoidanceWeight; }
	inline double WanderWeight() const { return m_dWanderWeight; }
	inline double SeekWeight() const { return m_dSeekWeight; }
	inline double FleeWeight() const { return m_dFleeWeight; }
	inline double ArriveWeight() const { return m_dArriveWeight; }
	inline double PursuitWeight() const { return m_dPursuitWeight; }
	inline double OffsetPursuitWeight() const { return m_dOffsetPursuitWeight; }
	inline double InterposeWeight() const { return m_dInterposeWeight; }
	inline double HideWeight() const { return m_dHideWeight; }
	inline double EvadeWeight() const { return m_dEvadeWeight; }
	inline double FollowPathWeight() const { return m_dFollowPathWeight; }

	inline double ViewDistance() const { return m_dViewDistance; }
	inline double MinDetectionBoxLength() const { return m_dMinDetectionBoxLength; }
	inline double WallDetectionFeelerLength() const { return m_dWallDetectionFeelerLength; }

	inline double PrWallAvoidance() const { return m_dPrWallAvoidance; }
	inline double PrObstacleAvoidance() const { return m_dPrObstacleAvoidance; }
	inline double PrSeparation() const { return m_dPrSeparation; }
	inline double PrAlignment() const { return m_dPrAlignment; }
	inline double PrCohesion() const { return m_dPrCohesion; }
	inline double PrWander() const { return m_dPrWander; }
	inline double PrSeek() const { return m_dPrSeek; }
	inline double PrFlee() const { return m_dPrFlee; }
	inline double PrEvade() const { return m_dPrEvade; }
	inline double PrHide() const { return m_dPrHide; }
	inline double PrArrive() const { return m_dPrArrive; }
};

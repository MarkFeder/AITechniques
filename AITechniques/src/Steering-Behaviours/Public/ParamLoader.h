#pragma once

#include "Constants.h"
#include "Common/Public/Misc/IniFileLoaderBase.h"
#include "Common/Public/Misc/Utils.h"

#define Prm (*ParamLoader::Instance())

class ParamLoader : public IniFileLoaderBase
{
private:

	ParamLoader()
		:IniFileLoaderBase("params.ini"),
		m_iNumAgents(GetNextParameterInt()),
		m_iNumObstacles(GetNextParameterInt()),
		m_dMinObstacleRadius(GetNextParameterDouble()),
		m_dMaxObstacleRadius(GetNextParameterDouble()),
		m_iNumCellsX(GetNextParameterInt())
	{}

public:

	static ParamLoader* Instance();

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
};

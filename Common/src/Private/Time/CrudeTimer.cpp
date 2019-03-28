#include "Public/Time/CrudeTimer.h"

//----------------------------- Instance ---------------------------

CrudeTimer* CrudeTimer::Instance()
{
	static CrudeTimer instance;

	return &instance;
}

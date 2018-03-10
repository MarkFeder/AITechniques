#include "Common/Public/Time/CrudeTimer.h"

CrudeTimer* CrudeTimer::Instance()
{
	static CrudeTimer instance;

	return &instance;
}

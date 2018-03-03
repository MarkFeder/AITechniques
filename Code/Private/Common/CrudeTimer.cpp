#include "Public/Common/CrudeTimer.h"

CrudeTimer* CrudeTimer::Instance()
{
	static CrudeTimer instance;

	return &instance;
}

#include "Public/Misc/FrameCounter.h"

//----------------------------- Instance ---------------------------

FrameCounter* FrameCounter::Instance()
{
	static FrameCounter instance;

	return &instance;
}

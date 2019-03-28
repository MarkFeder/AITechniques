#pragma once

// This library must be included
#pragma comment(lib, "winmm.lib")

#include <windows.h>

#define Clock CrudeTimer::Instance()

class CrudeTimer
{
private:

	// Set to the time (in seconds) when class is instantiated
	double m_dStartTime;

	// Set the start time
	CrudeTimer() { m_dStartTime = timeGetTime() * 0.001; }

public:

	// Copy constructor and assignment should be deleted
	CrudeTimer(const CrudeTimer&) = delete;
	CrudeTimer& operator=(const CrudeTimer&) = delete;

	static CrudeTimer* Instance();

	// Returns how much time has elapsed since the timer was started
	double GetElapsedTime() { return timeGetTime() * 0.001 - m_dStartTime; }
};
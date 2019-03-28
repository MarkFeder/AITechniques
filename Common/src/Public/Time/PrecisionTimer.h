#pragma once

#include <windows.h>
#include <cassert>

const double SMOOTHNESS = 5.0;

class PrecisionTimer
{
private:

	LONGLONG m_lCurrentTime;
	LONGLONG m_lLastTime;
	LONGLONG m_lLastTimeInTimeElapsed;
	LONGLONG m_lNextTime;
	LONGLONG m_lStartTime;
	LONGLONG m_lFrameTime;
	LONGLONG m_lPerFCountFreq;

	double m_dTimeElapsed;
	double m_dLastTimeElapsed;
	double m_dTimeScale;

	double m_dNormalFPS;
	double m_dSlowFPS;

	bool m_bStarted;

	// If true, a call to TimeElapsed() will return 0 if the current
	// time elapsed is much smaller than the previous. Used to counter
	// the problems associated with the user using menus/resizing/moving
	// a window, etc.
	bool m_bSmoothUpdates;

public:

	// Ctors
	PrecisionTimer();
	PrecisionTimer(double fps);

	// Starts the timer
	void Start();

	// Determines if enough time has passed to move onto next frame
	inline bool ReadyForNextFrame();

	// Only used this after a call to the above
	// double GetTimeElapsed() { return m_TimeElapsed; }

	//--------------------------- TimeElapsed --------------------------------
	//  Returns time elapsed since last call to this function.
	//------------------------------------------------------------------------

	inline double TimeElapsed()
	{
		m_dLastTimeElapsed = m_dTimeElapsed;

		QueryPerformanceCounter((LARGE_INTEGER*)& m_lCurrentTime);

		m_dTimeElapsed = (m_lCurrentTime - m_lLastTimeInTimeElapsed) * m_dTimeScale;
		m_lLastTimeInTimeElapsed = m_lCurrentTime;

		if (m_bSmoothUpdates)
		{
			return (m_dTimeElapsed < (m_dLastTimeElapsed * SMOOTHNESS)) ? m_dTimeElapsed : 0.0;
		}
		else
		{
			return m_dTimeElapsed;
		}
	}

	inline double CurrentTime()
	{
		QueryPerformanceCounter((LARGE_INTEGER*)& m_lCurrentTime);

		return (m_lCurrentTime - m_lStartTime) * m_dTimeScale;
	}

	bool Started() const { return m_bStarted; }

	void SmoothUpdatesOn() { m_bSmoothUpdates = true; }
	void SmoothUpdatesOff() { m_bSmoothUpdates = false; }
};

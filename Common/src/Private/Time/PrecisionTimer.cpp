#include "Public/Time/PrecisionTimer.h"

//-------------------------Default Constructor-------------------------------

PrecisionTimer::PrecisionTimer()
	:m_dNormalFPS(0.0),
	m_dSlowFPS(1.0),
	m_dTimeElapsed(0.0),
	m_lFrameTime(0),
	m_lLastTime(0),
	m_lLastTimeInTimeElapsed(0),
	m_lPerFCountFreq(0),
	m_bStarted(false),
	m_lStartTime(0),
	m_dLastTimeElapsed(0.0),
	m_bSmoothUpdates(false)
{
	// How many ticks per sec do we get
	QueryPerformanceCounter((LARGE_INTEGER*)& m_lPerFCountFreq);

	m_dTimeScale = 1.0 / m_lPerFCountFreq;
}

//-------------------------Constructor -------------------------------
// Use to specify FPS
//--------------------------------------------------------------------

PrecisionTimer::PrecisionTimer(double fps)
	:m_dNormalFPS(fps),
	m_dSlowFPS(1.0),
	m_dTimeElapsed(0.0),
	m_lFrameTime(0),
	m_lLastTime(0),
	m_lLastTimeInTimeElapsed(0),
	m_lPerFCountFreq(0),
	m_bStarted(false),
	m_lStartTime(0),
	m_dLastTimeElapsed(0.0),
	m_bSmoothUpdates(false)
{
	// How many ticks per sec do we get
	QueryPerformanceCounter((LARGE_INTEGER*)& m_lPerFCountFreq);

	m_dTimeScale = 1.0 / m_lPerFCountFreq;

	// Calculate ticks per frame
	m_lFrameTime = (LONGLONG)(m_lPerFCountFreq / m_dNormalFPS);
}

//-------------------------Start -------------------------------
// Call this immediately prior to game loop. Starts the timer
//--------------------------------------------------------------

void PrecisionTimer::Start()
{
	m_bStarted = true;

	m_dTimeElapsed = 0.0;

	// Get the time
	QueryPerformanceCounter((LARGE_INTEGER*)& m_lLastTime);

	// Keep a record of when the timer was started
	m_lStartTime = m_lLastTimeInTimeElapsed = m_lLastTime;

	// Update time to render next frame
	m_lNextTime = m_lLastTime + m_lFrameTime;
}

//-------------------------ReadyForNextFrame()-------------------------------
//  Returns true if it is time to move on to the next frame step. To be used if
//  FPS is set.
//----------------------------------------------------------------------------

inline bool PrecisionTimer::ReadyForNextFrame()
{
	assert(m_dNormalFPS && "PrecisionTimer::ReadyForNextFrame<No FPS set in timer>");

	QueryPerformanceCounter((LARGE_INTEGER*)& m_lCurrentTime);

	if (m_lCurrentTime > m_lNextTime)
	{
		m_dTimeElapsed = (m_lCurrentTime - m_lLastTime) * m_dTimeScale;
		m_lLastTime = m_lCurrentTime;

		// Update time to render next frame
		m_lNextTime = m_lCurrentTime + m_lFrameTime;

		return true;
	}

	return false;
}

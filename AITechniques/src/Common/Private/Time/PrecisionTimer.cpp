#include "Common/Public/Time/PrecisionTimer.h"

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

//--------------------------- TimeElapsed --------------------------------
//  Returns time elapsed since last call to this function.
//------------------------------------------------------------------------

inline double PrecisionTimer::TimeElapsed()
{
	m_dLastTimeElapsed = m_dTimeElapsed;

	QueryPerformanceCounter((LARGE_INTEGER*)& m_lCurrentTime);

	m_dTimeElapsed = (m_lCurrentTime - m_lLastTimeInTimeElapsed) * m_dTimeScale;
	m_lLastTimeInTimeElapsed = m_lCurrentTime;

	if (m_bSmoothUpdates)
	{
		if (m_dTimeElapsed < (m_dLastTimeElapsed * SMOOTHNESS))
		{
			return m_dTimeElapsed;
		}
		else
		{
			return 0.0;
		}
	}
	else
	{
		return m_dTimeElapsed;
	}
}
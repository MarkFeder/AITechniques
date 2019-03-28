#pragma once

#define TickCounter FrameCounter::Instance()

class FrameCounter
{
private:

	long m_lCount;

	int m_iFramesElapsed;

	FrameCounter()
		:m_lCount(0),
		m_iFramesElapsed(0)
	{}

public:

	// Copy ctor and assignment should be deleted
	FrameCounter(const FrameCounter&) = delete;
	FrameCounter& operator=(const FrameCounter&) = delete;

	static FrameCounter* Instance();

	void Update() { ++m_lCount; ++m_iFramesElapsed; }

	long GetCurrentFrame() const { return m_lCount; }

	void Reset() { m_lCount = 0; }

	void Start() { m_iFramesElapsed = 0; }

	int FramesElapsedSinceStartCalled() const { return m_iFramesElapsed; }
};

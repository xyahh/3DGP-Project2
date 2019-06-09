#pragma once
#include <chrono>

_3DGP_BEGIN_

class Timer
{
	using TimeDuration	= std::chrono::duration<float>;
	using Time			= std::chrono::high_resolution_clock;
	using TimePoint		= Time::time_point;

public:

	Timer();
	~Timer();

	void  Tick();
	float GetElapsedTime() const;
	void  ResetElapsedTime();
	bool  FlushAccumulatedTime();
	void  Reset();
	float Interpolation() const;
	static float GetDeltaTime();
	const std::string& GetFPS() const;

private:

	TimePoint	m_CurrentTime;
	TimePoint	m_PreviousTime;
	float		m_TimeFrame;
	float		m_TimeAccumulator;
	float		m_ElapsedTime;

	float		 m_FPSTimer;
	std::string	 m_FPSString;
	unsigned int m_FramesProcessed	{ 0 };
};

_3DGP_END_


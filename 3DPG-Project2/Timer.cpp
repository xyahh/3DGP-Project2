#include "stdafx.h"
#include "Timer.h"
#define DELTA_TIME 0.008f

_3DGP_USE_


Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::Tick()
{
	m_CurrentTime = Time::now();
	m_TimeFrame = TimeDuration(m_CurrentTime - m_PreviousTime).count();
	m_PreviousTime = m_CurrentTime;
	m_TimeAccumulator += m_TimeFrame;
	m_ElapsedTime += m_TimeFrame;
	m_FPSTimer += m_TimeFrame;
	++m_FramesProcessed;
	if (m_FPSTimer >= 1.f)
	{
		m_FPSString = std::to_string(m_FramesProcessed);
		m_FPSTimer = 0.f;
		m_FramesProcessed = 0;
	}
	
}

float Timer::GetElapsedTime() const
{
	return m_ElapsedTime;
}

void Timer::ResetElapsedTime()
{
	m_ElapsedTime = 0.f;
}

bool Timer::FlushAccumulatedTime()
{
	bool Result = m_TimeAccumulator >= DELTA_TIME;

	if (Result)
		m_TimeAccumulator -= DELTA_TIME;

	return Result;
}

float Timer::Interpolation() const
{
	return m_TimeAccumulator * DELTA_TIME;
}

float Timer::GetDeltaTime() const
{
	return DELTA_TIME;
}

const std::string& Timer::GetFPS() const
{
	return m_FPSString;
}

void Timer::Reset()
{
	m_CurrentTime = Time::now();
	m_PreviousTime = Time::now();
	m_ElapsedTime = 0.f;
	m_TimeAccumulator = 0.f;
	m_TimeFrame = 0.f;
	m_FPSTimer = 0.f;
}

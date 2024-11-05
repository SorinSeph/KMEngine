#include "CoreTimer.h"

FTimerManager& FTimerManager::GetTimerManager()
{
	static FTimerManager TimerManager;
	return TimerManager;
}

void BCoreTimer::Start()
{
	__int64 StartTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&StartTime);

	if (m_Stopped)
	{
		m_PausedTime += (StartTime - m_StopTime);

		m_PrevTime = StartTime;
		m_StopTime = 0;
		m_Stopped = false;
	}
}

void BCoreTimer::EngineTick()
{
	if (m_TimersArray.size())
	{
		for (int i = 0; i < m_TimersArray.size(); i++)
		{
			if (m_TimersArray[i]->StartTime <= GetFTotalTime())
			{
				m_TimersArray[i]->bRunning = true;
				m_TimersArray[i]->Execute();

				if (m_TimersArray[i]->EndTime <= GetFTotalTime() && m_TimersArray[i]->bRunning)
				{
					m_TimersArray.erase(m_TimersArray.begin() + i);
					break;
				}
			}
		}
	}
}

void BCoreTimer::Tick()
{
	if (m_Stopped)
	{
		m_DeltaTime = 0.0;
		return;
	}

	__int64 CurrTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&CurrTime);
	m_CurrTime = CurrTime;
	m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondsPerCount;
	m_PrevTime = m_CurrTime;

	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}
}

void BCoreTimer::Reset()
{
	__int64 CurrentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);

	m_BaseTime = CurrentTime;
	m_PrevTime = CurrentTime;
	m_StopTime = 0;
	m_Stopped = false;
}

double BCoreTimer::GetDDeltaTime() const
{
	return m_DeltaTime;
}

float BCoreTimer::GetFDeltaTime() const
{
	return (float)m_DeltaTime;
}

float BCoreTimer::GetFTotalTime() const
{
	if (m_Stopped)
	{
		return (float)(((m_StopTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
	}
	else
	{
		return (float)(((m_CurrTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
	}
}

void BCoreTimer::AddTimer(BTimerCore* InTimer)
{
	m_TimersArray.push_back(InTimer);
}
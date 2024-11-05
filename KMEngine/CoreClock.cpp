#include "CoreClock.h"

CTimerManager& CTimerManager::GetTimerManager()
{
	static CTimerManager TimerManager;
	return TimerManager;
}

void CCoreClock::Start()
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

void CCoreClock::EngineTick()
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

void CCoreClock::Tick()
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

void CCoreClock::Reset()
{
	__int64 CurrentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);

	m_BaseTime = CurrentTime;
	m_PrevTime = CurrentTime;
	m_StopTime = 0;
	m_Stopped = false;
}

double CCoreClock::GetDDeltaTime() const
{
	return m_DeltaTime;
}

float CCoreClock::GetFDeltaTime() const
{
	return (float)m_DeltaTime;
}

float CCoreClock::GetFTotalTime() const
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

void CCoreClock::AddTimer(CBaseTimer* InTimer)
{
	m_TimersArray.push_back(InTimer);
}
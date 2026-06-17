#pragma once

#include "Core/CoreEngine.h"
#include "Core/CoreClock.h"

class CKMEngineLoop
{
public:
	CKMEngineLoop(CCoreEngine* pCoreEngine) 
	{
		m_pCoreEngine = pCoreEngine;
		m_pCoreClock = pCoreEngine->GetCoreClock();
		m_pRenderer = pCoreEngine->GetRenderer();
	};

	HRESULT InitEngine()
	{
		return m_pCoreEngine->InitEngine();
	}

	void Update()
	{
		m_pCoreClock->Tick();
		m_pCoreClock->EngineTick();
		m_pCoreEngine->DetectInput();
	}

	void Render()
	{
		float X = m_pCoreEngine->GetXRotation();
		float Y = m_pCoreEngine->GetYRotation();
		float EyeX = m_pCoreEngine->GetEyeX();
		float EyeY = m_pCoreEngine->GetEyeY();
		float EyeZ = m_pCoreEngine->GetEyeZ();

		m_pRenderer->Render2(X, Y, EyeX, EyeY, EyeZ);
	}

	void Cleanup()
	{
		m_pRenderer->CleanupRenderer();
	}

	CCoreClock* GetCoreClock() { return m_pCoreClock; }
	CCoreEngine* GetCoreEngine() { return m_pCoreEngine; }

private:
	CCoreEngine* m_pCoreEngine;
	CCoreClock* m_pCoreClock;
	CRenderer* m_pRenderer;
};
#pragma once

#ifndef GRAPHICSMODULE_H
#define GRAPHICSMODULE_H

#include "BaseModule.h"
#include "Logger.h"
#include "Renderer.h"

class CGraphicsModule : public CBaseModule
{
public:
	CGraphicsModule() {};

	CRenderer m_Renderer;

	void SetRenderer()
	{
		m_Renderer.GetDX11Device()->SetRenderer(&m_Renderer);
	}

	void TestLog()
	{
		CLogger& Logger = CLogger::GetLogger();
		Logger.Log("CGraphicsModule::TestLog: ", "TestLog working");
	}
};

#endif
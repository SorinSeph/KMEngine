#pragma once

#include "BaseModule.h"
#include "GraphicsModule.h"
#include "UIModule.h"

class CModuleInitializer
{
public:
	void InitializeUIModule(HINSTANCE hInstance, int nCmdShow)
	{
		m_UIModule.Initialize(hInstance, nCmdShow);
	}

	void SetMediator(CMediator& Mediator)
	{
		m_UIModule.SetMediator(Mediator);
	}

private:
	CUIModule m_UIModule;
	CGraphicsModule m_GraphicsModule;
};
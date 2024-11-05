#pragma once

#include <Windows.h>

#include "DX11Device.h"

typedef long LRez;

#ifndef CORE_SUCCEEDED(EngineStart)
#define CORE_SUCCEEDED(EngineStart) (((LRez)(EngineStart)) >= 0)
#endif

#ifndef CORE_FAILED(EngineStart)
#define CORE_FAILED(EngineStart) (((LRez)(EngineStart)) < 0)
#endif

#define ENGINE_START (LRez)1L

class CoreWindowsApp
{
public:
	//CoreWindowsApp() { } 

private:
	DX11Device m_DX11Device;

	LRez EngineLoop()
	{
		return ENGINE_START;
	}
};


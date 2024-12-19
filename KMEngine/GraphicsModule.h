#pragma once
#include "BaseModule.h"
#include "Renderer.h"

class CGraphicsModule : public CBaseModule
{
public:
	CGraphicsModule() {};

	CRenderer m_Renderer;
};
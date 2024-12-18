#pragma once
#include "BaseModule.h"
#include "Renderer.h"

class Graphics_Module : public CBaseModule
{
public:
	Graphics_Module() {};

	CRenderer m_Renderer;
};
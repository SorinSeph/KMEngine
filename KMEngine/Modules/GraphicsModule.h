#pragma once

#ifndef GRAPHICSMODULE_H
#define GRAPHICSMODULE_H

#include "BaseModule.h"
#include "../Logger.h"
#include "../Renderer/Renderer.h"

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

	void SpawnGizmo(const CGameEntity3D& SelectedEntity)
	{
		m_Renderer.GetDX11Device()->SpawnGizmo(SelectedEntity);
	}

	CDX11Device m_DX11Device;
};

#endif
#pragma once

#ifndef GRAPHICSMODULE_H
#define GRAPHICSMODULE_H

#include "BaseModule.h"
#include "../Logger.h"
#include "../Renderer/Renderer.h"
//#include "../Renderer/RendererOpenGL.h"
#include "../EntityBuilder.h"

class CGraphicsModule : public CBaseModule
{
public:
	CGraphicsModule() 
	{
		m_EntityBuilder.SetGraphicsModule(this);
	};

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

	void GenerateTestPlane()
	{

	}

	CDX11Device m_DX11Device;

	CEntityBuilder m_EntityBuilder{};
};

#endif
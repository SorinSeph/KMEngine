#pragma once
#ifndef COREENGINE_H
#define COREENGINE_H

#include "DX11Device.h"
#include "OpenGLDevice.h"
#include <Windows.h>
#include <fstream>
#include "resource.h"
#include "Renderer/Renderer.h"
#include "Renderer/RendererOpenGL.h"
#include "Logger.h"
#include "World.h"
#include "Modules/BaseModule.h"
#include "Modules/UIModule.h"
#include "Modules/GraphicsModule.h"
#include "Modules/PhysicsModule.h"
#include "TerrainGenerator.h"

bool bClipCursor = false;


float RaycastOriginX = 0.0f;
float RaycastOriginY = 0.0f;
float RaycastOriginZ = 0.0f;
float RaycastDestinationX = 0.0f;
float RaycastDestinationY = 0.0f;
float RaycastDestinationZ = 0.0f;

class CCoreEngine
{
public:

    CCoreEngine(HINSTANCE hInstance, int nCmdShow)
        : m_HInstance{ hInstance }
        , m_NCmdShow{ nCmdShow }
        , m_Logger{ CLogger::GetLogger() }
    {
    }

    // UI Module
    HRESULT InitEngine()
    {
        m_UIModule.SetMediator(Mediator);
        m_UIModule.Initialize(m_HInstance, m_NCmdShow);
		Mediator.m_ModuleArray[0] = &m_UIModule;

        m_GraphicsModule.SetMediator(Mediator);

        m_GraphicsModule.m_Renderer.SetGraphicsModuleReference(&m_GraphicsModule);
        Mediator.m_ModuleArray[1] = &m_GraphicsModule;

        m_UIModule.Notify([=](CGraphicsModule& GraphicsModule) {
            GraphicsModule.m_Renderer.SetViewportOpenGL(CViewportWindow::m_ViewportHwnd, m_UIModule.ViewportWidth, m_UIModule.ViewportHeight);
        }, m_GraphicsModule);
        m_GraphicsModule.m_Renderer.InitOpenGLRenderer();

        m_PhysicsModule.SetMediator(Mediator);
        Mediator.m_ModuleArray[2] = &m_PhysicsModule;

		m_World.SetOpenGLDevice(&m_GraphicsModule.m_Renderer.m_pRendererOpenGL->m_OpenGLDevice);
        m_World.Init();

		m_GraphicsModule.m_EntityBuilder.SetOpenGLDevice(&m_GraphicsModule.m_Renderer.m_pRendererOpenGL->m_OpenGLDevice);
        m_GraphicsModule.m_EntityBuilder.CreateLight();
        m_GraphicsModule.m_EntityBuilder.CreateGizmo();

        return S_OK;
    }

    static HWND GetViewport()
    {
        return ViewportHwnd;
    }

    CViewportWindow GetViewportWindow()
    {
        return m_UIModule.m_ViewportWindow;
    }

    CRenderer* GetRenderer()
    {
        return &(m_GraphicsModule.m_Renderer);
    }

    CCoreClock* GetCoreClock()
    {
        return &m_CoreClock;
    }

    float GetXRotation()
    {
		return m_UIModule.m_ViewportWindow.GetXRotation();
    }

    float GetYRotation()
    {
        return m_UIModule.m_ViewportWindow.GetYRotation();
    }

    float GetEyeX()
    {
        return m_UIModule.m_ViewportWindow.GetEyeX();
    }

    float GetEyeY()
    {
        return m_UIModule.m_ViewportWindow.GetEyeY();
    }

    float GetEyeZ()
    {
        return m_UIModule.m_ViewportWindow.GetEyeZ();
    }

    void DetectInput()
    {
        m_UIModule.m_ViewportWindow.DetectKeyboardInput();
    }

private:
    HINSTANCE m_HInstance{ };
    int m_NCmdShow{ };
    CWorld m_World{ };

    float m_RotX;
    float m_RotY;

    float* m_RotX2;
    float* m_RotY2;

    CLogger& m_Logger;

    CCoreClock m_CoreClock;
    CUIModule m_UIModule;
    CGraphicsModule m_GraphicsModule;
    CPhysicsModule m_PhysicsModule;
    CMediator Mediator;
};

#endif
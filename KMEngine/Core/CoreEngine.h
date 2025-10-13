#pragma once
#ifndef COREENGINE_H
#define COREENGINE_H

#include "DX11Device.h"
#include <Windows.h>
#include <fstream>
#include "resource.h"
#include "Renderer/Renderer.h"
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
        , m_Renderer{ }
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

        // Code for initialising th directx renderer
        //m_GraphicsModule.m_Renderer.SetViewportSize(m_UIModule.ViewportWidth, m_UIModule.ViewportHeight);
		//m_GraphicsModule.m_Renderer.SetGraphicsModuleReference(&m_GraphicsModule);
		//Mediator.m_ModuleArray[1] = &m_GraphicsModule;

        //m_UIModule.Notify([](CGraphicsModule& GraphicsModule) {
        //    GraphicsModule.m_Renderer.SetViewport(CViewportWindow::m_ViewportHwnd);
        //}, m_GraphicsModule);
        //m_GraphicsModule.m_Renderer.InitDX11Renderer();

        m_GraphicsModule.m_Renderer.SetGraphicsModuleReference(&m_GraphicsModule);
        Mediator.m_ModuleArray[1] = &m_GraphicsModule;

        m_UIModule.Notify([=](CGraphicsModule& GraphicsModule) {
            GraphicsModule.m_Renderer.SetViewportOpenGL(CViewportWindow::m_ViewportHwnd, m_UIModule.ViewportWidth, m_UIModule.ViewportHeight);
            //GraphicsModule.m_Renderer.SetViewportOpenGL(CViewportWindow::m_ViewportHwnd);
        }, m_GraphicsModule);
        m_GraphicsModule.m_Renderer.InitOpenGLRenderer();

        m_PhysicsModule.SetMediator(Mediator);
        Mediator.m_ModuleArray[2] = &m_PhysicsModule;

        //m_Renderer.SetViewport(m_ViewportWindow.GetViewportHwnd());
        //m_Renderer.InitRenderer();
		//m_GraphicsModule.m_Renderer.SetViewport(viewportHwnd);

        //m_World.Init();
        //GameEntityBuilder EntityBuilder{ m_Renderer.GetDX11Device() };
        //EntityBuilder.InitDefaultEntities();
        //EntityBuilder.TestDevice();
        //CTerrainGenerator TerrainGenerator{ m_GraphicsModule.m_Renderer.GetDX11Device() };
        //TerrainGenerator.GenerateTestTerrain();

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

    float GetXRotation()
    {
		return m_UIModule.m_ViewportWindow.GetXRotation();
    }

    float GetYRotation()
    {
        return m_UIModule.m_ViewportWindow.GetYRotation();
    }

    float GetXRotation2()
    {
        float XRot{ m_UIModule.m_ViewportWindow.GetXRotation2() };
        return XRot;
    }

    float GetYRotation2()
    {
        float YRot{ m_UIModule.m_ViewportWindow.GetYRotation2() };
        return YRot;
    }

    float* GetXRotation3()
    {
        return m_UIModule.m_ViewportWindow.GetXRotation3();
    }

    float* GetYRotation3()
    {
        return m_UIModule.m_ViewportWindow.GetYRotation3();
    }

    float GetXRotation4()
    {
        return m_UIModule.m_ViewportWindow.GetXRotation4();
    }

    float GetYRotation4()
    {
        return m_UIModule.m_ViewportWindow.GetYRotation4();
    }

    float GetXRotation5()
    {
        return *m_RotX2;
    }

    float GetYRotation5()
    {
        return *m_RotY2;
    }

    float GetXMemberRotation()
    {
        return m_UIModule.m_ViewportWindow.GetXMemberRotation();
    }

    float GetYMemberRotation()
    {
        return m_UIModule.m_ViewportWindow.GetYMemberRotation();
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

    float GetRaycastX()
    {
        m_Logger.Log("EngineInitializer.h, GetRaycastX(): Raycast X =  ", m_UIModule.m_ViewportWindow.GetRaycastX(), "\n");
        return m_UIModule.m_ViewportWindow.GetRaycastX();
    }

    float GetRaycastY()
    {
        m_Logger.Log("EngineInitializer.h, GetRaycastY(): Raycast Y =  ", m_UIModule.m_ViewportWindow.GetRaycastY(), "\n");
        return m_UIModule.m_ViewportWindow.GetRaycastY();
    }

    float GetRotX()
    {
        return m_RotX;
    }

    float GetRotY()
    {
        return m_RotY;
    }

    void DetectInput()
    {
        m_UIModule.m_ViewportWindow.DetectKeyboardInput();
    }

private:
    HINSTANCE m_HInstance{ };
    int m_NCmdShow{ };
    CWorld m_World{ };
    CRenderer m_Renderer;

    float m_RotX;
    float m_RotY;

    float* m_RotX2;
    float* m_RotY2;

    CLogger& m_Logger;

    CUIModule m_UIModule;
    CGraphicsModule m_GraphicsModule;
    CPhysicsModule m_PhysicsModule;
    CMediator Mediator;
};

#endif
#pragma once
#ifndef COREENGINE_H
#define COREENGINE_H

#include "DX11Device.h"
#include <Windows.h>
#include "resource.h"
#include "Renderer.h"
#include "Logger.h"
#include "World.h"
//#include "GameEntityBuilder.h"
#include "BaseModule.h"
#include "UIModule.h"
//#include "ViewportWindow.h"
#include "GraphicsModule.h"
#include "PhysicsModule.h"
#include "TerrainGenerator.h"

//const wchar_t CLASS_NAME[] = L"KME Engine";
////const wchar_t VIEWPORT_NAME[] = L"Viewport";
//const wchar_t TOOLBAR_NAME[] = L"Toolbar";
//const wchar_t SIDETOOLBAR_NAME[] = L"SideToolbar";
////const wchar_t VIEWPORT_NAME[]{ L"Viewport" };
//
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK LeftToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK RightToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
////LRESULT CALLBACK ViewportWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//
//HWND ViewportHwnd;
//HWND RightToolbarHwnd;
//HWND LeftToolbarHwnd;
//
//float ViewportWidth{ };
//float ViewportHeight{ };

bool bClipCursor = false;
//float g_RotX{};
//float g_RotY{};

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
        , m_Renderer{ ViewportHwnd }
        //, m_ViewportWindow{ }
        //, m_RotX{ m_ViewportWindow.GetXRotation() }
        //, m_RotY{ m_ViewportWindow.GetYRotation() }
        //, m_RotX2{ m_ViewportWindow.GetXRotation3() }
        //, m_RotY2{ m_ViewportWindow.GetYRotation3() }
        , m_Logger{ CLogger::GetLogger() }
    {
    }

    void RayCast(long InX, long InY)
    {
        if (CanRaycast())
        {
            // Test
            //XMVECTOR OriginPoint = XMVectorSet(InX, InY, 0, 0);
            XMFLOAT3 OriginPointFloat3 = XMFLOAT3(InX, InY, 0);
            m_Logger.Log("CoreEngine.h, RayCast() : InX = ", InX, ", InY = ", InY, "\n");
            XMVECTOR OriginPoint = XMLoadFloat3(&OriginPointFloat3);

            XMFLOAT3 DestinationPointFloat3 = XMFLOAT3(InX, InY, 1);
            XMVECTOR DestinationPoint = XMLoadFloat3(&DestinationPointFloat3);

            XMMATRIX ProjectionMatrix = CDX11Device::m_ProjectionMatrix;
            XMMATRIX ViewMatrix = CDX11Device::m_ViewMatrix;
            XMMATRIX WorldMatrix = CDX11Device::m_WorldMatrix;

            XMVECTOR OriginPointUnprojected = XMVector3Unproject(OriginPoint, 0, 0, ViewportWidth, ViewportHeight, 0, 1, ProjectionMatrix, ViewMatrix, WorldMatrix);
            XMVECTOR DestinationPointUnprojected = XMVector3Unproject(DestinationPoint, 0, 0, ViewportWidth, ViewportHeight, 0, 1, ProjectionMatrix, ViewMatrix, WorldMatrix);

            XMFLOAT3 OriginPointUnprojectedFloat3;
            XMStoreFloat3(&OriginPointUnprojectedFloat3, OriginPointUnprojected);

            XMVECTOR RayDirection = DestinationPointUnprojected - OriginPointUnprojected;
            RayDirection = XMVector4Normalize(RayDirection);

            XMFLOAT4 RayDirectionFloat4;
            XMStoreFloat4(&RayDirectionFloat4, RayDirection);

            XMFLOAT4 RayOriginFloat4;
            XMStoreFloat4(&RayOriginFloat4, DestinationPointUnprojected);

            //RaycastOriginX = OriginPointUnprojectedFloat3.x;
            //RaycastOriginY = OriginPointUnprojectedFloat3.y;
            //RaycastOriginZ = OriginPointUnprojectedFloat3.z;

            RaycastOriginX = 0;
            RaycastOriginY = 0;
            RaycastOriginZ = 0;

            RaycastDestinationX = RayDirectionFloat4.x * 100;
            RaycastDestinationY = RayDirectionFloat4.y * 100;
            RaycastDestinationZ = RayDirectionFloat4.z * 100;
            //DX11Device* dx11Device = DX11Device::GetDX11Device();
            //dx11Device->InitLine(RaycastOriginX, RaycastOriginY, RaycastOriginZ, RaycastDestinationX, RaycastDestinationY, RaycastDestinationZ);

            //m_Renderer.Raycast(RaycastOriginX, RaycastOriginY, RaycastOriginZ, RaycastDestinationX, RaycastDestinationY, RaycastDestinationZ);
            //m_Renderer.Raycast(RaycastOriginX, RaycastOriginY, RaycastOriginZ, RaycastDestinationX, RaycastDestinationY, RaycastDestinationZ);
        }

        SetCanRaycast(false);
    }

    bool CanRaycast()
    {
        return m_UIModule.m_ViewportWindow.CanRaycast();
    }

    void SetCanRaycast(bool Value)
    {
        CViewportWindow::SetCanRaycast(Value);
    }

    // UI Module
    HRESULT InitEngine()
    {
        m_UIModule.SetMediator(Mediator);
        m_UIModule.Initialize(m_HInstance, m_NCmdShow);
		Mediator.m_Modules.push_back(&m_UIModule);
		Mediator.m_ModuleVector.push_back(&m_UIModule);
		Mediator.m_ModuleArray[0] = &m_UIModule;

        m_GraphicsModule.SetMediator(Mediator);
        m_GraphicsModule.SetRenderer();
        m_GraphicsModule.m_Renderer.SetViewportSize(m_UIModule.ViewportWidth, m_UIModule.ViewportHeight);
		m_GraphicsModule.m_Renderer.SetGraphicsModuleReference(&m_GraphicsModule);
        Mediator.m_Modules.push_back(&m_GraphicsModule);
        Mediator.m_ModuleVector.push_back(&m_GraphicsModule);
		Mediator.m_ModuleArray[1] = &m_GraphicsModule;

        m_UIModule.Notify([](CGraphicsModule& GraphicsModule) {
            GraphicsModule.m_Renderer.SetViewport(CViewportWindow::m_ViewportHwnd);
        }, m_GraphicsModule);

        m_PhysicsModule.SetMediator(Mediator);
        Mediator.m_ModuleArray[2] = &m_PhysicsModule;

        //m_Renderer.SetViewport(m_ViewportWindow.GetViewportHwnd());
        //m_Renderer.InitRenderer();
		//m_GraphicsModule.m_Renderer.SetViewport(viewportHwnd);
        m_GraphicsModule.m_Renderer.InitRenderer();

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

    //std::unique_ptr<CMediator> m_Mediator;
    CUIModule m_UIModule;
    CGraphicsModule m_GraphicsModule;
    CPhysicsModule m_PhysicsModule;
    CMediator Mediator;
};

#endif
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
        , m_ViewportWindow{ }
        , m_RotX{ m_ViewportWindow.GetXRotation() }
        , m_RotY{ m_ViewportWindow.GetYRotation() }
        , m_RotX2{ m_ViewportWindow.GetXRotation3() }
        , m_RotY2{ m_ViewportWindow.GetYRotation3() }
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
        return m_ViewportWindow.CanRaycast();
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

        m_GraphicsModule.SetMediator(Mediator);
        m_GraphicsModule.SetRenderer();
        m_GraphicsModule.m_Renderer.SetViewportSize(m_UIModule.ViewportWidth, m_UIModule.ViewportHeight);
		m_GraphicsModule.m_Renderer.SetGraphicsModuleReference(&m_GraphicsModule);
        Mediator.m_Modules.push_back(&m_GraphicsModule);
        Mediator.m_ModuleVector.push_back(&m_GraphicsModule);

        m_UIModule.Notify([](CGraphicsModule& GraphicsModule) {
            GraphicsModule.m_Renderer.SetViewport(CViewportWindow::m_ViewportHwnd);
        }, m_GraphicsModule);

        //m_Renderer.SetViewport(m_ViewportWindow.GetViewportHwnd());
        //m_Renderer.InitRenderer();
		//m_GraphicsModule.m_Renderer.SetViewport(viewportHwnd);
        m_GraphicsModule.m_Renderer.InitRenderer();

        //m_World.Init();
        //GameEntityBuilder EntityBuilder{ m_Renderer.GetDX11Device() };
        //EntityBuilder.InitDefaultEntities();
        //EntityBuilder.TestDevice();
        CTerrainGenerator TerrainGenerator{ m_Renderer.GetDX11Device() };
        //TerrainGenerator.GenerateTestTerrain();

        return S_OK;
    }

    static HWND GetViewport()
    {
        return ViewportHwnd;
    }

    CViewportWindow GetViewportWindow()
    {
        return m_ViewportWindow;
    }

    CRenderer* GetRenderer()
    {
        return &(m_GraphicsModule.m_Renderer);
    }

    float GetXRotation()
    {
        return m_ViewportWindow.GetXRotation();
    }

    float GetYRotation()
    {
        return m_ViewportWindow.GetYRotation();
    }

    float GetXRotation2()
    {
        float XRot{ m_ViewportWindow.GetXRotation2() };
        return XRot;
    }

    float GetYRotation2()
    {
        float YRot{ m_ViewportWindow.GetYRotation2() };
        return YRot;
    }

    float* GetXRotation3()
    {
        return m_ViewportWindow.GetXRotation3();
    }

    float* GetYRotation3()
    {
        return m_ViewportWindow.GetYRotation3();
    }

    float GetXRotation4()
    {
        return m_ViewportWindow.GetXRotation4();
    }

    float GetYRotation4()
    {
        return m_ViewportWindow.GetYRotation4();
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
        return m_ViewportWindow.GetXMemberRotation();
    }

    float GetYMemberRotation()
    {
        return m_ViewportWindow.GetYMemberRotation();
    }

    float GetEyeX()
    {
        return m_ViewportWindow.GetEyeX();
    }

    float GetEyeY()
    {
        return m_ViewportWindow.GetEyeY();
    }

    float GetEyeZ()
    {
        return m_ViewportWindow.GetEyeZ();
    }

    float GetRaycastX()
    {
        m_Logger.Log("EngineInitializer.h, GetRaycastX(): Raycast X =  ", m_ViewportWindow.GetRaycastX(), "\n");
        return m_ViewportWindow.GetRaycastX();
    }

    float GetRaycastY()
    {
        m_Logger.Log("EngineInitializer.h, GetRaycastY(): Raycast Y =  ", m_ViewportWindow.GetRaycastY(), "\n");
        return m_ViewportWindow.GetRaycastY();
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
        m_ViewportWindow.DetectKeyboardInput();
    }

private:

    /**
    * Rendering, gameplay and memory classes initialization
    * ! To be split into their own module
    */

    HINSTANCE m_HInstance{ };
    int m_NCmdShow{ };
    CWorld m_World{ };
    CViewportWindow m_ViewportWindow;
    CRenderer m_Renderer;

    float m_RotX;
    float m_RotY;

    float* m_RotX2;
    float* m_RotY2;

    CLogger& m_Logger;

    //std::unique_ptr<CMediator> m_Mediator;
    CUIModule m_UIModule;
    CGraphicsModule m_GraphicsModule;
    CMediator Mediator;
};

#endif
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//    HDC hdc;
//    PAINTSTRUCT ps;
//    RECT rect;
//    int cxBlock, cyBlock, x, y;
//
//    switch (uMsg)
//    {
//        case WM_CREATE:
//        {
//            HMENU hMenu, hSubMenu;
//            hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU1));
//            SetMenu(hwnd, hMenu);
//
//            return 0;
//        }
//
//        case WM_SIZE:
//        {
//            cxBlock = LOWORD(lParam) / 3;
//            cyBlock = HIWORD(lParam) / 3;
//
//            for (x = 0; x < 3; x++)
//            {
//                if (x == 0)
//                {
//                    MoveWindow(LeftToolbarHwnd, x * cxBlock, 50, cxBlock + 5, cyBlock * 3, TRUE);
//                }
//                else if (x == 1)
//                {
//                    MoveWindow(ViewportHwnd, x * cxBlock, 50, cxBlock + 5, cyBlock * 3, TRUE);
//                    ViewportWidth = cxBlock + 5;
//                    ViewportHeight = cyBlock * 3;
//                }
//                else if (x == 2)
//                {
//                    MoveWindow(RightToolbarHwnd, x * cxBlock, 50, cxBlock + 5, cyBlock * 3, TRUE);
//                }
//            }
//            return 0;
//        }
//
//        case WM_PAINT:
//        {
//            hdc = BeginPaint(hwnd, &ps);
//            GetClientRect(hwnd, &rect);
//            Rectangle(hdc, 0, 0, rect.right, rect.bottom);
//            HBRUSH Brush = CreateSolidBrush(RGB(40, 40, 40));
//            FillRect(hdc, &rect, Brush);
//            EndPaint(hwnd, &ps);
//            return 0;
//        }
//
//        case WM_DESTROY:
//        {
//            PostQuitMessage(0);
//            return 0;
//        }
//    }
//    return DefWindowProc(hwnd, uMsg, wParam, lParam);
//}

//LRESULT CALLBACK LeftToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    HDC         hdc;
//    PAINTSTRUCT ps;
//    RECT        rect;
//
//    switch (message)
//    {
//        case WM_CREATE:
//        {
//            SetWindowLong(hwnd, 0, 0);
//            return 0;
//        }
//
//        case WM_PAINT:
//        {
//            hdc = BeginPaint(hwnd, &ps);
//            GetClientRect(hwnd, &rect);
//            Rectangle(hdc, 0, 0, rect.right, rect.bottom);
//            HBRUSH Brush = CreateSolidBrush(RGB(70, 40, 40));
//            FillRect(hdc, &rect, Brush);
//            EndPaint(hwnd, &ps);
//            return 0;
//        }
//
//        case WM_COMMAND:
//        {
//            int wmId = LOWORD(wParam);
//            // Parse the menu selections:
//            switch (wmId)
//            {
//                case 1: // Button ID
//                    MessageBox(hwnd, L"Button clicked!", L"Notification", MB_OK);
//                    break;
//                default:
//                    return DefWindowProc(hwnd, message, wParam, lParam);
//            }
//        }
//        break;
//
//        case WM_DESTROY:
//        {
//            PostQuitMessage(0);
//            return 0;
//        }
//    }
//    return DefWindowProc(hwnd, message, wParam, lParam);
//}

//LRESULT CALLBACK ViewportWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    HDC         hdc;
//    PAINTSTRUCT ps;
//    RECT        rect;
//    POINT       CursorPoint;
//    //bool bClipCursor;
//
//    switch (message)
//    {
//        case WM_CREATE:
//        {
//            bClipCursor = false;
//            SetWindowLong(hwnd, 0, 0);       // on/off flag
//            return 0;
//        }
//
//        //case WM_PAINT:
//        //{
//        //    hdc = BeginPaint(hwnd, &ps);
//        //    GetClientRect(hwnd, &rect);
//        //    Rectangle(hdc, 0, 0, rect.right, rect.bottom);
//        //    HBRUSH Brush = CreateSolidBrush(RGB(0, 0, 255));
//        //    FillRect(hdc, &rect, Brush);
//        //    EndPaint(hwnd, &ps);
//        //    return 0;
//        //}
//
//        case WM_ACTIVATE:
//        {
//            ::bClipCursor = false;
//            return 0;
//        }
//
//        case WM_LBUTTONDOWN:
//        {
//            POINT mousePos;
//
//            GetCursorPos(&mousePos);
//            ScreenToClient(hwnd, &mousePos);
//
//            int mouseX = mousePos.x;
//            int mouseY = mousePos.y;
//
//            //KMEngine_Log << "MouseX on click is: " << mouseX << "\n";
//            //KMEngine_Log << "MouseY on click is: " << mouseY << "\n";
//
//            float mouseXUnprojected = ((2 * mouseX) / ViewportWidth - 1);
//            float mouseYUnprojected = 1 - ((2 * mouseY) / ViewportHeight);
//
//            //KMEngine_Log << "Unprojected MouseX on click is: " << mouseXUnprojected << "\n";
//            //KMEngine_Log << "Unprojected MouseY on click is: " << mouseYUnprojected << "\n";
//
//            UnprojectClick3(mouseX, mouseY);
//
//            //RaycastX = mouseXUnprojected;
//            //RaycastY = mouseYUnprojected;
//
//            return 0;
//        }
//
//        case WM_RBUTTONUP:
//        {
//            ::bClipCursor = false;
//            return 0;
//        }
//
//        case WM_RBUTTONDOWN:
//        {
//            ::bClipCursor = true;
//            POINT CursorPoint;
//            GetClientRect(hwnd, &rect);
//
//            //if (GetCursorPos(&CursorPoint))
//            //{
//            //    SetCursorPos(rect.right + 50, rect.bottom - 50); 
//            //}
//
//            return 0;
//        }
//
//        case WM_MOUSEMOVE:
//        {
//            POINT ptClientUL, ptClientLR;
//            GetClientRect(ViewportHwnd, &rect);
//            ptClientUL.x = rect.left;
//            ptClientUL.y = rect.top;
//
//            // Add one to the right and bottom sides, because the 
//            // coordinates retrieved by GetClientRect do not 
//            // include the far left and lowermost pixels. 
//
//            ptClientLR.x = rect.right + 1;
//            ptClientLR.y = rect.bottom + 1;
//            ClientToScreen(ViewportHwnd, &ptClientUL);
//            ClientToScreen(ViewportHwnd, &ptClientLR);
//
//            // Copy the client coordinates of the client area 
//            // to the rcClient structure. Confine the mouse cursor 
//            // to the client area by passing the rcClient structure 
//            // to the ClipCursor function. 
//
//            SetRect(&rect, ptClientUL.x, ptClientUL.y, ptClientLR.x, ptClientLR.y);
//
//            RECT rect2;
//            GetClientRect(ViewportHwnd, &rect2);
//
//
//            // Copy the client coordinates of the client area 
//            // to the rcClient structure. Confine the mouse cursor 
//            // to the client area by passing the rcClient structure 
//            // to the ClipCursor function. 
//            if (::bClipCursor)
//            {
//                SetCursorPos(3 * (rect2.right) / 2, rect2.bottom / 2);
//            }
//
//            //KMEngine_Log << "Viewport rect.left + rect.right / 2 is: " << (rect.left + rect.right) / 2 << "\n";
//            //KMEngine_Log << "Viewport rect.bottom / 2 is: " << rect.bottom / 2 << "\n";
//
//            return 0;
//        }
//
//        case WM_INPUT:
//        {
//            UINT dwSize = sizeof(RAWINPUT);
//            static BYTE lpb[sizeof(RAWINPUT)];
//
//            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
//
//            RAWINPUT* raw = (RAWINPUT*)lpb;
//
//            if (raw->header.dwType == RIM_TYPEMOUSE && ::bClipCursor)
//            {
//                float xPosRelative = raw->data.mouse.lLastX;
//                float yPosRelative = raw->data.mouse.lLastY;
//
//                g_RotX += yPosRelative * 0.15f;
//                g_RotY += xPosRelative * 0.15f;
//            }
//            break;
//        }
//
//        case WM_DESTROY:
//        {
//            PostQuitMessage(0);
//            return 0;
//        }
//
//        case WM_SETCURSOR:
//        {
//            SetCursor(LoadCursor(NULL, IDC_CROSS));
//            return 0;
//        }
//    }
//    return DefWindowProc(hwnd, message, wParam, lParam);
//}

//LRESULT CALLBACK RightToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    HDC         hdc;
//    PAINTSTRUCT ps;
//    RECT        rect;
//
//    switch (message)
//    {
//    case WM_CREATE:
//    {
//        SetWindowLong(hwnd, 0, 0);
//        return 0;
//    }
//
//    case WM_PAINT:
//    {
//        hdc = BeginPaint(hwnd, &ps);
//
//        GetClientRect(hwnd, &rect);
//        Rectangle(hdc, 0, 0, rect.right, rect.bottom);
//        HBRUSH Brush = CreateSolidBrush(RGB(40, 40, 40));
//        FillRect(hdc, &rect, Brush);
//
//        EndPaint(hwnd, &ps);
//        return 0;
//    }
//    case WM_DESTROY:
//    {
//        PostQuitMessage(0);
//        return 0;
//    }
//    }
//    return DefWindowProc(hwnd, message, wParam, lParam);
//}

//void UnprojectClick3(long InX, long InY)
//{
//    //XMVECTOR OriginPoint = XMVectorSet(InX, InY, 0, 0);
//    XMFLOAT3 OriginPointFloat3 = XMFLOAT3(InX, InY, 0);
//    XMVECTOR OriginPoint = XMLoadFloat3(&OriginPointFloat3);
//
//    XMFLOAT3 DestinationPointFloat3 = XMFLOAT3(InX, InY, 1);
//    XMVECTOR DestinationPoint = XMLoadFloat3(&DestinationPointFloat3);
//
//    XMMATRIX ProjectionMatrix = XMMatrixTranspose(DX11Device::m_ProjectionMatrix);
//    XMMATRIX ViewMatrix = XMMatrixTranspose(DX11Device::m_ViewMatrix);
//    XMMATRIX WorldMatrix = XMMatrixTranspose(DX11Device::m_WorldMatrix);
//
//    XMVECTOR OriginPointUnprojected = XMVector3Unproject(OriginPoint, 0, 0, ViewportWidth, ViewportHeight, 0, 1, ProjectionMatrix, ViewMatrix, WorldMatrix);
//    XMVECTOR DestinationPointUnprojected = XMVector3Unproject(DestinationPoint, 0, 0, ViewportWidth, ViewportHeight, 0, 1, ProjectionMatrix, ViewMatrix, WorldMatrix);
//
//    XMFLOAT3 OriginPointUnprojectedFloat3;
//    XMStoreFloat3(&OriginPointUnprojectedFloat3, OriginPointUnprojected);
//
//    //KMEngine_Log << "KMEngine.cpp, void UnprojectClick3: OriginPointUnprojected X = " << OriginPointUnprojectedFloat3.x << endl;
//    //KMEngine_Log << "KMEngine.cpp, void UnprojectClick3: OriginPointUnprojected Y = " << OriginPointUnprojectedFloat3.y << endl;
//    //KMEngine_Log << "KMEngine.cpp, void UnprojectClick3: OriginPointUnprojected Z = " << OriginPointUnprojectedFloat3.z << endl;
//
//
//    XMVECTOR RayDirection = DestinationPointUnprojected - OriginPointUnprojected;
//    RayDirection = XMVector4Normalize(RayDirection);
//
//    XMFLOAT4 RayDirectionFloat4;
//    XMStoreFloat4(&RayDirectionFloat4, RayDirection);
//
//    //KMEngine_Log << "KMEngine.cpp, void UnprojectClick3: Raydirection X = " << RayDirectionFloat4.x << endl;
//    //KMEngine_Log << "KMEngine.cpp, void UnprojectClick3: Raydirection Y = " << RayDirectionFloat4.y << endl;
//    //KMEngine_Log << "KMEngine.cpp, void UnprojectClick3: Raydirection Z = " << RayDirectionFloat4.z << endl;
//
//    XMFLOAT4 RayOriginFloat4;
//    XMStoreFloat4(&RayOriginFloat4, DestinationPointUnprojected);
//
//    RaycastOriginX = OriginPointUnprojectedFloat3.x;
//    RaycastOriginY = OriginPointUnprojectedFloat3.y;
//    RaycastOriginZ = OriginPointUnprojectedFloat3.z;
//
//    RaycastDestinationX = RayDirectionFloat4.x * 100;
//    RaycastDestinationY = RayDirectionFloat4.y * 100;
//    RaycastDestinationZ = RayDirectionFloat4.z * 100;
//
//    m_DX11Device->AddLine(RaycastOriginX, RaycastOriginY, RaycastOriginZ, RaycastDestinationX, RaycastDestinationY, RaycastDestinationZ);
//}

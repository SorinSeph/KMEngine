#pragma once

#include "DX11Device.h"
#include "ViewportWindow.h"
#include <Windows.h>
#include "resource.h"
#include "Renderer.h"

const wchar_t CLASS_NAME[] = L"KME Engine";
//const wchar_t VIEWPORT_NAME[] = L"Viewport";
const wchar_t TOOLBAR_NAME[] = L"Toolbar";
const wchar_t SIDETOOLBAR_NAME[] = L"SideToolbar";
//const wchar_t VIEWPORT_NAME[]{ L"Viewport" };

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LeftToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK RightToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK ViewportWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND ViewportHwnd;
HWND RightToolbarHwnd;
HWND LeftToolbarHwnd;

float ViewportWidth{ };
float ViewportHeight{ };

bool bClipCursor = false;
//float g_RotX{};
//float g_RotY{};

float RaycastOriginX = 0.0f;
float RaycastOriginY = 0.0f;
float RaycastOriginZ = 0.0f;
float RaycastDestinationX = 0.0f;
float RaycastDestinationY = 0.0f;
float RaycastDestinationZ = 0.0f;


//Renderer* g_Renderer{ nullptr };

class EngineInitializer
{
public:

    EngineInitializer(HINSTANCE hInstance, int nCmdShow)
        : m_HInstance{ hInstance }
        , m_NCmdShow{ nCmdShow }
        , m_Renderer{ ViewportHwnd }
        , m_ViewportWindow{ }
        , m_RotX{ m_ViewportWindow.GetXRotation() }
        , m_RotY{ m_ViewportWindow.GetYRotation() }
        , m_RotX2{ m_ViewportWindow.GetXRotation3() }
        , m_RotY2{ m_ViewportWindow.GetYRotation3() }
        //, m_DX11Device{ new DX11Device }
    {
        //g_Renderer = &m_Renderer;
    }

    void RayCast(long InX, long InY)
    {
        if (CanRaycast())
        {
            // Test
            //XMVECTOR OriginPoint = XMVectorSet(InX, InY, 0, 0);
            XMFLOAT3 OriginPointFloat3 = XMFLOAT3(InX, InY, 0);
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

            //KMEngine_Log << "KMEngine.cpp, void UnprojectClick3: OriginPointUnprojected X = " << OriginPointUnprojectedFloat3.x << endl;
            //KMEngine_Log << "KMEngine.cpp, void UnprojectClick3: OriginPointUnprojected Y = " << OriginPointUnprojectedFloat3.y << endl;
            //KMEngine_Log << "KMEngine.cpp, void UnprojectClick3: OriginPointUnprojected Z = " << OriginPointUnprojectedFloat3.z << endl;


            XMVECTOR RayDirection = DestinationPointUnprojected - OriginPointUnprojected;
            RayDirection = XMVector4Normalize(RayDirection);

            XMFLOAT4 RayDirectionFloat4;
            XMStoreFloat4(&RayDirectionFloat4, RayDirection);

            //KMEngine_Log << "KMEngine.cpp, void UnprojectClick3: Raydirection X = " << RayDirectionFloat4.x << endl;
            //KMEngine_Log << "KMEngine.cpp, void UnprojectClick3: Raydirection Y = " << RayDirectionFloat4.y << endl;
            //KMEngine_Log << "KMEngine.cpp, void UnprojectClick3: Raydirection Z = " << RayDirectionFloat4.z << endl;

            XMFLOAT4 RayOriginFloat4;
            XMStoreFloat4(&RayOriginFloat4, DestinationPointUnprojected);

            RaycastOriginX = OriginPointUnprojectedFloat3.x;
            RaycastOriginY = OriginPointUnprojectedFloat3.y;
            RaycastOriginZ = OriginPointUnprojectedFloat3.z;

            RaycastDestinationX = RayDirectionFloat4.x * 100;
            RaycastDestinationY = RayDirectionFloat4.y * 100;
            RaycastDestinationZ = RayDirectionFloat4.z * 100;
            //DX11Device* dx11Device = DX11Device::GetDX11Device();
            //dx11Device->InitLine(RaycastOriginX, RaycastOriginY, RaycastOriginZ, RaycastDestinationX, RaycastDestinationY, RaycastDestinationZ);

            m_Renderer.Raycast(RaycastOriginX, RaycastOriginY, RaycastOriginZ, RaycastDestinationX, RaycastDestinationY, RaycastDestinationZ);
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
        WNDCLASS wc[4]{ };

        wc[0].style = CS_HREDRAW | CS_VREDRAW;
        wc[0].lpfnWndProc = WindowProc;
        wc[0].cbClsExtra = 0;
        wc[0].cbWndExtra = 0;
        wc[0].hInstance = m_HInstance;
        wc[0].hCursor = LoadCursor(NULL, IDC_ARROW);
        wc[0].hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc[0].lpszClassName = CLASS_NAME;

        wc[1].style = CS_HREDRAW | CS_VREDRAW;
        wc[1].lpfnWndProc = LeftToolbarHwndProc;
        wc[1].cbClsExtra = 0;
        wc[1].cbWndExtra = 0;
        wc[1].hInstance = m_HInstance;
        wc[1].hCursor = LoadCursor(NULL, IDC_ARROW);
        wc[1].hbrBackground = NULL;
        wc[1].lpszClassName = TOOLBAR_NAME;

        wc[2].style = CS_HREDRAW | CS_VREDRAW;
        wc[2].lpfnWndProc = CViewportWindow::ViewportWndProc;
        wc[2].cbClsExtra = 0;
        wc[2].cbWndExtra = 0;
        wc[2].hInstance = m_HInstance;
        wc[2].hCursor = LoadCursor(NULL, IDC_ARROW);
        wc[2].hbrBackground = NULL;
        wc[2].lpszClassName = VIEWPORT_NAME;

        wc[3].style = CS_HREDRAW | CS_VREDRAW;
        wc[3].lpfnWndProc = RightToolbarHwndProc;
        wc[3].cbClsExtra = 0;
        wc[3].cbWndExtra = 0;
        wc[3].hInstance = m_HInstance;
        wc[3].hCursor = LoadCursor(NULL, IDC_ARROW);
        wc[3].hbrBackground = NULL;
        wc[3].lpszClassName = SIDETOOLBAR_NAME;

        for (int i = 0; i < 4; i++)
        {
            if (!RegisterClass(&wc[i]))
            {
                MessageBox(NULL, TEXT("Program requires Windows NT!"), CLASS_NAME, MB_ICONERROR);
                return 0;
            }
        }

        // Create the window.

        HWND hwnd = CreateWindow(
            CLASS_NAME,
            L"KMEngine",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            2560,
            1440,
            NULL,
            NULL,
            m_HInstance,
            NULL
        );

        LeftToolbarHwnd = CreateWindow(
            TOOLBAR_NAME,
            NULL,
            WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
            0,
            0,
            0,
            0,
            hwnd,
            NULL,
            (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE),
            NULL
        );

        CViewportWindow::SetViewportParentHWND(hwnd);
        m_ViewportWindow.CreateViewport();


        //ViewportHwnd = CreateWindow(
        //    VIEWPORT_NAME,
        //    NULL,
        //    WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
        //    0,
        //    0,
        //    0,
        //    0,
        //    hwnd,
        //    NULL,
        //    (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE),
        //    NULL
        //);

        ViewportHwnd = m_ViewportWindow.GetViewportHwnd();

        RightToolbarHwnd = CreateWindow(
            SIDETOOLBAR_NAME,
            NULL,
            WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
            0,
            0,
            0,
            0,
            hwnd,
            NULL,
            (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE),
            NULL
        );

        if (hwnd == NULL)
        {
            return 0;
        }

        RAWINPUTDEVICE Rid[1]{};
        Rid[0].usUsagePage = 0x01;
        Rid[0].usUsage = 0x02;
        Rid[0].dwFlags = RIDEV_INPUTSINK;
        Rid[0].hwndTarget = m_ViewportWindow.GetViewportHwnd();
        RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

        ShowWindow(hwnd, m_NCmdShow);
        UpdateWindow(hwnd);

        RECT RectViewport;
        GetClientRect(m_ViewportWindow.GetViewportHwnd(), &RectViewport);
        ViewportWidth = RectViewport.left + RectViewport.right;
        ViewportHeight = RectViewport.bottom;

        m_Renderer.SetViewport(m_ViewportWindow.GetViewportHwnd());
        m_Renderer.InitRenderer();

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

    Renderer GetRenderer()
    {
        return m_Renderer;
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
        return m_ViewportWindow.GetRaycastX();
    }

    float GetRaycastY()
    {
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
    HINSTANCE m_HInstance{ };
    int m_NCmdShow{ };
    Renderer m_Renderer;
    Renderer* m_pRenderer;
    //CDX11Device m_pDX11evice{ };
    CViewportWindow m_ViewportWindow;
    float m_RotX;
    float m_RotY;

    float* m_RotX2;
    float* m_RotY2;
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;
    int cxBlock, cyBlock, x, y;

    switch (uMsg)
    {
    case WM_CREATE:
    {
        HMENU hMenu, hSubMenu;
        hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU1));
        SetMenu(hwnd, hMenu);

        return 0;
    }

    case WM_SIZE:
    {
        cxBlock = LOWORD(lParam) / 3;
        cyBlock = HIWORD(lParam) / 3;

        for (x = 0; x < 3; x++)
        {
            if (x == 0)
            {
                MoveWindow(LeftToolbarHwnd, x * cxBlock, 50, cxBlock + 5, cyBlock * 3, TRUE);
            }
            else if (x == 1)
            {
                MoveWindow(ViewportHwnd, x * cxBlock, 50, cxBlock + 5, cyBlock * 3, TRUE);
                ViewportWidth = cxBlock + 5;
                ViewportHeight = cyBlock * 3;
            }
            else if (x == 2)
            {
                MoveWindow(RightToolbarHwnd, x * cxBlock, 50, cxBlock + 5, cyBlock * 3, TRUE);
            }
        }
        return 0;
    }

    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &rect);
        Rectangle(hdc, 0, 0, rect.right, rect.bottom);
        HBRUSH Brush = CreateSolidBrush(RGB(40, 40, 40));
        FillRect(hdc, &rect, Brush);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK LeftToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC         hdc;
    PAINTSTRUCT ps;
    RECT        rect;

    switch (message)
    {
    case WM_CREATE:
    {
        SetWindowLong(hwnd, 0, 0);
        return 0;
    }

    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &rect);
        Rectangle(hdc, 0, 0, rect.right, rect.bottom);
        HBRUSH Brush = CreateSolidBrush(RGB(40, 40, 40));
        FillRect(hdc, &rect, Brush);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

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

LRESULT CALLBACK RightToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC         hdc;
    PAINTSTRUCT ps;
    RECT        rect;

    switch (message)
    {
    case WM_CREATE:
    {
        SetWindowLong(hwnd, 0, 0);
        return 0;
    }

    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd, &ps);

        GetClientRect(hwnd, &rect);
        Rectangle(hdc, 0, 0, rect.right, rect.bottom);
        HBRUSH Brush = CreateSolidBrush(RGB(40, 40, 40));
        FillRect(hdc, &rect, Brush);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

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

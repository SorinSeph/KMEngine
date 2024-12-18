#pragma once
#include "BaseModule.h"
#include "resource.h"
#include "ViewportWindow.h"
#include <Windows.h>

const wchar_t CLASS_NAME[] = L"KME Engine";
const wchar_t TOOLBAR_NAME[] = L"Toolbar";
const wchar_t SIDETOOLBAR_NAME[] = L"SideToolbar";

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LeftToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK RightToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND ViewportHwnd;
HWND RightToolbarHwnd;
HWND LeftToolbarHwnd;

float ViewportWidth{ };
float ViewportHeight{ };

class CUIModule : public CBaseModule
{
public:
    HRESULT Initialize(HINSTANCE hInstance, int nCmdShow)
    {
        WNDCLASS wc[4]{ };

        wc[0].style = CS_HREDRAW | CS_VREDRAW;
        wc[0].lpfnWndProc = WindowProc;
        wc[0].cbClsExtra = 0;
        wc[0].cbWndExtra = 0;
        wc[0].hInstance = hInstance;
        wc[0].hCursor = LoadCursor(NULL, IDC_ARROW);
        wc[0].hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc[0].lpszClassName = CLASS_NAME;

        wc[1].style = CS_HREDRAW | CS_VREDRAW;
        wc[1].lpfnWndProc = LeftToolbarHwndProc;
        wc[1].cbClsExtra = 0;
        wc[1].cbWndExtra = 0;
        wc[1].hInstance = hInstance;
        wc[1].hCursor = LoadCursor(NULL, IDC_ARROW);
        wc[1].hbrBackground = NULL;
        wc[1].lpszClassName = TOOLBAR_NAME;

        wc[2].style = CS_HREDRAW | CS_VREDRAW;
        wc[2].lpfnWndProc = CViewportWindow::ViewportWndProc;
        wc[2].cbClsExtra = 0;
        wc[2].cbWndExtra = 0;
        wc[2].hInstance = hInstance;
        wc[2].hCursor = LoadCursor(NULL, IDC_ARROW);
        wc[2].hbrBackground = NULL;
        wc[2].lpszClassName = VIEWPORT_NAME;

        wc[3].style = CS_HREDRAW | CS_VREDRAW;
        wc[3].lpfnWndProc = RightToolbarHwndProc;
        wc[3].cbClsExtra = 0;
        wc[3].cbWndExtra = 0;
        wc[3].hInstance = hInstance;
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
            hInstance,
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

        HWND ButtonHwnd = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed
            L"Click Me",      // Button text
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
            10,         // x position
            10,         // y position
            100,        // Button width
            30,        // Button height
            LeftToolbarHwnd,       // Parent window
            (HMENU)1,       // Button ID
            (HINSTANCE)GetWindowLongPtr(LeftToolbarHwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed

        if (ButtonHwnd == NULL)
        {
            MessageBox(NULL, L"Button creation failed!", L"Error", MB_ICONERROR);
            return 0;
        }

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

        ViewportHwnd = m_ViewportWindow.GetViewportWnd();

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
        Rid[0].hwndTarget = m_ViewportWindow.GetViewportWnd();
        RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);

        RECT RectViewport;
        GetClientRect(m_ViewportWindow.GetViewportWnd(), &RectViewport);
        ViewportWidth = RectViewport.left + RectViewport.right;
        ViewportHeight = RectViewport.bottom;

        //m_World.Init();
        //GameEntityBuilder EntityBuilder{ m_Renderer.GetDX11Device() };
        //EntityBuilder.InitDefaultEntities();
        //EntityBuilder.TestDevice();
        //TerrainGenerator.GenerateTestTerrain();

        return S_OK;
    }

    //HWND ViewportHwnd;
    //HWND RightToolbarHwnd;
    //HWND LeftToolbarHwnd;

    CViewportWindow m_ViewportWindow{ };

    float ViewportWidth{ };
    float ViewportHeight{ };

    HINSTANCE m_HInstance{ };
    int m_NCmdShow{ };
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
            HBRUSH Brush = CreateSolidBrush(RGB(70, 40, 40));
            FillRect(hdc, &rect, Brush);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
                case 1: // Button ID
                    MessageBox(hwnd, L"Button clicked!", L"Notification", MB_OK);
                    break;
                default:
                    return DefWindowProc(hwnd, message, wParam, lParam);
            }
        }
        break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

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
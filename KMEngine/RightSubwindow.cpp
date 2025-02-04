#include "RightSubwindow.h"

void CRightSubwindow::CreateRightSubwindow(HWND& ParentHwnd)
{
	m_RightSubwindowHwnd = CreateWindow(
        L"RightSubwindow",
        NULL,
        WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
        0,
        0,
        0,
        0,
        ParentHwnd,
        NULL,
        (HINSTANCE)GetWindowLong(ParentHwnd, GWLP_HINSTANCE),
        NULL
    );
}

LRESULT CALLBACK CRightSubwindow::RightSubwindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC         hdc;
    PAINTSTRUCT ps;
    RECT        rect;

    switch (message)
    {
        case WM_CREATE:
        {
            SetWindowLong(hwnd, 0, 0);

            GetClientRect(hwnd, &rect);

            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            // Create the blue child window (top half)
            CRightSubwindow::m_OutlinerHwnd = CreateWindowEx(
                0,                              // Optional window styles
                L"Outliner",           // Window class
                NULL,                           // No window text
                WS_CHILD | WS_VISIBLE,          // Window style
                0, 0,                           // Position
                width, height / 2,              // Size
                hwnd,                           // Parent window
                NULL,                           // No menu
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                NULL                            // Additional application data
            );

        //    if (OutlinerHwnd == NULL)
        //    {
        //        MessageBox(hwnd, L"Could not create outliner window.", L"Error", MB_OK | MB_ICONERROR);
        //    }

        //    // Create the red child window (bottom half)
        //    PropertiesHwnd = CreateWindowEx(
        //        0,                              // Optional window styles
        //        L"Properties",            // Window class
        //        NULL,                           // No window text
        //        WS_CHILD | WS_VISIBLE,          // Window style
        //        0, height / 2,                  // Position
        //        width, height / 2,              // Size
        //        hwnd,                           // Parent window
        //        NULL,                           // No menu
        //        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        //        NULL                            // Additional application data
        //    );

        //    if (PropertiesHwnd == NULL)
        //    {
        //        MessageBox(hwnd, L"Could not create properties window.", L"Error", MB_OK | MB_ICONERROR);
        //    }

            return 0;
        }

        case WM_PAINT:
        {
            hdc = BeginPaint(hwnd, &ps);

            GetClientRect(hwnd, &rect);
            Rectangle(hdc, 0, 0, rect.right, rect.bottom);
            HBRUSH Brush = CreateSolidBrush(RGB(40, 40, 140));
            FillRect(hdc, &rect, Brush);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_SIZE:
        {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);

            // Resize the blue child window to top half
            if (OutlinerProc)
            {
                SetWindowPos(CRightSubwindow::m_OutlinerHwnd, NULL, 0, 0, width, height / 4, SWP_NOZORDER);
            }

            // Resize the red child window to bottom half
            //if (PropertiesProc)
            //{
            //    SetWindowPos(PropertiesHwnd, NULL, 0, height / 4, width, (height * 3) / 4, SWP_NOZORDER);
            //}

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

LRESULT CALLBACK CRightSubwindow::OutlinerProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Fill the child window with blue color
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0)); // Blue
            FillRect(hdc, &ps.rcPaint, hBrush);
            DeleteObject(hBrush);

            // Set text color to white and background mode to transparent
            SetTextColor(hdc, RGB(255, 255, 255)); // White text
            SetBkMode(hdc, TRANSPARENT);

            //CScene& Scene = CScene::GetScene();
            //auto& SceneEntityList = Scene.GetSceneList();
            //std::string EntityName = SceneEntityList.at(0).m_GameEntityTag;

            // Define the text to display
            const wchar_t* szText = L"Test text";

            // Get the client rectangle for text positioning
            RECT rect;
            GetClientRect(hwnd, &rect);

            const int Padding = 10;

            rect.left += Padding;

            // Draw the text aligned to top-left within the child window
            DrawText(hdc, szText, -1, &rect, DT_SINGLELINE | DT_LEFT | DT_TOP | DT_NOPREFIX);

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

HWND CRightSubwindow::m_OutlinerHwnd{ };
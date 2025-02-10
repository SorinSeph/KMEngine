#pragma once

#include <Windows.h>

class CRightSubwindow
{
public:
	static LRESULT CALLBACK RightSubwindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK OutlinerProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void CreateRightSubwindow(HWND& ParentHwnd);

	HWND GetRightSubwindowHwnd() const { return m_RightSubwindowHwnd; }

	HWND m_RightSubwindowHwnd{ };

	static HWND m_OutlinerHwnd;

	static HWND m_PositionXEditControl;
	static HWND	m_PositionYEditControl;
	static HWND m_PositionZEditControl;
	
};

//LRESULT CALLBACK CRightSubwindow::RightSubwindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
//
//            GetClientRect(hwnd, &rect);
//
//            int width = rect.right - rect.left;
//            int height = rect.bottom - rect.top;
//
//            // Create the blue child window (top half)
//        //    OutlinerHwnd = CreateWindowEx(
//        //        0,                              // Optional window styles
//        //        L"Outliner",           // Window class
//        //        NULL,                           // No window text
//        //        WS_CHILD | WS_VISIBLE,          // Window style
//        //        0, 0,                           // Position
//        //        width, height / 2,              // Size
//        //        hwnd,                           // Parent window
//        //        NULL,                           // No menu
//        //        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
//        //        NULL                            // Additional application data
//        //    );
//
//        //    if (OutlinerHwnd == NULL)
//        //    {
//        //        MessageBox(hwnd, L"Could not create outliner window.", L"Error", MB_OK | MB_ICONERROR);
//        //    }
//
//        //    // Create the red child window (bottom half)
//        //    PropertiesHwnd = CreateWindowEx(
//        //        0,                              // Optional window styles
//        //        L"Properties",            // Window class
//        //        NULL,                           // No window text
//        //        WS_CHILD | WS_VISIBLE,          // Window style
//        //        0, height / 2,                  // Position
//        //        width, height / 2,              // Size
//        //        hwnd,                           // Parent window
//        //        NULL,                           // No menu
//        //        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
//        //        NULL                            // Additional application data
//        //    );
//
//        //    if (PropertiesHwnd == NULL)
//        //    {
//        //        MessageBox(hwnd, L"Could not create properties window.", L"Error", MB_OK | MB_ICONERROR);
//        //    }
//
//        //    return 0;
//        //}
//
//        case WM_PAINT:
//        {
//            hdc = BeginPaint(hwnd, &ps);
//
//            GetClientRect(hwnd, &rect);
//            Rectangle(hdc, 0, 0, rect.right, rect.bottom);
//            HBRUSH Brush = CreateSolidBrush(RGB(40, 40, 40));
//            FillRect(hdc, &rect, Brush);
//
//            EndPaint(hwnd, &ps);
//            return 0;
//        }
//
//        //case WM_SIZE:
//        //{
//        //    int width = LOWORD(lParam);
//        //    int height = HIWORD(lParam);
//
//        //    // Resize the blue child window to top half
//        //    if (OutlinerProc)
//        //    {
//        //        SetWindowPos(OutlinerHwnd, NULL, 0, 0, width, height / 4, SWP_NOZORDER);
//        //    }
//
//        //    // Resize the red child window to bottom half
//        //    if (PropertiesProc)
//        //    {
//        //        SetWindowPos(PropertiesHwnd, NULL, 0, height / 4, width, (height * 3) / 4, SWP_NOZORDER);
//        //    }
//
//        //    return 0;
//        //}
//
//
//        case WM_DESTROY:
//        {
//            PostQuitMessage(0);
//            return 0;
//        }
//    }
//    return DefWindowProc(hwnd, message, wParam, lParam);
//}
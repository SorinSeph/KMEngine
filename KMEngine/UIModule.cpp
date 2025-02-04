#include "UIModule.h"
#include "Scene.h"

HRESULT CUIModule::Initialize(HINSTANCE hInstance, int nCmdShow)
{
    CViewportMessage& ViewportMessage = CViewportMessage::GetViewportMessage();
    ViewportMessage.m_pUIModule = this;
	ViewportMessage.m_TestInt = 5;

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
    wc[3].lpfnWndProc = CRightSubwindow::RightSubwindowProc;
    wc[3].cbClsExtra = 0;
    wc[3].cbWndExtra = 0;
    wc[3].hInstance = hInstance;
    wc[3].hCursor = LoadCursor(NULL, IDC_ARROW);
    wc[3].hbrBackground = NULL;
    wc[3].lpszClassName = L"RightSubwindow";

    for (int i = 0; i < 4; i++)
    {
        if (!RegisterClass(&wc[i]))
        {
            MessageBox(NULL, TEXT("Program requires Windows NT!"), CLASS_NAME, MB_ICONERROR);
            return 0;
        }
    }


    WNDCLASS OutlinerWC = { };
    OutlinerWC.lpfnWndProc = CRightSubwindow::OutlinerProc;
    OutlinerWC.hInstance = hInstance;
    OutlinerWC.lpszClassName = L"Outliner";
    OutlinerWC.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&OutlinerWC);

    WNDCLASS PropertiesWC = { };
    PropertiesWC.lpfnWndProc = PropertiesProc;
    PropertiesWC.hInstance = hInstance;
    PropertiesWC.lpszClassName = L"Properties";
    PropertiesWC.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&PropertiesWC);



    hwnd = CreateWindow(
        CLASS_NAME,
        L"KMEApp",
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
    m_pViewportWindow->CreateViewport();

    ViewportHwnd = m_pViewportWindow->GetViewportHwnd();

    m_RightSubwindow.CreateRightSubwindow(hwnd);
	RightSubwindowHwnd = m_RightSubwindow.GetRightSubwindowHwnd();
    //RightSubwindowHwnd = CreateWindow(
    //    SIDETOOLBAR_NAME,
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



    if (hwnd == NULL)
    {
        return 0;
    }

    RAWINPUTDEVICE Rid[1]{};
    Rid[0].usUsagePage = 0x01;
    Rid[0].usUsage = 0x02;
    Rid[0].dwFlags = RIDEV_INPUTSINK;
    Rid[0].hwndTarget = m_pViewportWindow->GetViewportHwnd();
    RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    RECT RectViewport;
    GetClientRect(m_pViewportWindow->GetViewportHwnd(), &RectViewport);
    ViewportWidth = RectViewport.left + RectViewport.right;
    ViewportHeight = RectViewport.bottom;

    //m_World.Init();
    //GameEntityBuilder EntityBuilder{ m_Renderer.GetDX11Device() };
    //EntityBuilder.InitDefaultEntities();
    //EntityBuilder.TestDevice();
    //TerrainGenerator.GenerateTestTerrain();

    return S_OK;
}

void CUIModule::TestLog(int X, int Y)
{
    // Create a stringstream object
    std::stringstream ss;

    // Construct the string with the integer values
    ss << "Value of X = " << X << "; and value of Y = " << Y;

    // Convert the stringstream to a std::string
    std::string logMessage = ss.str();

    // Log the message using CLogger
    CLogger& Logger = CLogger::GetLogger();
    Logger.Log("CUIModule::TestLog", logMessage.c_str());
}

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
                    MoveWindow(RightSubwindowHwnd, x * cxBlock, 50, cxBlock + 5, cyBlock * 3, TRUE);
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

//LRESULT CALLBACK RightToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
//            OutlinerHwnd = CreateWindowEx(
//                0,                              // Optional window styles
//                L"Outliner",           // Window class
//                NULL,                           // No window text
//                WS_CHILD | WS_VISIBLE,          // Window style
//                0, 0,                           // Position
//                width, height / 2,              // Size
//                hwnd,                           // Parent window
//                NULL,                           // No menu
//                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
//                NULL                            // Additional application data
//            );
//
//            if (OutlinerHwnd == NULL)
//            {
//                MessageBox(hwnd, L"Could not create outliner window.", L"Error", MB_OK | MB_ICONERROR);
//            }
//
//            // Create the red child window (bottom half)
//            PropertiesHwnd = CreateWindowEx(
//                0,                              // Optional window styles
//                L"Properties",            // Window class
//                NULL,                           // No window text
//                WS_CHILD | WS_VISIBLE,          // Window style
//                0, height / 2,                  // Position
//                width, height / 2,              // Size
//                hwnd,                           // Parent window
//                NULL,                           // No menu
//                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
//                NULL                            // Additional application data
//            );
//
//            if (PropertiesHwnd == NULL)
//            {
//                MessageBox(hwnd, L"Could not create properties window.", L"Error", MB_OK | MB_ICONERROR);
//            }
//
//            return 0;
//        }
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
//        case WM_SIZE:
//        {
//            int width = LOWORD(lParam);
//            int height = HIWORD(lParam);
//
//            // Resize the blue child window to top half
//            if (OutlinerProc)
//            {
//                SetWindowPos(OutlinerHwnd, NULL, 0, 0, width, height / 4, SWP_NOZORDER);
//            }
//
//            // Resize the red child window to bottom half
//            if (PropertiesProc)
//            {
//                SetWindowPos(PropertiesHwnd, NULL, 0, height / 4, width, (height * 3) / 4, SWP_NOZORDER);
//            }
//
//            return 0;
//        }
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

LRESULT CALLBACK OutlinerProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
            HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255)); // Blue
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

LRESULT CALLBACK PropertiesProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
            HBRUSH Brush = CreateSolidBrush(RGB(60, 0, 0));
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
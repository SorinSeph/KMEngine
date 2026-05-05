#include "UIModule.h"
#include "../Scene.h"
#include "../resource.h"
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")

HRESULT CUIModule::Initialize(HINSTANCE hInstance, int nCmdShow)
{
    CUIMessageQueue& MessageQueue = CUIMessageQueue::GetUIMessageQueue();
    MessageQueue.m_pUIModule = this;
	MessageQueue.m_TestInt = 5;

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

    HWND hTabControl = CreateWindowEx(
        0,
        WC_TABCONTROL,
        NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        0, 0, 0, 0,  // Will be resized in WM_SIZE
        LeftToolbarHwnd,
        (HMENU)100,  // Tab control ID
        (HINSTANCE)GetWindowLongPtr(LeftToolbarHwnd, GWLP_HINSTANCE),
        NULL
    );

    // Add tabs
    TCITEM tie = {};
    tie.mask = TCIF_TEXT;

    tie.pszText = (LPWSTR)L"Tab 1";
    TabCtrl_InsertItem(hTabControl, 0, &tie);

    tie.pszText = (LPWSTR)L"Tab 2";
    TabCtrl_InsertItem(hTabControl, 1, &tie);

    // @Temporary use this button to import the GLTF file
    HWND ImportButtonHWND = CreateWindow(
        L"BUTTON",  
        L"Import GLTF",      
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  
        10,        
        10,          
        120,        
        30,        
        LeftToolbarHwnd,       
        (HMENU)1,       
        (HINSTANCE)GetWindowLongPtr(LeftToolbarHwnd, GWLP_HINSTANCE),
        NULL);      

    if (ImportButtonHWND == NULL)
    {
        MessageBox(NULL, L"Button creation failed!", L"Error", MB_ICONERROR);
        return 0;
    }

    HWND PlayAnimationButtonHWND = CreateWindow(
        L"BUTTON",
        L"Play animation",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,
        50,
        120,
        30,
        LeftToolbarHwnd,
        (HMENU)2,
        (HINSTANCE)GetWindowLongPtr(LeftToolbarHwnd, GWLP_HINSTANCE),
        NULL);

    if (PlayAnimationButtonHWND == NULL)
    {
        MessageBox(NULL, L"Button creation failed!", L"Error", MB_ICONERROR);
        return 0;
    }

    // Show buttons for Tab 2 by default (index 1)
    TabCtrl_SetCurSel(hTabControl, 1);
    ShowWindow(ImportButtonHWND, SW_SHOW);
    ShowWindow(PlayAnimationButtonHWND, SW_SHOW);

    CViewportWindow::SetViewportParentHWND(hwnd);
    m_ViewportWindow.CreateViewport();

    ViewportHwnd = m_ViewportWindow.GetViewportHwnd();

    m_RightSubwindow.CreateRightSubwindow(hwnd);
	RightSubwindowHwnd = m_RightSubwindow.GetRightSubwindowHwnd();

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

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    RECT RectViewport;
    GetClientRect(m_ViewportWindow.GetViewportHwnd(), &RectViewport);
    ViewportWidth = RectViewport.left + RectViewport.right;
    ViewportHeight = RectViewport.bottom;

    return S_OK;
}

void CUIModule::SetOutlinerText()
{
    const std::wstring& newText{ L"UI Test" };
    SetWindowText(CRightSubwindow::m_OutlinerHwnd, newText.c_str());
    // InvalidateRect and UpdateWindow are optional
    InvalidateRect(CRightSubwindow::m_OutlinerHwnd, NULL, TRUE);
    UpdateWindow(CRightSubwindow::m_OutlinerHwnd);
}

void CUIModule::UpdateSelectedEntityString(std::string SelectedEntityString)
{
    m_RightSubwindow.UpdateSelectedEntityString(SelectedEntityString);
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

        case WM_COMMAND:
        {
			CLogger& Logger = CLogger::GetLogger();

            int WindowMenuId = LOWORD(wParam);
            switch (WindowMenuId)
            {
                case ID_IMPORT_GLTF:
                {
                    WCHAR szFile[MAX_PATH] = { 0 };
                    OPENFILENAMEW ofn = {};
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrFilter = L"GLTF Files\0*.gltf;*.glb\0All Files\0*.*\0\0";
                    ofn.nFilterIndex = 1;
                    ofn.lpstrTitle = L"Import GLTF File";
                    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_LONGNAMES;

                    if (GetOpenFileNameW(&ofn))
                    {
                        HANDLE hFile = CreateFileW(szFile, GENERIC_READ, FILE_SHARE_READ, NULL,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                        if (hFile == INVALID_HANDLE_VALUE)
                        {
                            MessageBox(hwnd, L"Could not open the selected file.", L"File Error", MB_OK | MB_ICONERROR);
                            return 0;
                        }

                        wstring FilePathWString(szFile);
                        string FilePathString(FilePathWString.begin(), FilePathWString.end());
      //                  std::string OutputString1{ "CUIModule::WindowProc: File path is " };
      //                  std::string OutputString{ OutputString1  + FilePathString };
						//Logger.Log(OutputString);

                        LARGE_INTEGER fileSize = {};
                        if (!GetFileSizeEx(hFile, &fileSize) || fileSize.QuadPart < 0)
                        {
                            CloseHandle(hFile);
                            MessageBox(hwnd, L"Could not determine file size.", L"File Error", MB_OK | MB_ICONERROR);
                            return 0;
                        }

                        std::string FileContent;

                        if (fileSize.QuadPart > 0)
                        {
                            size_t fileContentSize = static_cast<size_t>(fileSize.QuadPart);
                            FileContent.resize(fileContentSize);

                            DWORD bytesRead = 0;
                            if (!ReadFile(hFile, &FileContent[0], static_cast<DWORD>(fileContentSize), &bytesRead, NULL)
                                || bytesRead != fileContentSize)
                            {
                                CloseHandle(hFile);
                                MessageBox(hwnd, L"Error reading file.", L"File Error", MB_OK | MB_ICONERROR);
                                return 0;
                            }
                        }

                        CloseHandle(hFile);

					    CUIMessageQueue& UIMessageQueue = CUIMessageQueue::GetUIMessageQueue();
					    UIMessageQueue.ImportGLTF(FilePathString, FileContent);
                    }
                    return 0;
                }
                default:
                    return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
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

        case WM_SIZE:
        {
            // Resize tab control to fill the toolbar window
            HWND hTabControl = GetDlgItem(hwnd, 100);
            if (hTabControl)
            {
                RECT rcClient;
                GetClientRect(hwnd, &rcClient);
                SetWindowPos(hTabControl, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
            }
            return 0;
        }

        case WM_NOTIFY:
        {
            LPNMHDR pnmhdr = (LPNMHDR)lParam;
            if (pnmhdr->idFrom == 100 && pnmhdr->code == TCN_SELCHANGE)
            {
                HWND hTabControl = GetDlgItem(hwnd, 100);
                int iTab = TabCtrl_GetCurSel(hTabControl);

                // Get button handles
                HWND hButton1 = GetDlgItem(hwnd, 1);
                HWND hButton2 = GetDlgItem(hwnd, 2);

                // Show/hide buttons based on selected tab
                if (iTab == 0)  // Tab 1 - empty
                {
                    ShowWindow(hButton1, SW_HIDE);
                    ShowWindow(hButton2, SW_HIDE);
                }
                else if (iTab == 1)  // Tab 2 - show buttons
                {
                    ShowWindow(hButton1, SW_SHOW);
                    ShowWindow(hButton2, SW_SHOW);
                }
            }
            return 0;
        }

        case WM_COMMAND:
        {
            CUIMessageQueue& UIMessageQueue = CUIMessageQueue::GetUIMessageQueue();

            std::string FilePathString{ "Resources/Assets/3D_Objects/GLTF/rectangle_textured_skeletal_anim2.gltf" };
            std::string FileContent{};

            // @Temporary code to import and play a GLTF animation
            int ButtonId = LOWORD(wParam);
            if (ButtonId == 1)
            {
                std::ifstream file(FilePathString, std::ios::binary);
                if (!file)
                {
                    MessageBox(hwnd, L"Could not open file.", L"File Error", MB_OK | MB_ICONERROR);
                    return 0;
                }

                std::string File((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                FileContent = File;

                if (!file.good() && !file.eof())
                {
                    MessageBox(hwnd, L"Error reading file.", L"File Error", MB_OK | MB_ICONERROR);
                    return 0;
                }

                UIMessageQueue.ImportGLTF(FilePathString, FileContent);
            }
            else if (ButtonId == 2)
            {
                UIMessageQueue.PlayGLTFAnimation(FilePathString, FileContent);
            }
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

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

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

            HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255)); // Blue
            FillRect(hdc, &ps.rcPaint, hBrush);
            SetTextColor(hdc, RGB(255, 255, 255)); 
            SetBkMode(hdc, TRANSPARENT);
            DeleteObject(hBrush);

            const wchar_t* szText = L"Test text";

            RECT rect;
            GetClientRect(hwnd, &rect);
            const int Padding = 10;
			rect.left += Padding;
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
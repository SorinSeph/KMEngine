#include "RightSubwindow.h"
#include "ViewportMessage.h"
#include "GraphicsModule.h"
#include "UIModule.h"
#include "Scene.h"
#include <string>

// Initialize static members
HWND CRightSubwindow::m_OutlinerHwnd = nullptr;
HWND CRightSubwindow::m_PositionXLabel = nullptr;
HWND CRightSubwindow::m_PositionXEditControl = nullptr;
HWND CRightSubwindow::m_PositionYLabel = nullptr;
HWND CRightSubwindow::m_PositionYEditControl = nullptr;
HWND CRightSubwindow::m_PositionZLabel = nullptr;
HWND CRightSubwindow::m_PositionZEditControl = nullptr;

// Define unique IDs for Edit Controls
#define ID_EDIT_X 101
#define ID_EDIT_Y 102
#define ID_EDIT_Z 103

// Original Window Procedures for Edit Controls
static WNDPROC wpOrigEditProcX = nullptr;
static WNDPROC wpOrigEditProcY = nullptr;
static WNDPROC wpOrigEditProcZ = nullptr;

// Forward declaration of Subclass Procedure
LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

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
        (HINSTANCE)GetWindowLongPtr(ParentHwnd, GWLP_HINSTANCE),
        NULL
    );

    if (m_RightSubwindowHwnd == NULL)
    {
        MessageBox(NULL, L"Could not create RightSubwindow window.", L"Error", MB_OK | MB_ICONERROR);
    }
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

            // Create the Outliner window
            CRightSubwindow::m_OutlinerHwnd = CreateWindowEx(
                0,                              // Optional window styles
                L"Outliner",                    // Window class
                NULL,                           // No window text
                WS_CHILD | WS_VISIBLE,          // Window style
                0, 0,                           // Position
                width, height / 2,              // Size
                hwnd,                           // Parent window
                NULL,                           // No menu
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                NULL                            // Additional application data
            );

            if (CRightSubwindow::m_OutlinerHwnd == NULL)
            {
                MessageBox(hwnd, L"Could not create Outliner window.", L"Error", MB_OK | MB_ICONERROR);
            }

            return 0;
        }

        case WM_PAINT:
        {
            hdc = BeginPaint(hwnd, &ps);

            GetClientRect(hwnd, &rect);
            Rectangle(hdc, 0, 0, rect.right, rect.bottom);
            HBRUSH Brush = CreateSolidBrush(RGB(40, 40, 140)); // Example color
            FillRect(hdc, &rect, Brush);
            DeleteObject(Brush);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_SIZE:
        {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);

            // Resize the Outliner window
            if (CRightSubwindow::m_OutlinerHwnd)
            {
                SetWindowPos(CRightSubwindow::m_OutlinerHwnd, NULL, 0, 0, width, height / 2, SWP_NOZORDER);
            }

            return 0;
        }

        // Removed WM_DESTROY handler to prevent premature application termination

    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK CRightSubwindow::OutlinerProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
        {
            CViewportMessage& ViewportMessage = CViewportMessage::GetViewportMessage();

            CGraphicsModule* GraphicsModule = nullptr;

            if (ViewportMessage.m_pUIModule)
            {
                for (auto ModuleIt : ViewportMessage.m_pUIModule->m_pMediator->m_ModuleVector)
                {
                    if (ModuleIt.type() == typeid(CGraphicsModule*))
                    {
                        GraphicsModule = std::any_cast<CGraphicsModule*>(ModuleIt);
                        break;
                    }
                }
            }

            // Create Labels and Edit Controls for X, Y, Z inside Outliner
            // Initial positions will be set in WM_SIZE

            // X Label
            CRightSubwindow::m_PositionXLabel = CreateWindowEx(
                0, L"STATIC", L"X:", WS_CHILD | WS_VISIBLE | SS_LEFT,
                0, 0, 30, 20, hwnd, NULL,
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
            );

            // X Edit Control
            CRightSubwindow::m_PositionXEditControl = CreateWindowEx(
                WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                0, 0, 100, 20, hwnd, (HMENU)ID_EDIT_X,
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
            );

            // Subclass X Edit Control
            wpOrigEditProcX = (WNDPROC)SetWindowLongPtr(CRightSubwindow::m_PositionXEditControl, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);

            // Set default text for X Edit Control
            SetWindowText(CRightSubwindow::m_PositionXEditControl, L"-1");

            // Y Label
            CRightSubwindow::m_PositionYLabel = CreateWindowEx(
                0, L"STATIC", L"Y:", WS_CHILD | WS_VISIBLE | SS_LEFT,
                0, 0, 30, 20, hwnd, NULL,
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
            );

            // Y Edit Control
            CRightSubwindow::m_PositionYEditControl = CreateWindowEx(
                WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                0, 0, 100, 20, hwnd, (HMENU)ID_EDIT_Y,
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
            );

            // Subclass Y Edit Control
            wpOrigEditProcY = (WNDPROC)SetWindowLongPtr(CRightSubwindow::m_PositionYEditControl, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);

            // Set default text for Y Edit Control
            SetWindowText(CRightSubwindow::m_PositionYEditControl, L"-1");

            // Z Label
            CRightSubwindow::m_PositionZLabel = CreateWindowEx(
                0, L"STATIC", L"Z:", WS_CHILD | WS_VISIBLE | SS_LEFT,
                0, 0, 30, 20, hwnd, NULL,
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
            );

            // Z Edit Control
            CRightSubwindow::m_PositionZEditControl = CreateWindowEx(
                WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                0, 0, 100, 20, hwnd, (HMENU)ID_EDIT_Z,
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
            );

            // Subclass Z Edit Control
            wpOrigEditProcZ = (WNDPROC)SetWindowLongPtr(CRightSubwindow::m_PositionZEditControl, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);

            // Set default text for Z Edit Control
            SetWindowText(CRightSubwindow::m_PositionZEditControl, L"-1");

            // Optional: Check creation success
            if (!CRightSubwindow::m_PositionXLabel || !CRightSubwindow::m_PositionXEditControl ||
                !CRightSubwindow::m_PositionYLabel || !CRightSubwindow::m_PositionYEditControl ||
                !CRightSubwindow::m_PositionZLabel || !CRightSubwindow::m_PositionZEditControl)
            {
                MessageBox(hwnd, L"Could not create one or more controls.", L"Error", MB_OK | MB_ICONERROR);
            }

            return 0;
        }

        case WM_SIZE:
        {
            int width = LOWORD(lParam);
            // int height = HIWORD(lParam); // Not needed for fixed vertical position

            // Define control dimensions and spacing
            const int labelWidth = 30;
            const int editWidth = 100;
            const int controlHeight = 20;
            const int pairSpacing = 30;      // Space between label-edit pairs
            const int intraPairSpacing = 10; // Space between label and edit within a pair
            const int startY = 35;           // Fixed 35-pixel padding from the top

            // Calculate total width of all controls
            int totalPairs = 3;
            int totalWidth = totalPairs * (labelWidth + intraPairSpacing + editWidth) + (totalPairs - 1) * pairSpacing;

            // Calculate starting X position to center the controls horizontally
            int startX = (width - totalWidth) / 2;

            // Position X Label and Edit
            MoveWindow(CRightSubwindow::m_PositionXLabel, startX, startY, labelWidth, controlHeight, TRUE);
            MoveWindow(CRightSubwindow::m_PositionXEditControl, startX + labelWidth + intraPairSpacing, startY, editWidth, controlHeight, TRUE);

            // Position Y Label and Edit
            int startX_Y = startX + labelWidth + intraPairSpacing + editWidth + pairSpacing;
            MoveWindow(CRightSubwindow::m_PositionYLabel, startX_Y, startY, labelWidth, controlHeight, TRUE);
            MoveWindow(CRightSubwindow::m_PositionYEditControl, startX_Y + labelWidth + intraPairSpacing, startY, editWidth, controlHeight, TRUE);

            // Position Z Label and Edit
            int startX_Z = startX_Y + labelWidth + intraPairSpacing + editWidth + pairSpacing;
            MoveWindow(CRightSubwindow::m_PositionZLabel, startX_Z, startY, labelWidth, controlHeight, TRUE);
            MoveWindow(CRightSubwindow::m_PositionZEditControl, startX_Z + labelWidth + intraPairSpacing, startY, editWidth, controlHeight, TRUE);

            return 0;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Fill the window with red color
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0)); // Red
            FillRect(hdc, &ps.rcPaint, hBrush);
            DeleteObject(hBrush);

            // Set text properties
            SetTextColor(hdc, RGB(255, 255, 255)); // White text
            SetBkMode(hdc, TRANSPARENT);

            // Retrieve the window text
            wchar_t szText[256] = { 0 };
            GetWindowText(hwnd, szText, sizeof(szText) / sizeof(wchar_t));

            // Get client rect for text positioning
            RECT rect;
            GetClientRect(hwnd, &rect);

            const int Padding = 10;
            rect.left += Padding;

            // Draw the retrieved text
            DrawText(hdc, szText, -1, &rect, DT_SINGLELINE | DT_LEFT | DT_TOP | DT_NOPREFIX);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_CTLCOLORSTATIC:
        {
            HDC hdcStatic = (HDC)wParam;
            HWND hwndStatic = (HWND)lParam;

            // Create a brush for the label background if not already created
            static HBRUSH hbrLabelBackground = CreateSolidBrush(RGB(255, 0, 0)); // Red

            // Check if the static control is one of the labels
            if (hwndStatic == CRightSubwindow::m_PositionXLabel
                || hwndStatic == CRightSubwindow::m_PositionYLabel
                || hwndStatic == CRightSubwindow::m_PositionZLabel)
            {
                // Set text color to black
                SetTextColor(hdcStatic, RGB(0, 0, 0));
                SetBkMode(hdcStatic, TRANSPARENT); // Transparent background

                // Return the red brush
                return (INT_PTR)hbrLabelBackground;
            }

            // For other static controls, use default handling
            return DefWindowProc(hwnd, message, wParam, lParam);
        }

        case WM_CTLCOLOREDIT:
        {
            HDC hdcEdit = (HDC)wParam;
            HWND hwndEdit = (HWND)lParam;

            // Create a brush for the edit background if not already created
            static HBRUSH hbrEditBackground = CreateSolidBrush(RGB(255, 255, 255)); // White

            // Check if the edit control is one of the position edits
            if (hwndEdit == CRightSubwindow::m_PositionXEditControl
                || hwndEdit == CRightSubwindow::m_PositionYEditControl
                || hwndEdit == CRightSubwindow::m_PositionZEditControl)
            {
                // Set text color to black
                SetTextColor(hdcEdit, RGB(0, 0, 0));
                SetBkMode(hdcEdit, OPAQUE); // Opaque background

                // Set the background color
                SetBkColor(hdcEdit, RGB(255, 255, 255)); // White

                // Return the white brush
                return (INT_PTR)hbrEditBackground;
            }

            // For other edit controls, use default handling
            return DefWindowProc(hwnd, message, wParam, lParam);
        }

        // Removed WM_DESTROY handler to prevent premature application termination

    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

// Subclass Procedure for Edit Controls
LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CViewportMessage& ViewportMessage = CViewportMessage::GetViewportMessage();
	CLogger& Logger = CLogger::GetLogger();
    CScene& Scene = CScene::GetScene();

    wchar_t buffer[100];

    if (msg == WM_KEYDOWN && wParam == VK_RETURN)
    {
        // Determine the title based on which edit control is sending the message
        const wchar_t* title = nullptr;

        if (hwnd == CRightSubwindow::m_PositionXEditControl)
        {
            title = L"X";
            GetWindowText(hwnd, buffer, sizeof(buffer) / sizeof(wchar_t));

            for (auto& EntityIt : Scene.GetSceneList())
            {
                if (EntityIt.m_GameEntityTag == "TexturedCube")
                {
                    std::wstring ws(buffer);
                    int LocationX = std::stoi(ws);

					Logger.Log("RightSubwindow, EditSubclassProc(): LocationX = ", LocationX);

                    auto& EntityComponent = EntityIt.m_SceneGraph.m_pRootNode->m_TType;
                    EntityComponent.SetLocationF(
                        LocationX,
                        EntityComponent.GetLocationY(),
                        EntityComponent.GetLocationZ()
                    );
                }
            }
        }
        else if (hwnd == CRightSubwindow::m_PositionYEditControl)
        {
            title = L"Y";
            GetWindowText(hwnd, buffer, sizeof(buffer) / sizeof(wchar_t));

            for (auto& EntityIt : Scene.GetSceneList())
            {
                if (EntityIt.m_GameEntityTag == "TexturedCube")
                {
                    std::wstring ws(buffer);
                    int LocationY = std::stoi(ws);

                    Logger.Log("RightSubwindow, EditSubclassProc(): LocationX = ", LocationY);

                    auto& EntityComponent = EntityIt.m_SceneGraph.m_pRootNode->m_TType;
                    EntityComponent.SetLocationF(
                        EntityComponent.GetLocationX(),
                        LocationY,
                        EntityComponent.GetLocationZ()
                    );
                }
            }
        }
        else if (hwnd == CRightSubwindow::m_PositionZEditControl)
        {
            title = L"Z";
            GetWindowText(hwnd, buffer, sizeof(buffer) / sizeof(wchar_t));

            for (auto& EntityIt : Scene.GetSceneList())
            {
                if (EntityIt.m_GameEntityTag == "TexturedCube")
                {
                    std::wstring ws(buffer);
                    int LocationZ = std::stoi(ws);

                    Logger.Log("RightSubwindow, EditSubclassProc(): LocationX = ", LocationZ);

                    auto& EntityComponent = EntityIt.m_SceneGraph.m_pRootNode->m_TType;
                    EntityComponent.SetLocationF(
                        EntityComponent.GetLocationX(),
                        EntityComponent.GetLocationY(),
                        LocationZ
                        );
                }
            }
        }
        else
        {
            title = L"Unknown";
        }

        // Construct the message
        //std::wstring message = L"Input number = ";
        //message += buffer;

        // Optionally, display the input received
        // MessageBox(hwnd, message.c_str(), L"Input Received", MB_OK | MB_ICONINFORMATION);

        // Prevent the beep sound by returning 0
        return 0;
    }

    // Call the original window procedure for default handling
    if (hwnd == CRightSubwindow::m_PositionXEditControl)
    {
        return CallWindowProc(wpOrigEditProcX, hwnd, msg, wParam, lParam);
    }
    else if (hwnd == CRightSubwindow::m_PositionYEditControl)
    {
        return CallWindowProc(wpOrigEditProcY, hwnd, msg, wParam, lParam);
    }
    else if (hwnd == CRightSubwindow::m_PositionZEditControl)
    {
        return CallWindowProc(wpOrigEditProcZ, hwnd, msg, wParam, lParam);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}


#include "ViewportWindow.h"

void ViewportWindow::SetViewportParentHWND(HWND hwnd)
{
    ViewportWindow::m_ParentHwnd = hwnd;
}

void ViewportWindow::SetViewportSize(float Width, float Height)
{
    m_ViewportWidth = Width;
    m_ViewportHeight = Height;
}

float ViewportWindow::ConvertToRadians(float Degrees)
{
    return Degrees * (XM_PI / 180.0f);
}

void ViewportWindow::CreateViewport()
{
    m_ViewportHwnd = CreateWindow(
        VIEWPORT_NAME,
        NULL,
        WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS,
        0,
        0,
        0,
        0,
        m_ParentHwnd,
        NULL,
        (HINSTANCE)GetWindowLong(m_ParentHwnd, GWLP_HINSTANCE),
        NULL
    );
}

LRESULT CALLBACK ViewportWindow::ViewportWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC         hdc;
    PAINTSTRUCT ps;
    RECT        rect;
    POINT       CursorPoint;
    //bool bClipCursor;

    switch (message)
    {
    case WM_CREATE:
    {
        bClipCursor = false;
        SetWindowLong(hwnd, 0, 0);       // on/off flag
        RECT RectViewport;
        GetClientRect(m_ViewportHwnd, &RectViewport);
        m_ViewportWidth = RectViewport.left + RectViewport.right;
        m_ViewportHeight = RectViewport.bottom;
        return 0;
    }

    //case WM_PAINT:
    //{
    //    hdc = BeginPaint(hwnd, &ps);
    //    GetClientRect(hwnd, &rect);
    //    Rectangle(hdc, 0, 0, rect.right, rect.bottom);
    //    HBRUSH Brush = CreateSolidBrush(RGB(0, 0, 255));
    //    FillRect(hdc, &rect, Brush);
    //    EndPaint(hwnd, &ps);
    //    return 0;
    //}

    case WM_ACTIVATE:
    {
        bClipCursor = false;
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        g_CanRaycast = true;
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hwnd, &mousePos);

        int mouseX = mousePos.x;
        int mouseY = mousePos.y;

        //KMEngine_Log << "MouseX on click is: " << mouseX << "\n";
        //KMEngine_Log << "MouseY on click is: " << mouseY << "\n";

        float mouseXUnprojected = ((2 * mouseX) / m_ViewportWidth - 1);
        float mouseYUnprojected = 1 - ((2 * mouseY) / m_ViewportHeight);

        //KMEngine_Log << "Unprojected MouseX on click is: " << mouseXUnprojected << "\n";
        //KMEngine_Log << "Unprojected MouseY on click is: " << mouseYUnprojected << "\n";
        //RayCast(mouseX, mouseY);

        g_RaycastX = mouseX;
        g_RaycastY = mouseY;

        //RaycastX = mouseXUnprojected;
        //RaycastY = mouseYUnprojected;

        return 0;
    }

    case WM_RBUTTONUP:
    {
        bClipCursor = false;
        return 0;
    }

    case WM_RBUTTONDOWN:
    {
        bClipCursor = true;
        POINT CursorPoint;
        GetClientRect(hwnd, &rect);

        //if (GetCursorPos(&CursorPoint))
        //{
        //    SetCursorPos(rect.right + 50, rect.bottom - 50); 
        //}

        return 0;
    }

    case WM_MOUSEMOVE:
    {
        POINT ptClientUL, ptClientLR;
        GetClientRect(m_ViewportHwnd, &rect);
        ptClientUL.x = rect.left;
        ptClientUL.y = rect.top;

        // Add one to the right and bottom sides, because the 
        // coordinates retrieved by GetClientRect do not 
        // include the far left and lowermost pixels. 

        ptClientLR.x = rect.right + 1;
        ptClientLR.y = rect.bottom + 1;
        ClientToScreen(m_ViewportHwnd, &ptClientUL);
        ClientToScreen(m_ViewportHwnd, &ptClientLR);

        // Copy the client coordinates of the client area 
        // to the rcClient structure. Confine the mouse cursor 
        // to the client area by passing the rcClient structure 
        // to the ClipCursor function. 

        SetRect(&rect, ptClientUL.x, ptClientUL.y, ptClientLR.x, ptClientLR.y);

        RECT rect2;
        GetClientRect(m_ViewportHwnd, &rect2);


        // Copy the client coordinates of the client area 
        // to the rcClient structure. Confine the mouse cursor 
        // to the client area by passing the rcClient structure 
        // to the ClipCursor function. 
        if (bClipCursor)
        {
            SetCursorPos(3 * (rect2.right) / 2, rect2.bottom / 2);
        }

        //KMEngine_Log << "Viewport rect.left + rect.right / 2 is: " << (rect.left + rect.right) / 2 << "\n";
        //KMEngine_Log << "Viewport rect.bottom / 2 is: " << rect.bottom / 2 << "\n";

        return 0;
    }

    case WM_INPUT:
    {
        UINT dwSize = sizeof(RAWINPUT);
        static BYTE lpb[sizeof(RAWINPUT)];

        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

        RAWINPUT* raw = (RAWINPUT*)lpb;

        if (raw->header.dwType == RIM_TYPEMOUSE && bClipCursor)
        {
            float xPosRelative = raw->data.mouse.lLastX;
            float yPosRelative = raw->data.mouse.lLastY;

            g_RotX += yPosRelative * 0.15f;
            g_RotY += xPosRelative * 0.15f;

            Logger& SLogger = Logger::GetLogger();
            SLogger.Log("ViewportWindow.cpp, ViewportWindow::ViewportWndProc:", "\ng_RotX = ", g_RotX, "\ng_RotY =", g_RotY, "\n");
        }
        break;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }

    case WM_SETCURSOR:
    {
        SetCursor(LoadCursor(NULL, IDC_CROSS));
        return 0;
    }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

HWND ViewportWindow::GetViewportWnd()
{
    return m_ViewportHwnd;
}

float ViewportWindow::GetXRotation()
{
    float XRot{ g_RotX };
    return XRot;
}

float ViewportWindow::GetYRotation()
{
    float YRot{ g_RotY };
    return YRot;
}

float& ViewportWindow::GetXRotation2()
{
    return *m_RotX;
}

float& ViewportWindow::GetYRotation2()
{
    return *m_RotY;
}

float* ViewportWindow::GetXRotation3()
{
    return m_RotX;
}

float* ViewportWindow::GetYRotation3()
{
    return m_RotY;
}

float ViewportWindow::GetXRotation4()
{
    return *m_RotX;
}

float ViewportWindow::GetYRotation4()
{
    return *m_RotY;
}

float ViewportWindow::GetXMemberRotation()
{
    return m_RotX2;
}

float ViewportWindow::GetYMemberRotation()
{
    return m_RotY2;
}

float ViewportWindow::GetEyeX()
{
    return vpEyeX;
}

float ViewportWindow::GetEyeY()
{
    return vpEyeY;
}

float ViewportWindow::GetEyeZ()
{
    return vpEyeZ;
}

float ViewportWindow::GetRaycastX()
{
    float RaycastX{ g_RaycastX };
    return RaycastX;
}

float ViewportWindow::GetRaycastY()
{
    float RaycastY{ g_RaycastY };
    return RaycastY;
}

bool ViewportWindow::CanRaycast()
{
    return g_CanRaycast;
}

void ViewportWindow::SetCanRaycast(bool Value)
{
    g_CanRaycast = Value;
}

bool ViewportWindow::InitViewportDirectInput(HINSTANCE hInstance, HWND hwnd)
{
    HRESULT hr = S_OK;

    hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_DirectInput, NULL);
    hr = _DirectInput->CreateDevice(GUID_SysKeyboard, &_DIKeyboard, NULL);
    hr = _DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
    hr = _DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    return true;
}

//bool ViewportWindow::InitDirectInput(HINSTANCE hInstance)
//{
//    HRESULT hr = S_OK;
//
//    hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_DirectInput, NULL);
//    hr = _DirectInput->CreateDevice(GUID_SysKeyboard, &_DIKeyboard, NULL);
//    hr = _DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
//    hr = _DIKeyboard->SetCooperativeLevel(m_ViewportHwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
//
//    return true;
//}

void ViewportWindow::DetectKeyboardInput()
{
    DIMOUSESTATE mouseCurrState;

    BYTE keyboardState[256];

    _DIKeyboard->Acquire();
    _DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

    if (keyboardState[DIK_ESCAPE] & 0x80)
        PostMessage(ViewportWindow::m_ParentHwnd, WM_DESTROY, 0, 0);

    if (keyboardState[DIK_A] & 0x80)
    {
        Logger& SLogger = Logger::GetLogger();
        SLogger.Log("ViewportWindow.cpp, ViewportWindow::DetectKeyboardInput:", "\nA Pressed\n");

        vpEyeX += std::sinf(ConvertToRadians(g_RotY - 90)) * SpeedScale;
        vpEyeZ += std::cosf(ConvertToRadians(g_RotY - 90)) * SpeedScale;
    }

    if (keyboardState[DIK_D] & 0x80)
    {
        vpEyeX -= std::sinf(ConvertToRadians(g_RotY - 90)) * SpeedScale;
        vpEyeZ -= std::cosf(ConvertToRadians(g_RotY - 90)) * SpeedScale;
    }
    else
    {
        vpEyeXOffset = 0;
    }

    if (keyboardState[DIK_W] & 0x80)
    {
        vpEyeX += std::sinf(ConvertToRadians(g_RotY)) * SpeedScale;
        vpEyeZ += std::cosf(ConvertToRadians(g_RotY)) * SpeedScale;
        vpEyeY -= std::sinf(ConvertToRadians(g_RotX)) * SpeedScale;
    }

    if (keyboardState[DIK_S] & 0x80)
    {
        vpEyeX -= std::sinf(ConvertToRadians(g_RotY)) * SpeedScale;
        vpEyeZ -= std::cosf(ConvertToRadians(g_RotY)) * SpeedScale;
        vpEyeY += std::sinf(ConvertToRadians(g_RotX)) * SpeedScale;
    }

    if (keyboardState[DIK_Z] & 0x80)
    {
        vpEyeY += 0.001f;
    }

    //if (keyboardState[DIK_X] & 0x80)
    //{
    //    EyeY -= 0.001f;
    //}

    //if (keyboardState[DIK_S] & 0x80)
    //{
    //    EyeZOffset = 0.001f;
    //}
    //else
    //{
    //    EyeZOffset = 0;
    //}

    //if (keyboardState[DIK_Y] & 0x80)
    //{
    //    EyeZ -= 0.01f;
    //}

    if (keyboardState[DIK_LEFT] & 0x80)
    {
        g_RotY -= 0.02f;
    }
    if (keyboardState[DIK_RIGHT] & 0x80)
    {
        g_RotY += 0.02f;
    }
}

HWND ViewportWindow::m_ParentHwnd{ };
HWND ViewportWindow::m_ViewportHwnd{ };
float ViewportWindow::m_ViewportWidth{ 0 };
float ViewportWindow::m_ViewportHeight{ 0 };
bool ViewportWindow::bClipCursor{ false };
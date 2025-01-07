#ifndef UIMODULE_H
#define UIMODULE_H

#include <sstream>
#include "BaseModule.h"
#include "resource.h"
#include <Windows.h>
#include "ViewportWindow.h"

const wchar_t CLASS_NAME[] = L"KME Engine";
const wchar_t TOOLBAR_NAME[] = L"Toolbar";
const wchar_t SIDETOOLBAR_NAME[] = L"SideToolbar";

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LeftToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK RightToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

inline HWND ViewportHwnd;
inline HWND RightToolbarHwnd;
inline HWND LeftToolbarHwnd;

inline float ViewportWidth{ };
inline float ViewportHeight{ };
class CViewportWindow;

class CUIModule : public CBaseModule
{
public:
    HRESULT Initialize(HINSTANCE hInstance, int nCmdShow);

    void TestLog(int X, int Y);

    //HWND ViewportHwnd;
    //HWND RightToolbarHwnd;
    //HWND LeftToolbarHwnd;

    CViewportWindow* m_pViewportWindow{ };

    float ViewportWidth{ };
    float ViewportHeight{ };

    HINSTANCE m_HInstance{ };
    int m_NCmdShow{ };
};

#endif

//template <typename ...Args>
//void CViewportMessage::Send(auto&& Ptr, Args&&... args)
//{
//    m_pUIModule->Notify(Ptr, args...);
//}
#ifndef UIMODULE_H
#define UIMODULE_H

#include <sstream>
#include "BaseModule.h"
#include "resource.h"
#include <Windows.h>
#include "ViewportWindow.h"
#include "RightSubwindow.h"

const wchar_t CLASS_NAME[] = L"KME Engine";
const wchar_t TOOLBAR_NAME[] = L"Toolbar";
const wchar_t SIDETOOLBAR_NAME[] = L"SideToolbar";

/**
* To refactor window handles and procedures in their own class
*/

// Main windows procedures
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LeftToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK OutlinerProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK PropertiesProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// Main window handles
inline HWND hwnd;
inline HWND ViewportHwnd;
inline HWND RightSubwindowHwnd;
inline HWND LeftToolbarHwnd;

// Subwindows handles
inline HWND OutlinerHwnd;
inline HWND PropertiesHwnd;

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
    CRightSubwindow m_RightSubwindow{ };

    void SetOutlinerText();

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
#pragma once

#include <Windows.h>

LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);



class CRightSubwindow
{
public:
    static LRESULT CALLBACK RightSubwindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK OutlinerProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    
    static LRESULT CALLBACK EditXProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK EditYProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK EditZProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void CreateRightSubwindow(HWND& ParentHwnd);

    HWND GetRightSubwindowHwnd() const { return m_RightSubwindowHwnd; }

    HWND m_RightSubwindowHwnd{ };

    static HWND m_OutlinerHwnd;

    // Separate HWNDs for labels and edit controls
    static HWND m_PositionXLabel;
    static HWND m_PositionXEditControl;
    static HWND m_PositionYLabel;
    static HWND m_PositionYEditControl;
    static HWND m_PositionZLabel;
    static HWND m_PositionZEditControl;
};

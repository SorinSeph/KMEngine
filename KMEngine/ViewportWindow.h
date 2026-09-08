#ifndef VIEWPORTWINDOW_H
#define VIEWPORTWINDOW_H

#include <Windows.h>
#include <dinput.h>
#include <cmath>
#include "Logger.h"
#include "UIMessageQueue.h"
#include "World.h"

const float PI = 3.141592654f;

static float g_RotX{ 10.f };
static float g_RotY{ 0.f };

const wchar_t VIEWPORT_NAME[]{ L"Viewport" };

static LPDIRECTINPUT8 _DirectInput;
static IDirectInputDevice8* _DIKeyboard;

class CUIModule;
class CUIMessageQueue;

class CViewportWindow
{
public:
	CViewportWindow()
		: m_RotX{ &g_RotX }
		, m_RotY{ &g_RotY }
		, m_EyeX{ 0.f }
		, m_EyeY{ 0.f }
		, m_EyeZ{ 0.f }
		, m_SpeedScale{}
	{
		g_RotX = 10.f;
	}

	static void SetViewportParentHWND(HWND hwnd);

	static void SetViewportSize(float Width, float Height);

	static LRESULT CALLBACK ViewportWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void CreateViewport();

	HWND GetViewportHwnd();

	float GetXRotation();
	float GetYRotation();

	float& GetXRotation2();
	float& GetYRotation2();

	float* GetXRotation3();
	float* GetYRotation3();

	float GetXRotation4();
	float GetYRotation4();

	float GetXMemberRotation();
	float GetYMemberRotation();

	float GetEyeX();
	float GetEyeY();
	float GetEyeZ();

	bool InitViewportDirectInput(HINSTANCE hInstance, HWND hwnd);
	void DetectKeyboardInput();

	static HWND m_ViewportHwnd;
	static float m_ViewportWidth;
	static float m_ViewportHeight;

	int m_YKeyPressed;

	bool m_bWorldSet{ false };
	CWorld* m_pWorld{ nullptr };

private:

	float ConvertToRadians(float Degrees);

	static HWND m_ParentHwnd;


	static bool bClipCursor;

	HWND MainHWND{ };

	bool bSpeedScale;
	float m_SpeedScale;

	float* m_RotX;
	float* m_RotY;

	float m_EyeX;
	float m_EyeY;
	float m_EyeZ;

	CUIModule* m_pUIModule{ nullptr };
};

#endif
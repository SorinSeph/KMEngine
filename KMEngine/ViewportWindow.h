#pragma once
#include <Windows.h>
#include <dinput.h>
#include <cmath>
#include "Logger.h"

#define SpeedScale 0.00025f

const float XM_PI = 3.141592654f;

static float g_RotX;
static float g_RotY;

static bool g_CanRaycast;
static float g_RaycastX;
static float g_RaycastY;

const wchar_t VIEWPORT_NAME[]{ L"Viewport" };

static LPDIRECTINPUT8 _DirectInput;
static IDirectInputDevice8* _DIKeyboard;

class ViewportWindow
{
public:
	ViewportWindow()
		: m_RotX{ &g_RotX }
		, m_RotY{ &g_RotY }
		, m_RotX2{ g_RotX }
		, m_RotY2{ g_RotY }
	{}

	static void SetViewportParentHWND(HWND hwnd);

	static void SetViewportSize(float Width, float Height);

	static LRESULT CALLBACK ViewportWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void CreateViewport();

	HWND GetViewportWnd();

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

	float GetRaycastX();
	float GetRaycastY();
	bool CanRaycast();
	static void SetCanRaycast(bool Value);

	bool InitViewportDirectInput(HINSTANCE hInstance, HWND hwnd);
	void DetectKeyboardInput();

private:

	float ConvertToRadians(float Degrees);

	static HWND m_ParentHwnd;
	static HWND m_ViewportHwnd;
	static float m_ViewportWidth;
	static float m_ViewportHeight;
	static bool bClipCursor;

	HWND MainHWND{ };

	float* m_RotX;
	float* m_RotY;

	float m_RotX2;
	float m_RotY2;

	float vpEyeX;
	float vpEyeY;
	float vpEyeZ;
	float vpEyeXOffset;
};
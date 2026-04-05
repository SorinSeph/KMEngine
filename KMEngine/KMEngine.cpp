#ifndef UNICODE
#define UNICODE
#endif 

#include "KMEngineLoop.h"
#include "resource.h"
#include "Cube.h"
#include "CubeTerrainGenerator.h"
#include "PrimitiveGeometryFactory.h"
#include "DDSTextureLoader.h"
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib") //adds link to control control DLL
//#include <d3d11_1.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <d3dcompiler.h>
#include <cmath>
#pragma comment(lib, "d3dcompiler.lib")
#include <fstream>
#include <DirectXCollision.h>

#pragma comment (lib, "dxguid.lib")
#include <dinput.h>
#pragma comment (lib, "dinput8.lib")

#include "Logger.h"
#include "Scene.h"

using namespace DirectX;
using namespace std;


HWND                            g_hWnd = nullptr;
LPDIRECTINPUT8                  g_DirectInput;
IDirectInputDevice8*            g_DIKeyboard;
HRESULT g_hr = S_OK;

ofstream KMEngine_Log;

const wchar_t CLASS_NAME2[] = L"KME Engine";
const wchar_t VIEWPORT_NAME2[] = L"Viewport";
const wchar_t TOOLBAR_NAME2[] = L"Toolbar";
const wchar_t SIDETOOLBAR_NAME2[] = L"SideToolbar";


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);
bool InitDirectInput(HINSTANCE hInstance);
void DetectKeyboardInput();


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    KMEngine_Log.open("Log.txt");

    CCoreEngine CoreEngine(hInstance, nCmdShow);
    CKMEngineLoop KMEngineLoop{ &CoreEngine };
    CCoreClock* pCoreClock = KMEngineLoop.GetCoreClock();
    CLogger& Logger = CLogger::GetLogger();

    Logger.Log("KMEngine.cpp, engine started\n");

    CTimerManager& TimerManager = CTimerManager::GetTimerManager();
    TimerManager.m_pCoreClock = pCoreClock;

    CScene& Scene = CScene::GetScene();

    if (FAILED(KMEngineLoop.InitEngine()))
        return 0;

    if (!KMEngineLoop.GetCoreEngine()->GetViewportWindow().InitViewportDirectInput(hInstance, g_hWnd))
    {
        MessageBox(0, L"Failed to initialize Direct Input", L"Error", MB_OK);
        return 0;
    }

	CUIMessageQueue& ViewportMessage = CUIMessageQueue::GetUIMessageQueue();

    pCoreClock->Reset();

    MSG Msg = { 0 };
    while (WM_QUIT != Msg.message)
    {
        if (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
        else
        {
            KMEngineLoop.Update();
            KMEngineLoop.Render();
        }
    }
    KMEngineLoop.Cleanup();

    return 0;
}

bool InitDirectInput(HINSTANCE hInstance)
{
    HRESULT hr = S_OK;

    hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_DirectInput, NULL);
    hr = g_DirectInput->CreateDevice(GUID_SysKeyboard, &g_DIKeyboard, NULL);
    hr = g_DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
    hr = g_DIKeyboard->SetCooperativeLevel(g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    return true;
}
#ifndef UNICODE
#define UNICODE
#endif 

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

#include "Core/CoreClock.h"
#include "Core/CoreEngine.h"

using namespace DirectX;
using namespace std;

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------

HWND                            g_hWnd = nullptr;
LPDIRECTINPUT8                  g_DirectInput;
IDirectInputDevice8*            g_DIKeyboard;
HRESULT g_hr = S_OK;

float rot = 0.01f;
float rotx = 0;
float rotz = 0;
float roty = 0;

float scaleX = 1.0f;
float scaleY = 1.0f;

long PreviousPointX = 0;
long PreviousPointY = 0;

float CameraPosition;

float EyeX = 0.0f;
float EyeY = 0.0f;
float EyeZ = 0.0f;

float EyeXOffset = 0.0f;
float EyeZOffset = 0.0f;

XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

bool bY = false;
bool bTranslated = false;

// Primitive collison test, remove later
bool bCollision = false;

std::vector<BYTE> rawBuffer;

ofstream KMEngine_Log;

// Collision

struct CollisionRay
{
    XMVECTOR origin;
    XMVECTOR direction;
};

CollisionRay g_PrimaryRay;

struct CollisionAABox
{
    BoundingBox aabox;
    ContainmentType collision;
};

CollisionAABox g_CollisionAABox;

// String names

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
    CCoreClock CoreClock;
    CLogger& Logger = CLogger::GetLogger();

    Logger.Log("KMEngine.cpp, engine started\n");

    CTimerManager& TimerManager = CTimerManager::GetTimerManager();
    TimerManager.m_pCoreClock = &CoreClock;

    CScene& Scene = CScene::GetScene();

    if (FAILED(CoreEngine.InitEngine()))
        return 0;

    CRenderer* Renderer = CoreEngine.GetRenderer();

    if (!CoreEngine.GetViewportWindow().InitViewportDirectInput(hInstance, g_hWnd))
    {
        MessageBox(0, L"Failed to initialize Direct Input", L"Error", MB_OK);
        return 0;
    }

	CUIMessageQueue& ViewportMessage = CUIMessageQueue::GetUIMessageQueue();
	float* pMouseX = &ViewportMessage.m_MouseX;
	float& MouseX = ViewportMessage.m_MouseX;

    CoreClock.Reset();

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
            Logger.Log("KMEngine.cpp, wWinMain: mouse x is: ", MouseX);
            CoreClock.Tick();
            CoreClock.EngineTick();
            CoreEngine.DetectInput();
            float X = CoreEngine.GetXRotation();
            float Y = CoreEngine.GetYRotation();
            float eyeX = CoreEngine.GetEyeX();
            float eyeY = CoreEngine.GetEyeY();
            float eyeZ = CoreEngine.GetEyeZ();

            Renderer->Render2(X, Y, eyeX, eyeY, eyeZ);

            Logger.Log("KMEngine.cpp, wWinMain() \nTotalTime: ", CoreClock.GetFTotalTime(), "\n");
            Logger.Log("KMEngine.cpp, wWinMain() \n", "RotX = ", X, "\nRotY = ", Y, "\nEyeX = ", EyeX, "\nEyeY = ", EyeY, "\nEyeZ = ", EyeZ, "\n");
            //CoreEngine.RayCast(RayX, RayY);

            int SceneListIndex = 0;

            for (auto SceneListIt : CScene::GetScene().GetSceneList())
            {
                std::string EntityTag = SceneListIt.m_GameEntityTag;
                Logger.Log("Entity Tag at ", SceneListIndex, " is: ", EntityTag);
                SceneListIndex++;
            }
        }
    }
    Renderer->CleanupRenderer();

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
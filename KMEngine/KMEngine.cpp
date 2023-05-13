#ifndef UNICODE
#define UNICODE
#endif 

#include "resource.h"
#include "Cube.h"
#include "CubeTerrainGenerator.h"
#include "PrimitiveGeometryFactory.h"
#include "DXStructuresInitializer.h"
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

#include "DX11Device.h"
#include "CoreTimer.h"
#include "EngineInitializer.h"

using namespace DirectX;
using namespace std;

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------

HWND                            g_hWnd = nullptr;
LPDIRECTINPUT8                  g_DirectInput;
IDirectInputDevice8*            g_DIKeyboard;
XMMATRIX                        g_World;
XMMATRIX                        g_View;
XMMATRIX                        g_Projection;
Cube                            g_Cube;
Cube                            g_ObstacleCube;
Cube                            g_PositionTestCube;
GameEntity3D                    g_CubeEntity;
GameEntity3D                    g_CubeEntity2;

// Pointer inits
//ID3D11Buffer* g_pIndexBuffer = nullptr;

// Array inits
ID3D11Buffer* g_aIndexBuffer[2] = {};


HRESULT g_hr = S_OK;

std::vector<GameEntity3D> m_GameEntityList;

//bool bClipCursor = false;

//HWND ViewportHwnd;
//HWND SideToolbarHwnd;
//HWND ToolbarHwnd;

float rot = 0.01f;
float rotx = 0;
float rotz = 0;
float roty = 0;

//float g_RotX = 0;
//float g_RotY = 0;

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

//float ViewportWidth = 0;
//float ViewportHeight = 0;

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

//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK ToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK SideToolbarHwndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK ViewportWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);
//HRESULT InitEngine(HINSTANCE hInstance, int nCmdShow);
bool InitDirectInput(HINSTANCE hInstance);
void DetectKeyboardInput();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    KMEngine_Log.open("Log.txt");

    EngineInitializer FEngineInitializer(hInstance, nCmdShow);
    BCoreTimer CoreTimer;
    Logger& SLogger = Logger::GetLogger();

    FTimerManager& TimerManager = FTimerManager::GetTimerManager();
    TimerManager.CoreTimerRef = &CoreTimer;

    Scene& SScene = Scene::GetScene();

    if (FAILED(FEngineInitializer.InitEngine()))
        return 0;

    Renderer MRenderer = FEngineInitializer.GetRenderer();

    if (!FEngineInitializer.GetViewportWindow().InitViewportDirectInput(hInstance, g_hWnd))
    {
        MessageBox(0, L"Failed to initialize Direct Input", L"Error", MB_OK);
        return 0;
    }

    //if (FAILED(InitEngine(hInstance, nCmdShow)))
    //    return 0;

   // DX11Device m_DX11Device;

    //if (FAILED(InitDirectXDevice()))
    //if (FAILED(m_DX11Device.InitDX11Device()))
    //{
    //    MessageBox(nullptr, L"Failed to initialize D3D11 Device", L"Error", MB_OK);
    //    return 0;
    //}

    //if (!InitDirectInput(hInstance))
    //{
    //    MessageBox(0, L"Failed to initialize Direct Input", L"Error", MB_OK);
    //    return 0;
    //}

    CoreTimer.Reset();

    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            CoreTimer.Tick();
            CoreTimer.EngineTick();
            FEngineInitializer.DetectInput();
            float X = FEngineInitializer.GetXRotation();
            float Y = FEngineInitializer.GetYRotation();
            float eyeX = FEngineInitializer.GetEyeX();
            float eyeY = FEngineInitializer.GetEyeY();
            float eyeZ = FEngineInitializer.GetEyeZ();
            MRenderer.Render(X, Y, eyeX, eyeY, eyeZ);
            SLogger.Log("KMEngine.cpp, wWinMain() \nTotalTime: ", CoreTimer.GetFTotalTime(), "\n");
            SLogger.Log("KMEngine.cpp, wWinMain() \n", "RotX = ", X, "\nRotY = ", Y, "\nEyeX = ", EyeX, "\nEyeY = ", EyeY, "\nEyeZ = ", EyeZ, "\n");

            float RayX = FEngineInitializer.GetRaycastX();
            float RayY = FEngineInitializer.GetRaycastY();
            FEngineInitializer.RayCast(RayX, RayY);

            int SceneListIndex = 0;

            for (auto SceneListIt : Scene::GetScene().GetSceneList2())
            {
                std::string EntityTag = SceneListIt.m_GameEntityTag;
                SLogger.Log("Entity Tag at ", SceneListIndex, " is: ", EntityTag);
                SceneListIndex++;
            }

            // m_DX11Device.Render(g_RotX, g_RotY, EyeX, EyeY, EyeZ);
            //if (CoreTimer.GetFTotalTime() >= 6.0f)
            //{
            //    Scene& SScene = Scene::GetScene();
            //    continue;
            //}
        }
    }
    MRenderer.CleanupRenderer();
    //CleanupDevice();

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
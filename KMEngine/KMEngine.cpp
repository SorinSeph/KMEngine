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

#include "DX11Device.h"

#include "EngineInitializer.h"

using namespace DirectX;
using namespace std;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------


#define SpeedScale 0.00025f


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

    if ( FAILED (FEngineInitializer.InitEngine()) )
        return 0;

    Renderer MRenderer = FEngineInitializer.GetRenderer();

    //if (FAILED(InitEngine(hInstance, nCmdShow)))
    //    return 0;

   // DX11Device m_DX11Device;

    //if (FAILED(InitDirectXDevice()))
    //if (FAILED(m_DX11Device.InitDX11Device()))
    //{
    //    MessageBox(nullptr, L"Failed to initialize D3D11 Device", L"Error", MB_OK);
    //    return 0;
    //}

    if (!InitDirectInput(hInstance))
    {
        MessageBox(0, L"Failed to initialize Direct Input", L"Error", MB_OK);
        return 0;
    }

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
            DetectKeyboardInput();
            MRenderer.Render(g_RotX, g_RotY, EyeX, EyeY, EyeZ);
           // m_DX11Device.Render(g_RotX, g_RotY, EyeX, EyeY, EyeZ);
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

void DetectKeyboardInput()
{
    DIMOUSESTATE mouseCurrState;

    BYTE keyboardState[256];

    g_DIKeyboard->Acquire();
    g_DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

    if (keyboardState[DIK_ESCAPE] & 0x80)
        PostMessage(g_hWnd, WM_DESTROY, 0, 0);

    if (keyboardState[DIK_A] & 0x80)
    {
        EyeX += std::sinf(XMConvertToRadians(g_RotY - 90)) * SpeedScale;
        EyeZ += std::cosf(XMConvertToRadians(g_RotY - 90)) * SpeedScale;
    }

    if (keyboardState[DIK_D] & 0x80)
    {
        EyeX -= std::sinf(XMConvertToRadians(g_RotY - 90)) * SpeedScale;
        EyeZ -= std::cosf(XMConvertToRadians(g_RotY - 90)) * SpeedScale;
    }
    else
    {
        EyeXOffset = 0;
    }

    if (keyboardState[DIK_W] & 0x80)
    {
        if (!bCollision)
        {
            EyeX += std::sinf(XMConvertToRadians(g_RotY)) * SpeedScale;
            EyeZ += std::cosf(XMConvertToRadians(g_RotY)) * SpeedScale;
            EyeY -= std::sinf(XMConvertToRadians(g_RotX)) * SpeedScale;
        }
    }

    if (keyboardState[DIK_S] & 0x80)
    {
        EyeX -= std::sinf(XMConvertToRadians(g_RotY)) * SpeedScale;
        EyeZ -= std::cosf(XMConvertToRadians(g_RotY)) * SpeedScale;
        EyeY += std::sinf(XMConvertToRadians(g_RotX)) * SpeedScale;
    }

    if (keyboardState[DIK_Z] & 0x80)
    {
        EyeY += 0.001f;
    }

    if (keyboardState[DIK_X] & 0x80)
    {
        EyeY -= 0.001f;
    }

    if (keyboardState[DIK_S] & 0x80)
    {
        EyeZOffset = 0.001f;
    }
    else
    {
        EyeZOffset = 0;
    }

    if (keyboardState[DIK_Y] & 0x80)
    {
        EyeZ -= 0.01f;
    }

    if (keyboardState[DIK_LEFT] & 0x80)
    {
        g_RotY -= 0.02f;
    }
    if (keyboardState[DIK_RIGHT] & 0x80)
    {
        g_RotY += 0.02f;
    }
}


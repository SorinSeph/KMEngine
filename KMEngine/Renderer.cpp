#include "Renderer.h"
#include "CoreTimer.h"
#include "Scene.h"

Renderer::Renderer(HWND Viewport)
{
    m_DX11Device.SetViewport(m_Viewport);
}

void Renderer::SetViewport(HWND InViewport)
{
    m_DX11Device.SetViewport(InViewport);
}

void Renderer::InitRenderer()
{
    m_DX11Device.InitDX11Device();
}

void Renderer::Render(float RotX, float RotY, float LocX, float LocY, float LocZ)
{
    m_DX11Device.Render(RotX, RotY, LocX, LocY, LocZ);
}

void Renderer::Raycast(float OriginX, float OriginY, float OriginZ, float DestX, float DestY, float DestZ)
{
    FTimerManager& TimerManager = FTimerManager::GetTimerManager();
    Scene& SScene = Scene::GetScene();

    m_DX11Device.InitLine(OriginX, OriginY, OriginZ, DestX, DestY, DestZ);
    AddOutline();

    //SceneList& TempSceneList = SScene.m_SceneList; 

    //TimerManager.SetTimer<SceneList, void, &SceneList::RemoveLastElement>(TempSceneList, 3.0f);
    //TimerManager.SetTimer<SceneList, void, &SceneList::BreakpointTest>(SScene.m_SceneList, 3.0f);
     
    //TimerManager.SetTimer<DX11Device, void, &DX11Device::RemoveLastElementFromScene>(m_DX11Device, 3.0f); // Working

    //TimerManager.SetTimer<Scene, void, >
    //TimerManager.SetTimer<Scene, void, &Scene::RemoveLastEntityFromScene>(&Scene::GetScene());
}

/**
* Code for future editor and decoupling from DXDevice class
*/

void Renderer::AddOutline()
{
    m_DX11Device.CheckCollision();
}

void Renderer::AddGizmo()
{
    m_DX11Device.AddGizmo();
}

void Renderer::CleanupRenderer()
{
    m_DX11Device.CleanupDX11Device();
}
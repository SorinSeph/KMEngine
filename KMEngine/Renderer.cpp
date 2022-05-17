#include "Renderer.h"

Renderer::Renderer(HWND Viewport)
{
    m_DX11Device = DX11Device::GetDX11Device();
    m_Scene = Scene::GetScene();
    m_DX11Device->SetViewport(m_Viewport);
}

void Renderer::SetViewport(HWND InViewport)
{
    m_Viewport = InViewport;
    m_DX11Device->SetViewport(m_Viewport);
}

void Renderer::InitRenderer()
{
    m_DX11Device->InitDX11Device();
}

void Renderer::Render(float RotX, float RotY, float LocX, float LocY, float LocZ)
{
    m_DX11Device->Render(RotX, RotY, LocX, LocY, LocZ);
}

void Renderer::CleanupRenderer()
{
    m_DX11Device->CleanupDX11Device();
}
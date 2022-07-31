#include "Renderer.h"

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
    m_DX11Device.InitLine(OriginX, OriginY, OriginZ, DestX, DestY, DestZ);
}

void Renderer::AddOutline()
{
    m_DX11Device.AddOutline();
}

void Renderer::CleanupRenderer()
{
    m_DX11Device.CleanupDX11Device();
}
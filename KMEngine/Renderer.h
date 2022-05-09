#pragma once
#include <d3d11.h>
#include "DX11Device.h"

HRESULT CompileShaderFromFile(const wchar_t* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

class Renderer
{
public:
    Renderer(HWND Viewport)
    {
        m_DX11Device.SetViewport(Viewport);
    }

	//Renderer(UINT ViewportWidth, UINT ViewportHeight)
 //       : m_ViewportWidth{ ViewportWidth }
 //       , m_ViewportHeight{ ViewportHeight }
 //   {
 //       m_DX11Device.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
 //   }

    void SetViewport(HWND InViewport)
    {
        m_Viewport = InViewport;
        m_DX11Device.SetViewport(m_Viewport);
    }

    void InitRenderer()
    {
        m_DX11Device.InitDX11Device();
    }

    void CreatePixelShader(HRESULT hr, ID3D11Device* D3D11Device, ID3D11PixelShader* PixelShader)
    {
        // Compile the pixel shader
        ID3DBlob* pPSBlob = nullptr;
        //hr = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_4_0", &pPSBlob);
        hr = CompileShaderFromFile(L"C:/Users/Dell Gray/source/repos/KMEngine/KMEngine/texture.ps", "PS", "ps_5_0", &pPSBlob);
        if (FAILED(hr))
        {
            MessageBox(nullptr, L"The PS file cannot be compiled.", L"Error", MB_OK);
            return;
        }

        // Create the pixel shader
        hr = D3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &PixelShader);
        pPSBlob->Release();
        if (FAILED(hr))
        {
            MessageBox(nullptr, L"Failed to create pixel shader", L"Error", MB_OK);
            return;
        }
    }

    void Render(float RotX, float RotY, float LocX, float LocY, float LocZ)
    {
        m_DX11Device.Render(RotX, RotY, LocX, LocY, LocZ);
    }

    void CleanupRenderer()
    {
        m_DX11Device.CleanupDX11Device();
    }

private:
    UINT m_ViewportWidth{ };
    UINT m_ViewportHeight{ };
    HWND m_Viewport{ };
    DX11Device m_DX11Device;
};
#pragma once
#include <d3d11.h>
#include "DX11Device.h"

HRESULT CompileShaderFromFile(const wchar_t* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

class Renderer
{
public:
    Renderer(HWND Viewport);

    void SetViewport(HWND InViewport);

    void InitRenderer();

    DX11Device* GetDX11Device();

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

    void Raycast(float OriginX, float OriginY, float OriginZ, float DestX, float DestY, float DestZ);

    void AddOutline();

    void AddGizmo();

    void Render(float RotX, float RotY, float EyeX, float EyeY, float EyeZ);

    void CleanupRenderer();

private:
    UINT m_ViewportWidth{ };
    UINT m_ViewportHeight{ };
    HWND m_Viewport{ };
    DX11Device m_DX11Device{ };
    float   m_EyeX{ };
    float   m_EyeY{ };
    float   m_EyeZ{ };
};

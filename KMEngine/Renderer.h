#pragma once
#include <d3d11.h>
#include "DX11Device.h"

HRESULT CompileShaderFromFile(const wchar_t* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

class CGraphicsModule;

class CRenderer
{
public:
    CRenderer();

    CRenderer(HWND Viewport);

    CGraphicsModule* m_pGraphicsModule{ nullptr };

	void SetGraphicsModuleReference(CGraphicsModule* GraphicsModule)
	{
		m_pGraphicsModule = GraphicsModule;
	}

    void TestGraphicsModuleLog();

    void SetViewport(HWND InViewport);

	void SetViewportSize(int Width, int Height);

    void InitRenderer();

    CDX11Device* GetDX11Device();

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

    void Raycast(float DestinationX, float DestinationY);

    void AddOutline();

    void AddGizmo();

    void Render(float RotX, float RotY, float EyeX, float EyeY, float EyeZ);

    void CleanupRenderer();

    CDX11Device m_DX11Device{};

private:
    UINT m_ViewportWidth{ };
    UINT m_ViewportHeight{ };
    HWND m_Viewport{ };

    float   m_EyeX{ };
    float   m_EyeY{ };
    float   m_EyeZ{ };

    // Test variables / functions

	float m_CubeLocZ{ 0.0f };
};

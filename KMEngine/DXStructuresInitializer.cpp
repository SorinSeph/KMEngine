#pragma once

#include "DXStructuresInitializer.h"

void CDXStructuresInitializer::CreateDriverType()
{
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        m_DriverType = driverTypes[driverTypeIndex];
        m_HR = D3D11CreateDevice(nullptr, m_DriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &m_D3DDevice, &m_FeatureLevel, &m_pImmediateContext);

        if (m_HR == E_INVALIDARG)
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            m_HR = D3D11CreateDevice(nullptr, m_DriverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                D3D11_SDK_VERSION, &m_D3DDevice, &m_FeatureLevel, &m_pImmediateContext);
        }

        if (SUCCEEDED(m_HR))
            break;
    }

    if (FAILED(m_HR))
        return;
}

void CDXStructuresInitializer::SetSwapChainDesc1(int Width, int Height)
{
    m_SwapChainDesc1.Width = Width;
    m_SwapChainDesc1.Height = Height;
    m_SwapChainDesc1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_SwapChainDesc1.SampleDesc.Count = 1;
    m_SwapChainDesc1.SampleDesc.Quality = 0;
    m_SwapChainDesc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_SwapChainDesc1.BufferCount = 1;
}

DXGI_SWAP_CHAIN_DESC1 CDXStructuresInitializer::GetSwapChainDesc1()
{
    return m_SwapChainDesc1;
}

void CDXStructuresInitializer::SetSwapChainDesc(int Width, int Height, HWND OutputHWnd)
{
    m_SwapChainDesc.BufferCount = 1;
    m_SwapChainDesc.BufferDesc.Width = Width;
    m_SwapChainDesc.BufferDesc.Height = Height;
    m_SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    m_SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_SwapChainDesc.OutputWindow = OutputHWnd;
    m_SwapChainDesc.SampleDesc.Count = 1;
    m_SwapChainDesc.SampleDesc.Quality = 0;
    m_SwapChainDesc.Windowed = TRUE;
}

DXGI_SWAP_CHAIN_DESC CDXStructuresInitializer::GetSwapChainDesc()
{
    return m_SwapChainDesc;
}

void CDXStructuresInitializer::SetBufferDesc()
{
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.ByteWidth = sizeof(SSimpleVertex) * 24;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
}

D3D11_BUFFER_DESC CDXStructuresInitializer::GetBufferDesc()
{
    return m_BufferDesc;
}


void CDXStructuresInitializer::SetSubresourceData(std::vector<CGameEntity3D> GameEntityList)
{
    int GameEntityListSize = GameEntityList.size();
    std::vector<SSimpleVertex> TotalVerticesVector;

    for (int i = 0; i < GameEntityListSize; i++)
    {
        int CurrentVertexListSize = GameEntityList.at(i).GetVerticesList().size();

        for (int j = 0; j < CurrentVertexListSize; j++)
        {
            TotalVerticesVector.push_back(GameEntityList.at(i).GetVerticesList().at(j));
        }
    }

    int TotalVerticesVectorSize = TotalVerticesVector.size();

    SSimpleVertex* VerticesArray = new SSimpleVertex[TotalVerticesVectorSize];

    for (int i = 0; i < TotalVerticesVectorSize; i++)
    {
        VerticesArray[i] = TotalVerticesVector.at(i);
    }

    m_SubresourceData.pSysMem = VerticesArray;
}

D3D11_SUBRESOURCE_DATA CDXStructuresInitializer::GetSubresourceData()
{
    return m_SubresourceData;
}

void CDXStructuresInitializer::SetSamplerDesc()
{
    ZeroMemory(&m_SamplerDesc, sizeof(m_SamplerDesc));
    m_SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    m_SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    m_SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    m_SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    m_SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    m_SamplerDesc.MinLOD = 0;
    m_SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
}

D3D11_SAMPLER_DESC CDXStructuresInitializer::GetSamplerDesc()
{
    return m_SamplerDesc;
}

void CDXStructuresInitializer::SetDepthStencilDesc()
{
    m_DepthStencilDesc.DepthEnable = TRUE;
    m_DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    m_DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    m_DepthStencilDesc.StencilEnable = FALSE;
    m_DepthStencilDesc.StencilReadMask = 0xFF;
    m_DepthStencilDesc.StencilWriteMask = 0xFF;
}

D3D11_DEPTH_STENCIL_DESC CDXStructuresInitializer::GetDepthStencilDesc()
{
    return m_DepthStencilDesc;
}

void CDXStructuresInitializer::SetTexture2DDesc(int Width, int Height)
{
    m_Texture2DDesc.Width = Width;
    m_Texture2DDesc.Height = Height;
    m_Texture2DDesc.MipLevels = 1;
    m_Texture2DDesc.ArraySize = 1;
    m_Texture2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    m_Texture2DDesc.SampleDesc.Count = 1;
    m_Texture2DDesc.SampleDesc.Quality = 0;
    m_Texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
    m_Texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    m_Texture2DDesc.CPUAccessFlags = 0;
    m_Texture2DDesc.MiscFlags = 0;
}

D3D11_TEXTURE2D_DESC CDXStructuresInitializer::GetTexture2DDesc()
{
    return m_Texture2DDesc;
}

void CDXStructuresInitializer::SetDepthStencilViewDesc(D3D11_TEXTURE2D_DESC Texture2DDesc)
{
    m_DepthStencilViewDesc.Format = Texture2DDesc.Format;
    m_DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    m_DepthStencilViewDesc.Texture2D.MipSlice = 0;
}

D3D11_DEPTH_STENCIL_VIEW_DESC CDXStructuresInitializer::GetDepthStencilViewDesc()
{
    return m_DepthStencilViewDesc;
}

void CDXStructuresInitializer::SetViewPort(int Width, int Height)
{
    m_ViewPort.Width = (FLOAT)Width;
    m_ViewPort.Height = (FLOAT)Height;
    m_ViewPort.TopLeftX = 0;
    m_ViewPort.TopLeftY = 0;
    m_ViewPort.MinDepth = 0.0f;
    m_ViewPort.MaxDepth = 1.0f;
}

D3D11_VIEWPORT CDXStructuresInitializer::GetViewPort()
{
    return m_ViewPort;
}
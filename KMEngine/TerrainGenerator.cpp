#pragma once
#include <cassert>
#include "TerrainGenerator.h"

#define CBT_IMPLEMENTATION
#include "cbt.h"

HRESULT CTerrainGenerator::GenerateTestTerrain()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;

    CLogger& SLogger = CLogger::GetLogger();

    CGameEntity3D BaseCube = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::Cube);
    BaseCube.m_GameEntityTag = "Terrain";
    BaseCube.SetLocationF(0.0f, -4.0f, 0.0f);
    BaseCube.SetScale(2.5f, 2.5f, 2.5f);

    BaseCube.m_DXResConfig.m_pDX11Device = m_pDX11Device;

    HRESULT m_HR{};

    if (m_pDX11Device)
    {
        SLogger.Log("TerrainGenerator.cpp, GenerateTestTerrain() : m_pDX11Device is not null");
    }

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = m_pDX11Device->CompileShaderFromFile(L"TextureShader.fxh", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the vertex shader
    ID3D11VertexShader* TempVertexShader{ nullptr };
    m_HR = m_pDX11Device->m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &TempVertexShader);
    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return m_HR;
    }
    BaseCube.SetVertexShader(TempVertexShader);
    auto VertexShaderLambda = [=]() {
        BaseCube.m_DXResConfig.m_pDX11Device->m_pImmediateContext->VSSetShader(TempVertexShader, nullptr, 0);
        };
    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    ID3D11InputLayout* TempVertexLayout{ nullptr };
    m_HR = m_pDX11Device->m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &TempVertexLayout);
    pVSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;

    // Set the input layout
    BaseCube.SetInputLayout(TempVertexLayout);
    m_pDX11Device->m_pImmediateContext->IASetInputLayout(TempVertexLayout);

    auto InputLayoutLambda = [=]() {
        BaseCube.m_DXResConfig.m_pDX11Device->m_pImmediateContext->IASetInputLayout(TempVertexLayout);
        };

    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = m_pDX11Device->CompileShaderFromFile(L"TextureShader.fxh", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the pixel shader
    ID3D11PixelShader* TempPixelShader{ nullptr };
    m_HR = m_pDX11Device->m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &TempPixelShader);
    pPSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;
    BaseCube.SetPixelShader(m_pDX11Device->m_PixelShader);
    auto PixelShaderLambda = [=]() mutable {
        BaseCube.m_DXResConfig.m_pDX11Device->m_pImmediateContext->PSSetShader(TempPixelShader, nullptr, 0);
        };
    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(PixelShaderLambda);

    // Create vertex buffer
    //Simple_Color_Vertex vertices[] =
    //{
    //    { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
    //    { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
    //    { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
    //    { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
    //    { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
    //    { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
    //    { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
    //    { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
    //};

    SSimpleVertex vertices[] =
    {
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

        //{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        //{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        //{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        //{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

        //{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        //{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
        //{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        //{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

        //{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        //{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
        //{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        //{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

        //{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
        //{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
        //{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        //{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

        //{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
        //{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
        //{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        //{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }
    };

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleVertex) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    ID3D11Buffer* TempVertexBuffer{ nullptr };
    D3D11_SUBRESOURCE_DATA InitData{};
    InitData.pSysMem = vertices;
    m_HR = m_pDX11Device->m_pD3D11Device->CreateBuffer(&bd, &InitData, &TempVertexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set vertex buffer
    UINT stride = sizeof(SSimpleVertex);
    UINT offset = 0;
    BaseCube.SetVertexBuffer(TempVertexBuffer);
    m_pDX11Device->m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);

    auto VertexBufferLambda = [=]() mutable {
        BaseCube.m_DXResConfig.m_pDX11Device->m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);
        };
    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);


    // Create index buffer
    WORD indices[] =
    {
        0, 1, 2,
        0, 3, 2

        //6, 4, 5,
        //7, 4, 6,

        //11, 9, 8,
        //10, 9, 11,

        //14, 12, 13,
        //15, 12, 14,

        //19, 17, 16,
        //18, 17, 19,

        //22, 20, 21,
        //23, 20, 22

    };

    ID3D11Buffer* TempIndexBuffer{ nullptr };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 6;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    m_HR = m_pDX11Device->m_pD3D11Device->CreateBuffer(&bd, &InitData, &TempIndexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set index buffer
    BaseCube.SetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_pDX11Device->m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    auto IndexBufferLambda = [=]() mutable {
        BaseCube.m_DXResConfig.m_pDX11Device->m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        };

    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

    // Set primitive topology
    m_pDX11Device->m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    ID3D11Buffer* TempConstantBuffer{ nullptr };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pDX11Device->m_pD3D11Device->CreateBuffer(&bd, nullptr, &TempConstantBuffer);
    if (FAILED(m_HR))
        return m_HR;
    BaseCube.SetConstantBuffer(TempConstantBuffer);
    auto ConstantBufferLambda = [=]()
        {
            BaseCube.m_DXResConfig.m_pDX11Device->m_pImmediateContext->VSSetConstantBuffers(0, 1, &TempConstantBuffer);
        };
    BaseCube.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);


    const wchar_t* TextureName = L"grey_grid.dds";
    m_HR = CreateDDSTextureFromFile(m_pDX11Device->m_pD3D11Device, TextureName, nullptr, &m_pDX11Device->m_TextureRV);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
        return m_HR;
    }
    auto TextureLambda = [=]() mutable {
        BaseCube.m_DXResConfig.m_pDX11Device->m_pImmediateContext->PSSetShaderResources(0, 1, &BaseCube.m_DXResConfig.m_pDX11Device->m_TextureRV);
        };
    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(TextureLambda);

    //D3D11_SAMPLER_DESC sampDesc{};
    //ZeroMemory(&sampDesc, sizeof(sampDesc));
    //sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    //sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    //sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    //sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    //sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    //sampDesc.MinLOD = 0;
    //sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    //m_hr = m_D3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);

    //D3D11_RASTERIZER_DESC rasterDesc = {};
    //rasterDesc.FillMode = D3D11_FILL_SOLID;
    //rasterDesc.CullMode = D3D11_CULL_NONE;
    //rasterDesc.FrontCounterClockwise = false;
    //rasterDesc.DepthBias = 0;
    //rasterDesc.DepthBiasClamp = 0.0f;
    //rasterDesc.SlopeScaledDepthBias = 0.0f;
    //rasterDesc.DepthClipEnable = true;
    //rasterDesc.ScissorEnable = false;
    //rasterDesc.MultisampleEnable = false;
    //rasterDesc.AntialiasedLineEnable = false;

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    m_HR = m_pDX11Device->m_pD3D11Device->CreateSamplerState(&sampDesc, &m_pDX11Device->m_SamplerLinear);
    if (FAILED(m_HR))
        return m_HR;

    //m_hr = m_D3D11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerState);
    //if (FAILED(m_hr))
    //{
    //    MessageBox(nullptr, L"Failed to create rasterizer state", L"Error", MB_OK);
    //    return m_hr;
    //}

    //m_ImmediateContext->RSSetState(m_RasterizerState);

    SScene.AddEntityToScene(BaseCube);

    //// Initialize the world matrix
    //g_World = XMMatrixIdentity();

    //// Initialize the view matrix
    //XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
    //XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    //XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    //g_View = XMMatrixLookAtLH(Eye, At, Up);

    // Initialize the projection matrix
    //m_pDX11Device->m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.01f, 100.0f);

    return S_OK;
}

CDX11Device* CTerrainGenerator::GetDX11Device()
{
    return m_pDX11Device;
}

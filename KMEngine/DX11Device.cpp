#include <bitset>
#include <locale>
#include <codecvt>

#include "DX11Device.h"

#define FLT_MAX          3.402823466e+38F

using namespace Internal;
using namespace std;

XMMATRIX DX11Device::m_WorldMatrix;
XMMATRIX DX11Device::m_ViewMatrix;
XMMATRIX DX11Device::m_ProjectionMatrix;

int SceneLoc = 0;

XMVECTOR g_RayOrigin;
XMVECTOR g_RayDestination;

XMGLOBALCONST XMVECTORF32 _RayEpsilon = { { { 1e-20f, 1e-20f, 1e-20f, 1e-20f } } };
XMGLOBALCONST XMVECTORF32 _FltMin = { { { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX } } };
XMGLOBALCONST XMVECTORF32 _FltMax = { { { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX } } };

HRESULT DX11Device::InitDX11Device()
{
    PrimitiveGeometryFactory GeometryFactory;

    InitDriveTypeAndFeatureLevel();
    IDXGIFactory1* dxgiFactory = InitDXGIFactory();
    InitSwapChain(dxgiFactory);
    InitRenderTargetView();
    //D3D11_TEXTURE2D_DESC descDepth = InitTexture2D();
    //InitDepthStencilView(descDepth);
    InitDisabledDepthStencil();
    InitDefaultDepthStencil();
    //InitOutlineDepthStencil();
    InitViewport();
    InitCube();
    InitCubeOutline();

    TestVector();

    //Spawn a line intersecting the cube 
    //AddTestLine();

    //InitLine(0, 0, 0, 0, 0, 4);

    return S_OK;
}

void DX11Device::SetViewport(HWND InViewport)
{
    if (InViewport)
        m_Viewport = InViewport;

    RECT rc;
    if (m_Viewport)
    {
        m_ViewportWidth = 639;
        m_ViewportHeight = 1020;
        //GetClientRect(m_Viewport, &rc);
        //m_ViewportWidth = rc.right - rc.left;
        //m_ViewportHeight = rc.bottom - rc.top;
    }
}

//void DX11Device::SetViewportSize(UINT width, UINT height)
//{
//    m_ViewportWidth = (float)width;
//    m_ViewportHeight = (float)height;
//}

void DX11Device::InitDriveTypeAndFeatureLevel()
{
    //RECT rc;
    //GetClientRect(m_Viewport, &rc);
    //UINT m_ViewportWidth = rc.right - rc.left;
    //UINT m_ViewportHeight = rc.bottom - rc.top;

    PrimitiveGeometryFactory GeometryFactory;

    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
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
        m_hr = D3D11CreateDevice(nullptr, m_DriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &m_D3D11Device, &m_FreatureLevel, &m_ImmediateContext);

        if (m_hr == E_INVALIDARG)
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            m_hr = D3D11CreateDevice(nullptr, m_DriverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                D3D11_SDK_VERSION, &m_D3D11Device, &m_FreatureLevel, &m_ImmediateContext);
        }

        if (SUCCEEDED(m_hr))
            break;
    }
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize driver type", L"Error", MB_OK);
        return;
    }
}

IDXGIFactory1* DX11Device::InitDXGIFactory()
{
    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        m_hr = m_D3D11Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
        if (SUCCEEDED(m_hr))
        {
            IDXGIAdapter* adapter = nullptr;
            m_hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(m_hr))
            {
                m_hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to obtain DXGI factory from device", L"Error", MB_OK);
        return 0;
    }

    return dxgiFactory;
}

void DX11Device::InitSwapChain(IDXGIFactory1* dxgiFactory)
{
    IDXGIFactory2* dxgiFactory2 = nullptr;
    m_hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
    if (dxgiFactory2)
    {
        // DirectX 11.1 or later
        m_hr = m_D3D11Device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&m_D3D11Device1));
        if (SUCCEEDED(m_hr))
        {
            (void)m_ImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&m_ImmediateContext1));
        }

        DXGI_SWAP_CHAIN_DESC1 sd{ };
        sd.Width = m_ViewportWidth;
        sd.Height = m_ViewportHeight;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;
        m_hr = dxgiFactory2->CreateSwapChainForHwnd(m_D3D11Device, m_Viewport, &sd, nullptr, nullptr, &m_SwapChain1);
        if (SUCCEEDED(m_hr))
        {
            m_hr = m_SwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&m_SwapChain));
        }

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd{ };
        sd.BufferCount = 1;
        sd.BufferDesc.Width = m_ViewportWidth;
        sd.BufferDesc.Height = m_ViewportHeight;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = m_Viewport;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        m_hr = dxgiFactory->CreateSwapChain(m_D3D11Device, &sd, &m_SwapChain);
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    dxgiFactory->MakeWindowAssociation(m_Viewport, DXGI_MWA_NO_ALT_ENTER);

    dxgiFactory->Release();

    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize swap chain", L"Error", MB_OK);
        return;
    }
}

void DX11Device::InitRenderTargetView()
{
    ID3D11Texture2D* pBackBuffer{ nullptr };
    m_hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to get swapchain buffer for render target view", L"Error", MB_OK);
        return;
    }

    m_hr = m_D3D11Device->CreateRenderTargetView(pBackBuffer, nullptr, &m_RenderTargetView);
    pBackBuffer->Release();
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize render target view", L"Error", MB_OK);
        return;
    }
}

D3D11_TEXTURE2D_DESC DX11Device::InitTexture2D()
{
    D3D11_TEXTURE2D_DESC descDepth{ };
    descDepth.Width = m_ViewportWidth;
    descDepth.Height = m_ViewportHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    m_hr = m_D3D11Device->CreateTexture2D(&descDepth, nullptr, &m_DepthStencil);

    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize Texture Desc", L"Error", MB_OK);
        return { };
    }

    return descDepth;
}

void DX11Device::InitDepthStencilView(D3D11_TEXTURE2D_DESC descDepth)
{
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV{};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    m_hr = m_D3D11Device->CreateDepthStencilView(m_DepthStencil, &descDSV, &m_DepthStencilView);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize Stencil View Desc", L"Error", MB_OK);
        return;
    }

    m_ImmediateContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
}

void DX11Device::InitViewport()
{
    D3D11_VIEWPORT vp{ };
    vp.Width = (FLOAT)m_ViewportWidth;
    vp.Height = (FLOAT)m_ViewportHeight;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    m_ImmediateContext->RSSetViewports(1, &vp);
}

void DX11Device::InitDefaultDepthStencil()
{
    defDescDepth.Width = m_ViewportWidth;
    defDescDepth.Height = m_ViewportHeight;
    defDescDepth.MipLevels = 1;
    defDescDepth.ArraySize = 1;
    defDescDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    defDescDepth.SampleDesc.Count = 1;
    defDescDepth.SampleDesc.Quality = 0;
    defDescDepth.Usage = D3D11_USAGE_DEFAULT;
    defDescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    defDescDepth.CPUAccessFlags = 0;
    defDescDepth.MiscFlags = 0;
    m_hr = m_D3D11Device->CreateTexture2D(&defDescDepth, NULL, &pDefDepthStencil);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize depth stencil", L"Error", MB_OK);
        return;
    }

    defDescDepthStencilViewDesc.Format = defDescDepth.Format;
    defDescDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    defDescDepthStencilViewDesc.Texture2D.MipSlice = 0;
    m_hr = m_D3D11Device->CreateDepthStencilView(pDefDepthStencil, &defDescDepthStencilViewDesc, &pDefDepthStencilView);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize depth stencil view", L"Error", MB_OK);
        return;
    }

    m_ImmediateContext->OMSetRenderTargets(1, &m_RenderTargetView, pDefDepthStencilView);


    // code for first cube, outline stencil mask, NOT RENDERING


    //defDepthStencilDesc.DepthEnable = TRUE;
    //defDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    //defDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    //defDepthStencilDesc.StencilEnable = TRUE;
    //defDepthStencilDesc.StencilReadMask = 0xFF;
    //defDepthStencilDesc.StencilWriteMask = 0xFF;

    //defDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    //// Write to the stencil on pass.
    //defDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
    //defDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    //// Stencil operations if pixel is back-facing.
    //// Since we do not care about back-facing pixels, always keep original value.
    //defDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;


    defDepthStencilDesc.DepthEnable = TRUE;
    defDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    defDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    defDepthStencilDesc.StencilEnable = TRUE;
    defDepthStencilDesc.StencilReadMask = 0xFF;
    defDepthStencilDesc.StencilWriteMask = 0xFF;

    defDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    // Write to the stencil on pass.
    defDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
    defDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    defDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;


    m_hr = m_D3D11Device->CreateDepthStencilState(&defDepthStencilDesc, &pDefDepthStencilState);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize depth stencil state", L"Error", MB_OK);
        return;
    }
}

void DX11Device::InitDisabledDepthStencil()
{
    ID3D11Texture2D* pDepthStencil = NULL;
    D3D11_TEXTURE2D_DESC descDepth;
    descDepth.Width = m_ViewportWidth;
    descDepth.Height = m_ViewportWidth;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    m_hr = m_D3D11Device->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

    D3D11_DEPTH_STENCIL_DESC dsDesc;

    // Depth test parameters
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil test parameters
    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create depth stencil state
    m_D3D11Device->CreateDepthStencilState(&dsDesc, &m_DepthStencilStateDisabled);

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    descDSV.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    // Create the depth stencil view
//    ID3D11DepthStencilView* pDSV;
//    m_hr = m_D3D11Device->CreateDepthStencilView(pDepthStencil, // Depth stencil texture
//        &descDSV, // Depth stencil desc
//        &pDSV);  // [out] Depth stencil view
//
//// Bind the depth stencil view
//    m_ImmediateContext->OMSetRenderTargets(1,          // One rendertarget view
//        &m_RenderTargetView,      // Render target view, created earlier
//        pDSV);     // Depth stencil view for the render target



    // Create the depth stencil outline
    defDepthStencilDesc.DepthEnable = true;
    defDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    defDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    defDepthStencilDesc.StencilEnable = true;
}

void DX11Device::InitCube()
{
    Scene& SScene = Scene::GetScene();
    PrimitiveGeometryFactory GeometryFactory;

    // Original cube entity
    m_CubeEntity = GeometryFactory.CreateEntity3D(PrimitiveGeometryType::Cube);

    m_CubeEntity.SetLocationF(3.0f, 0.0f, 0.0f);
    m_CubeEntity.m_GameEntityTag = "Original_Tag";
    m_CubeEntity.m_DXResConfig.m_UID = "UID1234";
    m_CubeEntity.PhysicalMesh.SetIndicesList(GeometryFactory.GetIndicesList());
    m_CubeEntity.PhysicalMesh.SetStride(sizeof(SimpleVertex));

    XMFLOAT3 CubeEntityCenter{ 3, 0, 0 };
    XMFLOAT3 CubeEntityExtents{ 0.5f, 0.5f, 0.5f };

    CollisionComponent Collision = m_CubeEntity.GetCollisionComponent();
    Collision.AABox.Center = { 3, 0, 0 };
    Collision.AABox.Extents = { 0.5f, 0.5f, 0.5f };
    Collision.CollisionType = DISJOINT;
    m_CubeEntity.SetCollisionParams(CubeEntityCenter, CubeEntityExtents, DISJOINT);


    // Cube Entity Component 1
    GameEntity3DComponent CubeComponent1 = GeometryFactory.CreateEntity3DComponent(PrimitiveGeometryType::Cube);


    CubeComponent1.SetLocationF(3.0f, 0.0f, 0.0f);
    CubeComponent1.m_GameEntityTag = "Original_Tag";
    CubeComponent1.m_DXResConfig.m_UID = "UID1234";
    CubeComponent1.PhysicalMesh.SetIndicesList(GeometryFactory.GetIndicesList());
    CubeComponent1.PhysicalMesh.SetStride(sizeof(SimpleVertex));

    CollisionComponent Collision2 = CubeComponent1.GetCollisionComponent();
    Collision.AABox.Center = { 3, 0, 0 };
    Collision.AABox.Extents = { 0.5f, 0.5f, 0.5f };
    Collision.CollisionType = DISJOINT;
    CubeComponent1.SetCollisionParams(CubeEntityCenter, CubeEntityExtents, DISJOINT);

    m_CubeEntity.m_GameEntity3DComponent.push_back(CubeComponent1);

    SScene.AddEntityToScene2(m_CubeEntity);
    SScene.SetCollisionParams2(SceneLoc, CubeEntityCenter, CubeEntityExtents, DISJOINT);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_hr = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/texture.vs", "VS", "vs_5_0", &pVSBlob);

    if (FAILED(m_hr))
    {
        MessageBox(nullptr,
            L"The VS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the vertex shader
    m_hr = m_D3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_VertexShader);

    if (FAILED(m_hr))
    {
        pVSBlob->Release();
        return;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    m_hr = m_D3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_VertexLayout);
    pVSBlob->Release();
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize input layout", L"Error", MB_OK);
        return;
    }

    //SScene.SetInputLayout2(SceneLoc, m_VertexLayout);
    SScene.SetComponentInputLayout(SceneLoc, m_VertexLayout);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    //hr = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_4_0", &pPSBlob);
    m_hr = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/texture.ps", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"The PS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the pixel shader
    m_hr = m_D3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_PixelShader);
    pPSBlob->Release();
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to create pixel shader", L"Error", MB_OK);
        return;
    }

    SScene.SetVertexShader2(SceneLoc, m_VertexShader);
    SScene.SetPixelShader2(SceneLoc, m_PixelShader);

    SScene.SetComponentVertexShader(SceneLoc, m_VertexShader);
    SScene.SetComponentPixelShader(SceneLoc, m_PixelShader);

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData{};

    std::vector<SimpleVertex> TotalVerticesVector{ SScene.GetSceneList2().at(SceneLoc).GetVerticesList() };
    int TotalVerticesVectorSize = TotalVerticesVector.size();
    SimpleVertex* VerticesArray = new SimpleVertex[TotalVerticesVectorSize];
    for (int i = 0; i < TotalVerticesVectorSize; i++)
    {
        VerticesArray[i] = TotalVerticesVector.at(i);
    }

    SimpleVertex* VerticesArray2 = SScene.GetSceneList2().at(0).GetVerticesList().data();

    InitData.pSysMem = VerticesArray;

    m_hr = m_D3D11Device->CreateBuffer(&bd, &InitData, &m_VertexBuffer);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize vertex buffer", L"Error", MB_OK);
        return;
    }

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;

    //SScene.SetVertexbuffer2(SceneLoc, m_VertexBuffer);
    SScene.SetComponentVertexbuffer(SceneLoc, m_VertexBuffer);

    //WORD* arr = IndicesVector.data();
    WORD* Indices = m_CubeEntity.PhysicalMesh.GetIndicesList().data();
    auto TempIndicesList = m_CubeEntity.PhysicalMesh.GetIndicesList();
    WORD* Indices2 = TempIndicesList.data();
    auto TempIndicesList3 = SScene.GetSceneList2().at(0).PhysicalMesh.GetIndicesList();
    WORD* Indices3 = TempIndicesList3.data();

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = Indices3;
    m_hr = m_D3D11Device->CreateBuffer(&bd, &InitData, &m_IndexBufferArray[0]);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize index buffer", L"Error", MB_OK);
        return;
    }
    //SScene.SetIndexbuffer2(SceneLoc, m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);
    SScene.SetComponentIndexbuffer(SceneLoc, m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);

    // Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_hr = m_D3D11Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize constant buffer", L"Error", MB_OK);
        return;
    }
    SScene.SetConstantBuffer2(SceneLoc, m_ConstantBuffer);
    SScene.SetComponentConstantBuffer(SceneLoc, m_ConstantBuffer);

    const wchar_t* TextureName = L"grey_grid.dds";
    m_hr = CreateDDSTextureFromFile(m_D3D11Device, TextureName, nullptr, &m_TextureRV);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
        return;
    }

    D3D11_SAMPLER_DESC sampDesc{};
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    m_hr = m_D3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);

    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.ScissorEnable = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.AntialiasedLineEnable = false;

    m_hr = m_D3D11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerState);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to create rasterizer state", L"Error", MB_OK);
        return;
    }

    m_ImmediateContext->RSSetState(m_RasterizerState);

    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.1f, 10.0f);
    m_WorldMatrix = XMMatrixIdentity();

    m_ImmediateContext->PSSetShaderResources(0, 1, &m_TextureRV);

    SceneLoc++;
}

void DX11Device::InitCubeOutline()
{
    Scene& SScene = Scene::GetScene();
    PrimitiveGeometryFactory GeometryFactory;

    m_CubeOutline = GeometryFactory.CreateEntity3D(PrimitiveGeometryType::CubeTest);
    m_CubeOutline.SetLocationF(3.0f, 0.0f, 0.0f);
    m_CubeOutline.SetScale(1.02f, 1.02f, 1.02f);
    m_CubeOutline.m_GameEntityTag = "Outline";
    m_CubeOutline.PhysicalMesh.SetStride(sizeof(SimpleColorVertex));
    //m_GameEntityList.push_back(m_CubeEntity);
    //SScene.AddEntityToScene(m_CubeOutline);

    // Create the depth stencil outline
    defDepthStencilDesc.DepthEnable = true;
    defDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    defDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    defDepthStencilDesc.StencilEnable = true;
    defDepthStencilDesc.StencilReadMask = 0xFF;
    defDepthStencilDesc.StencilWriteMask = 0xFF;

    // It does not matter what we write since we are not using the values after this step.
    // In other words, we are only using the values to mask pixels.
    defDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    // The stencil test passes if the passed parameter is equal to value in the buffer.
    defDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    // Again, we do not care about back-facing pixels.
    defDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

    //hr = g_pd3dDevice->CreateDepthStencilState(&g_DepthStencilDesc, &g_pDepthStencilOutlineState);

    m_D3D11Device->CreateDepthStencilState(&defDepthStencilDesc, &pDepthStencilStateOutline);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    // hr = CompileShaderFromFile(L"Tutorial04.fxh", "VS", "vs_4_0", &pVSBlob);
    m_hr = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/solid_color.vs", "VS", "vs_5_0", &pVSBlob);

    if (FAILED(m_hr))
    {
        MessageBox(nullptr,
            L"The VS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the vertex shader
    m_hr = m_D3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_VertexShader2);

    if (FAILED(m_hr))
    {
        pVSBlob->Release();
        return;
    }

    m_CubeOutline.m_DXResConfig.SetVertexShader(m_VertexShader2);
    //SScene.SetVertexShader(SceneLoc, m_VertexShader2);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    m_hr = m_D3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_VertexLayout2);
    pVSBlob->Release();
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize input layout", L"Error", MB_OK);
        return;
    }

    m_CubeOutline.m_DXResConfig.SetInputLayout(m_VertexLayout2);
    //SScene.SetInputLayout(SceneLoc, m_VertexLayout2);

    // Set the input layout
    //m_ImmediateContext->IASetInputLayout(m_VertexLayout);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    //hr = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_4_0", &pPSBlob);
    m_hr = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/solid_color.ps", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_hr))
    {

        MessageBox(nullptr, L"The PS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the pixel shader
    m_hr = m_D3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_PixelShader2);
    pPSBlob->Release();
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to create pixel shader", L"Error", MB_OK);
        return;
    }

    m_CubeOutline.m_DXResConfig.SetPixelShader(m_PixelShader2);
    //SScene.SetPixelShader(SceneLoc, m_PixelShader2);

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleColorVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData{};
    //std::vector<SimpleColorVertex> TotalVerticesVector{ SScene.GetSceneList().at(SceneLoc).GetSimpleColorVerticesList() };
    //int TotalVerticesVectorSize = TotalVerticesVector.size();
    int TotalVerticesVectorSize = m_CubeOutline.GetSimpleColorVerticesList().size();

    SimpleColorVertex* VerticesArray = new SimpleColorVertex[TotalVerticesVectorSize];
    for (int i = 0; i < TotalVerticesVectorSize; i++)
    {
        VerticesArray[i] = m_CubeOutline.GetSimpleColorVerticesList().at(i);
    }
    InitData.pSysMem = VerticesArray;

    m_hr = m_D3D11Device->CreateBuffer(&bd, &InitData, &m_VertexBuffer2);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize vertex buffer", L"Error", MB_OK);
        return;
    }

    // Set vertex buffer
    UINT stride = sizeof(SimpleColorVertex);
    UINT offset = 0;
    //m_ImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

    m_CubeOutline.m_DXResConfig.SetVertexBuffer(m_VertexBuffer2);
    //SScene.SetVertexbuffer(SceneLoc, m_VertexBuffer2);

    // Pyramid indices,
    WORD indices[] =
    {
        3,1,0,
        2,1,3,

        6,4,5,
        7,4,6,

        11,9,8,
        10,9,11,

        14,12,13,
        15,12,14,

        19,17,16,
        18,17,19,

        22,20,21,
        23,20,22

    };

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    m_hr = m_D3D11Device->CreateBuffer(&bd, &InitData, &m_IndexBuffer2);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize index buffer", L"Error", MB_OK);
        return;
    }

    m_CubeOutline.m_DXResConfig.SetIndexBuffer(m_IndexBuffer2, DXGI_FORMAT_R16_UINT, 0);
    //SScene.SetIndexbuffer(SceneLoc, m_IndexBuffer2, DXGI_FORMAT_R16_UINT, 0);

    // Set index buffer
    //m_ImmediateContext->IASetIndexBuffer(m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);

    // Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_hr = m_D3D11Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer2);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize constant buffer", L"Error", MB_OK);
        return;
    }

    m_CubeOutline.m_DXResConfig.SetConstantBuffer(m_ConstantBuffer2);
    //SScene.SetConstantBuffer(SceneLoc, m_ConstantBuffer2);

    const wchar_t* TextureName = L"UV_Color_Grid.dds";
    m_hr = CreateDDSTextureFromFile(m_D3D11Device, TextureName, nullptr, &m_TextureRV2);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
        return;
    }

    D3D11_SAMPLER_DESC sampDesc{};
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    m_hr = m_D3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize sampler desc", L"Error", MB_OK);
        return;
    }

    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.01f, 100.0f);
    m_WorldMatrix = XMMatrixIdentity();

    //SceneLoc++;

    m_UnrenderedList.push_back(m_CubeOutline);

    //m_ImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    //m_ImmediateContext->IASetIndexBuffer(m_IndexBufferArray[2], DXGI_FORMAT_R16_UINT, 0);
    //m_ImmediateContext->IASetInputLayout(m_VertexLayout);
    //m_ImmediateContext->VSSetShader(m_VertexShader, nullptr, 0);
    ////m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
    //m_ImmediateContext->PSSetShader(m_PixelShader, nullptr, 0);
    //m_ImmediateContext->PSSetShaderResources(0, 1, &m_TextureRV4);
    //m_ImmediateContext->PSSetSamplers(0, 1, &m_SamplerLinear);
    //m_ImmediateContext->OMSetDepthStencilState(pDefDepthStencilState, 0);
}

void DX11Device::InitArrow()
{
    Scene& SScene = Scene::GetScene();
    PrimitiveGeometryFactory GeometryFactory;

    GameEntity3D m_Arrow = GeometryFactory.CreateEntity3D(PrimitiveGeometryType::Arrow);
    auto SelectedEntity = SScene.GetSceneList2().at(0);

    XMVECTOR LocationVector = SelectedEntity.GetLocationVector();
    XMFLOAT4 LocationFloat;
    XMStoreFloat4(&LocationFloat, LocationVector);

    m_Arrow.SetLocationF(LocationFloat.x, LocationFloat.y, LocationFloat.z);
    //m_Arrow.SetLocationF(3.0f, 0.0f, 0.0f);
    m_Arrow.SetScale(0.1f, 0.1f, 0.1f);
    m_Arrow.m_GameEntityTag = "Arrow";
    m_Arrow.m_DXResConfig.m_UID = "UID1234";

    GameEntity3DComponent ArrowXAxis;
    GameEntity3DComponent ArrowYAxis;
    GameEntity3DComponent ArrowZAxis;
    m_Arrow.m_GameEntity3DComponent.push_back(ArrowXAxis);
    auto& ArrowX = m_Arrow.m_GameEntity3DComponent.at(0);

    TEntityPhysicalMesh ArrowPhysicalMesh;
    ArrowX.m_PhysicalMeshVector.push_back(ArrowPhysicalMesh);
    m_Arrow.PhysicalMesh.SetStride(sizeof(SimpleColorVertex));

    ArrowX.m_PhysicalMeshVector.at(0).SetSimpleColorVerticesList(m_Arrow.m_PhysicalMeshVector.at(0).GetSimpleColorVerticesList());
    ArrowX.m_PhysicalMeshVector.at(0).SetIndicesList(m_Arrow.m_PhysicalMeshVector.at(0).GetIndicesList());
    ArrowX.m_PhysicalMeshVector.at(0).SetStride(sizeof(SimpleColorVertex));

    SScene.AddEntityToScene2(m_Arrow);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_hr = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/solid_color.vs", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"The VS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the vertex shader
    m_hr = m_D3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_VertexShader3);
    if (FAILED(m_hr))
    {
        pVSBlob->Release();
        return;
    }

    m_Arrow.m_DXResConfig.SetVertexShader(m_VertexShader3);
    SScene.SetVertexShader2(SceneLoc, m_VertexShader3);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    m_hr = m_D3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_VertexLayout3);
    pVSBlob->Release();
    if (FAILED(m_hr))
        return;

    m_Arrow.m_DXResConfig.SetInputLayout(m_VertexLayout3);
    SScene.SetInputLayout2(SceneLoc, m_VertexLayout3);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_hr = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/solid_color.ps", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"The PS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the pixel shader
    m_hr = m_D3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_PixelShader3);
    pPSBlob->Release();
    if (FAILED(m_hr))
        return;

    // Add the vertex and pixel shader to the DX Resources object
    m_Arrow.m_DXResConfig.SetVertexShader(m_VertexShader3);
    m_Arrow.m_DXResConfig.SetPixelShader(m_PixelShader3);

    SScene.SetVertexShader2(SceneLoc, m_VertexShader3);
    SScene.SetPixelShader2(SceneLoc, m_PixelShader3);

    // Create vertex buffer
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleColorVertex) * 512;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData{};

    std::vector<SimpleColorVertex> TotalVerticesVector{ m_Arrow.m_PhysicalMeshVector.at(0).GetSimpleColorVerticesList() };
    int TotalVerticesVectorSize = TotalVerticesVector.size();
    SimpleColorVertex* VerticesArray = new SimpleColorVertex[TotalVerticesVectorSize];
    for (int i = 0; i < TotalVerticesVectorSize; i++)
    {
        VerticesArray[i] = TotalVerticesVector.at(i);
    }
    InitData.pSysMem = VerticesArray;
    m_hr = m_D3D11Device->CreateBuffer(&bd, &InitData, &m_VertexBuffer3);
    if (FAILED(m_hr))
        return;

    //std::vector<SimpleColorVertex> TotalVerticesVector{ SScene.GetSceneList2().at(SceneLoc).GetSimpleColorVerticesList() };
    //int TotalVerticesVectorSize = TotalVerticesVector.size();
    //SimpleColorVertex* VerticesArray = new SimpleColorVertex[TotalVerticesVectorSize];
    //for (int i = 0; i < TotalVerticesVectorSize; i++)
    //{
    //    VerticesArray[i] = TotalVerticesVector.at(i);
    //}
    //InitData.pSysMem = VerticesArray;
    //m_hr = m_D3D11Device->CreateBuffer(&bd, &InitData, &m_VertexBuffer3);
    //if (FAILED(m_hr))
    //    return;

    // Set vertex bufferds
    UINT stride = sizeof(SimpleColorVertex);
    UINT offset = 0;

    m_Arrow.m_DXResConfig.SetVertexBuffer(m_VertexBuffer3);
    SScene.SetVertexbuffer2(SceneLoc, m_VertexBuffer3);
    //m_ImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer2, &stride, &offset);

    // Create index buffer
    auto TempArrowIndicesList = SScene.GetSceneList2().at(3).PhysicalMesh.GetIndicesList();
    WORD* TempArrowIndices = TempArrowIndicesList.data();

    auto TempArrowIndicesList2 = SScene.GetSceneList2().at(3).m_PhysicalMeshVector.at(0).GetIndicesList();
    WORD* TempArrowIndices2 = TempArrowIndicesList.data();

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 512;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = TempArrowIndices2;
    m_hr = m_D3D11Device->CreateBuffer(&bd, &InitData, &m_IndexBuffer3);
    if (FAILED(m_hr))
        return;

    // Set index buffer
    m_Arrow.m_DXResConfig.SetIndexBuffer(m_IndexBuffer3, DXGI_FORMAT_R16_UINT, 0);
    SScene.SetIndexbuffer2(SceneLoc, m_IndexBuffer3, DXGI_FORMAT_R16_UINT, 0);

    // Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_hr = m_D3D11Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer3);
    if (FAILED(m_hr))
        return;

    m_Arrow.m_DXResConfig.SetConstantBuffer(m_ConstantBuffer3);
    SScene.SetConstantBuffer2(SceneLoc, m_ConstantBuffer3);

    SceneLoc++;

    m_UnrenderedList.push_back(m_Arrow);
}

void DX11Device::AddTestLine()
{
    Scene& SScene = Scene::GetScene();
    /*ConstantBuffer RayCB{};*/
    PrimitiveGeometryFactory GeometryFactory;

    ////XMFLOAT3 OriginVector(OriginX, OriginY, OriginZ);
    ////XMFLOAT3 DestinationVector(DestinationX, DestinationY, DestinationZ);
    //XMFLOAT3 DestinationVector(sinf(XMConvertToRadians(92)), 0, cosf(XMConvertToRadians(92)));

    //XMVECTOR RayOrigin = XMLoadFloat3(&OriginVector);

    //// Unnormalized vector, must be normalized
    //XMVECTOR RayDestinationTemp = XMLoadFloat3(&DestinationVector);
    //XMVECTOR RayDestination = XMVector3Normalize(RayDestinationTemp);

    //XMFLOAT3 F2V2{ 0.0f, 0.0f, 1.0f };
    //XMVECTOR UnitVector = XMLoadFloat3(&F2V2);

    //g_RayOrigin = RayOrigin;
    //g_RayDestination = RayDestinationTemp;
    //g_RayDestination = UnitVector;

    GameEntity3D Linetrace;
    Linetrace.m_GameEntityTag = "Linetrace";
    Linetrace.SetLocationF(0.0f, 0.0f, 0.0f);
    SScene.AddEntityToScene2(Linetrace);

    //SimpleVertex RayVertices[] =
    //{
    //    { XMFLOAT3(OriginX, OriginY, OriginZ), XMFLOAT2(1.0f, 0.0f)},
    //    { XMFLOAT3(DestinationX, DestinationY, DestinationZ), XMFLOAT2(1.0f, 0.0f)}
    //};

    SimpleVertex RayVertices[] =
    {
        //{ XMFLOAT3(OriginX, OriginY, OriginZ), XMFLOAT2(1.0f, 0.0f)},
        //{ XMFLOAT3(DestinationX, DestinationY, DestinationZ), XMFLOAT2(1.0f, 0.0f)}
        { XMFLOAT3(0.f, -0.1f, 0.f), XMFLOAT2(1.0f, 0.0f)},
        { XMFLOAT3(0.992644 * 10, 0.000792577 * 10, 0.121069 * 10), XMFLOAT2(1.0f, 0.0f)}
    };

    //bd.Usage = D3D11_USAGE_DEFAULT;
    //bd.ByteWidth = sizeof(ConstantBuffer);
    //bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //bd.CPUAccessFlags = 0;
    //m_hr = m_D3D11Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer);
    //if (FAILED(m_hr))
    //{
    //    MessageBox(nullptr, L"Failed to initialize constant buffer", L"Error", MB_OK);
    //    return;
    //}


    D3D11_BUFFER_DESC RayBuffer{ };
    RayBuffer.Usage = D3D11_USAGE_DEFAULT;
    RayBuffer.ByteWidth = sizeof(ConstantBuffer);
    RayBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    RayBuffer.CPUAccessFlags = 0;
    m_hr = m_D3D11Device->CreateBuffer(&RayBuffer, nullptr, &m_ConstantBuffer4);
    SScene.SetConstantBuffer2(SceneLoc, m_ConstantBuffer4);

    D3D11_SUBRESOURCE_DATA RayData = {};
    RayData.pSysMem = RayVertices;

    m_hr = m_D3D11Device->CreateBuffer(&RayBuffer, &RayData, &m_VertexBuffer4);
    SScene.SetVertexbuffer2(SceneLoc, m_VertexBuffer4);

    Logger& SLogger = Logger::GetLogger();
    SLogger.Log("InitLine, called from DX11Device.cpp");

    SceneLoc++;

    m_UnrenderedList.push_back(Linetrace);
}

bool DX11Device::DoesIntersect(FXMVECTOR Origin, FXMVECTOR Direction, XMFLOAT3 Center, XMFLOAT3 Extents, float& Dist)
{
    Logger& SLogger = Logger::GetLogger();

    XMFLOAT3 fDirection{ 0, 0, 0 };
    XMStoreFloat3(&fDirection, Direction);
    SLogger.Log("DX11Device::DoesIntersect, Ray Direction is: ", fDirection.x, " ", fDirection.y, " ", fDirection.z);

    // Load the box.
    XMVECTOR vCenter = XMLoadFloat3(&Center);
    /*XMFLOAT3 fCenter{ 0, 0, 0 };
    XMStoreFloat3(&fCenter, vCenter);
    SLogger.Log("DX11Device::DoesIntersect, Box Center is: ", fCenter.x, " ", fCenter.y, " ", fCenter.z);*/

    XMVECTOR vExtents = XMLoadFloat3(&Extents);
    /*XMFLOAT3 fExtents{ 0, 0, 0 };
    XMStoreFloat3(&fExtents, vExtents);
    SLogger.Log("DX11Device::DoesIntersect, Box Extents is: ", fExtents.x, " ", fExtents.y, " ", fExtents.z);*/

    // Adjust ray origin to be relative to center of the box.
    //XMVECTOR TOrigin = XMVectorSubtract(vCenter, Origin);
    //XMFLOAT3 fOrigin{ 0, 0, 0 };
    //XMStoreFloat3(&fOrigin, TOrigin);
    //SLogger.Log("DX11Device::DoesIntersect, TOrigin is: ", fOrigin.x, " ", fOrigin.y, " ", fOrigin.z);

    //XMFLOAT3 AxisDotDirectionFloat{ sinf(XMConvertToRadians(92)), 0, cosf(XMConvertToRadians(92)) };
    //XMVECTOR AxisDotDirection{ XMLoadFloat3(&AxisDotDirectionFloat) };
    XMVECTOR AxisDotDirection{ Direction };

    // Compute the dot product againt each axis of the box.
    // Since the axii are (1,0,0), (0,1,0), (0,0,1) no computation is necessary.
    XMVECTOR AxisDotOrigin = XMVectorSubtract(vCenter, Origin);
    XMFLOAT3 fAxisDotOrigin{ 0, 0, 0 };
    XMStoreFloat3(&fAxisDotOrigin, AxisDotOrigin);
    SLogger.Log("DX11Device::DoesIntersect, Axis Dot Origin is: ", fAxisDotOrigin.x, " ", fAxisDotOrigin.y, " ", fAxisDotOrigin.z);

    //XMVECTOR AxisDotDirection = Direction;

    // Test against all three axii simultaneously.
    XMVECTOR InverseAxisDotDirection = XMVectorReciprocal(AxisDotDirection);
    XMFLOAT3 fInverseAxisDotDirection{ 0, 0, 0 };
    XMStoreFloat3(&fInverseAxisDotDirection, InverseAxisDotDirection);
    SLogger.Log("DX11Device::DoesIntersect, Inverse Axis Dot Direction is: ", fInverseAxisDotDirection.x, " ", fInverseAxisDotDirection.y, " ", fInverseAxisDotDirection.z);

    // if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
    XMVECTOR IsParallel = XMVectorLessOrEqual(XMVectorAbs(AxisDotDirection), _RayEpsilon);
    XMFLOAT3 fIsParallel{ 0, 0, 0 };
    XMStoreFloat3(&fIsParallel, IsParallel);
    SLogger.Log("DX11Device::DoesIntersect, Is Parallel is: ", fIsParallel.x, " ", fIsParallel.y, " ", fIsParallel.z);

    XMVECTOR t1 = XMVectorMultiply(XMVectorSubtract(AxisDotOrigin, vExtents), InverseAxisDotDirection);
    XMFLOAT3 ft1{ 0, 0, 0 };
    XMStoreFloat3(&ft1, t1);
    SLogger.Log("DX11Device::DoesIntersect, t1 is: ", ft1.x, " ", ft1.y, " ", ft1.z);

    XMVECTOR t2 = XMVectorMultiply(XMVectorAdd(AxisDotOrigin, vExtents), InverseAxisDotDirection);
    XMFLOAT3 ft2{ 0, 0, 0 };
    XMStoreFloat3(&ft2, t2);
    SLogger.Log("DX11Device::DoesIntersect, t2 is: ", ft2.x, " ", ft2.y, " ", ft2.z);

    // Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
    // use the results from any directions parallel to the slab.
    XMVECTOR t_min = XMVectorSelect(XMVectorMin(t1, t2), _FltMin, IsParallel);
    XMFLOAT3 ft_min{ 0, 0, 0 };
    XMStoreFloat3(&ft_min, t_min);
    SLogger.Log("DX11Device::DoesIntersect, t_min is: ", ft_min.x, " ", ft_min.y, " ", ft_min.z);

    XMVECTOR t_max = XMVectorSelect(XMVectorMax(t1, t2), _FltMax, IsParallel);
    XMFLOAT3 ft_max{ 0, 0, 0 };
    XMStoreFloat3(&ft_max, t_max);
    SLogger.Log("DX11Device::DoesIntersect, t_max is: ", ft_max.x, " ", ft_max.y, " ", ft_max.z);

    // t_min.x = maximum( t_min.x, t_min.y, t_min.z );
    // t_max.x = minimum( t_max.x, t_max.y, t_max.z );
    XMVECTOR new_t_min = XMVectorMax(t_min, XMVectorSplatY(t_min));  // x = max(x,y)
    XMFLOAT3 fnew_t_min{ 0, 0, 0 };
    XMStoreFloat3(&fnew_t_min, new_t_min);
    SLogger.Log("DX11Device::DoesIntersect, t_min: ", fnew_t_min.x, " ", fnew_t_min.y, " ", fnew_t_min.z);

    XMVECTOR new_t_min2 = XMVectorMax(new_t_min, XMVectorSplatZ(new_t_min));  // x = max(max(x,y),z)
    XMFLOAT3 fnew_t_min2{ 0, 0, 0 };
    XMStoreFloat3(&fnew_t_min2, new_t_min2);
    SLogger.Log("DX11Device::DoesIntersect, t_min: ", fnew_t_min2.x, " ", fnew_t_min2.y, " ", fnew_t_min2.z);

    XMVECTOR new_t_max = XMVectorMin(t_max, XMVectorSplatY(t_max));  // x = min(x,y)
    XMFLOAT3 fnew_t_max{ 0, 0, 0 };
    XMStoreFloat3(&fnew_t_max, new_t_max);
    SLogger.Log("DX11Device::DoesIntersect, t_max: ", fnew_t_max.x, " ", fnew_t_max.y, " ", fnew_t_max.z);

    XMVECTOR new_t_max2 = XMVectorMin(new_t_max, XMVectorSplatZ(new_t_max));  // x = min(min(x,y),z)
    XMFLOAT3 fnew_t_max2{ 0, 0, 0 };
    XMStoreFloat3(&fnew_t_max2, new_t_max2);
    SLogger.Log("DX11Device::DoesIntersect, t_max: ", fnew_t_max2.x, " ", fnew_t_max2.y, " ", fnew_t_max2.z);

    // if ( t_min > t_max ) return false;
    XMVECTOR NoIntersection = XMVectorGreater(XMVectorSplatX(new_t_min2), XMVectorSplatX(new_t_max2));
    XMFLOAT3 fNoIntersection{ 0, 0, 0 };
    XMStoreFloat3(&fNoIntersection, NoIntersection);
    SLogger.Log("DX11Device::DoesIntersect, No Intersection is: ", fNoIntersection.x, " ", fNoIntersection.y, " ", fNoIntersection.z);

    // if ( t_max < 0.0f ) return false;
    XMVECTOR NoIntersection2 = XMVectorOrInt(NoIntersection, XMVectorLess(XMVectorSplatX(t_max), XMVectorZero()));
    XMFLOAT3 fNoIntersection2{ 0, 0, 0 };
    XMStoreFloat3(&fNoIntersection2, NoIntersection2);
    SLogger.Log("DX11Device::DoesIntersect, No Intersection2 is: ", fNoIntersection2.x, " ", fNoIntersection2.y, " ", fNoIntersection2.z);

    // if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return false;
    XMVECTOR ParallelOverlap = XMVectorInBounds(AxisDotOrigin, vExtents);
    XMFLOAT3 fParallelOverlap{ 0, 0, 0 };
    XMStoreFloat3(&fParallelOverlap, ParallelOverlap);
    SLogger.Log("DX11Device::DoesIntersect, Parallel overlap: ", fParallelOverlap.x, " ", fParallelOverlap.y, " ", fParallelOverlap.z);

    XMVECTOR NoIntersection3 = XMVectorOrInt(NoIntersection2, XMVectorAndCInt(IsParallel, ParallelOverlap));
    XMFLOAT3 fNoIntersection3{ 0, 0, 0 };
    XMStoreFloat3(&fNoIntersection3, NoIntersection3);
    SLogger.Log("DX11Device::DoesIntersect, NoIntersection3 is: ", fNoIntersection3.x, " ", fNoIntersection3.y, " ", fNoIntersection3.z);

    if (!DirectX::Internal::XMVector3AnyTrue(NoIntersection))
    {
        // Store the x-component to *pDist
        XMStoreFloat(&Dist, t_min);
        SLogger.Log("Intersection TRUE");
        return true;
    }

    Dist = 0.f;
    SLogger.Log("Intersection FALSE");
    return false;
}

void DX11Device::TestVector()
{
    Logger& SLogger = Logger::GetLogger();
    SLogger.Log("DX11Device::TestVector()");

    // Load vector (2, 4, 1)
    XMFLOAT3 F2V{ 2.0f, 4.0f, 1.0f };
    XMVECTOR TestVector = XMLoadFloat3(&F2V);

    // Get its length
    TestVector = XMVector3Length(TestVector);

    // Store its members
    XMFLOAT3 TestFloat1{ };
    XMStoreFloat3(&TestFloat1, TestVector);

    SLogger.Log("Vector1 length is: X = ", TestFloat1.x, ", Y = ", TestFloat1.y, ", Z = ", TestFloat1.z);

    float Var0 = 0.f;
    std::bitset<32> Float0(Var0);
    std::bitset<32> FltMax(FLT_MAX);
    std::bitset<32> mFltMax(-FLT_MAX);
    SLogger.Log("0 = ", Float0);
    SLogger.Log("~0 = ", ~Float0);
    SLogger.Log("-FltMax = ", mFltMax);

    union
    {
        unsigned long ul;
        float f;
    } u;

    u.ul = 1084227584; // in HEX, this is 0x40A00000

    SLogger.Log("float value is: ", u.f);

    // Load vector (0, 0, 1)
    XMFLOAT3 F2V2{ 0.0f, 0.0f, 1.0f };
    XMVECTOR UnitVector = XMLoadFloat3(&F2V2);

    //// Get its length
    //UnitVector = XMVector3Length(UnitVector);

    //// Store its members
    //XMFLOAT3 TestFloat2{ };
    //XMStoreFloat3(&TestFloat2, UnitVector);

    XMVECTOR Difference = XMVectorSubtract(XMVector3Length(UnitVector), XMVectorSplatOne());

    XMFLOAT3 TestFloat2{ };
    XMStoreFloat3(&TestFloat2, Difference);

    SLogger.Log("Difference is: X = ", TestFloat2.x, ", Y = ", TestFloat2.y, ", Z = ", TestFloat2.z);

    XMVECTORF32 UnitVectorEpsilon = { { { 1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f } } };

    // Store epsilon vector members
    XMFLOAT3 EpsilonFloat{ };
    XMStoreFloat3(&EpsilonFloat, UnitVectorEpsilon);

    SLogger.Log("Epsilon vector is: X = ", EpsilonFloat.x, ", Y = ", EpsilonFloat.y, ", Z = ", EpsilonFloat.z);

    if (XMVector4Less(XMVectorAbs(Difference), UnitVectorEpsilon))
    {
        SLogger.Log("Difference is less than Epsilon");
    }
    else
    {
        SLogger.Log("Difference is NOT LESS than Epsilon");
    }

    if (XMVector3IsUnit(UnitVector))
    {
        SLogger.Log("Unit vector TRUE");
    }
    else
    {
        SLogger.Log("Unit vector FALSE");
    }

}

void DX11Device::InitLine(float OriginX, float OriginY, float OriginZ, float DestinationX, float DestinationY, float DestinationZ)
{
    Scene& SScene = Scene::GetScene();
    /*ConstantBuffer RayCB{};*/
    PrimitiveGeometryFactory GeometryFactory;

    XMFLOAT3 OriginVector(OriginX, OriginY, OriginZ);
    XMFLOAT3 DestinationVector(DestinationX / 100, DestinationY / 100, DestinationZ / 100);
    //XMFLOAT3 DestinationVector(sinf(XMConvertToRadians(92)), 0, cosf(XMConvertToRadians(92)));

    XMVECTOR RayOrigin = XMLoadFloat3(&OriginVector);

    // Unnormalized vector, must be normalized
    XMVECTOR RayDestinationTemp = XMLoadFloat3(&DestinationVector);
    XMVECTOR RayDestination = XMVector3Normalize(RayDestinationTemp);

    XMFLOAT3 F2V2{ 0.0f, 0.0f, 1.0f };
    XMVECTOR UnitVector = XMLoadFloat3(&F2V2);

    g_RayOrigin = RayOrigin;
    g_RayDestination = RayDestination;
    //g_RayDestination = UnitVector;

    GameEntity3D Linetrace;
    Linetrace.m_GameEntityTag = "Linetrace";
    Linetrace.SetLocationF(0.0f, 0.0f, 0.0f);
    SScene.AddEntityToScene2(Linetrace);

    //SimpleVertex RayVertices[] =
    //{
    //    { XMFLOAT3(OriginX, OriginY, OriginZ), XMFLOAT2(1.0f, 0.0f)},
    //    { XMFLOAT3(DestinationX, DestinationY, DestinationZ), XMFLOAT2(1.0f, 0.0f)}
    //};

    SimpleVertex RayVertices[] =
    {
        { XMFLOAT3(OriginX, OriginY, OriginZ), XMFLOAT2(1.0f, 0.0f)},
        { XMFLOAT3(DestinationX, DestinationY, DestinationZ), XMFLOAT2(1.0f, 0.0f)}
        //{ XMFLOAT3(0, 0, 0), XMFLOAT2(1.0f, 0.0f)},
        //{ XMFLOAT3(sinf(XMConvertToRadians(92)) * 10, 0, cosf(XMConvertToRadians(92)) * 10), XMFLOAT2(1.0f, 0.0f)}
    };

    //bd.Usage = D3D11_USAGE_DEFAULT;
    //bd.ByteWidth = sizeof(ConstantBuffer);
    //bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //bd.CPUAccessFlags = 0;
    //m_hr = m_D3D11Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer);
    //if (FAILED(m_hr))
    //{
    //    MessageBox(nullptr, L"Failed to initialize constant buffer", L"Error", MB_OK);
    //    return;
    //}


    D3D11_BUFFER_DESC RayBuffer{ };
    RayBuffer.Usage = D3D11_USAGE_DEFAULT;
    RayBuffer.ByteWidth = sizeof(ConstantBuffer);
    RayBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    RayBuffer.CPUAccessFlags = 0;
    m_hr = m_D3D11Device->CreateBuffer(&RayBuffer, nullptr, &m_ConstantBuffer4);
    SScene.SetConstantBuffer2(SceneLoc, m_ConstantBuffer4);

    D3D11_SUBRESOURCE_DATA RayData = {};
    RayData.pSysMem = RayVertices;

    m_hr = m_D3D11Device->CreateBuffer(&RayBuffer, &RayData, &m_VertexBuffer4);
    SScene.SetVertexbuffer2(SceneLoc, m_VertexBuffer4);

    Logger& SLogger = Logger::GetLogger();
    SLogger.Log("InitLine, called from DX11Device.cpp");

    SceneLoc++;

    m_UnrenderedList.push_back(Linetrace);
}

void DX11Device::CheckCollision()
{
    Logger& SLogger = Logger::GetLogger();
    float fDistance = -1.0f;
    float fDist;

    CollisionComponent Collision{ m_CubeEntity.GetCollisionComponent() };

    XMFLOAT3 fRayOrigin{ 0, 0, 0 };
    XMStoreFloat3(&fRayOrigin, g_RayOrigin);

    XMFLOAT3 fRayDestination{ 0, 0, 0 };
    XMStoreFloat3(&fRayDestination, g_RayDestination);

    SLogger.Log("DX11Device::CheckCollision: CollisionAABox center vector is: X = ", Collision.AABox.Center.x,
        ", Y = ", Collision.AABox.Center.y,
        ", Z = ", Collision.AABox.Center.z);

    SLogger.Log("DX11Device::CheckCollision: CollisionAABox extents vector is: X = ", Collision.AABox.Extents.x,
        ", Y = ", Collision.AABox.Extents.y,
        ", Z = ", Collision.AABox.Extents.z);

    SLogger.Log("DX11Device::CheckCollision: Ray Origin vector is: X = ", fRayOrigin.x,
        ", Y = ", fRayOrigin.y,
        ", Z = ", fRayOrigin.z);

    SLogger.Log("DX11Device::CheckCollision: Ray Direction vector is: X = ", fRayDestination.x,
        ", Y = ", fRayDestination.y,
        ", Z = ", fRayDestination.z);

    if (DoesIntersect(g_RayOrigin, g_RayDestination, Collision.AABox.Center, Collision.AABox.Extents, fDist))
    {
        fDistance = fDist;
        Collision.CollisionType = INTERSECTS;
        AddOutline();
        AddGizmo();
    }

    //if (Collision.AABox.Intersects(g_RayOrigin, g_RayDestination, fDist))
    //{
    //    fDistance = fDist;
    //    Collision.CollisionType = INTERSECTS;
    //    AddOutline();
    //}

    //if (g_SecondarySpheres[3].sphere.Intersects(g_PrimaryRay.origin, g_PrimaryRay.direction, fDist))
    //{
    //    fDistance = fDist;
    //    g_SecondarySpheres[3].collision = INTERSECTS;
    //}
}

void DX11Device::AddOutline()
{
    Scene& SScene = Scene::GetScene();
    SScene.AddEntityToScene2(m_CubeOutline);
    SceneLoc++;
}

void DX11Device::AddGizmo()
{
    Scene& SScene = Scene::GetScene();
    GameEntity3D& SelectedEntity = SScene.GetSceneList2().at(0);
    InitArrow();
    GameEntity3D& SelectedEntity2 = SScene.GetSceneList2().at(0);
}

void DX11Device::RemoveLineTraceTimer()
{
    Scene& SScene = Scene::GetScene();
    for (int SceneEntityIt = 0; SceneEntityIt < SScene.GetSceneList2().size(); SceneEntityIt++)
    {
        if (SScene.GetSceneList2().at(SceneEntityIt).m_GameEntityTag == "Linetrace")
        {
            //m_GameEntityList.at(It).m_GameEntityTag = "LinetraceNEW";
            //m_GameEntityList.erase(m_GameEntityList.begin() + It);

            SScene.GetSceneList2().erase((SScene.GetSceneList2().begin() + SceneEntityIt));
        }
    }
}

void DX11Device::RemoveLastElementFromScene()
{
    Scene& SScene = Scene::GetScene();
    //SScene.m_SceneList.m_GameEntityList.pop_back();
    SScene.RemoveLastEntityFromScene2();
}

void DX11Device::Render(float RotX, float RotY, float EyeX, float EyeY, float EyeZ)
{
    Logger& SLogger = Logger::GetLogger();
    Scene& SScene = Scene::GetScene();
    //std::vector<GameEntity3D> SceneList = SScene.GetSceneList2();

    XMFLOAT3 CameraPos(EyeX, EyeY, EyeZ);
    XMVECTOR CameraVec = XMLoadFloat3(&CameraPos);

    // To modify according to every object

    XMFLOAT3 ObjectPos(0.0f, 0.0f, 4.0f);
    XMVECTOR ObjectVec = XMLoadFloat3(&ObjectPos);

    XMVECTOR CameraObjectSub = XMVectorSubtract(CameraVec, ObjectVec);
    XMVECTOR length = XMVector3Length(CameraObjectSub);

    float distance = 0.0f;
    XMStoreFloat(&distance, length);

    // 2.0 * atan(tan(FovAngleY * 0.5) / AspectRatio).
    float ratio = (float)m_ViewportWidth / m_ViewportHeight;
    float worldSize = (std::tan(XM_PIDIV4 * 0.5) / ratio) * distance;
    float size = 0.004f * worldSize;

    m_ViewMatrix = XMMatrixIdentity();
    XMMATRIX RotationMatrixX
    (
        1.f, 0.f, 0.f, 0.f,
        0.f, std::cosf(XMConvertToRadians(RotX)), -std::sinf(XMConvertToRadians(RotX)), 0.f,
        0.f, std::sinf(XMConvertToRadians(RotX)), std::cosf(XMConvertToRadians(RotX)), 0.f,
        0.f, 0.f, 0.f, 1.f
    );

    XMMATRIX RotationMatrixY
    (
        std::cosf(XMConvertToRadians(RotY)), 0.f, std::sinf(XMConvertToRadians(RotY)), 0.f,
        0.f, 1.f, 0.f, 0.f,
        -std::sinf(XMConvertToRadians(RotY)), 0.f, std::cosf(XMConvertToRadians(RotY)), 0.f,
        0.f, 0.f, 0.f, 1.f
    );

    XMMATRIX RotationMatrixXY = RotationMatrixY * RotationMatrixX;
    m_ViewMatrix = XMMatrixTranslation(-EyeX, -EyeY, -EyeZ) * RotationMatrixXY;

    m_ImmediateContext->ClearRenderTargetView(m_RenderTargetView, Colors::MidnightBlue);
    m_ImmediateContext->ClearDepthStencilView(pDefDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(SimpleVertex);
    UINT stride2 = sizeof(SimpleColorVertex);
    UINT offset = 0;

    int SceneEntityIndex = 0;
    for (auto SceneEntityIt : SScene.GetSceneList2())
    {
        if (SceneEntityIt.m_GameEntityTag != "Linetrace" && SceneEntityIt.m_GameEntityTag != "Original_Tag")
        {
            if (SceneEntityIt.m_GameEntityTag == "Arrow")
            {
                SceneEntityIt.SetScale(size, size, size);
            }

            ConstantBuffer cb = SceneEntityIt.GetConstantBuffer();
            cb.mWorld = XMMatrixTranspose(cb.mWorld);
            cb.mView = XMMatrixTranspose(m_ViewMatrix);
            cb.mProjection = XMMatrixTranspose(m_ProjectionMatrix);

            ID3D11Buffer* ConstantBuffer = SceneEntityIt.m_DXResConfig.m_ConstantBuffer;
            m_ImmediateContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb, 0, 0);

            ID3D11Buffer* VertexBuffer = SceneEntityIt.m_DXResConfig.m_VertexBuffer;
            assert(VertexBuffer != nullptr);

            UINT Stride;

            if (SceneEntityIt.m_GameEntity3DComponent.size())
            {
                for (auto ComponentIt : SceneEntityIt.m_GameEntity3DComponent)
                {
                    Stride = ComponentIt.m_PhysicalMeshVector.at(0).GetStride();
                    m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &offset);
                }
            }

            if (SceneEntityIt.m_PhysicalMeshVector.size())
            {
                for (auto PhysycalMeshIt : SceneEntityIt.m_PhysicalMeshVector)
                {
                    Stride = PhysycalMeshIt.GetStride();
                    m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &offset);
                }
            }
            else
            {
                Stride = SceneEntityIt.PhysicalMesh.GetStride();
                m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &offset);
            }

            //if (SceneEntityIndex == 0)
            //{
            //    m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
            //}
            //else
            //{
            //    m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride2, &offset);
            //}

            ID3D11Buffer* IndexBuffer = SceneEntityIt.m_DXResConfig.m_IndexBuffer;
            DXGI_FORMAT IndexBufferFormat = SceneEntityIt.m_DXResConfig.m_IndexBufferFormat;
            UINT IndexBufferOffset = SceneEntityIt.m_DXResConfig.m_IndexBufferOffset;
            m_ImmediateContext->IASetIndexBuffer(IndexBuffer, IndexBufferFormat, IndexBufferOffset);

            ID3D11InputLayout* InputLayout = SceneEntityIt.m_DXResConfig.GetInputLayout();
            m_ImmediateContext->IASetInputLayout(InputLayout);

            ID3D11VertexShader* VertexShader = SceneEntityIt.m_DXResConfig.m_VertexShader;
            ID3D11PixelShader* PixelShader = SceneEntityIt.m_DXResConfig.m_PixelShader;

            m_ImmediateContext->VSSetShader(VertexShader, nullptr, 0);
            m_ImmediateContext->PSSetShader(PixelShader, nullptr, 0);

            m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);

            if (SceneEntityIt.m_GameEntityTag == "Arrow")
            {
                m_ImmediateContext->OMSetDepthStencilState(m_DepthStencilStateDisabled, 0);
            }
            else if (SceneEntityIt.m_GameEntityTag == "Outline")
            {
                m_ImmediateContext->OMSetDepthStencilState(pDepthStencilStateOutline, 0);
            }
            else
            {
                m_ImmediateContext->OMSetDepthStencilState(pDefDepthStencilState, 0);
            }

            m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_ImmediateContext->DrawIndexed(512, 0, 0);

            //m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
            //m_ImmediateContext->Draw(2, 0);
        }

        //else if (SceneEntityIt.m_GameEntityTag == "Original_Tag")
        //{
        //    ConstantBuffer cb = SceneEntityIt.GetConstantBuffer();
        //    cb.mWorld = XMMatrixTranspose(cb.mWorld);
        //    cb.mView = XMMatrixTranspose(m_ViewMatrix);
        //    cb.mProjection = XMMatrixTranspose(m_ProjectionMatrix);

        //    ID3D11Buffer* ConstantBuffer = SceneEntityIt.m_GameEntity3DComponent.at(0).m_DXResConfig.m_ConstantBuffer;
        //    m_ImmediateContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb, 0, 0);

        //    ID3D11Buffer* VertexBuffer = SceneEntityIt.m_GameEntity3DComponent.at(0).m_DXResConfig.m_VertexBuffer;
        //    assert(VertexBuffer != nullptr);

        //    m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

        //    //ID3D11Buffer* IndexBuffer = SceneEntityIt.m_DXResConfig.m_IndexBuffer;
        //    ID3D11Buffer* IndexBuffer = SceneEntityIt.m_GameEntity3DComponent.at(0).m_DXResConfig.m_IndexBuffer;
        //    DXGI_FORMAT IndexBufferFormat = SceneEntityIt.m_GameEntity3DComponent.at(0).m_DXResConfig.m_IndexBufferFormat;
        //    UINT IndexBufferOffset = SceneEntityIt.m_GameEntity3DComponent.at(0).m_DXResConfig.m_IndexBufferOffset;
        //    m_ImmediateContext->IASetIndexBuffer(IndexBuffer, IndexBufferFormat, IndexBufferOffset);

        //    ID3D11InputLayout* InputLayout = SceneEntityIt.m_GameEntity3DComponent.at(0).m_DXResConfig.GetInputLayout();
        //    m_ImmediateContext->IASetInputLayout(InputLayout);

        //    ID3D11VertexShader* VertexShader = SceneEntityIt.m_GameEntity3DComponent.at(0).m_DXResConfig.m_VertexShader;
        //    ID3D11PixelShader* PixelShader = SceneEntityIt.m_GameEntity3DComponent.at(0).m_DXResConfig.m_PixelShader;

        //    m_ImmediateContext->VSSetShader(VertexShader, nullptr, 0);
        //    m_ImmediateContext->PSSetShader(PixelShader, nullptr, 0);

        //    m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
        //    m_ImmediateContext->OMSetDepthStencilState(pDefDepthStencilState, 0);

        //    m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //    m_ImmediateContext->DrawIndexed(512, 0, 0);
        //}

        else if (SceneEntityIt.m_GameEntityTag == "Original_Tag")
        {
            for (auto SceneEntityComponentIt : SceneEntityIt.m_GameEntity3DComponent)
            {
                ConstantBuffer cb = SceneEntityIt.GetConstantBuffer();
                cb.mWorld = XMMatrixTranspose(cb.mWorld);
                cb.mView = XMMatrixTranspose(m_ViewMatrix);
                cb.mProjection = XMMatrixTranspose(m_ProjectionMatrix);

                ID3D11Buffer* ConstantBuffer = SceneEntityComponentIt.m_DXResConfig.m_ConstantBuffer;
                m_ImmediateContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb, 0, 0);

                ID3D11Buffer* VertexBuffer = SceneEntityComponentIt.m_DXResConfig.m_VertexBuffer;
                assert(VertexBuffer != nullptr);

                m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

                //ID3D11Buffer* IndexBuffer = SceneEntityIt.m_DXResConfig.m_IndexBuffer;
                ID3D11Buffer* IndexBuffer = SceneEntityComponentIt.m_DXResConfig.m_IndexBuffer;
                DXGI_FORMAT IndexBufferFormat = SceneEntityComponentIt.m_DXResConfig.m_IndexBufferFormat;
                UINT IndexBufferOffset = SceneEntityComponentIt.m_DXResConfig.m_IndexBufferOffset;
                m_ImmediateContext->IASetIndexBuffer(IndexBuffer, IndexBufferFormat, IndexBufferOffset);

                ID3D11InputLayout* InputLayout = SceneEntityComponentIt.m_DXResConfig.GetInputLayout();
                m_ImmediateContext->IASetInputLayout(InputLayout);

                ID3D11VertexShader* VertexShader = SceneEntityComponentIt.m_DXResConfig.m_VertexShader;
                ID3D11PixelShader* PixelShader = SceneEntityComponentIt.m_DXResConfig.m_PixelShader;

                m_ImmediateContext->VSSetShader(VertexShader, nullptr, 0);
                m_ImmediateContext->PSSetShader(PixelShader, nullptr, 0);

                m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
                m_ImmediateContext->OMSetDepthStencilState(pDefDepthStencilState, 0);

                m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                m_ImmediateContext->DrawIndexed(512, 0, 0);
            }
        }

        else
        {
            ID3D11Buffer* VertexBuffer = SceneEntityIt.m_DXResConfig.GetVertexBuffer();
            m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
            m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

            ConstantBuffer cb = SceneEntityIt.GetConstantBuffer();
            cb.mWorld = XMMatrixTranspose(cb.mWorld);
            cb.mView = XMMatrixTranspose(m_ViewMatrix);
            cb.mProjection = XMMatrixTranspose(m_ProjectionMatrix);

            ID3D11Buffer* ConstantBuffer = SceneEntityIt.m_DXResConfig.m_ConstantBuffer;
            m_ImmediateContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb, 0, 0);

            m_ImmediateContext->Draw(2, 0);
        }

        SceneEntityIndex++;
        SLogger.Log("DX11Device.cpp, function Render(): EntityTag = ", SceneEntityIt.m_GameEntityTag, "\n");
    }
    m_SwapChain->Present(0, 0);
}

void DX11Device::CleanupDX11Device()
{
    if (m_ImmediateContext) m_ImmediateContext->ClearState();

    if (m_ConstantBuffer) m_ConstantBuffer->Release();
    if (m_ConstantBuffer3) m_ConstantBuffer3->Release();
    if (m_VertexBuffer) m_VertexBuffer->Release();
    for (auto indexBuffer : m_IndexBufferArray)
    {
        if (indexBuffer)
        {
            indexBuffer->Release();
        }
    }
    if (m_VertexLayout) m_VertexLayout->Release();
    if (m_VertexShader) m_VertexShader->Release();
    if (m_PixelShader) m_PixelShader->Release();
    if (m_RenderTargetView) m_RenderTargetView->Release();
    if (m_SwapChain1) m_SwapChain1->Release();
    if (m_SwapChain) m_SwapChain->Release();
    if (m_ImmediateContext1) m_ImmediateContext1->Release();
    if (m_ImmediateContext) m_ImmediateContext->Release();
    if (m_D3D11Device1) m_D3D11Device1->Release();
    if (m_D3D11Device) m_D3D11Device->Release();
    if (m_TextureRV) m_TextureRV->Release();
}
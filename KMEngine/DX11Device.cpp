#include "DX11Device.h"

XMMATRIX DX11Device::m_WorldMatrix;
XMMATRIX DX11Device::m_ViewMatrix;
XMMATRIX DX11Device::m_ProjectionMatrix;

int SceneLoc = 0;

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
    InitOutlineDepthStencil();
    InitViewport();
    InitShaders();
    InitShaders2();
    InitShaders3();

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
        GetClientRect(m_Viewport, &rc);
        m_ViewportWidth = rc.right - rc.left;
        m_ViewportHeight = rc.bottom - rc.top;
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
}

{
    // Create the depth stencil outline
    defDepthStencilDesc.DepthEnable = true;
    defDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    defDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    defDepthStencilDesc.StencilEnable = true;

void DX11Device::InitShaders()
{
    Scene& SScene = Scene::GetScene();
    PrimitiveGeometryFactory GeometryFactory;

    m_CubeEntity = GeometryFactory.CreateEntity3D(PrimitiveGeometryType::Cube);
    m_CubeEntity.SetLocationF(0.0f, 0.0f, 4.0f);
    m_CubeEntity.m_GameEntityTag = "Original_Tag";
    m_CubeEntity.m_DXResConfig.m_UID = "UID1234";

    CollisionComponent Collision = m_CubeEntity.GetCollisionComponent();
    Collision.AABox.Center = { 0, 0, 10 };
    Collision.AABox.Extents = { 1.0f, 1.0f, 1.0f };
    Collision.CollisionType = DISJOINT;

    SScene.AddEntityToScene(m_CubeEntity);
    //m_GameEntityList.push_back(m_CubeEntity);

    //g_SecondaryAABoxes[i].aabox.Center = XMFLOAT3(0, 0, 0);
    //g_SecondaryAABoxes[i].aabox.Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);
    //g_SecondaryAABoxes[i].collision = DISJOINT;

    //m_CubeEntity2 = GeometryFactory.CreateEntity3D(PrimitiveGeometryType::Cube);
    //m_CubeEntity2.SetLocationF(10.0f, 0.0f, 0.0f);
    //m_CubeEntity2.GetCollisionComponent().AABox.Center = { 0, 0, 10 };
    //m_CubeEntity2.m_GameEntityTag = "Default";
    //SScene.AddEntityToScene(m_CubeEntity2);
    //m_CubeEntity2.SetScale(0.25f, 0.25f, 0.25f);
    //m_GameEntityList.push_back(m_CubeEntity2);
    //SScene.AddEntityToScene(m_CubeEntity2);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_hr = CompileShaderFromFile(L"C:/Users/Dell Gray/source/repos/KMEngine/KMEngine/texture.vs", "VS", "vs_5_0", &pVSBlob);

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

    SScene.SetInputLayout(SceneLoc, m_VertexLayout);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    //hr = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_4_0", &pPSBlob);
    m_hr = CompileShaderFromFile(L"C:/Users/Dell Gray/source/repos/KMEngine/KMEngine/texture.ps", "PS", "ps_5_0", &pPSBlob);
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

    SScene.SetVertexShader(SceneLoc, m_VertexShader);
    SScene.SetPixelShader(SceneLoc, m_PixelShader);

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData{};

    std::vector<SimpleVertex> TotalVerticesVector{ SScene.GetSceneList().at(SceneLoc).GetVerticesList() };
    int TotalVerticesVectorSize = TotalVerticesVector.size();
    SimpleVertex* VerticesArray = new SimpleVertex[TotalVerticesVectorSize];
    for (int i = 0; i < TotalVerticesVectorSize; i++)
    {
        VerticesArray[i] = TotalVerticesVector.at(i);
    }
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

    SScene.SetVertexbuffer(SceneLoc, m_VertexBuffer);

    // Cube indices
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
    m_hr = m_D3D11Device->CreateBuffer(&bd, &InitData, &m_IndexBufferArray[0]);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize index buffer", L"Error", MB_OK);
        return;
    }

    SScene.SetIndexbuffer(SceneLoc, m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);

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
    SScene.SetConstantBuffer(SceneLoc, m_ConstantBuffer);

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

void DX11Device::InitShaders2()
{
    Scene& SScene = Scene::GetScene();
    PrimitiveGeometryFactory GeometryFactory;

    m_CubeOutline = GeometryFactory.CreateEntity3D(PrimitiveGeometryType::CubeTest);
    m_CubeOutline.SetLocationF(0.0f, 0.0f, 4.0f);
    m_CubeOutline.SetScale(1.02f, 1.02f, 1.02f);
    m_CubeOutline.m_GameEntityTag = "Outline";
    //m_GameEntityList.push_back(m_CubeEntity);
    //SScene.AddEntityToScene(m_CubeOutline);

    //// Create the depth stencil outline
    //defDepthStencilDesc.DepthEnable = true;
    //defDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    //defDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    //defDepthStencilDesc.StencilEnable = true;
    //defDepthStencilDesc.StencilReadMask = 0xFF;
    //defDepthStencilDesc.StencilWriteMask = 0xFF;

    //// It does not matter what we write since we are not using the values after this step.
    //// In other words, we are only using the values to mask pixels.
    //defDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    //// The stencil test passes if the passed parameter is equal to value in the buffer.
    //defDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    //// Again, we do not care about back-facing pixels.
    //defDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

    ////hr = g_pd3dDevice->CreateDepthStencilState(&g_DepthStencilDesc, &g_pDepthStencilOutlineState);

    //m_D3D11Device->CreateDepthStencilState(&defDepthStencilDesc, &pDepthStencilStateOutline);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    // hr = CompileShaderFromFile(L"Tutorial04.fxh", "VS", "vs_4_0", &pVSBlob);
    m_hr = CompileShaderFromFile(L"C:/Users/Dell Gray/source/repos/KMEngine/KMEngine/solid_color.vs", "VS", "vs_5_0", &pVSBlob);

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
    m_hr = CompileShaderFromFile(L"C:/Users/Dell Gray/source/repos/KMEngine/KMEngine/solid_color.ps", "PS", "ps_5_0", &pPSBlob);
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

void DX11Device::InitShaders3()
{

    Scene& SScene = Scene::GetScene();
    PrimitiveGeometryFactory GeometryFactory;

    GameEntity3D m_Arrow = GeometryFactory.CreateEntity3D(PrimitiveGeometryType::Arrow);
    m_Arrow.SetLocationF(0.0f, 0.0f, 4.0f);
    m_Arrow.SetScale(0.1f, 0.1f, 0.1f);
    m_Arrow.m_GameEntityTag = "ConstantScale";
    m_Arrow.m_DXResConfig.m_UID = "UID1234";
    SScene.AddEntityToScene(m_Arrow);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_hr = CompileShaderFromFile(L"C:/Users/Dell Gray/source/repos/KMEngine/KMEngine/solid_color.vs", "VS", "vs_5_0", &pVSBlob);
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
    SScene.SetVertexShader(SceneLoc, m_VertexShader3);

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
    SScene.SetInputLayout(SceneLoc, m_VertexLayout3);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_hr = CompileShaderFromFile(L"C:/Users/Dell Gray/source/repos/KMEngine/KMEngine/solid_color.ps", "PS", "ps_5_0", &pPSBlob);
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

    SScene.SetVertexShader(SceneLoc, m_VertexShader3);
    SScene.SetPixelShader(SceneLoc, m_PixelShader3);

    // Create vertex buffer
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleColorVertex) * 512;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData{};

    std::vector<SimpleColorVertex> TotalVerticesVector{ SScene.GetSceneList().at(SceneLoc).GetSimpleColorVerticesList() };
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

    // Set vertex buffer
    UINT stride = sizeof(SimpleColorVertex);
    UINT offset = 0;

    m_Arrow.m_DXResConfig.SetVertexBuffer(m_VertexBuffer3);
    SScene.SetVertexbuffer(SceneLoc, m_VertexBuffer3);
    //m_ImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer2, &stride, &offset);

    // Create index buffer

        ////// Arrow indices
    WORD ArrowIndices[] =
    {
        //3,1,0,
        //2,1,3,

        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6,
        0, 6, 7,
        0, 7, 8,
        0, 8, 9,
        0, 9, 10,
        0, 10, 11,
        0, 11, 12,
        0, 12, 13,
        0, 13, 14,
        0, 14, 15,
        0, 15, 16,
        0, 1, 16,

        1, 2, 17,
        17, 18, 2,
        3, 2, 18,
        18, 19, 3,
        4, 3, 19,
        19, 20, 4,
        5, 4, 20,
        20, 21, 5,
        6, 5, 21, 
        21, 22, 6,
        7, 6, 22,
        22, 23, 7,
        8, 7, 23,
        23, 24, 8,
        9, 8, 24,
        24, 25, 9,
        10, 9, 25,
        25, 26, 10,
        11, 10, 26,
        26, 27, 11,
        12, 11, 27,
        27, 28, 12,
        13, 12, 28,
        28, 29, 13,
        13, 14, 29,
        29, 30, 14,
        15, 14, 30,
        30, 31, 15,
        16, 15, 31,
        31, 32, 16,
        1, 16, 32,
        32, 17, 1,

        17, 18, 33,
        33, 34, 18,
        18, 19, 34,
        34, 35, 19,
        19, 20, 35,
        35, 36, 20,
        20, 21, 36,
        36, 37, 21,
        21, 22, 37,
        37, 38, 22,
        22, 23, 38,
        38, 39, 23,
        23, 24, 39, 
        39, 40, 24,
        24, 25, 40,
        40, 41, 25,
        25, 26, 41, 
        41, 42, 26,
        26, 27, 42,
        42, 43, 27,
        27, 28, 43, 
        43, 44, 28,
        28, 29, 44,
        44, 45, 29,
        29, 30, 45,
        45, 46, 30,
        30, 31, 46,
        46, 47, 31,
        31, 32, 47,
        47, 48, 32,
        32, 17, 48,
        48, 33, 17,

        49, 33, 34,
        49, 34, 35,
        49, 35, 36,
        49, 36, 37,
        49, 37, 38,
        49, 38, 39,
        49, 39, 40,
        49, 41, 42,
        49, 42, 43,
        49, 43, 44,
        49, 44, 45,
        49, 45, 46,
        49, 46, 47,
        49, 47, 48
    }; 

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 512;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = ArrowIndices;
    m_hr = m_D3D11Device->CreateBuffer(&bd, &InitData, &m_IndexBuffer3);
    if (FAILED(m_hr))
        return;

    // Set index buffer
    m_Arrow.m_DXResConfig.SetIndexBuffer(m_IndexBuffer3, DXGI_FORMAT_R16_UINT, 0);
    SScene.SetIndexbuffer(SceneLoc, m_IndexBuffer3, DXGI_FORMAT_R16_UINT, 0);

    // Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_hr = m_D3D11Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer3);
    if (FAILED(m_hr))
        return;

    m_Arrow.m_DXResConfig.SetConstantBuffer(m_ConstantBuffer3);
    SScene.SetConstantBuffer(SceneLoc, m_ConstantBuffer3);

    SceneLoc++;

    m_UnrenderedList.push_back(m_Arrow);
}

void DX11Device::AddLine(float OriginX, float OriginY, float OriginZ, float DestinationX, float DestinationY, float DestinationZ)
{
    Scene& SScene = Scene::GetScene();
    ConstantBuffer RayCB{};

    //RaycastDestinationX = -std::sinf(XMConvertToRadians(g_RotY)) * 5;
   // RaycastDestinationY = -std::cosf(XMConvertToRadians(g_RotY)) * 5;
    //RaycastDestinationZ = std::sinf(XMConvertToRadians(g_RotX)) * 5;

    PrimitiveGeometryFactory GeometryFactory;

    GameEntity3D Linetrace{ GeometryFactory.CreateLinetrace(OriginX, OriginY, OriginZ, DestinationX, DestinationY, DestinationZ) };
    Linetrace.m_GameEntityTag = "Linetrace";
    Linetrace.SetLocationF(0.0f, 0.0f, 0.0f);
    SScene.AddEntityToScene(Linetrace);

    //SimpleVertex RayVertices[] =
    //{
    //    { XMFLOAT3(OriginX, OriginY, OriginZ), XMFLOAT2(1.0f, 0.0f)},
    //    { XMFLOAT3(DestinationX, DestinationY, DestinationZ), XMFLOAT2(1.0f, 0.0f)}
    //};

    D3D11_BUFFER_DESC RayConstantBuffer{ };
    RayConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
    RayConstantBuffer.ByteWidth = sizeof(ConstantBuffer);
    RayConstantBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    RayConstantBuffer.CPUAccessFlags = 0;
    m_hr = m_D3D11Device->CreateBuffer(&RayConstantBuffer, nullptr, &m_ConstantBuffer4);
    SScene.SetConstantBuffer(SceneLoc, m_ConstantBuffer4);

    std::vector<SimpleColorVertex> LinetraceList = Linetrace.GetSimpleColorVerticesList();
    int LinetraceListSize = LinetraceList.size();
    SimpleColorVertex* LinetraceVertex = new SimpleColorVertex[LinetraceListSize];

    for (int i = 0; i < LinetraceListSize; i++)
    {
        LinetraceVertex[i] = LinetraceList.at(i);
    }

    //InitData.pSysMem = VertexObj;

    D3D11_BUFFER_DESC RayBuffer = {};
    RayBuffer.Usage = D3D11_USAGE_DEFAULT;
    RayBuffer.ByteWidth = sizeof(SimpleColorVertex) * 8;
    RayBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    RayBuffer.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA RayData = {};
    RayData.pSysMem = LinetraceVertex;

    m_hr = m_D3D11Device->CreateBuffer(&RayBuffer, &RayData, &m_VertexBuffer4);
    SScene.SetVertexbuffer(SceneLoc, m_VertexBuffer4);

    // Set vertex buffer
    UINT stride = sizeof(SimpleColorVertex);
    UINT offset = 0;
    //m_ImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer3, &stride, &offset);

    SceneLoc++;
}

void DX11Device::InitLine(float OriginX, float OriginY, float OriginZ, float DestinationX, float DestinationY, float DestinationZ)
{
    Scene& SScene = Scene::GetScene();
    /*ConstantBuffer RayCB{};*/
    PrimitiveGeometryFactory GeometryFactory;

    GameEntity3D Linetrace;
    Linetrace.m_GameEntityTag = "Linetrace";
    Linetrace.SetLocationF(0.0f, 0.0f, 0.0f);
    SScene.AddEntityToScene(Linetrace);

    //SimpleVertex RayVertices[] =
    //{
    //    { XMFLOAT3(OriginX, OriginY, OriginZ), XMFLOAT2(1.0f, 0.0f)},
    //    { XMFLOAT3(DestinationX, DestinationY, DestinationZ), XMFLOAT2(1.0f, 0.0f)}
    //};

    SimpleVertex RayVertices[] =
    {
        { XMFLOAT3(OriginX, OriginY, OriginZ), XMFLOAT2(1.0f, 0.0f)},
        { XMFLOAT3(DestinationX, DestinationY, DestinationZ), XMFLOAT2(1.0f, 0.0f)}
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
    SScene.SetConstantBuffer(SceneLoc, m_ConstantBuffer4);

    D3D11_SUBRESOURCE_DATA RayData = {};
    RayData.pSysMem = RayVertices;

    m_hr = m_D3D11Device->CreateBuffer(&RayBuffer, &RayData, &m_VertexBuffer4);
    SScene.SetVertexbuffer(SceneLoc, m_VertexBuffer4);

    Logger& SLogger = Logger::GetLogger();
    SLogger.Log("InitLine, called from DX11Device.cpp");

    SceneLoc++;

    m_UnrenderedList.push_back(Linetrace);
}

void DX11Device::CheckCollision()
{

}

void DX11Device::AddOutline()
{
    Scene& SScene = Scene::GetScene();
    SScene.AddEntityToScene(m_CubeOutline);
    SceneLoc++;
}

void DX11Device::Render(float RotX, float RotY, float EyeX, float EyeY, float EyeZ)
{
    Logger& SLogger = Logger::GetLogger();
    Scene& SScene = Scene::GetScene();
    std::vector<GameEntity3D> SceneList = SScene.GetSceneList();

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
    float worldSize = (std::tan (XM_PIDIV4 * 0.5) / ratio) * distance;
    float size = 0.004f * worldSize;

    /*SLogger.TemplatedLog("Eye X, Y and Z are: ", EyeX, EyeY, EyeZ);*/
    SLogger.Log("Oi ", 12345," ", true);

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
    for (auto SceneEntityIt : SceneList)
    {
        if (SceneEntityIt.m_GameEntityTag != "Linetrace")
        {
            if (SceneEntityIt.m_GameEntityTag == "ConstantScale")
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

            if (SceneEntityIndex == 0)
            {
                m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
            }
            else
            {
                m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride2, &offset);
            }

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

            if (SceneEntityIt.m_GameEntityTag == "ConstantScale")
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
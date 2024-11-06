#include <bitset>
#include <locale>
#include <codecvt>

#include "CoreClock.h"
#include "DX11Device.h"

#define FLT_MAX          3.402823466e+38F

int gClicked = 0;

using namespace Internal;
using namespace std;

XMMATRIX CDX11Device::m_WorldMatrix;
XMMATRIX CDX11Device::m_ViewMatrix;
XMMATRIX CDX11Device::m_ProjectionMatrix;

int SceneLoc = 0;

/* Int for color highlight in gizmo shader */
int ArrowClicked = 0;

XMVECTOR g_RayOrigin;
XMVECTOR g_RayDestination;

XMGLOBALCONST XMVECTORF32 _RayEpsilon = { { { 1e-20f, 1e-20f, 1e-20f, 1e-20f } } };
XMGLOBALCONST XMVECTORF32 _FltMin = { { { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX } } };
XMGLOBALCONST XMVECTORF32 _FltMax = { { { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX } } };

HRESULT CDX11Device::InitDX11Device()
{
    InitDriveTypeAndFeatureLevelFinal();
    IDXGIFactory1* dxgiFactory = InitDXGIFactoryFinal();
    InitSwapChainFinal(dxgiFactory);
    InitRenderTargetViewFinal();
    InitViewportFinal();

    //D3D11_TEXTURE2D_DESC Texture2D = InitTexture2D();
    //InitDepthStencilView(Texture2D);
    //InitDisabledDepthStencil();
    InitDefaultDepthStencil3();
    //InitBaseCube();

    //AddLight();
    InitTexturedCube();
    //InitTexturedCube2();
    //InitRaycast(0, 0, 0, 100, 2, 3);
    //InterpMoveCube();
    //InitTexturedCube2();
    //AddTestLine2();
    //GenerateTerrain();
    //AddTestLine3();

    return S_OK;
}

void CDX11Device::SetViewport(HWND InViewport)
{
    if (InViewport)
        m_Viewport = InViewport;

    RECT rc;
    if (m_Viewport)
    {
        m_ViewportWidth = 639;
        m_ViewportHeight = 1020;
    }
}

D3D11_TEXTURE2D_DESC CDX11Device::InitTexture2D()
{
    D3D11_TEXTURE2D_DESC DescDepth{ };
    DescDepth.Width = m_ViewportWidth;
    DescDepth.Height = m_ViewportHeight;
    DescDepth.MipLevels = 1;
    DescDepth.ArraySize = 1;
    DescDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    DescDepth.SampleDesc.Count = 1;
    DescDepth.SampleDesc.Quality = 0;
    DescDepth.Usage = D3D11_USAGE_DEFAULT;
    DescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    DescDepth.CPUAccessFlags = 0;
    DescDepth.MiscFlags = 0;
    m_HR = m_pD3D11Device->CreateTexture2D(&DescDepth, nullptr, &m_pDepthStencil);

    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize Texture Desc", L"Error", MB_OK);
        return { };
    }

    return DescDepth;
}

void CDX11Device::InitDepthStencilView(D3D11_TEXTURE2D_DESC descDepth)
{
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV{};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    m_HR = m_pD3D11Device->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_DepthStencilView);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize Stencil View Desc", L"Error", MB_OK);
        return;
    }

    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_DepthStencilView);
}

UINT CDX11Device::GetViewportWidth()
{
    return m_ViewportWidth;
}

UINT CDX11Device::GetViewportHeight()
{
    return m_ViewportHeight;
}

void CDX11Device::InitDefaultDepthStencil3()
{

    /*defDescDepth3.Width = m_ViewportWidth;
    defDescDepth3.Height = m_ViewportHeight;
    defDescDepth3.MipLevels = 1;
    defDescDepth3.ArraySize = 1;
    defDescDepth3.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    defDescDepth3.SampleDesc.Count = 1;
    defDescDepth3.SampleDesc.Quality = 0;
    defDescDepth3.Usage = D3D11_USAGE_DEFAULT;
    defDescDepth3.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    defDescDepth3.CPUAccessFlags = 0;
    defDescDepth3.MiscFlags = 0;
    m_hr = m_D3D11Device->CreateTexture2D(&defDescDepth3, NULL, &pDefDepthStencil3);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize depth stencil", L"Error", MB_OK);
        return;
    }

    defDescDepthStencilViewDesc3.Format = defDescDepth3.Format;
    defDescDepthStencilViewDesc3.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    defDescDepthStencilViewDesc3.Texture2D.MipSlice = 0;
    m_hr = m_D3D11Device->CreateDepthStencilView(pDefDepthStencil3, &defDescDepthStencilViewDesc3, &pDefDepthStencilView3);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize depth stencil view", L"Error", MB_OK);
        return;
    }

    defDepthStencilDesc3.DepthEnable = true;
    defDepthStencilDesc3.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    defDepthStencilDesc3.DepthFunc = D3D11_COMPARISON_LESS;

    defDepthStencilDesc3.StencilEnable = true;
    defDepthStencilDesc3.StencilReadMask = 0xFF;
    defDepthStencilDesc3.StencilWriteMask = 0xFF;

    defDepthStencilDesc3.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc3.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    defDepthStencilDesc3.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc3.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    defDepthStencilDesc3.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc3.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    defDepthStencilDesc3.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc3.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    m_hr = m_D3D11Device->CreateDepthStencilState(&defDepthStencilDesc3, &pDefDepthStencilState3);
    if (FAILED(m_hr))
    {
        MessageBox(nullptr, L"Failed to initialize depth stencil state", L"Error", MB_OK);
        return;
    }

    m_ImmediateContext->OMSetDepthStencilState(pDefDepthStencilState3, 0);*/

    ID3D11Texture2D* pDepthStencil3 = NULL;
    defDescDepth3.Width = m_ViewportWidth;
    defDescDepth3.Height = m_ViewportHeight;
    defDescDepth3.MipLevels = 1;
    defDescDepth3.ArraySize = 1;
    defDescDepth3.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    defDescDepth3.SampleDesc.Count = 1;
    defDescDepth3.SampleDesc.Quality = 0;
    defDescDepth3.Usage = D3D11_USAGE_DEFAULT;
    defDescDepth3.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    defDescDepth3.CPUAccessFlags = 0;
    defDescDepth3.MiscFlags = 0;
    m_HR = m_pD3D11Device->CreateTexture2D(&defDescDepth3, NULL, &pDepthStencil3);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize depth stencil", L"Error", MB_OK);
        return;
    }


    defDescDepthStencilViewDesc3.Format = defDescDepth3.Format;
    defDescDepthStencilViewDesc3.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    defDescDepthStencilViewDesc3.Texture2D.MipSlice = 0;

    // Create the depth stencil view
    m_HR = m_pD3D11Device->CreateDepthStencilView(pDepthStencil3, // Depth stencil texture
        &defDescDepthStencilViewDesc3, // Depth stencil desc
        &pDefDepthStencilView3);  // [out] Depth stencil view
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize depth stencil view", L"Error", MB_OK);
        return;
    }

    // Depth test parameters
    defDepthStencilDesc3.DepthEnable = true;
    defDepthStencilDesc3.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    defDepthStencilDesc3.DepthFunc = D3D11_COMPARISON_LESS;

    // Stencil test parameters
    defDepthStencilDesc3.StencilEnable = true;
    defDepthStencilDesc3.StencilReadMask = 0xFF;
    defDepthStencilDesc3.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    defDepthStencilDesc3.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc3.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    defDepthStencilDesc3.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc3.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    defDepthStencilDesc3.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc3.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    defDepthStencilDesc3.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    defDepthStencilDesc3.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create depth stencil state
    m_pD3D11Device->CreateDepthStencilState(&defDepthStencilDesc3, &pDefDepthStencilState3);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize depth stencil state", L"Error", MB_OK);
        return;
    }




    // Bind the depth stencil view
    m_pImmediateContext->OMSetRenderTargets(1,          // One rendertarget view
        &m_pRenderTargetView,      // Render target view, created earlier
        pDefDepthStencilView3);     // Depth stencil view for the render target


    m_pImmediateContext->OMSetDepthStencilState(pDefDepthStencilState3, 0);
}

void CDX11Device::InitDisabledDepthStencil()
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
    m_HR = m_pD3D11Device->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

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
    m_pD3D11Device->CreateDepthStencilState(&dsDesc, &m_pDepthStencilStateDisabled);

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
    OutlineDepthStencilDesc.DepthEnable = true;
    OutlineDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    OutlineDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    OutlineDepthStencilDesc.StencilEnable = true;
}

void CDX11Device::InitCube()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;

    m_CubeEntity = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::Cube);

    // Original cube entity
    m_CubeEntity.SetLocationF(3.0f, 0.0f, 3.0f);
    m_CubeEntity.m_GameEntityTag = "CubeEntity";
    //m_CubeEntity.m_DXResConfig.m_UID = "UID1234";
    m_CubeEntity.PhysicalMesh.SetIndicesList(GeometryFactory.GetIndicesList());
    m_CubeEntity.PhysicalMesh.SetStride(sizeof(SSimpleVertex));
    m_CubeEntity.PhysicalMesh.SetSimpleVerticesList(m_CubeEntity.GetVerticesList());

    XMFLOAT3 CubeEntityCenter{ 3, 0, 3 };
    XMFLOAT3 CubeEntityExtents{ 0.5f, 0.5f, 0.5f };

    // Original cube entity
    SCollisionComponent Collision = m_CubeEntity.GetCollisionComponent();
    Collision.AABox.Center = { 3, 0, 3 };
    Collision.AABox.Extents = { 0.5f, 0.5f, 0.5f };
    Collision.CollisionType = DISJOINT;
    m_CubeEntity.SetCollisionParams(CubeEntityCenter, CubeEntityExtents, DISJOINT);

    //Working
    //SScene.AddEntityToScene2(m_CubeEntity);
    //SScene.SetCollisionParams2(SceneLoc, CubeEntityCenter, CubeEntityExtents, DISJOINT);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/texture.vs", "VS", "vs_5_0", &pVSBlob);

    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The VS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the vertex shader
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_VertexShader);

    if (FAILED(m_HR))
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
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_VertexLayout);
    pVSBlob->Release();
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize input layout", L"Error", MB_OK);
        return;
    }


    m_CubeEntity.SetInputLayout(m_VertexLayout);
    //SScene.SetInputLayout2(SceneLoc, m_VertexLayout);


    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/texture.ps", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The PS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the pixel shader
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_PixelShader);
    pPSBlob->Release();
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to create pixel shader", L"Error", MB_OK);
        return;
    }


    m_CubeEntity.SetVertexShader(m_VertexShader);
    m_CubeEntity.SetPixelShader(m_PixelShader);
    //SScene.SetVertexShader2(SceneLoc, m_VertexShader);
    //SScene.SetPixelShader2(SceneLoc, m_PixelShader);

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData{};

    //std::vector<Simple_Vertex> TotalVerticesVector{ SScene.GetSceneList2().at(SceneLoc).GetVerticesList() };
    std::vector<SSimpleVertex> TotalVerticesVector{ m_CubeEntity.PhysicalMesh.GetSimpleVerticesList() };
    int TotalVerticesVectorSize = TotalVerticesVector.size();
    SSimpleVertex* VerticesArray = new SSimpleVertex[TotalVerticesVectorSize];
    for (int i = 0; i < TotalVerticesVectorSize; i++)
    {
        VerticesArray[i] = TotalVerticesVector.at(i);
    }
    //auto SimpleVerticesList = m_CubeEntity.PhysicalMesh.GetSimpleVerticesList();
    //Simple_Vertex* VerticesArray3 = SimpleVerticesList.data();
    InitData.pSysMem = VerticesArray;

    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_VertexBuffer);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize vertex buffer", L"Error", MB_OK);
        return;
    }

    // Set vertex buffer
    UINT stride = sizeof(SSimpleVertex);
    UINT offset = 0;

    m_CubeEntity.SetVertexBuffer(m_VertexBuffer);
    //SScene.SetVertexbuffer2(SceneLoc, m_VertexBuffer);

    //WORD* arr = IndicesVector.data();
    //WORD* Indices = m_CubeEntity.PhysicalMesh.GetIndicesList().data();
    //auto TempIndicesList = m_CubeEntity.PhysicalMesh.GetIndicesList();
    //WORD* Indices2 = TempIndicesList.data();
    //auto TempIndicesList3 = SScene.GetSceneList2().at(0).PhysicalMesh.GetIndicesList();
    //WORD* Indices3 = TempIndicesList3.data();
    auto TempIndicesList4 = m_CubeEntity.PhysicalMesh.GetIndicesList();
    WORD* Indices4 = TempIndicesList4.data();

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = Indices4;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_IndexBufferArray[0]);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize index buffer", L"Error", MB_OK);
        return;
    }

    m_CubeEntity.SetIndexBuffer(m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);
    //SScene.SetIndexbuffer2(SceneLoc, m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);

    // Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize constant buffer", L"Error", MB_OK);
        return;
    }

    m_CubeEntity.SetConstantBuffer(m_ConstantBuffer);
    //SScene.SetConstantBuffer2(SceneLoc, m_ConstantBuffer);
    //SScene.SetComponentConstantBuffer(SceneLoc, m_ConstantBuffer);

    //const wchar_t* TextureName = L"grey_grid.dds";
    const wchar_t* TextureName = L"tex_stickman.dds";
    m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureRV);
    if (FAILED(m_HR))
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
    m_HR = m_pD3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);

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

    m_HR = m_pD3D11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerState);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to create rasterizer state", L"Error", MB_OK);
        return;
    }

    SScene.AddEntityToScene(m_CubeEntity);

    m_pImmediateContext->RSSetState(m_RasterizerState);

    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.1f, 10.0f);
    m_WorldMatrix = XMMatrixIdentity();

    m_pImmediateContext->PSSetShaderResources(0, 1, &m_TextureRV);

    SceneLoc++;
}

HRESULT CDX11Device::AddLight()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;
    CGameEntity3D BaseCube = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::Cube);
    BaseCube.m_GameEntityTag = "TexturedCube2";
    BaseCube.SetLocationF(-3.0f, 0.0f, 0.0f);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"TextureShader.fxh", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the vertex shader
    ID3D11VertexShader* TempVertexShader{ nullptr };
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &TempVertexShader);
    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return m_HR;
    }
    BaseCube.SetVertexShader(TempVertexShader);
    auto VertexShaderLambda = [=]() {
        m_pImmediateContext->VSSetShader(TempVertexShader, nullptr, 0);
        };
    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    ID3D11InputLayout* TempVertexLayout{ nullptr };
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &TempVertexLayout);
    pVSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;

    // Set the input layout
    BaseCube.SetInputLayout(TempVertexLayout);
    m_pImmediateContext->IASetInputLayout(TempVertexLayout);

    auto InputLayoutLambda = [=]() {
        m_pImmediateContext->IASetInputLayout(TempVertexLayout);
        };

    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"TextureShader.fxh", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the pixel shader
    ID3D11PixelShader* TempPixelShader{ nullptr };
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &TempPixelShader);
    pPSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;
    BaseCube.SetPixelShader(m_PixelShader);
    auto PixelShaderLambda = [=]() {
        m_pImmediateContext->PSSetShader(TempPixelShader, nullptr, 0);
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

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }

        //       //vertex coordinates for the back face
              //{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },     // top left
              //{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },    // bottom left
              //{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },      // top right
              //{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },     // bottom right

              //// vertex coordinates for the front face
              //{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },      // top left
              //{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },     // bottom left
              //{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },       // top right
              //{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },      // bottom right

        //      { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },     // #0 back top left
        //      { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },      // #1 back top right
        //      { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },       // #2 front top right
        //      { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },      // #3 front top left
        //      { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },    // #4 back bottom left
        //      { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },     // #5 back bottom right
        //      { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },      // #6 front bottom right
              //{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },     // #7 front bottom left
    };

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleVertex) * 36;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    ID3D11Buffer* TempVertexBuffer{ nullptr };
    D3D11_SUBRESOURCE_DATA InitData{};
    InitData.pSysMem = vertices;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &TempVertexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set vertex buffer
    UINT stride = sizeof(SSimpleVertex);
    UINT offset = 0;
    BaseCube.SetVertexBuffer(TempVertexBuffer);
    m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);

    auto VertexBufferLambda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);
        };
    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);


    // Create index buffer
    WORD indices[] =
    {
        3,1,0,
        2,1,3,

        0,5,4,
        1,5,0,

        3,4,7,
        0,4,3,

        1,6,5,
        2,6,1,

        2,7,6,
        3,7,2,

        6,4,5,
        7,4,6,
    };

    ID3D11Buffer* TempIndexBuffer{ nullptr };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &TempIndexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set index buffer
    BaseCube.SetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    auto IndexBufferLambda = [=]() {
        m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        };
    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

    // Set primitive topology
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    ID3D11Buffer* TempConstantBuffer{ nullptr };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &TempConstantBuffer);
    if (FAILED(m_HR))
        return m_HR;
    BaseCube.SetConstantBuffer(TempConstantBuffer);
    auto ConstantBufferLambda = [=]() {
        m_pImmediateContext->VSSetConstantBuffers(0, 1, &TempConstantBuffer);
        };
    BaseCube.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);


    //const wchar_t* TextureName = L"grey_grid.dds";
    const wchar_t* TextureName = L"tex_stickman.dds";
    m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureRV);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
        return m_HR;
    }
    auto TextureLambda = [=]() {
        m_pImmediateContext->PSSetShaderResources(0, 1, &m_TextureRV);
        };
    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(TextureLambda);

    D3D11_SAMPLER_DESC sampDesc{};
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    m_HR = m_pD3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);
    if (FAILED(m_HR))
        return m_HR;

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

    //D3D11_SAMPLER_DESC sampDesc = {};
    //sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    //sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    //sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    //sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    //sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    //sampDesc.MinLOD = 0;
    //sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    //m_hr = m_D3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);
    if (FAILED(m_HR))
        return m_HR;

    m_HR = m_pD3D11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerState);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to create rasterizer state", L"Error", MB_OK);
        return m_HR;
    }

    m_pImmediateContext->RSSetState(m_RasterizerState);

    SScene.AddEntityToScene(BaseCube);

    //// Initialize the world matrix
    //g_World = XMMatrixIdentity();

    //// Initialize the view matrix
    //XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
    //XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    //XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    //g_View = XMMatrixLookAtLH(Eye, At, Up);

    // Initialize the projection matrix
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.01f, 100.0f);

    return S_OK;
}

class Terrain_Test : public CGameEntity3D {};

HRESULT CDX11Device::GenerateTerrain()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;
    CGameEntity3D BaseCube = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::Cube);
    BaseCube.m_GameEntityTag = "Terrain";
    BaseCube.SetLocationF(0.0f, -4.0f, 0.0f);
    BaseCube.SetScale(2.5f, 2.5f, 2.5f);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"TextureShader.fxh", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the vertex shader
    ID3D11VertexShader* TempVertexShader{ nullptr };
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &TempVertexShader);
    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return m_HR;
    }
    BaseCube.SetVertexShader(TempVertexShader);
    auto VertexShaderLambda = [=]() {
        m_pImmediateContext->VSSetShader(TempVertexShader, nullptr, 0);
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
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &TempVertexLayout);
    pVSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;

    // Set the input layout
    BaseCube.SetInputLayout(TempVertexLayout);
    m_pImmediateContext->IASetInputLayout(TempVertexLayout);

    auto InputLayoutLambda = [=]() {
        m_pImmediateContext->IASetInputLayout(TempVertexLayout);
        };

    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"TextureShader.fxh", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the pixel shader
    ID3D11PixelShader* TempPixelShader{ nullptr };
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &TempPixelShader);
    pPSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;
    BaseCube.SetPixelShader(m_PixelShader);
    auto PixelShaderLambda = [=]() {
        m_pImmediateContext->PSSetShader(TempPixelShader, nullptr, 0);
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
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &TempVertexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set vertex buffer
    UINT stride = sizeof(SSimpleVertex);
    UINT offset = 0;
    BaseCube.SetVertexBuffer(TempVertexBuffer);
    m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);

    auto VertexBufferLambda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);
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
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &TempIndexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set index buffer
    BaseCube.SetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    auto IndexBufferLambda = [=]() {
        m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        };

    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

    // Set primitive topology
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    ID3D11Buffer* TempConstantBuffer{ nullptr };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &TempConstantBuffer);
    if (FAILED(m_HR))
        return m_HR;
    BaseCube.SetConstantBuffer(TempConstantBuffer);
    auto ConstantBufferLambda = [=]()
        {
            m_pImmediateContext->VSSetConstantBuffers(0, 1, &TempConstantBuffer);
        };
    BaseCube.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);


    //const wchar_t* TextureName = L"grey_grid.dds";
    const wchar_t* TextureName = L"tex_stickman.dds";
    m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureRV);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
        return m_HR;
    }
    auto TextureLambda = [=]() {
        m_pImmediateContext->PSSetShaderResources(0, 1, &m_TextureRV);
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
    m_HR = m_pD3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);
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
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.01f, 100.0f);

    return S_OK;

    //Terrain_Test Terrain;

    //Scene_& Scene = Scene_::GetScene();
    //Primitive_Geometry_Factory GeometryFactory;
    //HRESULT HR{};
    //DXResourcesConfig DefaultDXResourceConfig;

    //// Init resources for Cube Component Entity
    ////Terrain = static_cast<Terrain_>(GeometryFactory.CreateEntity3D(Primitive_Geometry_Type::Cube));
    //Terrain.SetLocationF(0.0f, -4.0f, 0.0f);
    ////m_CubeEntity.SetScale(0.1f, 0.1f, 0.1f);
    //Terrain.m_GameEntityTag = "Terrain";
    ////mTerrain.m_DXResConfig.m_UID = "UID1234";
    //Terrain.PhysicalMesh.SetIndicesList(GeometryFactory.GetIndicesList());
    //auto CubeEntityIndicesList = Terrain.PhysicalMesh.GetIndicesList();
    //auto CubeEntityIndicesList2 = GeometryFactory.GetIndicesList();
    //Terrain.PhysicalMesh.SetStride(sizeof(Simple_Vertex));
    //Terrain.PhysicalMesh.SetSimpleVerticesList(Terrain.GetVerticesList());

    //XMFLOAT3 CubeEntityCenter{ 3, 0, 0 };
    //XMFLOAT3 CubeEntityExtents{ 0.5f, 0.5f, 0.5f };

    //CollisionComponent Collision = Terrain.GetCollisionComponent();
    //Collision.AABox.Center = { 3, 0, 0 };
    //Collision.AABox.Extents = { 0.5f, 0.5f, 0.5f };
    //Collision.CollisionType = DISJOINT;
    //Terrain.SetCollisionParams(CubeEntityCenter, CubeEntityExtents, DISJOINT);

    //// Cube Entity Component 1
    //Game_Entity_3D_Component TerrainComponent = GeometryFactory.CreateEntity3DComponent(Primitive_Geometry_Type::Plane);
    //TerrainComponent.DXResourceHashKey = 1;

    //TerrainComponent.SetLocationF(0.0f, -2.0f, 3.0f);
    ////TerrainComponent.SetScale(0.1f, 0.1f, 0.1f);
    //TerrainComponent.m_GameEntityTag = "Cube Component1";
    ////TerrainComponent.m_DXResConfig.m_UID = "UID1234";
    //TerrainComponent.PhysicalMesh.SetIndicesList(GeometryFactory.GetIndicesList());
    //TerrainComponent.PhysicalMesh.SetStride(sizeof(Simple_Vertex));
    //TerrainComponent.PhysicalMesh.SetSimpleVerticesList(TerrainComponent.GetVerticesList());

    //CollisionComponent Collision2 = TerrainComponent.GetCollisionComponent();
    //Collision2.AABox.Center = { 3, 0, 0 };
    //Collision2.AABox.Extents = { 0.5f, 0.5f, 0.5f };
    //Collision2.CollisionType = DISJOINT;
    //TerrainComponent.SetCollisionParams(CubeEntityCenter, CubeEntityExtents, DISJOINT);

    ////!!
    ////
    ////m_CubeEntity.m_GameEntity3DComponent.push_back(TerrainComponent);
    ////m_CubeEntity.m_GameEntity3DComponent.push_back(CubeComponent2);
    ////SScene.AddEntityToScene2(m_CubeEntity);

    //// Working
    ////SScene.SetCollisionParams2(SceneLoc, CubeEntityCenter, CubeEntityExtents, DISJOINT);

    //// Compile the vertex shader
    //ID3D11VertexShader* VertexShader{ nullptr };
    //ID3DBlob* pVSBlob = nullptr;
    //HR = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/texture.vs", "VS", "vs_5_0", &pVSBlob);

    //if (FAILED(HR))
    //{
    //    MessageBox(nullptr, L"The VS file cannot be compiled.", L"Error", MB_OK);
    //    return;
    //}

    //// Create the vertex shader
    //HR = m_D3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &VertexShader);

    //if (FAILED(HR))
    //{
    //    pVSBlob->Release();
    //    return;
    //}

    //TerrainComponent.m_DXResConfig.SetVertexShader(VertexShader);
    //Terrain.m_DXResConfig.SetVertexShader(VertexShader);
    //DefaultDXResourceConfig.SetVertexShader(VertexShader);

    //auto VertexShaderLambda = [=]() mutable {
    //    //assert(m_pDX11Device);
    //    m_ImmediateContext->VSSetShader(VertexShader, nullptr, 0);
    //    };
    //Terrain.m_DXResConfig.ContextResourcePtr.push_back(VertexShaderLambda);

    //// Define the input layout
    //D3D11_INPUT_ELEMENT_DESC layout[] =
    //{
    //    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //};
    //UINT numElements = ARRAYSIZE(layout);

    //// Create the input layout

    //ID3D11InputLayout* VertexLayout{ nullptr };
    //HR = m_D3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &VertexLayout);
    //pVSBlob->Release();

    //if (FAILED(HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize input layout", L"Error", MB_OK);
    //    return;
    //}

    //TerrainComponent.m_DXResConfig.SetInputLayout(VertexLayout);
    //Terrain.m_DXResConfig.SetInputLayout(VertexLayout);
    //DefaultDXResourceConfig.SetInputLayout(VertexLayout);

    //auto InputLayoutLambda = [=]() mutable {
    //    m_ImmediateContext->IASetInputLayout(VertexLayout);
    //    };
    //Terrain.m_DXResConfig.ContextResourcePtr.push_back(InputLayoutLambda);

    //// Compile the pixel shader

    //ID3D11PixelShader* PixelShader{ nullptr };
    //ID3DBlob* pPSBlob = nullptr;
    ////hr = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_4_0", &pPSBlob);
    //HR = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/texture.ps", "PS", "ps_5_0", &pPSBlob);
    //if (FAILED(HR))
    //{
    //    MessageBox(nullptr, L"The PS file cannot be compiled.", L"Error", MB_OK);
    //    return;
    //}

    //// Create the pixel shader
    //HR = m_D3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &PixelShader);
    //pPSBlob->Release();
    //if (FAILED(HR))
    //{
    //    MessageBox(nullptr, L"Failed to create pixel shader", L"Error", MB_OK);
    //    return;
    //}

    //TerrainComponent.m_DXResConfig.SetPixelShader(PixelShader);
    //Terrain.m_DXResConfig.SetPixelShader(PixelShader);
    //DefaultDXResourceConfig.SetPixelShader(PixelShader);

    //auto PixelShaderLambda = [=]() mutable {
    //    m_ImmediateContext->PSSetShader(PixelShader, nullptr, 0);
    //};
    //Terrain.m_DXResConfig.ContextResourcePtr.push_back(PixelShaderLambda);

    //// Create the vertex buffer

    //ID3D11Buffer* VertexBuffer{};
    //D3D11_BUFFER_DESC bd{};
    //bd.Usage = D3D11_USAGE_DEFAULT;
    //bd.ByteWidth = sizeof(Simple_Vertex) * 100;
    //bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    //bd.CPUAccessFlags = 0;

    //D3D11_SUBRESOURCE_DATA InitData{};

    //std::vector<Simple_Vertex> TotalVerticesVectorFinal{ Terrain.PhysicalMesh.GetSimpleVerticesList() };

    ////std::vector<Simple_Vertex> TotalVerticesVector{ SScene.GetSceneList2().at(SceneLoc).GetVerticesList() };
    //int TotalVerticesVectorSize = TotalVerticesVectorFinal.size();
    //Simple_Vertex* VerticesArray = new Simple_Vertex[TotalVerticesVectorSize];
    //for (int i = 0; i < TotalVerticesVectorSize; i++)
    //{
    //    VerticesArray[i] = TotalVerticesVectorFinal.at(i);
    //}

    //// Dangling pointer
    ////Simple_Vertex* VerticesArray2 = SScene.GetSceneList2().at(0).GetVerticesList().data();

    //// { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

    //Simple_Vertex VerticesArrayOG[] =
    //{
    //    { XMFLOAT3(-5.5f, -0.5f, -5.5f), XMFLOAT2(1.0f, 0.0f) },    // bottom left
    //    { XMFLOAT3(5.5f, -0.5f, -5.5f), XMFLOAT2(0.0f, 0.0f) },     // bottom right
    //    { XMFLOAT3(5.5f, -0.5f, 5.5f), XMFLOAT2(0.0f, 1.0f) },      // top right
    //    { XMFLOAT3(-5.5f, -0.5f, 5.5f), XMFLOAT2(1.0f, 1.0f) },     // top left
    //};

    //InitData.pSysMem = VerticesArrayOG;

    //HR = m_D3D11Device->CreateBuffer(&bd, &InitData, &VertexBuffer);
    //if (FAILED(HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize vertex buffer", L"Error", MB_OK);
    //    return;
    //}

    //// Set vertex buffer
    //UINT stride = sizeof(Simple_Vertex);
    //UINT offset = 0;

    //TerrainComponent.m_DXResConfig.SetVertexBuffer(VertexBuffer);
    //Terrain.m_DXResConfig.SetVertexBuffer(VertexBuffer);
    //TerrainComponent.SetVertexBuffer(VertexBuffer);
    //TerrainComponent.SetOwnVertexBuffer(VertexBuffer);
    //DefaultDXResourceConfig.SetVertexBuffer(VertexBuffer);

    //auto VertexBufferLambda = [=]() mutable {
    //    m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
    //};
    //Terrain.m_DXResConfig.ContextResourcePtr.push_back(VertexBufferLambda);

    //// Create the index buffer

    //ID3D11Buffer* IndexBuffer{};
    ////WORD* arr = IndicesVector.data();
    //WORD* Indices = Terrain.PhysicalMesh.GetIndicesList().data();
    //auto TempIndicesList = Terrain.PhysicalMesh.GetIndicesList();
    //WORD* Indices2 = TempIndicesList.data();
    ////auto TempIndicesList3 = SScene.GetSceneList2().at(0).PhysicalMesh.GetIndicesList();
    ////WORD* Indices3 = TempIndicesList3.data();

    //WORD* TempIndicesList4 = CubeEntityIndicesList.data();

    //WORD indicesVector[] = {
    //    0, 1, 2,
    //    3, 0, 2
    //};

    ////WORD indicesVector[] = {
    ////        3,1,0,
    ////        2,1,3,

    ////        6,4,5,
    ////        7,4,6,

    ////        11,9,8,
    ////        10,9,11,

    ////        14,12,13,
    ////        15,12,14,

    ////        19,17,16,
    ////        18,17,19,

    ////        22,20,21,
    ////        23,20,22
    ////};

    //bd.Usage = D3D11_USAGE_DEFAULT;
    //bd.ByteWidth = sizeof(WORD) * 100;        // 36 vertices needed for 12 triangles in a triangle list
    //bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    //bd.CPUAccessFlags = 0;
    //InitData.pSysMem = indicesVector;
    //HR = m_D3D11Device->CreateBuffer(&bd, &InitData, &IndexBuffer);

    //if (FAILED(HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize index buffer", L"Error", MB_OK);
    //    return;
    //}

    //TerrainComponent.m_DXResConfig.SetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    //Terrain.m_DXResConfig.SetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    //TerrainComponent.SetIndexOwnBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    //DefaultDXResourceConfig.SetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    //auto IndexBufferLambda = [=]() mutable {
    //    m_ImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    //    };
    //Terrain.m_DXResConfig.ContextResourcePtr.push_back(IndexBufferLambda);

    //// Create the constant buffer
    //ID3D11Buffer* mConstantBuffer{ };
    //bd.Usage = D3D11_USAGE_DEFAULT;
    //bd.ByteWidth = sizeof(Constant_Buffer);
    //bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //bd.CPUAccessFlags = 0;
    //HR = m_D3D11Device->CreateBuffer(&bd, nullptr, &mConstantBuffer);

    //if (FAILED(HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize constant buffer", L"Error", MB_OK);
    //    return;
    //}

    //TerrainComponent.m_DXResConfig.SetConstantBuffer(mConstantBuffer);
    //Terrain.m_DXResConfig.SetConstantBuffer(mConstantBuffer);
    //DefaultDXResourceConfig.SetConstantBuffer(mConstantBuffer);

    //auto ConstantBufferLambda = [=]() mutable {
    //    m_ImmediateContext->VSSetConstantBuffers(0, 1, &mConstantBuffer);
    //};
    //Terrain.m_DXResConfig.ContextResourcePtr.push_back(ConstantBufferLambda);

    //// Create the texture resource view

    //ID3D11ShaderResourceView* TextureRV{ nullptr };
    //const wchar_t* TextureName = L"grey_grid.dds";
    //HR = CreateDDSTextureFromFile(m_D3D11Device, TextureName, nullptr, &TextureRV);
    //if (FAILED(HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
    //    return;
    //}

    //DefaultDXResourceConfig.SetTextureResourceView(TextureRV);

    //Terrain.m_GameEntity3DComponent.push_back(TerrainComponent);

    //auto TextureLambda = [=]() mutable {
    //    m_ImmediateContext->PSSetShaderResources(0, 1, &TextureRV);
    //};
    //Terrain.m_DXResConfig.ContextResourcePtr.push_back(TextureLambda);
    //Terrain.m_DXResConfig.SetTextureResourceView(TextureRV);



    //Scene.InsertDXResourceConfig(1, DefaultDXResourceConfig);
    //Scene.AddEntityToScene(Terrain);


}



// Working
//void DX11_Device::InitCubeComponents()
//{
//    Scene& SScene = Scene::GetScene();
//    PrimitiveGeometryFactory GeometryFactory;
//
//    // Original cube entity
//    m_CubeEntity = GeometryFactory.CreateEntity3D(PrimitiveGeometryType::Cube);
//
//    m_CubeEntity.SetLocationF(3.0f, 0.0f, 0.0f);
//    //m_CubeEntity.SetScale(0.1f, 0.1f, 0.1f);
//    m_CubeEntity.m_GameEntityTag = "CubeEntity";
//    m_CubeEntity.m_DXResConfig.m_UID = "UID1234";
//    m_CubeEntity.PhysicalMesh.SetIndicesList(GeometryFactory.GetIndicesList());
//    auto CubeEntityIndicesList = m_CubeEntity.PhysicalMesh.GetIndicesList();
//    auto CubeEntityIndicesList2 = GeometryFactory.GetIndicesList();
//    m_CubeEntity.PhysicalMesh.SetStride(sizeof(Simple_Vertex));
//
//    XMFLOAT3 CubeEntityCenter{ 3, 0, 0 };
//    XMFLOAT3 CubeEntityExtents{ 0.5f, 0.5f, 0.5f };
//
//    CollisionComponent Collision = m_CubeEntity.GetCollisionComponent();
//    Collision.AABox.Center = { 3, 0, 0 };
//    Collision.AABox.Extents = { 0.5f, 0.5f, 0.5f };
//    Collision.CollisionType = DISJOINT;
//    m_CubeEntity.SetCollisionParams(CubeEntityCenter, CubeEntityExtents, DISJOINT);
//
//    // Cube Entity Component 1
//    GameEntity3DComponent CubeComponent1 = GeometryFactory.CreateEntity3DComponent(PrimitiveGeometryType::Cube);
//
//    CubeComponent1.SetLocationF(3.0f, 0.0f, 0.0f);
//    CubeComponent1.SetScale(0.1f, 0.1f, 0.1f);
//    CubeComponent1.m_GameEntityTag = "Cube Component1";
//    CubeComponent1.m_DXResConfig.m_UID = "UID1234";
//    CubeComponent1.PhysicalMesh.SetIndicesList(GeometryFactory.GetIndicesList());
//    CubeComponent1.PhysicalMesh.SetStride(sizeof(Simple_Vertex));
//
//    CollisionComponent Collision2 = CubeComponent1.GetCollisionComponent();
//    Collision2.AABox.Center = { 3, 0, 0 };
//    Collision2.AABox.Extents = { 0.5f, 0.5f, 0.5f };
//    Collision2.CollisionType = DISJOINT;
//    CubeComponent1.SetCollisionParams(CubeEntityCenter, CubeEntityExtents, DISJOINT);
//
//
//    // Cube Entity Component 2
//    GameEntity3DComponent CubeComponent2 = GeometryFactory.CreateEntity3DComponent(PrimitiveGeometryType::Cube);
//
//    CubeComponent2.SetLocationF(0, 0, 3.0f);
//    //CubeComponent2.SetScale(0.25f, 0.25f, 0.25f);
//    CubeComponent2.m_GameEntityTag = "Cube Component2";
//    CubeComponent2.m_DXResConfig.m_UID = "UID1234";
//    CubeComponent2.PhysicalMesh.SetIndicesList(GeometryFactory.GetIndicesList());
//    CubeComponent2.PhysicalMesh.SetStride(sizeof(Simple_Vertex));
//
//    //!!
//    //
//    //m_CubeEntity.m_GameEntity3DComponent.push_back(CubeComponent1);
//    //m_CubeEntity.m_GameEntity3DComponent.push_back(CubeComponent2);
//    //SScene.AddEntityToScene2(m_CubeEntity);
//
//    // Working
//    //SScene.SetCollisionParams2(SceneLoc, CubeEntityCenter, CubeEntityExtents, DISJOINT);
//
//    // Compile the vertex shader
//    ID3DBlob* pVSBlob = nullptr;
//    m_hr = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/texture.vs", "VS", "vs_5_0", &pVSBlob);
//
//    if (FAILED(m_hr))
//    {
//        MessageBox(nullptr,
//            L"The VS file cannot be compiled.", L"Error", MB_OK);
//        return;
//    }
//
//    // Create the vertex shader
//    m_hr = m_D3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_VertexShader);
//
//    if (FAILED(m_hr))
//    {
//        pVSBlob->Release();
//        return;
//    }
//
//    // Define the input layout
//    D3D11_INPUT_ELEMENT_DESC layout[] =
//    {
//        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//    };
//    UINT numElements = ARRAYSIZE(layout);
//
//    // Create the input layout
//    m_hr = m_D3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_VertexLayout);
//    pVSBlob->Release();
//    if (FAILED(m_hr))
//    {
//        MessageBox(nullptr, L"Failed to initialize input layout", L"Error", MB_OK);
//        return;
//    }
//
//    //SScene.SetInputLayout2(SceneLoc, m_VertexLayout);
//    CubeComponent1.m_DXResConfig.SetInputLayout(m_VertexLayout);
//    CubeComponent2.m_DXResConfig.SetInputLayout(m_VertexLayout);
//
//    //SScene.SetComponentInputLayout(SceneLoc, 0, m_VertexLayout);
//    //SScene.SetComponentInputLayout(SceneLoc, 1, m_VertexLayout);
//
//    // Compile the pixel shader
//    ID3DBlob* pPSBlob = nullptr;
//    //hr = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_4_0", &pPSBlob);
//    m_hr = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/texture.ps", "PS", "ps_5_0", &pPSBlob);
//    if (FAILED(m_hr))
//    {
//        MessageBox(nullptr, L"The PS file cannot be compiled.", L"Error", MB_OK);
//        return;
//    }
//
//    // Create the pixel shader
//    m_hr = m_D3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_PixelShader);
//    pPSBlob->Release();
//    if (FAILED(m_hr))
//    {
//        MessageBox(nullptr, L"Failed to create pixel shader", L"Error", MB_OK);
//        return;
//    }
//
//
//
//    //SScene.SetVertexShader2(SceneLoc, m_VertexShader);
//    //SScene.SetPixelShader2(SceneLoc, m_PixelShader);
//
//    CubeComponent1.m_DXResConfig.SetVertexShader(m_VertexShader);
//    CubeComponent1.m_DXResConfig.SetVertexShader(m_VertexShader);
//    CubeComponent2.m_DXResConfig.SetPixelShader(m_PixelShader);
//    CubeComponent2.m_DXResConfig.SetPixelShader(m_PixelShader);
//
//    //SScene.SetComponentVertexShader(SceneLoc, 0, m_VertexShader);
//    //SScene.SetComponentPixelShader(SceneLoc, 0, m_PixelShader);
//    //SScene.SetComponentVertexShader(SceneLoc, 1, m_VertexShader);
//    //SScene.SetComponentPixelShader(SceneLoc, 1, m_PixelShader);
//
//    D3D11_BUFFER_DESC bd{};
//    bd.Usage = D3D11_USAGE_DEFAULT;
//    bd.ByteWidth = sizeof(Simple_Vertex) * 24;
//    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//    bd.CPUAccessFlags = 0;
//
//    D3D11_SUBRESOURCE_DATA InitData{};
//
//    std::vector<Simple_Vertex> TotalVerticesVectorFinal{ m_CubeEntity.PhysicalMesh.GetSimpleVerticesList() };
//
//    //std::vector<Simple_Vertex> TotalVerticesVector{ SScene.GetSceneList2().at(SceneLoc).GetVerticesList() };
//    int TotalVerticesVectorSize = TotalVerticesVectorFinal.size();
//    Simple_Vertex* VerticesArray = new Simple_Vertex[TotalVerticesVectorSize];
//    for (int i = 0; i < TotalVerticesVectorSize; i++)
//    {
//        VerticesArray[i] = TotalVerticesVectorFinal.at(i);
//    }
//
//    // Dangling pointer
//    //Simple_Vertex* VerticesArray2 = SScene.GetSceneList2().at(0).GetVerticesList().data();
//
//    InitData.pSysMem = VerticesArray;
//
//    m_hr = m_D3D11Device->CreateBuffer(&bd, &InitData, &m_VertexBuffer);
//    if (FAILED(m_hr))
//    {
//        MessageBox(nullptr, L"Failed to initialize vertex buffer", L"Error", MB_OK);
//        return;
//    }
//
//    // Set vertex buffer
//    UINT stride = sizeof(Simple_Vertex);
//    UINT offset = 0;
//
//
//
//    CubeComponent1.m_DXResConfig.SetVertexBuffer(m_VertexBuffer);
//    CubeComponent2.m_DXResConfig.SetVertexBuffer(m_VertexBuffer);
//    //SScene.SetComponentVertexbuffer(SceneLoc, 0, m_VertexBuffer);
//    //SScene.SetComponentVertexbuffer(SceneLoc, 1, m_VertexBuffer);
//
//    //WORD* arr = IndicesVector.data();
//    WORD* Indices = m_CubeEntity.PhysicalMesh.GetIndicesList().data();
//    auto TempIndicesList = m_CubeEntity.PhysicalMesh.GetIndicesList();
//    WORD* Indices2 = TempIndicesList.data();
//    //auto TempIndicesList3 = SScene.GetSceneList2().at(0).PhysicalMesh.GetIndicesList();
//    //WORD* Indices3 = TempIndicesList3.data();
//
//    WORD* TempIndicesList4 = CubeEntityIndicesList.data();
//
//    WORD indicesVector[] = {
//            3,1,0,
//            2,1,3,
//
//            6,4,5,
//            7,4,6,
//
//            11,9,8,
//            10,9,11,
//
//            14,12,13,
//            15,12,14,
//
//            19,17,16,
//            18,17,19,
//
//            22,20,21,
//            23,20,22
//    };
//
//    bd.Usage = D3D11_USAGE_DEFAULT;
//    bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
//    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//    bd.CPUAccessFlags = 0;
//    InitData.pSysMem = indicesVector;
//    m_hr = m_D3D11Device->CreateBuffer(&bd, &InitData, &m_IndexBufferArray[0]);
//    if (FAILED(m_hr))
//    {
//        MessageBox(nullptr, L"Failed to initialize index buffer", L"Error", MB_OK);
//        return;
//    }
//
//    CubeComponent1.m_DXResConfig.SetIndexBuffer(m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);
//    CubeComponent2.m_DXResConfig.SetIndexBuffer(m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);
//    //SScene.SetComponentIndexbuffer(SceneLoc, 0, m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);
//    //SScene.SetComponentIndexbuffer(SceneLoc, 1, m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);
//
//    // Create the constant buffer
//    bd.Usage = D3D11_USAGE_DEFAULT;
//    bd.ByteWidth = sizeof(ConstantBuffer);
//    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//    bd.CPUAccessFlags = 0;
//    m_hr = m_D3D11Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer);
//    if (FAILED(m_hr))
//    {
//        MessageBox(nullptr, L"Failed to initialize constant buffer", L"Error", MB_OK);
//        return;
//    }
//
//    CubeComponent1.m_DXResConfig.SetConstantBuffer(m_ConstantBuffer);
//    CubeComponent2.m_DXResConfig.SetConstantBuffer(m_ConstantBuffer);
//    //SScene.SetConstantBuffer2(SceneLoc, m_ConstantBuffer);
//    //SScene.SetComponentConstantBuffer(SceneLoc, 0, m_ConstantBuffer);
//    //SScene.SetComponentConstantBuffer(SceneLoc, 1, m_ConstantBuffer);
//
//    const wchar_t* TextureName = L"grey_grid.dds";
//    m_hr = CreateDDSTextureFromFile(m_D3D11Device, TextureName, nullptr, &m_TextureRV);
//    if (FAILED(m_hr))
//    {
//        MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
//        return;
//    }
//
//    D3D11_SAMPLER_DESC sampDesc{};
//    ZeroMemory(&sampDesc, sizeof(sampDesc));
//    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
//    sampDesc.MinLOD = 0;
//    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
//    m_hr = m_D3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);
//
//    D3D11_RASTERIZER_DESC rasterDesc = {};
//    rasterDesc.FillMode = D3D11_FILL_SOLID;
//    rasterDesc.CullMode = D3D11_CULL_NONE;
//    rasterDesc.FrontCounterClockwise = false;
//    rasterDesc.DepthBias = 0;
//    rasterDesc.DepthBiasClamp = 0.0f;
//    rasterDesc.SlopeScaledDepthBias = 0.0f;
//    rasterDesc.DepthClipEnable = true;
//    rasterDesc.ScissorEnable = false;
//    rasterDesc.MultisampleEnable = false;
//    rasterDesc.AntialiasedLineEnable = false;
//
//    m_hr = m_D3D11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerState);
//    if (FAILED(m_hr))
//    {
//        MessageBox(nullptr, L"Failed to create rasterizer state", L"Error", MB_OK);
//        return;
//    }
//
//    m_ImmediateContext->RSSetState(m_RasterizerState);
//
//    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.1f, 10.0f);
//    m_WorldMatrix = XMMatrixIdentity();
//
//    m_ImmediateContext->PSSetShaderResources(0, 1, &m_TextureRV);
//
//    m_CubeEntity.m_GameEntity3DComponent.push_back(CubeComponent1);
//    m_CubeEntity.m_GameEntity3DComponent.push_back(CubeComponent2);
//    SScene.AddEntityToScene2(m_CubeEntity);
//
//    SceneLoc++;
//}

void CDX11Device::InitSingleCubeComponent()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;
    CDXResourcesConfig DefaultDXResourceConfig;

    m_CubeEntity2 = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::Cube);

    m_CubeEntity2.SetLocationF(3.0f, 0.0f, 0.0f);
    //m_CubeEntity.SetScale(0.1f, 0.1f, 0.1f);
    m_CubeEntity2.m_GameEntityTag = "SingleCubeComponentEntity";
    //m_CubeEntity2.m_DXResConfig.m_UID = "UID1234";
    m_CubeEntity2.PhysicalMesh.SetIndicesList(GeometryFactory.GetIndicesList());
    auto CubeEntityIndicesList = m_CubeEntity2.PhysicalMesh.GetIndicesList();
    auto CubeEntityIndicesList2 = GeometryFactory.GetIndicesList();
    m_CubeEntity2.PhysicalMesh.SetStride(sizeof(SSimpleVertex));
    m_CubeEntity2.PhysicalMesh.SetSimpleVerticesList(m_CubeEntity2.GetVerticesList());

    XMFLOAT3 CubeEntityCenter{ 3, 0, 0 };
    XMFLOAT3 CubeEntityExtents{ 0.5f, 0.5f, 0.5f };

    SCollisionComponent Collision = m_CubeEntity2.GetCollisionComponent();
    Collision.AABox.Center = { 3, 0, 0 };
    Collision.AABox.Extents = { 0.5f, 0.5f, 0.5f };
    Collision.CollisionType = DISJOINT;
    m_CubeEntity2.SetCollisionParams(CubeEntityCenter, CubeEntityExtents, DISJOINT);

    // Cube Entity Component 1
    CGameEntity3DComponent CubeComponent1 = GeometryFactory.CreateEntity3DComponent(EPrimitiveGeometryType::Cube);

    CubeComponent1.DXResourceHashKey = 3;
    CubeComponent1.SetLocationF(3.0f, 0.0f, 0.0f);
    //CubeComponent1.SetScale(0.1f, 0.1f, 0.1f);
    CubeComponent1.m_GameEntityTag = "SingleCubeComponent";
    //CubeComponent1.m_DXResConfig.m_UID = "UID1234";
    CubeComponent1.PhysicalMesh.SetIndicesList(GeometryFactory.GetIndicesList());
    CubeComponent1.PhysicalMesh.SetStride(sizeof(SSimpleVertex));
    CubeComponent1.PhysicalMesh.SetSimpleVerticesList(CubeComponent1.GetVerticesList());

    SCollisionComponent Collision2 = CubeComponent1.GetCollisionComponent();
    Collision2.AABox.Center = { 3, 0, 0 };
    Collision2.AABox.Extents = { 0.5f, 0.5f, 0.5f };
    Collision2.CollisionType = DISJOINT;
    CubeComponent1.SetCollisionParams(CubeEntityCenter, CubeEntityExtents, DISJOINT);

    //!!
    //
    //m_CubeEntity.m_GameEntity3DComponent.push_back(CubeComponent1);
    //m_CubeEntity.m_GameEntity3DComponent.push_back(CubeComponent2);
    //SScene.AddEntityToScene2(m_CubeEntity);

    // Working
    //SScene.SetCollisionParams2(SceneLoc, CubeEntityCenter, CubeEntityExtents, DISJOINT);

    // Compile the vertex shader
    ID3D11VertexShader* TempVertexShader{ nullptr };
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/texture.vs", "VS", "vs_5_0", &pVSBlob);

    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The VS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the vertex shader
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &TempVertexShader);

    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return;
    }

    // Define the input layout
    ID3D11InputLayout* TempVertexLayout{ nullptr };
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &TempVertexLayout);

    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize input layout", L"Error", MB_OK);
        return;
    }

    //SScene.SetInputLayout2(SceneLoc, m_VertexLayout);
    CubeComponent1.m_DXResConfig.SetInputLayout(TempVertexLayout);
    DefaultDXResourceConfig.SetInputLayout(TempVertexLayout);
    //SScene.SetComponentInputLayout(SceneLoc, 0, m_VertexLayout);
    //SScene.SetComponentInputLayout(SceneLoc, 1, m_VertexLayout);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    //hr = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_4_0", &pPSBlob);
    m_HR = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/texture.ps", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The PS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the pixel shader
    ID3D11PixelShader* TempPixelShader{ nullptr };
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &TempPixelShader);
    pPSBlob->Release();

    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to create pixel shader", L"Error", MB_OK);
        return;
    }

    //SScene.SetVertexShader2(SceneLoc, m_VertexShader);
    //SScene.SetPixelShader2(SceneLoc, m_PixelShader);


    CubeComponent1.m_DXResConfig.SetVertexShader(TempVertexShader);
    CubeComponent1.m_DXResConfig.SetPixelShader(TempPixelShader);

    //SScene.SetComponentVertexShader(SceneLoc, 0, m_VertexShader);
    //SScene.SetComponentPixelShader(SceneLoc, 0, m_PixelShader);
    //SScene.SetComponentVertexShader(SceneLoc, 1, m_VertexShader);
    //SScene.SetComponentPixelShader(SceneLoc, 1, m_PixelShader);

    ID3D11Buffer* TempVertexBuffer{ nullptr };
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData{};

    std::vector<SSimpleVertex> TotalVerticesVectorFinal{ m_CubeEntity2.PhysicalMesh.GetSimpleVerticesList() };

    //std::vector<Simple_Vertex> TotalVerticesVector{ SScene.GetSceneList2().at(SceneLoc).GetVerticesList() };
    int TotalVerticesVectorSize = TotalVerticesVectorFinal.size();
    SSimpleVertex* VerticesArray = new SSimpleVertex[TotalVerticesVectorSize];
    for (int i = 0; i < TotalVerticesVectorSize; i++)
    {
        VerticesArray[i] = TotalVerticesVectorFinal.at(i);
    }

    // Dangling pointer
    //Simple_Vertex* VerticesArray2 = SScene.GetSceneList2().at(0).GetVerticesList().data();

    InitData.pSysMem = VerticesArray;

    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &TempVertexBuffer);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize vertex buffer", L"Error", MB_OK);
        return;
    }

    // Set vertex buffer
    UINT stride = sizeof(SSimpleVertex);
    UINT offset = 0;



    CubeComponent1.m_DXResConfig.SetVertexBuffer(TempVertexBuffer);
    CubeComponent1.SetOwnVertexBuffer(TempVertexBuffer);
    DefaultDXResourceConfig.SetVertexBuffer(TempVertexBuffer);
    //SScene.SetComponentVertexbuffer(SceneLoc, 0, m_VertexBuffer);
    //SScene.SetComponentVertexbuffer(SceneLoc, 1, m_VertexBuffer);

    //WORD* arr = IndicesVector.data();
    WORD* Indices = m_CubeEntity2.PhysicalMesh.GetIndicesList().data();
    auto TempIndicesList = m_CubeEntity2.PhysicalMesh.GetIndicesList();
    WORD* Indices2 = TempIndicesList.data();
    //auto TempIndicesList3 = SScene.GetSceneList2().at(0).PhysicalMesh.GetIndicesList();
    //WORD* Indices3 = TempIndicesList3.data();

    WORD* TempIndicesList4 = CubeEntityIndicesList.data();

    WORD indicesVector[] = {
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
    InitData.pSysMem = indicesVector;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_IndexBufferArray[0]);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize index buffer", L"Error", MB_OK);
        return;
    }

    CubeComponent1.m_DXResConfig.SetIndexBuffer(m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);
    //SScene.SetComponentIndexbuffer(SceneLoc, 0, m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);
    //SScene.SetComponentIndexbuffer(SceneLoc, 1, m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);

    // Create the constant buffer
    ID3D11Buffer* TempConstantBuffer{ nullptr };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &TempConstantBuffer);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize constant buffer", L"Error", MB_OK);
        return;
    }

    CubeComponent1.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    //SScene.SetConstantBuffer2(SceneLoc, m_ConstantBuffer);
    //SScene.SetComponentConstantBuffer(SceneLoc, 0, m_ConstantBuffer);
    //SScene.SetComponentConstantBuffer(SceneLoc, 1, m_ConstantBuffer);

    //const wchar_t* TextureName = L"grey_grid.dds";
    const wchar_t* TextureName = L"tex_stickman.dds";
    m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureRV);
    if (FAILED(m_HR))
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
    m_HR = m_pD3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);

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

    m_HR = m_pD3D11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerState);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to create rasterizer state", L"Error", MB_OK);
        return;
    }

    m_pImmediateContext->RSSetState(m_RasterizerState);

    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.1f, 10.0f);
    m_WorldMatrix = XMMatrixIdentity();

    m_pImmediateContext->PSSetShaderResources(0, 1, &m_TextureRV);

    m_CubeEntity2.m_GameEntity3DComponent.push_back(CubeComponent1);
    SScene.InsertDXResourceConfig(3, DefaultDXResourceConfig);
    SScene.AddEntityToScene(m_CubeEntity2);

    SceneLoc++;
}

void CDX11Device::InitCubeComponents()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;

    // Original cube entity
    m_CubeEntity2 = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::Cube);

    m_CubeEntity2.SetLocationF(-3.0f, 0.0f, 0.0f);
    //m_CubeEntity.SetScale(0.1f, 0.1f, 0.1f);
    m_CubeEntity2.m_GameEntityTag = "MultipleCubeComponentEntity";
    //m_CubeEntity2.m_DXResConfig.m_UID = "UID1234";
    m_CubeEntity2.PhysicalMesh.SetIndicesList(GeometryFactory.GetIndicesList());
    auto CubeEntityIndicesList = m_CubeEntity2.PhysicalMesh.GetIndicesList();
    auto CubeEntityIndicesList2 = GeometryFactory.GetIndicesList();
    m_CubeEntity2.PhysicalMesh.SetStride(sizeof(SSimpleVertex));
    m_CubeEntity2.PhysicalMesh.SetSimpleVerticesList(m_CubeEntity2.GetVerticesList());

    XMFLOAT3 CubeEntityCenter{ 3, 0, 0 };
    XMFLOAT3 CubeEntityExtents{ 0.5f, 0.5f, 0.5f };

    SCollisionComponent Collision = m_CubeEntity2.GetCollisionComponent();
    Collision.AABox.Center = { 3, 0, 0 };
    Collision.AABox.Extents = { 0.5f, 0.5f, 0.5f };
    Collision.CollisionType = DISJOINT;
    m_CubeEntity2.SetCollisionParams(CubeEntityCenter, CubeEntityExtents, DISJOINT);

    // Cube Entity Component 1
    CGameEntity3DComponent CubeComponent1 = GeometryFactory.CreateEntity3DComponent(EPrimitiveGeometryType::Cube);

    CubeComponent1.SetLocationF(3.0f, 0.0f, 0.0f);
    CubeComponent1.SetScale(0.1f, 0.1f, 0.1f);
    CubeComponent1.m_GameEntityTag = "Cube Component1";
    //CubeComponent1.m_DXResConfig.m_UID = "UID1234";
    CubeComponent1.PhysicalMesh.SetIndicesList(GeometryFactory.GetIndicesList());
    CubeComponent1.PhysicalMesh.SetStride(sizeof(SSimpleVertex));
    CubeComponent1.PhysicalMesh.SetSimpleVerticesList(CubeComponent1.GetVerticesList());

    SCollisionComponent Collision2 = CubeComponent1.GetCollisionComponent();
    Collision2.AABox.Center = { 3, 0, 0 };
    Collision2.AABox.Extents = { 0.5f, 0.5f, 0.5f };
    Collision2.CollisionType = DISJOINT;
    CubeComponent1.SetCollisionParams(CubeEntityCenter, CubeEntityExtents, DISJOINT);


    // Cube Entity Component 2
    CGameEntity3DComponent CubeComponent2 = GeometryFactory.CreateEntity3DComponent(EPrimitiveGeometryType::Cube);

    CubeComponent2.SetLocationF(0, 0, 3.0f);
    //CubeComponent2.SetScale(0.25f, 0.25f, 0.25f);
    CubeComponent2.m_GameEntityTag = "Cube Component2";
    //CubeComponent2.m_DXResConfig.m_UID = "UID1234";
    CubeComponent2.PhysicalMesh.SetIndicesList(GeometryFactory.GetIndicesList());
    CubeComponent2.PhysicalMesh.SetStride(sizeof(SSimpleVertex));
    CubeComponent2.PhysicalMesh.SetSimpleVerticesList(CubeComponent2.GetVerticesList());

    //!!
    //
    //m_CubeEntity.m_GameEntity3DComponent.push_back(CubeComponent1);
    //m_CubeEntity.m_GameEntity3DComponent.push_back(CubeComponent2);
    //SScene.AddEntityToScene2(m_CubeEntity);

    // Working
    //SScene.SetCollisionParams2(SceneLoc, CubeEntityCenter, CubeEntityExtents, DISJOINT);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/texture.vs", "VS", "vs_5_0", &pVSBlob);

    if (FAILED(m_HR))
    {
        MessageBox(nullptr,
            L"The VS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the vertex shader
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_VertexShader);

    if (FAILED(m_HR))
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
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_VertexLayout);
    pVSBlob->Release();
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize input layout", L"Error", MB_OK);
        return;
    }

    //SScene.SetInputLayout2(SceneLoc, m_VertexLayout);
    CubeComponent1.m_DXResConfig.SetInputLayout(m_VertexLayout);
    CubeComponent2.m_DXResConfig.SetInputLayout(m_VertexLayout);

    //SScene.SetComponentInputLayout(SceneLoc, 0, m_VertexLayout);
    //SScene.SetComponentInputLayout(SceneLoc, 1, m_VertexLayout);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    //hr = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_4_0", &pPSBlob);
    m_HR = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/texture.ps", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The PS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the pixel shader
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_PixelShader);
    pPSBlob->Release();
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to create pixel shader", L"Error", MB_OK);
        return;
    }



    //SScene.SetVertexShader2(SceneLoc, m_VertexShader);
    //SScene.SetPixelShader2(SceneLoc, m_PixelShader);

    CubeComponent1.m_DXResConfig.SetVertexShader(m_VertexShader);
    CubeComponent1.m_DXResConfig.SetPixelShader(m_PixelShader);
    CubeComponent2.m_DXResConfig.SetVertexShader(m_VertexShader);
    CubeComponent2.m_DXResConfig.SetPixelShader(m_PixelShader);

    //SScene.SetComponentVertexShader(SceneLoc, 0, m_VertexShader);
    //SScene.SetComponentPixelShader(SceneLoc, 0, m_PixelShader);
    //SScene.SetComponentVertexShader(SceneLoc, 1, m_VertexShader);
    //SScene.SetComponentPixelShader(SceneLoc, 1, m_PixelShader);

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData{};

    std::vector<SSimpleVertex> TotalVerticesVectorFinal{ m_CubeEntity2.PhysicalMesh.GetSimpleVerticesList() };

    //std::vector<Simple_Vertex> TotalVerticesVector{ SScene.GetSceneList2().at(SceneLoc).GetVerticesList() };
    int TotalVerticesVectorSize = TotalVerticesVectorFinal.size();
    SSimpleVertex* VerticesArray = new SSimpleVertex[TotalVerticesVectorSize];
    for (int i = 0; i < TotalVerticesVectorSize; i++)
    {
        VerticesArray[i] = TotalVerticesVectorFinal.at(i);
    }

    // Dangling pointer
    //Simple_Vertex* VerticesArray2 = SScene.GetSceneList2().at(0).GetVerticesList().data();

    InitData.pSysMem = VerticesArray;

    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_VertexBuffer);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize vertex buffer", L"Error", MB_OK);
        return;
    }

    // Set vertex buffer
    UINT stride = sizeof(SSimpleVertex);
    UINT offset = 0;



    CubeComponent1.m_DXResConfig.SetVertexBuffer(m_VertexBuffer);
    CubeComponent2.m_DXResConfig.SetVertexBuffer(m_VertexBuffer);
    //SScene.SetComponentVertexbuffer(SceneLoc, 0, m_VertexBuffer);
    //SScene.SetComponentVertexbuffer(SceneLoc, 1, m_VertexBuffer);

    //WORD* arr = IndicesVector.data();
    WORD* Indices = m_CubeEntity2.PhysicalMesh.GetIndicesList().data();
    auto TempIndicesList = m_CubeEntity2.PhysicalMesh.GetIndicesList();
    WORD* Indices2 = TempIndicesList.data();
    //auto TempIndicesList3 = SScene.GetSceneList2().at(0).PhysicalMesh.GetIndicesList();
    //WORD* Indices3 = TempIndicesList3.data();

    WORD* TempIndicesList4 = CubeEntityIndicesList.data();

    WORD indicesVector[] = {
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
    InitData.pSysMem = indicesVector;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_IndexBufferArray[0]);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize index buffer", L"Error", MB_OK);
        return;
    }

    CubeComponent1.m_DXResConfig.SetIndexBuffer(m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);
    CubeComponent2.m_DXResConfig.SetIndexBuffer(m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);
    //SScene.SetComponentIndexbuffer(SceneLoc, 0, m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);
    //SScene.SetComponentIndexbuffer(SceneLoc, 1, m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);

    // Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize constant buffer", L"Error", MB_OK);
        return;
    }

    CubeComponent1.m_DXResConfig.SetConstantBuffer(m_ConstantBuffer);
    CubeComponent2.m_DXResConfig.SetConstantBuffer(m_ConstantBuffer);
    //SScene.SetConstantBuffer2(SceneLoc, m_ConstantBuffer);
    //SScene.SetComponentConstantBuffer(SceneLoc, 0, m_ConstantBuffer);
    //SScene.SetComponentConstantBuffer(SceneLoc, 1, m_ConstantBuffer);

    //const wchar_t* TextureName = L"grey_grid.dds";
    const wchar_t* TextureName = L"tex_stickman.dds";
    m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureRV);
    if (FAILED(m_HR))
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
    m_HR = m_pD3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);

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

    m_HR = m_pD3D11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerState);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to create rasterizer state", L"Error", MB_OK);
        return;
    }

    m_pImmediateContext->RSSetState(m_RasterizerState);

    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.1f, 10.0f);
    m_WorldMatrix = XMMatrixIdentity();

    m_pImmediateContext->PSSetShaderResources(0, 1, &m_TextureRV);

    m_CubeEntity2.m_GameEntity3DComponent.push_back(CubeComponent1);
    m_CubeEntity2.m_GameEntity3DComponent.push_back(CubeComponent2);
    SScene.AddEntityToScene(m_CubeEntity2);

    SceneLoc++;
}

void CDX11Device::InitCubeOutline()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;

    m_CubeOutline = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::Cube);
    m_CubeOutline.SetLocationF(3.0f, 0.0f, 3.0f);
    m_CubeOutline.SetScale(1.02f, 1.02f, 1.02f);
    m_CubeOutline.m_GameEntityTag = "Outline";
    m_CubeOutline.PhysicalMesh.SetStride(sizeof(SSimpleColorVertex));
    //m_GameEntityList.push_back(m_CubeEntity);
    //SScene.AddEntityToScene(m_CubeOutline);

    // Create the depth stencil outline
    OutlineDepthStencilDesc.DepthEnable = true;
    OutlineDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    OutlineDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    OutlineDepthStencilDesc.StencilEnable = true;
    OutlineDepthStencilDesc.StencilReadMask = 0xFF;
    OutlineDepthStencilDesc.StencilWriteMask = 0xFF;

    // It does not matter what we write since we are not using the values after this step.
    // In other words, we are only using the values to mask pixels.
    OutlineDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    // The stencil test passes if the passed parameter is equal to value in the buffer.
    OutlineDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    // Again, we do not care about back-facing pixels.
    OutlineDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

    //hr = g_pd3dDevice->CreateDepthStencilState(&g_DepthStencilDesc, &g_pDepthStencilOutlineState);

    m_pD3D11Device->CreateDepthStencilState(&OutlineDepthStencilDesc, &pDepthStencilStateOutline);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    // hr = CompileShaderFromFile(L"Tutorial04.fxh", "VS", "vs_4_0", &pVSBlob);
    m_HR = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/solid_color.vs", "VS", "vs_5_0", &pVSBlob);

    if (FAILED(m_HR))
    {
        MessageBox(nullptr,
            L"The VS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the vertex shader
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_VertexShader2);

    if (FAILED(m_HR))
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
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_VertexLayout2);
    pVSBlob->Release();
    if (FAILED(m_HR))
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
    m_HR = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/solid_color.ps", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {

        MessageBox(nullptr, L"The PS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the pixel shader
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_PixelShader2);
    pPSBlob->Release();
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to create pixel shader", L"Error", MB_OK);
        return;
    }

    m_CubeOutline.m_DXResConfig.SetPixelShader(m_PixelShader2);
    //SScene.SetPixelShader(SceneLoc, m_PixelShader2);

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleColorVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData{};
    //std::vector<Simple_Color_Vertex> TotalVerticesVector{ SScene.GetSceneList().at(SceneLoc).GetSimpleColorVerticesList() };
    //int TotalVerticesVectorSize = TotalVerticesVector.size();
    int TotalVerticesVectorSize = m_CubeOutline.GetSimpleColorVerticesList().size();

    SSimpleColorVertex* VerticesArray = new SSimpleColorVertex[TotalVerticesVectorSize];
    for (int i = 0; i < TotalVerticesVectorSize; i++)
    {
        VerticesArray[i] = m_CubeOutline.GetSimpleColorVerticesList().at(i);
    }


    SSimpleColorVertex CubeOutlineVertices[] = {
                { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                // Front face

                { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
    };

    InitData.pSysMem = CubeOutlineVertices;
    //InitData.pSysMem = VerticesArray;

    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_VertexBuffer2);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize vertex buffer", L"Error", MB_OK);
        return;
    }

    // Set vertex buffer
    UINT stride = sizeof(SSimpleColorVertex);
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
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_IndexBuffer2);
    if (FAILED(m_HR))
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
    bd.ByteWidth = sizeof(SConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer2);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize constant buffer", L"Error", MB_OK);
        return;
    }

    m_CubeOutline.m_DXResConfig.SetConstantBuffer(m_ConstantBuffer2);
    //SScene.SetConstantBuffer(SceneLoc, m_ConstantBuffer2);

    const wchar_t* TextureName = L"UV_Color_Grid.dds";
    m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureRV2);
    if (FAILED(m_HR))
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
    m_HR = m_pD3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);
    if (FAILED(m_HR))
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

void CDX11Device::InitCubeOutline2()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;

    m_CubeOutlineEntity = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::CubeTest);
    m_CubeOutlineEntity.SetLocationF(-3.0f, 0.0f, 0.0f);
    m_CubeOutlineEntity.SetScale(1.02f, 1.02f, 1.02f);
    m_CubeOutlineEntity.m_GameEntityTag = "Outline";
    m_CubeOutlineEntity.PhysicalMesh.SetStride(sizeof(SSimpleColorVertex));
    //m_GameEntityList.push_back(m_CubeEntity);
    //SScene.AddEntityToScene(m_CubeOutlineEntity);

    // Create the depth stencil outline
    OutlineDepthStencilDesc.DepthEnable = true;
    OutlineDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    OutlineDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    OutlineDepthStencilDesc.StencilEnable = true;
    OutlineDepthStencilDesc.StencilReadMask = 0xFF;
    OutlineDepthStencilDesc.StencilWriteMask = 0xFF;

    // It does not matter what we write since we are not using the values after this step.
    // In other words, we are only using the values to mask pixels.
    OutlineDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    // The stencil test passes if the passed parameter is equal to value in the buffer.
    OutlineDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    // Again, we do not care about back-facing pixels.
    OutlineDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

    //hr = g_pd3dDevice->CreateDepthStencilState(&g_DepthStencilDesc, &g_pDepthStencilOutlineState);

    m_pD3D11Device->CreateDepthStencilState(&OutlineDepthStencilDesc, &pDepthStencilStateOutline);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    // hr = CompileShaderFromFile(L"Tutorial04.fxh", "VS", "vs_4_0", &pVSBlob);
    m_HR = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/SolidColorShader.fxh", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The VS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the vertex shader
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_VertexShader2);

    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return;
    }

    m_CubeOutlineEntity.m_DXResConfig.SetVertexShader(m_VertexShader2);
    //SScene.SetVertexShader(SceneLoc, m_VertexShader2);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_VertexLayout2);
    pVSBlob->Release();
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize input layout", L"Error", MB_OK);
        return;
    }

    m_CubeOutlineEntity.m_DXResConfig.SetInputLayout(m_VertexLayout2);
    //SScene.SetInputLayout(SceneLoc, m_VertexLayout2);

    // Set the input layout
    //m_ImmediateContext->IASetInputLayout(m_VertexLayout);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    //hr = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_4_0", &pPSBlob);
    m_HR = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/SolidColorShader.fxh", "PS", "ps_4_0", &pPSBlob);
    if (FAILED(m_HR))
    {

        MessageBox(nullptr, L"The PS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the pixel shader
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_PixelShader2);
    pPSBlob->Release();
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to create pixel shader", L"Error", MB_OK);
        return;
    }

    m_CubeOutlineEntity.m_DXResConfig.SetPixelShader(m_PixelShader2);
    //SScene.SetPixelShader(SceneLoc, m_PixelShader2);

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleColorVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData{};
    //std::vector<Simple_Color_Vertex> TotalVerticesVector{ SScene.GetSceneList().at(SceneLoc).GetSimpleColorVerticesList() };
    //int TotalVerticesVectorSize = TotalVerticesVector.size();
    int TotalVerticesVectorSize = m_CubeOutlineEntity.GetSimpleColorVerticesList().size();

    SSimpleColorVertex* VerticesArray = new SSimpleColorVertex[TotalVerticesVectorSize];
    for (int i = 0; i < TotalVerticesVectorSize; i++)
    {
        VerticesArray[i] = m_CubeOutlineEntity.GetSimpleColorVerticesList().at(i);
    }
    InitData.pSysMem = VerticesArray;

    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_VertexBuffer2);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize vertex buffer", L"Error", MB_OK);
        return;
    }

    // Set vertex buffer
    UINT stride = sizeof(SSimpleColorVertex);
    UINT offset = 0;
    //m_ImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

    m_CubeOutlineEntity.m_DXResConfig.SetVertexBuffer(m_VertexBuffer2);
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
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_IndexBuffer2);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize index buffer", L"Error", MB_OK);
        return;
    }

    m_CubeOutlineEntity.m_DXResConfig.SetIndexBuffer(m_IndexBuffer2, DXGI_FORMAT_R16_UINT, 0);
    //SScene.SetIndexbuffer(SceneLoc, m_IndexBuffer2, DXGI_FORMAT_R16_UINT, 0);

    // Set index buffer
    //m_ImmediateContext->IASetIndexBuffer(m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);

    // Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer2);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize constant buffer", L"Error", MB_OK);
        return;
    }

    m_CubeOutlineEntity.m_DXResConfig.SetConstantBuffer(m_ConstantBuffer2);
    //SScene.SetConstantBuffer(SceneLoc, m_ConstantBuffer2);

    const wchar_t* TextureName = L"UV_Color_Grid.dds";
    m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureRV2);
    if (FAILED(m_HR))
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
    m_HR = m_pD3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize sampler desc", L"Error", MB_OK);
        return;
    }

    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.01f, 100.0f);
    m_WorldMatrix = XMMatrixIdentity();

    //SceneLoc++;
    SScene.AddEntityToScene(m_CubeEntity2);
    m_UnrenderedList.push_back(m_CubeOutlineEntity);
}


void CDX11Device::InitSingleCubeOutline()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;

    m_CubeOutlineEntity = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::CubeTest);
    m_CubeOutlineEntity.SetLocationF(3.0f, 0.0f, 0.0f);
    m_CubeOutlineEntity.SetScale(1.02f, 1.02f, 1.02f);
    m_CubeOutlineEntity.m_GameEntityTag = "Outline";
    m_CubeOutlineEntity.PhysicalMesh.SetStride(sizeof(SSimpleColorVertex));
    //m_GameEntityList.push_back(m_CubeEntity);
    //SScene.AddEntityToScene(m_CubeOutlineEntity);

    // Create the depth stencil outline
    OutlineDepthStencilDesc.DepthEnable = true;
    OutlineDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    OutlineDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    OutlineDepthStencilDesc.StencilEnable = true;
    OutlineDepthStencilDesc.StencilReadMask = 0xFF;
    OutlineDepthStencilDesc.StencilWriteMask = 0xFF;

    // It does not matter what we write since we are not using the values after this step.
    // In other words, we are only using the values to mask pixels.
    OutlineDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    // The stencil test passes if the passed parameter is equal to value in the buffer.
    OutlineDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    // Again, we do not care about back-facing pixels.
    OutlineDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

    //hr = g_pd3dDevice->CreateDepthStencilState(&g_DepthStencilDesc, &g_pDepthStencilOutlineState);

    m_pD3D11Device->CreateDepthStencilState(&OutlineDepthStencilDesc, &pDepthStencilStateOutline);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    // hr = CompileShaderFromFile(L"Tutorial04.fxh", "VS", "vs_4_0", &pVSBlob);
    m_HR = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/solid_color.vs", "VS", "vs_5_0", &pVSBlob);

    if (FAILED(m_HR))
    {
        MessageBox(nullptr,
            L"The VS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the vertex shader
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_VertexShader2);

    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return;
    }

    m_CubeOutlineEntity.m_DXResConfig.SetVertexShader(m_VertexShader2);
    //SScene.SetVertexShader(SceneLoc, m_VertexShader2);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_VertexLayout2);
    pVSBlob->Release();
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize input layout", L"Error", MB_OK);
        return;
    }

    m_CubeOutlineEntity.m_DXResConfig.SetInputLayout(m_VertexLayout2);
    //SScene.SetInputLayout(SceneLoc, m_VertexLayout2);

    // Set the input layout
    //m_ImmediateContext->IASetInputLayout(m_VertexLayout);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    //hr = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_4_0", &pPSBlob);
    m_HR = CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/solid_color.ps", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {

        MessageBox(nullptr, L"The PS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the pixel shader
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_PixelShader2);
    pPSBlob->Release();
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to create pixel shader", L"Error", MB_OK);
        return;
    }

    m_CubeOutlineEntity.m_DXResConfig.SetPixelShader(m_PixelShader2);
    //SScene.SetPixelShader(SceneLoc, m_PixelShader2);

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleColorVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData{};
    //std::vector<Simple_Color_Vertex> TotalVerticesVector{ SScene.GetSceneList().at(SceneLoc).GetSimpleColorVerticesList() };
    //int TotalVerticesVectorSize = TotalVerticesVector.size();
    int TotalVerticesVectorSize = m_CubeOutlineEntity.GetSimpleColorVerticesList().size();

    SSimpleColorVertex* VerticesArray = new SSimpleColorVertex[TotalVerticesVectorSize];
    for (int i = 0; i < TotalVerticesVectorSize; i++)
    {
        VerticesArray[i] = m_CubeOutlineEntity.GetSimpleColorVerticesList().at(i);
    }
    InitData.pSysMem = VerticesArray;

    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_VertexBuffer2);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize vertex buffer", L"Error", MB_OK);
        return;
    }

    // Set vertex buffer
    UINT stride = sizeof(SSimpleColorVertex);
    UINT offset = 0;
    //m_ImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

    m_CubeOutlineEntity.m_DXResConfig.SetVertexBuffer(m_VertexBuffer2);
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
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_IndexBuffer2);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize index buffer", L"Error", MB_OK);
        return;
    }

    m_CubeOutlineEntity.m_DXResConfig.SetIndexBuffer(m_IndexBuffer2, DXGI_FORMAT_R16_UINT, 0);
    //SScene.SetIndexbuffer(SceneLoc, m_IndexBuffer2, DXGI_FORMAT_R16_UINT, 0);

    // Set index buffer
    //m_ImmediateContext->IASetIndexBuffer(m_IndexBufferArray[0], DXGI_FORMAT_R16_UINT, 0);

    // Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer2);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize constant buffer", L"Error", MB_OK);
        return;
    }

    m_CubeOutlineEntity.m_DXResConfig.SetConstantBuffer(m_ConstantBuffer2);
    //SScene.SetConstantBuffer(SceneLoc, m_ConstantBuffer2);

    const wchar_t* TextureName = L"UV_Color_Grid.dds";
    m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureRV2);
    if (FAILED(m_HR))
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
    m_HR = m_pD3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize sampler desc", L"Error", MB_OK);
        return;
    }

    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.01f, 100.0f);
    m_WorldMatrix = XMMatrixIdentity();

    //SceneLoc++;

    m_UnrenderedList.push_back(m_CubeOutlineEntity);
}

void CDX11Device::InitArrow()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;

    CGameEntity3D m_Arrow = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::Arrow);

    //auto SelectedEntity = SScene.GetSceneList().at(0);
    //XMVECTOR LocationVector = SelectedEntity.GetLocationVector();
    //XMFLOAT4 LocationFloat;
    //XMStoreFloat4(&LocationFloat, LocationVector);
    //m_Arrow.SetLocationF(LocationFloat.x, LocationFloat.y, LocationFloat.z);

    m_Arrow.SetLocationF(3.0f, 0.0f, 0.0f);
    m_Arrow.SetScale(0.1f, 0.1f, 0.1f);
    m_Arrow.m_GameEntityTag = "GizmoArrow";
    //m_Arrow.m_DXResConfig.m_UID = "UID1234";

    CGameEntity3DComponent ArrowX;
    CGameEntity3DComponent ArrowY;
    CGameEntity3DComponent ArrowZ;


    //m_Arrow.m_GameEntity3DComponent.push_back(ArrowXAxis);
    //auto& ArrowX = m_Arrow.m_GameEntity3DComponent.at(0);

    TEntityPhysicalMesh ArrowPhysicalMesh;
    ArrowX.PhysicalMesh = ArrowPhysicalMesh;
    m_Arrow.PhysicalMesh.SetStride(sizeof(SSimpleColorVertex));

    ArrowX.PhysicalMesh.SetSimpleColorVerticesList(m_Arrow.PhysicalMesh.GetSimpleColorVerticesList());
    ArrowX.PhysicalMesh.SetIndicesList(m_Arrow.PhysicalMesh.GetIndicesList());
    ArrowX.PhysicalMesh.SetStride(sizeof(SSimpleColorVertex));

    //SScene.AddEntityToScene2(m_Arrow);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"C:/Users/sefce/source/KMEngine/KMEngine/GizmoRedArrow.vs", "VS", "vs_5_0", &pVSBlob);

    const wchar_t* ErrorMessage{ L"The VS file cannot be compiled for the Arrow Entity" };

    if (FAILED(m_HR))
    {
        //MessageBox(nullptr, L"The VS file cannot be compiled.", L"Error", MB_OK);
        MessageBox(nullptr, ErrorMessage, L"Error", MB_OK);
        return;
    }

    // Create the vertex shader
    m_HR = CompileShaderFromFile(L"C:/Users/sefce/source/KMEngine/KMEngine/GizmoRedArrow.ps", "PS", "ps_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        if (pVSBlob)
        {
            pVSBlob->Release();
        }
        return;
    }

    ArrowX.m_DXResConfig.SetVertexShader(m_VertexShader3);
    //SScene.SetVertexShader2(SceneLoc, m_VertexShader3);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_VertexLayout3);
    pVSBlob->Release();
    if (FAILED(m_HR))
        return;

    ArrowX.m_DXResConfig.SetInputLayout(m_VertexLayout3);
    //SScene.SetInputLayout2(SceneLoc, m_VertexLayout3);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"C:/Users/sefce/source/KMEngine/KMEngine/GizmoRedArrow.ps", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The PS file cannot be compiled for the Gizmo.", L"Error", MB_OK);
        return;
    }

    // Create the pixel shader
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_PixelShader3);
    pPSBlob->Release();
    if (FAILED(m_HR))
        return;

    // Add the vertex and pixel shader to the DX Resources object
    ArrowX.m_DXResConfig.SetVertexShader(m_VertexShader3);
    ArrowX.m_DXResConfig.SetPixelShader(m_PixelShader3);

    //SScene.SetVertexShader2(SceneLoc, m_VertexShader3);
    //SScene.SetPixelShader2(SceneLoc, m_PixelShader3);

    // Create vertex buffer
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleColorVertex) * 512;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData{};

    std::vector<SSimpleColorVertex> TotalVerticesVector{ m_Arrow.m_PhysicalMeshVector.at(0).GetSimpleColorVerticesList() };
    int TotalVerticesVectorSize = TotalVerticesVector.size();
    SSimpleColorVertex* VerticesArray = new SSimpleColorVertex[TotalVerticesVectorSize];
    for (int i = 0; i < TotalVerticesVectorSize; i++)
    {
        VerticesArray[i] = TotalVerticesVector.at(i);
    }
    InitData.pSysMem = VerticesArray;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_VertexBuffer3);
    if (FAILED(m_HR))
        return;

    //std::vector<Simple_Color_Vertex> TotalVerticesVector{ SScene.GetSceneList2().at(SceneLoc).GetSimpleColorVerticesList() };
    //int TotalVerticesVectorSize = TotalVerticesVector.size();
    //Simple_Color_Vertex* VerticesArray = new Simple_Color_Vertex[TotalVerticesVectorSize];
    //for (int i = 0; i < TotalVerticesVectorSize; i++)
    //{
    //    VerticesArray[i] = TotalVerticesVector.at(i);
    //}
    //InitData.pSysMem = VerticesArray;
    //m_hr = m_D3D11Device->CreateBuffer(&bd, &InitData, &m_VertexBuffer3);
    //if (FAILED(m_hr))
    //    return;

    // Set vertex bufferds
    UINT stride = sizeof(SSimpleColorVertex);
    UINT offset = 0;

    ArrowX.m_DXResConfig.SetVertexBuffer(m_VertexBuffer3);
    //SScene.SetVertexbuffer2(SceneLoc, m_VertexBuffer3);
    //m_ImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer2, &stride, &offset);

    // Create index buffer
    auto TempArrowIndicesList = SScene.GetSceneList().at(3).PhysicalMesh.GetIndicesList();
    WORD* TempArrowIndices = TempArrowIndicesList.data();

    //auto TempArrowIndicesList2 = SScene.GetSceneList2().at(3).m_PhysicalMeshVector.at(0).GetIndicesList();
    //WORD* TempArrowIndices2 = TempArrowIndicesList.data();

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 512;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = TempArrowIndices;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_IndexBuffer3);
    if (FAILED(m_HR))
        return;

    // Set index buffer
    ArrowX.m_DXResConfig.SetIndexBuffer(m_IndexBuffer3, DXGI_FORMAT_R16_UINT, 0);
    //SScene.SetIndexbuffer2(SceneLoc, m_IndexBuffer3, DXGI_FORMAT_R16_UINT, 0);

    // Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SArrowConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer3);
    if (FAILED(m_HR))
        return;

    ArrowX.m_DXResConfig.SetArrowConstantBuffer(m_ConstantBuffer3);
    //SScene.SetConstantBuffer2(SceneLoc, m_ConstantBuffer3);

    m_Arrow.m_GameEntity3DComponent.push_back(ArrowX);
    SScene.AddEntityToScene(m_Arrow);

    SceneLoc++;

    m_UnrenderedList.push_back(m_Arrow);
}

void CDX11Device::AddTestLine()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;

    CGameEntity3D Linetrace;
    Linetrace.m_GameEntityTag = "Linetrace";
    Linetrace.SetLocationF(0.0f, 0.0f, 0.0f);
    SScene.AddEntityToScene(Linetrace);

    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"Tutorial04.fxh", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return;
    }

    // Create the vertex shader
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_LinetraceVertexShader);
    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return;
    }

    auto VertexShaderLambda = [=]() {
        m_pImmediateContext->VSSetShader(m_LinetraceVertexShader, nullptr, 0);
        };

    Linetrace.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return;
    }

    // Create the pixel shader
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_LinetracePixelShader);
    pPSBlob->Release();
    if (FAILED(m_HR))
        return;

    auto PixelShaderLambda = [=]() {
        m_pImmediateContext->PSSetShader(m_LinetracePixelShader, nullptr, 0);
        };

    Linetrace.m_DXResConfig.m_pContextResourcePtr.push_back(PixelShaderLambda);

    SSimpleVertex RayVertices[] = {
        { XMFLOAT3(0.f, -0.1f, 0.f), XMFLOAT2(1.0f, 0.0f)},
        { XMFLOAT3(0.992644 * 10, 0.000792577 * 10, 0.121069 * 10), XMFLOAT2(1.0f, 0.0f)}
    };

    D3D11_BUFFER_DESC RayBuffer{ };
    RayBuffer.Usage = D3D11_USAGE_DEFAULT;
    RayBuffer.ByteWidth = sizeof(SSimpleVertex) * 20;
    RayBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    RayBuffer.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&RayBuffer, nullptr, &m_LinetraceConstantBuffer);
    //SScene.SetConstantBuffer(SceneLoc, m_LinetraceConstantBuffer);

    auto ConstantBufferLambda = [=]() {
        m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_LinetraceConstantBuffer);
        };

    Linetrace.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);


    D3D11_SUBRESOURCE_DATA RayData = {};
    RayData.pSysMem = RayVertices;

    m_HR = m_pD3D11Device->CreateBuffer(&RayBuffer, &RayData, &m_LinetraceVertexBuffer);
    SScene.SetVertexbuffer(0, m_LinetraceVertexBuffer);

    UINT stride = sizeof(SSimpleVertex);
    UINT offset = 0;

    auto VertexBufferLambda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &m_LinetraceVertexBuffer, &stride, &offset);
        };

    Linetrace.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);

    CLogger& SLogger = CLogger::GetLogger();
    SLogger.Log("InitLine, called from DX11_Device.cpp");

    SceneLoc++;

    m_UnrenderedList.push_back(Linetrace);
}

void CDX11Device::AddTestLine2()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;
    CGameEntity3D Linetrace;
    Linetrace.m_GameEntityTag = "Linetrace";
    Linetrace.SetLocationF(0.0f, 0.0f, 2.0f);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"Tutorial04.fxh", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return;
    }

    // Create the vertex shader
    ID3D11VertexShader* TempVertexShader{ nullptr };
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &TempVertexShader);
    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return;
    }
    Linetrace.SetVertexShader(TempVertexShader);
    auto VertexShaderLambda = [=]() {
        m_pImmediateContext->VSSetShader(TempVertexShader, nullptr, 0);
        };
    Linetrace.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);
    Linetrace.m_DXResConfig.SetVertexShader(TempVertexShader);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    ID3D11InputLayout* TempVertexLayout{ nullptr };
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &TempVertexLayout);
    pVSBlob->Release();
    if (FAILED(m_HR))
        return;

    // Set the input layout
    Linetrace.SetInputLayout(TempVertexLayout);
    Linetrace.m_DXResConfig.SetInputLayout(TempVertexLayout);

    auto InputLayoutLambda = [=]() {
        m_pImmediateContext->IASetInputLayout(TempVertexLayout);
        };

    Linetrace.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);
    m_pImmediateContext->IASetInputLayout(TempVertexLayout);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return;
    }

    // Create the pixel shader
    ID3D11PixelShader* TempPixelShader{ nullptr };
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &TempPixelShader);
    pPSBlob->Release();
    if (FAILED(m_HR))
        return;
    Linetrace.SetPixelShader(m_PixelShader);
    auto PixelShaderLambda = [=]() {
        m_pImmediateContext->PSSetShader(TempPixelShader, nullptr, 0);
        };
    Linetrace.m_DXResConfig.m_pContextResourcePtr.push_back(PixelShaderLambda);
    Linetrace.m_DXResConfig.SetPixelShader(TempPixelShader);

    SSimpleColorVertex RayVertices[] = 
    {
        { XMFLOAT3(0.f, -0.1f, 0.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f)},
        { XMFLOAT3(0.992644 * 10, 0.000792577 * 10, 0.121069 * 10), XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f)}
    };

    ID3D11Buffer* TempVertexBuffer{ nullptr };

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleColorVertex) * 2;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData{};
    InitData.pSysMem = RayVertices;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &TempVertexBuffer);
    if (FAILED(m_HR))
        return;

    // Set vertex buffer
    UINT stride = sizeof(SSimpleColorVertex);
    UINT offset = 0;
    Linetrace.SetVertexBuffer(TempVertexBuffer);
    Linetrace.m_DXResConfig.SetVertexBuffer(TempVertexBuffer);

    auto VertexBufferLamnda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);
        };
    Linetrace.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLamnda);

    //m_ImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);

    // Create the constant buffer
    ID3D11Buffer* TempConstantBuffer{ nullptr };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &TempConstantBuffer);
    if (FAILED(m_HR))
        return;

    Linetrace.SetConstantBuffer(TempConstantBuffer);
    auto ConstantBufferLambda = [=]() {
        m_pImmediateContext->VSSetConstantBuffers(0, 1, &TempConstantBuffer);
        };
    Linetrace.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    Linetrace.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);

    SScene.AddEntityToScene(Linetrace);

    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.01f, 100.0f);
}

bool CDX11Device::DoesIntersect(FXMVECTOR Origin, FXMVECTOR Direction, XMFLOAT3 Center, XMFLOAT3 Extents, float& Dist)
{
    CLogger& SLogger = CLogger::GetLogger();

    XMFLOAT3 fDirection{ 0, 0, 0 };
    XMStoreFloat3(&fDirection, Direction);
    SLogger.Log("DX11_Device::DoesIntersect, Ray Direction is: ", fDirection.x, " ", fDirection.y, " ", fDirection.z);

    // Load the box.
    XMVECTOR vCenter = XMLoadFloat3(&Center);
    /*XMFLOAT3 fCenter{ 0, 0, 0 };
    XMStoreFloat3(&fCenter, vCenter);
    SLogger.Log("DX11_Device::DoesIntersect, Box Center is: ", fCenter.x, " ", fCenter.y, " ", fCenter.z);*/

    XMVECTOR vExtents = XMLoadFloat3(&Extents);
    /*XMFLOAT3 fExtents{ 0, 0, 0 };
    XMStoreFloat3(&fExtents, vExtents);
    SLogger.Log("DX11_Device::DoesIntersect, Box Extents is: ", fExtents.x, " ", fExtents.y, " ", fExtents.z);*/

    // Adjust ray origin to be relative to center of the box.
    //XMVECTOR TOrigin = XMVectorSubtract(vCenter, Origin);
    //XMFLOAT3 fOrigin{ 0, 0, 0 };
    //XMStoreFloat3(&fOrigin, TOrigin);
    //SLogger.Log("DX11_Device::DoesIntersect, TOrigin is: ", fOrigin.x, " ", fOrigin.y, " ", fOrigin.z);

    //XMFLOAT3 AxisDotDirectionFloat{ sinf(XMConvertToRadians(92)), 0, cosf(XMConvertToRadians(92)) };
    //XMVECTOR AxisDotDirection{ XMLoadFloat3(&AxisDotDirectionFloat) };
    XMVECTOR AxisDotDirection{ Direction };

    // Compute the dot product againt each axis of the box.
    // Since the axii are (1,0,0), (0,1,0), (0,0,1) no computation is necessary.
    XMVECTOR AxisDotOrigin = XMVectorSubtract(vCenter, Origin);
    XMFLOAT3 fAxisDotOrigin{ 0, 0, 0 };
    XMStoreFloat3(&fAxisDotOrigin, AxisDotOrigin);
    SLogger.Log("DX11_Device::DoesIntersect, Axis Dot Origin is: ", fAxisDotOrigin.x, " ", fAxisDotOrigin.y, " ", fAxisDotOrigin.z);

    //XMVECTOR AxisDotDirection = Direction;

    // Test against all three axii simultaneously.
    XMVECTOR InverseAxisDotDirection = XMVectorReciprocal(AxisDotDirection);
    XMFLOAT3 fInverseAxisDotDirection{ 0, 0, 0 };
    XMStoreFloat3(&fInverseAxisDotDirection, InverseAxisDotDirection);
    SLogger.Log("DX11_Device::DoesIntersect, Inverse Axis Dot Direction is: ", fInverseAxisDotDirection.x, " ", fInverseAxisDotDirection.y, " ", fInverseAxisDotDirection.z);

    // if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
    XMVECTOR IsParallel = XMVectorLessOrEqual(XMVectorAbs(AxisDotDirection), _RayEpsilon);
    XMFLOAT3 fIsParallel{ 0, 0, 0 };
    XMStoreFloat3(&fIsParallel, IsParallel);
    SLogger.Log("DX11_Device::DoesIntersect, Is Parallel is: ", fIsParallel.x, " ", fIsParallel.y, " ", fIsParallel.z);

    XMVECTOR t1 = XMVectorMultiply(XMVectorSubtract(AxisDotOrigin, vExtents), InverseAxisDotDirection);
    XMFLOAT3 ft1{ 0, 0, 0 };
    XMStoreFloat3(&ft1, t1);
    SLogger.Log("DX11_Device::DoesIntersect, t1 is: ", ft1.x, " ", ft1.y, " ", ft1.z);

    XMVECTOR t2 = XMVectorMultiply(XMVectorAdd(AxisDotOrigin, vExtents), InverseAxisDotDirection);
    XMFLOAT3 ft2{ 0, 0, 0 };
    XMStoreFloat3(&ft2, t2);
    SLogger.Log("DX11_Device::DoesIntersect, t2 is: ", ft2.x, " ", ft2.y, " ", ft2.z);

    // Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
    // use the results from any directions parallel to the slab.
    XMVECTOR t_min = XMVectorSelect(XMVectorMin(t1, t2), _FltMin, IsParallel);
    XMFLOAT3 ft_min{ 0, 0, 0 };
    XMStoreFloat3(&ft_min, t_min);
    SLogger.Log("DX11_Device::DoesIntersect, t_min is: ", ft_min.x, " ", ft_min.y, " ", ft_min.z);

    XMVECTOR t_max = XMVectorSelect(XMVectorMax(t1, t2), _FltMax, IsParallel);
    XMFLOAT3 ft_max{ 0, 0, 0 };
    XMStoreFloat3(&ft_max, t_max);
    SLogger.Log("DX11_Device::DoesIntersect, t_max is: ", ft_max.x, " ", ft_max.y, " ", ft_max.z);

    // t_min.x = maximum( t_min.x, t_min.y, t_min.z );
    // t_max.x = minimum( t_max.x, t_max.y, t_max.z );
    XMVECTOR new_t_min = XMVectorMax(t_min, XMVectorSplatY(t_min));  // x = max(x,y)
    XMFLOAT3 fnew_t_min{ 0, 0, 0 };
    XMStoreFloat3(&fnew_t_min, new_t_min);
    SLogger.Log("DX11_Device::DoesIntersect, t_min: ", fnew_t_min.x, " ", fnew_t_min.y, " ", fnew_t_min.z);

    XMVECTOR new_t_min2 = XMVectorMax(new_t_min, XMVectorSplatZ(new_t_min));  // x = max(max(x,y),z)
    XMFLOAT3 fnew_t_min2{ 0, 0, 0 };
    XMStoreFloat3(&fnew_t_min2, new_t_min2);
    SLogger.Log("DX11_Device::DoesIntersect, t_min: ", fnew_t_min2.x, " ", fnew_t_min2.y, " ", fnew_t_min2.z);

    XMVECTOR new_t_max = XMVectorMin(t_max, XMVectorSplatY(t_max));  // x = min(x,y)
    XMFLOAT3 fnew_t_max{ 0, 0, 0 };
    XMStoreFloat3(&fnew_t_max, new_t_max);
    SLogger.Log("DX11_Device::DoesIntersect, t_max: ", fnew_t_max.x, " ", fnew_t_max.y, " ", fnew_t_max.z);

    XMVECTOR new_t_max2 = XMVectorMin(new_t_max, XMVectorSplatZ(new_t_max));  // x = min(min(x,y),z)
    XMFLOAT3 fnew_t_max2{ 0, 0, 0 };
    XMStoreFloat3(&fnew_t_max2, new_t_max2);
    SLogger.Log("DX11_Device::DoesIntersect, t_max: ", fnew_t_max2.x, " ", fnew_t_max2.y, " ", fnew_t_max2.z);

    // if ( t_min > t_max ) return false;
    XMVECTOR NoIntersection = XMVectorGreater(XMVectorSplatX(new_t_min2), XMVectorSplatX(new_t_max2));
    XMFLOAT3 fNoIntersection{ 0, 0, 0 };
    XMStoreFloat3(&fNoIntersection, NoIntersection);
    SLogger.Log("DX11_Device::DoesIntersect, No Intersection is: ", fNoIntersection.x, " ", fNoIntersection.y, " ", fNoIntersection.z);

    // if ( t_max < 0.0f ) return false;
    XMVECTOR NoIntersection2 = XMVectorOrInt(NoIntersection, XMVectorLess(XMVectorSplatX(t_max), XMVectorZero()));
    XMFLOAT3 fNoIntersection2{ 0, 0, 0 };
    XMStoreFloat3(&fNoIntersection2, NoIntersection2);
    SLogger.Log("DX11_Device::DoesIntersect, No Intersection2 is: ", fNoIntersection2.x, " ", fNoIntersection2.y, " ", fNoIntersection2.z);

    // if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return false;
    XMVECTOR ParallelOverlap = XMVectorInBounds(AxisDotOrigin, vExtents);
    XMFLOAT3 fParallelOverlap{ 0, 0, 0 };
    XMStoreFloat3(&fParallelOverlap, ParallelOverlap);
    SLogger.Log("DX11_Device::DoesIntersect, Parallel overlap: ", fParallelOverlap.x, " ", fParallelOverlap.y, " ", fParallelOverlap.z);

    XMVECTOR NoIntersection3 = XMVectorOrInt(NoIntersection2, XMVectorAndCInt(IsParallel, ParallelOverlap));
    XMFLOAT3 fNoIntersection3{ 0, 0, 0 };
    XMStoreFloat3(&fNoIntersection3, NoIntersection3);
    SLogger.Log("DX11_Device::DoesIntersect, NoIntersection3 is: ", fNoIntersection3.x, " ", fNoIntersection3.y, " ", fNoIntersection3.z);

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

void CDX11Device::TestVector()
{
    CLogger& SLogger = CLogger::GetLogger();
    SLogger.Log("DX11_Device::TestVector()");

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

void CDX11Device::InitRaycast(float OriginX, float OriginY, float OriginZ, float DestinationX, float DestinationY, float DestinationZ)
{
	// Lambdas: vertes shader, pixel shader, vertex buffer, constant buffer
    CScene& SScene = CScene::GetScene();
	CLogger& Logger = CLogger::GetLogger();
    CPrimitiveGeometryFactory GeometryFactory;

    CGameEntity3D Linetrace;
    Linetrace.m_GameEntityTag = "Raycast";
    Linetrace.SetLocationF(0.0f, 0.0f, 0.0f);

    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"Tutorial04.fxh", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return;
    }

    // Create the vertex shader
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_LinetraceVertexShader);
    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return;
    }

    auto VertexShaderLambda = [=]() {
        m_pImmediateContext->VSSetShader(m_LinetraceVertexShader, nullptr, 0);
    };

    Linetrace.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return;
    }

    // Create the pixel shader
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_LinetracePixelShader);
    pPSBlob->Release();
    if (FAILED(m_HR))
        return;

    auto PixelShaderLambda = [=]() {
        m_pImmediateContext->PSSetShader(m_LinetracePixelShader, nullptr, 0);
    };

    Linetrace.m_DXResConfig.m_pContextResourcePtr.push_back(PixelShaderLambda);

    SSimpleVertex RayVertices[] = {
        { XMFLOAT3(OriginX, OriginY, OriginZ), XMFLOAT2(1.0f, 0.0f)},
        { XMFLOAT3(DestinationX, DestinationY, DestinationZ), XMFLOAT2(1.0f, 0.0f)}
    };

	Logger.Log("Raycasting Destination X, Y, Z: ", DestinationX, " ", DestinationY, " ", DestinationZ);

    // Create the vertex buffer
    D3D11_BUFFER_DESC RayBuffer{ };
    RayBuffer.Usage = D3D11_USAGE_DEFAULT;
    RayBuffer.ByteWidth = sizeof(SSimpleVertex) * 2;
    RayBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    RayBuffer.CPUAccessFlags = 0;

    ID3D11Buffer* TempVertexBuffer{ nullptr };
    D3D11_SUBRESOURCE_DATA InitData{};
    InitData.pSysMem = RayVertices;
    m_HR = m_pD3D11Device->CreateBuffer(&RayBuffer, &InitData, &TempVertexBuffer);
    if (FAILED(m_HR))
        return;

    UINT stride = sizeof(SSimpleVertex);
    UINT offset = 0;

    auto VertexBufferLambda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);
    };

    Linetrace.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);
    //m_HR = m_pD3D11Device->CreateBuffer(&RayBuffer, nullptr, &m_LinetraceConstantBuffer);
    //SScene.SetConstantBuffer(SceneLoc, m_LinetraceConstantBuffer);

    // Create the constant buffer
    ID3D11Buffer* TempConstantBuffer{ nullptr };
    RayBuffer.Usage = D3D11_USAGE_DEFAULT;
    RayBuffer.ByteWidth = sizeof(SConstantBuffer);
    RayBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    RayBuffer.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&RayBuffer, nullptr, &TempConstantBuffer);
    if (FAILED(m_HR))
        return;

    Linetrace.SetConstantBuffer(TempConstantBuffer);
    auto ConstantBufferLambda = [=]() {
        m_pImmediateContext->VSSetConstantBuffers(0, 1, &TempConstantBuffer);
        };
    Linetrace.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    Linetrace.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);

    CLogger& SLogger = CLogger::GetLogger();
    SLogger.Log("InitLine, called from DX11_Device.cpp");

    SScene.AddEntityToScene(Linetrace);

    SceneLoc++;
}

void CDX11Device::InitLine(float OriginX, float OriginY, float OriginZ, float DestinationX, float DestinationY, float DestinationZ)
{
    CScene& SScene = CScene::GetScene();
    /*ConstantBuffer RayCB{};*/
    CPrimitiveGeometryFactory GeometryFactory;

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

    CGameEntity3D Linetrace;
    Linetrace.m_GameEntityTag = "Linetrace";
    Linetrace.SetLocationF(0.0f, 0.0f, 0.0f);
    SScene.AddEntityToScene(Linetrace);

    //Simple_Vertex RayVertices[] =
    //{
    //    { XMFLOAT3(OriginX, OriginY, OriginZ), XMFLOAT2(1.0f, 0.0f)},
    //    { XMFLOAT3(DestinationX, DestinationY, DestinationZ), XMFLOAT2(1.0f, 0.0f)}
    //};

    SSimpleVertex RayVertices[] =
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
    RayBuffer.ByteWidth = sizeof(SConstantBuffer);
    RayBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    RayBuffer.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&RayBuffer, nullptr, &m_ConstantBuffer4);
    SScene.SetConstantBuffer(SceneLoc, m_ConstantBuffer4);

    D3D11_SUBRESOURCE_DATA RayData = {};
    RayData.pSysMem = RayVertices;

    m_HR = m_pD3D11Device->CreateBuffer(&RayBuffer, &RayData, &m_VertexBuffer4);
    SScene.SetVertexbuffer(SceneLoc, m_VertexBuffer4);

    CLogger& SLogger = CLogger::GetLogger();
    SLogger.Log("InitLine, called from DX11_Device.cpp");

    SceneLoc++;

    m_UnrenderedList.push_back(Linetrace);
}

void CDX11Device::CheckCollision(float OriginX, float OriginY, float OriginZ, float DestX, float DestY, float DestZ)
{
    CLogger& SLogger = CLogger::GetLogger();
    float fDistance = -1.0f;
    float fDist;

    SCollisionComponent Collision{ m_CubeEntity2.GetCollisionComponent() };

    XMFLOAT3 fRayOrigin{ OriginX, OriginY, OriginZ };
    XMVECTOR mRayOrigin = XMLoadFloat3(&fRayOrigin);
    //XMStoreFloat3(&fRayOrigin, mRayOrigin);
    //XMStoreFloat3(&fRayOrigin, g_RayOrigin);

    XMFLOAT3 fRayDestination{ DestX, DestY, DestZ };
    XMVECTOR mRayDirection = XMLoadFloat3(&fRayDestination);
    SLogger.Log("DX11_Device::CheckCollision: Ray Direction vector is:",
        "X = ", fRayDestination.x,
        ", Y = ", fRayDestination.y,
        ", Z = ", fRayDestination.z);
    //XMStoreFloat3(&fRayDestination, g_RayDestination);
    //XMStoreFloat3(&fRayDestination, mRayDirection);

    XMFLOAT3 CubeCenter{ 3, 0, 0 };
    XMFLOAT3 CubeExtents{ 0.5f, 0.5f, 0.5f };

    SLogger.Log("DX11_Device::CheckCollision: CollisionAABox center vector is:",
        "X = ", Collision.AABox.Center.x,
        ", Y = ", Collision.AABox.Center.y,
        ", Z = ", Collision.AABox.Center.z);

    SLogger.Log("DX11_Device::CheckCollision: CollisionAABox extents vector is:",
        "X = ", Collision.AABox.Extents.x,
        ", Y = ", Collision.AABox.Extents.y,
        ", Z = ", Collision.AABox.Extents.z);

    SLogger.Log("DX11_Device::CheckCollision: Ray Origin vector is:",
        "X = ", fRayOrigin.x,
        ", Y = ", fRayOrigin.y,
        ", Z = ", fRayOrigin.z);

    SLogger.Log("DX11_Device::CheckCollision: Dest Params:",
        "DestX = ", DestX,
        ", DestY = ", DestY,
        ", DestZ = ", DestZ);

    SLogger.Log("DX11_Device::CheckCollision: Testing difference. g_RayOrigin ",
        "DestX = ", DestX,
        ", DestY = ", DestY,
        ", DestZ = ", DestZ);

    //if (DoesIntersect(g_RayOrigin, g_RayDestination, Collision.AABox.Center, Collision.AABox.Extents, fDist))
    if (DoesIntersect(mRayOrigin, mRayDirection, Collision.AABox.Center, Collision.AABox.Extents, fDist))
    {
        fDistance = fDist;
        Collision.CollisionType = INTERSECTS;
        AddOutline();
        SLogger.Log("DX11_Device.cpp - CheckCollision(): Collision TRUE");
        //AddGizmo();
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

void CDX11Device::AddOutline()
{
    CScene& SScene = CScene::GetScene();
    SScene.AddEntityToScene(m_CubeOutlineEntity);
    SceneLoc++;
}

void CDX11Device::AddGizmo()
{
    CScene& SScene = CScene::GetScene();
    CGameEntity3D& SelectedEntity = SScene.GetSceneList().at(0);
    InitArrow();
    CGameEntity3D& SelectedEntity2 = SScene.GetSceneList().at(0);
}

void CDX11Device::RemoveLineTraceTimer()
{
    CScene& SScene = CScene::GetScene();
    for (int SceneEntityIt = 0; SceneEntityIt < SScene.GetSceneList().size(); SceneEntityIt++)
    {
        if (SScene.GetSceneList().at(SceneEntityIt).m_GameEntityTag == "Linetrace")
        {
            //m_GameEntityList.at(It).m_GameEntityTag = "LinetraceNEW";
            //m_GameEntityList.erase(m_GameEntityList.begin() + It);

            SScene.GetSceneList().erase((SScene.GetSceneList().begin() + SceneEntityIt));
        }
    }
}

void CDX11Device::RemoveLastElementFromScene()
{
    CScene& SScene = CScene::GetScene();
    //SScene.m_SceneList.m_GameEntityList.pop_back();
    SScene.RemoveLastEntityFromScene();
}

void CDX11Device::Render(float RotX, float RotY, float EyeX, float EyeY, float EyeZ)
{
    CLogger& SLogger = CLogger::GetLogger();
    CScene& SScene = CScene::GetScene();
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

    m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, Colors::MidnightBlue);
    m_pImmediateContext->ClearDepthStencilView(pDefDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(SSimpleVertex);
    UINT stride2 = sizeof(SSimpleColorVertex);
    UINT offset = 0;

    int SceneEntityIndex = 0;

    /*   for (auto SceneEntityIt : SScene.GetSceneList())
       {
           ConstantBuffer cb = SceneEntityIt.GetConstantBuffer();
           cb.mWorld = XMMatrixTranspose(cb.mWorld);
           cb.mView = XMMatrixTranspose(m_ViewMatrix);
           cb.mProjection = XMMatrixTranspose(m_ProjectionMatrix);

           ID3D11Buffer* ConstantBuffer = SceneEntityIt.m_DXResConfig.m_ConstantBuffer;
           m_ImmediateContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb, 0, 0);

           ID3D11Buffer* VertexBuffer = SceneEntityIt.m_DXResConfig.m_VertexBuffer;
           assert(VertexBuffer != nullptr);

           m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

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
           m_ImmediateContext->OMSetDepthStencilState(pDefDepthStencilState, 0);

           m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
           m_ImmediateContext->DrawIndexed(512, 0, 0);

       }*/


       /**
       * Working loop, to delete whole function
       */

       //for (auto SceneEntityIt : SScene.GetSceneList())
       //{
       //    // Render Cube 
       //    if (SceneEntityIt.m_GameEntityTag == "SingleCubeComponentEntity")
       //    {
       //        if (SceneEntityIt.m_GameEntity3DComponent.size())
       //        {
       //            for (auto SceneEntityComponentIt : SceneEntityIt.m_GameEntity3DComponent)
       //            {
       //                ConstantBuffer cb = SceneEntityComponentIt.GetConstantBuffer();
       //                cb.mWorld = XMMatrixTranspose(cb.mWorld);
       //                cb.mView = XMMatrixTranspose(m_ViewMatrix);
       //                cb.mProjection = XMMatrixTranspose(m_ProjectionMatrix);

       //                ID3D11Buffer* ConstantBuffer = SceneEntityComponentIt.m_DXResConfig.m_ConstantBuffer;
       //                m_ImmediateContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb, 0, 0);

       //                ID3D11Buffer* VertexBuffer = SceneEntityComponentIt.m_DXResConfig.m_VertexBuffer;
       //                assert(VertexBuffer != nullptr);

       //                m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

       //                ID3D11Buffer* IndexBuffer = SceneEntityComponentIt.m_DXResConfig.m_IndexBuffer;
       //                DXGI_FORMAT IndexBufferFormat = SceneEntityComponentIt.m_DXResConfig.m_IndexBufferFormat;
       //                UINT IndexBufferOffset = SceneEntityComponentIt.m_DXResConfig.m_IndexBufferOffset;
       //                m_ImmediateContext->IASetIndexBuffer(IndexBuffer, IndexBufferFormat, IndexBufferOffset);

       //                ID3D11InputLayout* InputLayout = SceneEntityComponentIt.m_DXResConfig.GetInputLayout();
       //                m_ImmediateContext->IASetInputLayout(InputLayout);

       //                ID3D11VertexShader* VertexShader = SceneEntityComponentIt.m_DXResConfig.m_VertexShader;
       //                ID3D11PixelShader* PixelShader = SceneEntityComponentIt.m_DXResConfig.m_PixelShader;

       //                m_ImmediateContext->VSSetShader(VertexShader, nullptr, 0);
       //                m_ImmediateContext->PSSetShader(PixelShader, nullptr, 0);

       //                m_ImmediateContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
       //                m_ImmediateContext->OMSetDepthStencilState(pDefDepthStencilState, 0);

       //                m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
       //                m_ImmediateContext->DrawIndexed(512, 0, 0);
       //            }
       //        }
       //    }
       //    // Render Cube yellow outline
       //    else if (SceneEntityIt.m_GameEntityTag != "Linetrace")
       //    {
       //        ConstantBuffer cb = SceneEntityIt.GetConstantBuffer();
       //        cb.mWorld = XMMatrixTranspose(cb.mWorld);
       //        cb.mView = XMMatrixTranspose(m_ViewMatrix);
       //        cb.mProjection = XMMatrixTranspose(m_ProjectionMatrix);

       //        ID3D11Buffer* CB = SceneEntityIt.m_DXResConfig.m_ConstantBuffer;
       //        m_ImmediateContext->UpdateSubresource(CB, 0, nullptr, &cb, 0, 0);

       //        ID3D11Buffer* VertexBuffer = SceneEntityIt.m_DXResConfig.m_VertexBuffer;
       //        assert(VertexBuffer != nullptr);

       //        m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride2, &offset);

       //        ID3D11Buffer* IndexBuffer = SceneEntityIt.m_DXResConfig.m_IndexBuffer;
       //        DXGI_FORMAT IndexBufferFormat = SceneEntityIt.m_DXResConfig.m_IndexBufferFormat;
       //        UINT IndexBufferOffset = SceneEntityIt.m_DXResConfig.m_IndexBufferOffset;
       //        m_ImmediateContext->IASetIndexBuffer(IndexBuffer, IndexBufferFormat, IndexBufferOffset);

       //        ID3D11InputLayout* InputLayout = SceneEntityIt.m_DXResConfig.GetInputLayout();
       //        m_ImmediateContext->IASetInputLayout(InputLayout);

       //        ID3D11VertexShader* VertexShader = SceneEntityIt.m_DXResConfig.m_VertexShader;
       //        ID3D11PixelShader* PixelShader = SceneEntityIt.m_DXResConfig.m_PixelShader;

       //        m_ImmediateContext->VSSetShader(VertexShader, nullptr, 0);
       //        m_ImmediateContext->PSSetShader(PixelShader, nullptr, 0);

       //        m_ImmediateContext->VSSetConstantBuffers(0, 1, &CB);
       //        m_ImmediateContext->OMSetDepthStencilState(pDepthStencilStateOutline, 0);

       //        m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
       //        m_ImmediateContext->DrawIndexed(512, 0, 0);
       //    }
       //    else
       //    {
       //        ID3D11Buffer* VertexBuffer = SceneEntityIt.m_DXResConfig.GetVertexBuffer();
       //        m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
       //        m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

       //        ConstantBuffer cb = SceneEntityIt.GetConstantBuffer();
       //        cb.mWorld = XMMatrixTranspose(cb.mWorld);
       //        cb.mView = XMMatrixTranspose(m_ViewMatrix);
       //        cb.mProjection = XMMatrixTranspose(m_ProjectionMatrix);

       //        ID3D11Buffer* ConstantBuffer = SceneEntityIt.m_DXResConfig.m_ConstantBuffer;
       //        m_ImmediateContext->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb, 0, 0);

       //        m_ImmediateContext->Draw(2, 0);
       //    }
       //}

       //m_SwapChain->Present(0, 0);
}

void CDX11Device::CleanupDX11Device()
{
    if (m_pImmediateContext) m_pImmediateContext->ClearState();

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
    if (m_pRenderTargetView) m_pRenderTargetView->Release();
    if (m_SwapChain1) m_SwapChain1->Release();
    if (m_SwapChain) m_SwapChain->Release();
    if (m_pImmediateContext1) m_pImmediateContext1->Release();
    if (m_pImmediateContext) m_pImmediateContext->Release();
    if (m_pD3D11Device1) m_pD3D11Device1->Release();
    if (m_pD3D11Device) m_pD3D11Device->Release();
    if (m_TextureRV) m_TextureRV->Release();
}

void CDX11Device::InitDriveTypeAndFeatureLevelFinal()
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
            D3D11_SDK_VERSION, &m_pD3D11Device, &m_FreatureLevel, &m_pImmediateContext);

        if (m_HR == E_INVALIDARG)
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            m_HR = D3D11CreateDevice(nullptr, m_DriverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                D3D11_SDK_VERSION, &m_pD3D11Device, &m_FreatureLevel, &m_pImmediateContext);
        }

        if (SUCCEEDED(m_HR))
            break;
    }
    if (FAILED(m_HR))
        return;
}

IDXGIFactory1* CDX11Device::InitDXGIFactoryFinal()
{
    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        m_HR = m_pD3D11Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
        if (SUCCEEDED(m_HR))
        {
            IDXGIAdapter* adapter = nullptr;
            m_HR = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(m_HR))
            {
                m_HR = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(m_HR))
        return nullptr;

    return dxgiFactory;
}

void CDX11Device::InitSwapChainFinal(IDXGIFactory1* dxgiFactory)
{
    // Create swap chain
    IDXGIFactory2* dxgiFactory2 = nullptr;
    m_HR = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
    if (dxgiFactory2)
    {
        // DirectX 11.1 or later
        m_HR = m_pD3D11Device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&m_pD3D11Device1));
        if (SUCCEEDED(m_HR))
        {
            (void)m_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&m_pImmediateContext1));
        }

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = m_ViewportWidth;
        sd.Height = m_ViewportHeight;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        m_HR = dxgiFactory2->CreateSwapChainForHwnd(m_pD3D11Device, m_Viewport, &sd, nullptr, nullptr, &m_SwapChain1);
        if (SUCCEEDED(m_HR))
        {
            m_HR = m_SwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&m_SwapChain));
        }

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd = {};
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

        m_HR = dxgiFactory->CreateSwapChain(m_pD3D11Device, &sd, &m_SwapChain);
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    dxgiFactory->MakeWindowAssociation(m_Viewport, DXGI_MWA_NO_ALT_ENTER);

    dxgiFactory->Release();

    if (FAILED(m_HR))
        return;
}

void CDX11Device::InitRenderTargetViewFinal()
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    m_HR = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (FAILED(m_HR))
        return;

    m_HR = m_pD3D11Device->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(m_HR))
        return;

    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);
}

void CDX11Device::InitViewportFinal()
{
    D3D11_VIEWPORT vp;
    vp.Width = m_ViewportWidth;
    vp.Height = m_ViewportHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pImmediateContext->RSSetViewports(1, &vp);
}


HRESULT CDX11Device::InitBaseCube()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;
    CGameEntity3D BaseCube = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::Cube);
    BaseCube.m_GameEntityTag = "BaseCube";
    BaseCube.SetLocationF(0.0f, 0.0f, 3.0f);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"Tutorial04.fxh", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the vertex shader
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_VertexShader);
    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return m_HR;
    }
    BaseCube.SetVertexShader(m_VertexShader);
    BaseCube.m_DXResConfig.SetVertexShader(m_VertexShader);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), &m_VertexLayout);
    pVSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;

    // Set the input layout
    BaseCube.SetInputLayout(m_VertexLayout);
    BaseCube.m_DXResConfig.SetInputLayout(m_VertexLayout);
    m_pImmediateContext->IASetInputLayout(m_VertexLayout);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the pixel shader
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_PixelShader);
    pPSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;
    BaseCube.SetPixelShader(m_PixelShader);
    BaseCube.m_DXResConfig.SetPixelShader(m_PixelShader);

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

    SSimpleColorVertex vertices[] =
    {
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
    };
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleColorVertex) * 8;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = vertices;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_VertexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set vertex buffer
    UINT stride = sizeof(SSimpleColorVertex);
    UINT offset = 0;
    BaseCube.SetVertexBuffer(m_VertexBuffer);
    BaseCube.m_DXResConfig.SetVertexBuffer(m_VertexBuffer);
    m_pImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

    // Create index buffer
    WORD indices[] =
    {
        3,1,0,
        2,1,3,

        0,5,4,
        1,5,0,

        3,4,7,
        0,4,3,

        1,6,5,
        2,6,1,

        2,7,6,
        3,7,2,

        6,4,5,
        7,4,6,
    };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_IndexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set index buffer
    BaseCube.SetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    BaseCube.m_DXResConfig.SetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_pImmediateContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer);
    if (FAILED(m_HR))
        return m_HR;
    BaseCube.SetConstantBuffer(m_ConstantBuffer);
    BaseCube.m_DXResConfig.SetConstantBuffer(m_ConstantBuffer);

    SScene.AddEntityToScene(BaseCube);

    //// Initialize the world matrix
    //g_World = XMMatrixIdentity();

    //// Initialize the view matrix
    //XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
    //XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    //XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    //g_View = XMMatrixLookAtLH(Eye, At, Up);

    // Initialize the projection matrix
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.01f, 100.0f);

    return S_OK;
}

HRESULT CDX11Device::AddTestLine3()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;
    CGameEntity3D Linetrace = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::Cube);
    Linetrace.m_GameEntityTag = "Linetrace";
    Linetrace.SetLocationF(0.0f, 0.0f, 3.0f);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"Tutorial04.fxh", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the vertex shader
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_VertexShader);
    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return m_HR;
    }
    Linetrace.SetVertexShader(m_VertexShader);
    Linetrace.m_DXResConfig.SetVertexShader(m_VertexShader);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), &m_VertexLayout);
    pVSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;

    // Set the input layout
    Linetrace.SetInputLayout(m_VertexLayout);
    Linetrace.m_DXResConfig.SetInputLayout(m_VertexLayout);
    m_pImmediateContext->IASetInputLayout(m_VertexLayout);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the pixel shader
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_PixelShader);
    pPSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;
    Linetrace.SetPixelShader(m_PixelShader);
    Linetrace.m_DXResConfig.SetPixelShader(m_PixelShader);

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

    SSimpleColorVertex vertices[] =
    {
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
    };
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleColorVertex) * 8;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = vertices;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_VertexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set vertex buffer
    UINT stride = sizeof(SSimpleColorVertex);
    UINT offset = 0;
    Linetrace.SetVertexBuffer(m_VertexBuffer);
    Linetrace.m_DXResConfig.SetVertexBuffer(m_VertexBuffer);
    m_pImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

    // Create index buffer
    WORD indices[] =
    {
        3,1,0,
        2,1,3,

        0,5,4,
        1,5,0,

        3,4,7,
        0,4,3,

        1,6,5,
        2,6,1,

        2,7,6,
        3,7,2,

        6,4,5,
        7,4,6,
    };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &m_IndexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set index buffer
    Linetrace.SetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    Linetrace.m_DXResConfig.SetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_pImmediateContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer);
    if (FAILED(m_HR))
        return m_HR;
    Linetrace.SetConstantBuffer(m_ConstantBuffer);
    Linetrace.m_DXResConfig.SetConstantBuffer(m_ConstantBuffer);

    SScene.AddEntityToScene(Linetrace);

    //// Initialize the world matrix
    //g_World = XMMatrixIdentity();

    //// Initialize the view matrix
    //XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
    //XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    //XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    //g_View = XMMatrixLookAtLH(Eye, At, Up);

    // Initialize the projection matrix
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.01f, 100.0f);

    return S_OK;
}

HRESULT CDX11Device::InitTexturedCube()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;
    MainBaseCube = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::Cube);
    MainBaseCube.m_GameEntityTag = "TexturedCube";
    MainBaseCube.SetLocationF(-6.f, 0.0f, 0.0f);
    MainBaseCube.SetScale(0.25f, 0.25f, 0.25f);

	CTimerManager& TimerManager = CTimerManager::GetTimerManager();

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"TextureShader.fxh", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the vertex shader
    ID3D11VertexShader* TempVertexShader{ nullptr };
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &TempVertexShader);
    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return m_HR;
    }
    MainBaseCube.SetVertexShader(TempVertexShader);
    auto VertexShaderLambda = [=]() {
        m_pImmediateContext->VSSetShader(TempVertexShader, nullptr, 0);
        };
    MainBaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    ID3D11InputLayout* TempVertexLayout{ nullptr };
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &TempVertexLayout);
    pVSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;

    // Set the input layout
    MainBaseCube.SetInputLayout(TempVertexLayout);
    m_pImmediateContext->IASetInputLayout(TempVertexLayout);

    auto InputLayoutLambda = [=]() {
        m_pImmediateContext->IASetInputLayout(TempVertexLayout);
        };

    MainBaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"TextureShader.fxh", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the pixel shader
    ID3D11PixelShader* TempPixelShader{ nullptr };
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &TempPixelShader);
    pPSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;
    MainBaseCube.SetPixelShader(m_PixelShader);
    auto PixelShaderLambda = [=]() {
        m_pImmediateContext->PSSetShader(TempPixelShader, nullptr, 0);
        };
    MainBaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(PixelShaderLambda);

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

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }

        //       //vertex coordinates for the back face
              //{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },     // top left
              //{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },    // bottom left
              //{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },      // top right
              //{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },     // bottom right

              //// vertex coordinates for the front face
              //{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },      // top left
              //{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },     // bottom left
              //{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },       // top right
              //{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },      // bottom right

        //      { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },     // #0 back top left
        //      { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },      // #1 back top right
        //      { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },       // #2 front top right
        //      { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },      // #3 front top left
        //      { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },    // #4 back bottom left
        //      { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },     // #5 back bottom right
        //      { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },      // #6 front bottom right
              //{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },     // #7 front bottom left
    };

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleVertex) * 36;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    ID3D11Buffer* TempVertexBuffer{ nullptr };
    D3D11_SUBRESOURCE_DATA InitData{};
    InitData.pSysMem = vertices;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &TempVertexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set vertex buffer
    UINT stride = sizeof(SSimpleVertex);
    UINT offset = 0;
    MainBaseCube.SetVertexBuffer(TempVertexBuffer);
    m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);

    auto VertexBufferLambda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);
        };
    MainBaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);


    // Create index buffer
    WORD indices[] =
    {
        3,1,0,
        2,1,3,

        0,5,4,
        1,5,0,

        3,4,7,
        0,4,3,

        1,6,5,
        2,6,1,

        2,7,6,
        3,7,2,

        6,4,5,
        7,4,6,
    };

    ID3D11Buffer* TempIndexBuffer{ nullptr };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &TempIndexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set index buffer
    MainBaseCube.SetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    auto IndexBufferLambda = [=]() {
        m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        };
    MainBaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

    // Set primitive topology
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    ID3D11Buffer* TempConstantBuffer{ nullptr };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &TempConstantBuffer);
    if (FAILED(m_HR))
        return m_HR;
    MainBaseCube.SetConstantBuffer(TempConstantBuffer);
    auto ConstantBufferLambda = [=]() {
        m_pImmediateContext->VSSetConstantBuffers(0, 1, &TempConstantBuffer);
        };
    MainBaseCube.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    MainBaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);


    const wchar_t* TextureName = L"tex_stickman.dds";
    m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureColorGridRV);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
        return m_HR;
    }
    auto TextureLambda = [=]() {
        m_pImmediateContext->PSSetShaderResources(0, 1, &m_TextureColorGridRV);
    };
    MainBaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(TextureLambda);

    //D3D11_SAMPLER_DESC sampDesc{};
    //ZeroMemory(&sampDesc, sizeof(sampDesc));
    //sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    //sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    //sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    //sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    //sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    //sampDesc.MinLOD = 0;
    //sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    //m_HR = m_pD3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);
    //if (FAILED(m_HR))
    //    return m_HR;

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

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    m_HR = m_pD3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);
    if (FAILED(m_HR))
        return m_HR;

    m_HR = m_pD3D11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerState);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to create rasterizer state", L"Error", MB_OK);
        return m_HR;
    }

    m_pImmediateContext->RSSetState(m_RasterizerState);

    //auto TimerManager = CTimerManager::GetTimerManager();



    //for (auto SceneEntityIt : SScene.GetSceneList())
    //{
    //    if (SceneEntityIt.m_GameEntityTag == "TexturedCube")
    //    {
    //        InterpMoveCubeRef = &SceneEntityIt;
    //    }
    //}

    // NOT WORKING
    //CTimerManager& TimerManager = CTimerManager::GetTimerManager();
    InterpMoveCubeRef = &MainBaseCube;
	TimerManager.SetTimer3<CDX11Device, void, &CDX11Device::InterpMoveCube>(this, 2.0f, 10.0f);

    // Initialize the world matrix
    //g_World = XMMatrixIdentity();

    //// Initialize the view matrix
    //XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
    //XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    //XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    //g_View = XMMatrixLookAtLH(Eye, At, Up);

    // Initialize the projection matrix
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.01f, 100.0f);

    SScene.AddEntityToScene(MainBaseCube);

    return S_OK;
}

void CDX11Device::InterpMoveCube()
{
	CLogger& Logger = CLogger::GetLogger();
    Logger.Log("InterpMoveCube Called");
    CTimerManager& TimerManager = CTimerManager::GetTimerManager();

	//CScene& Scene = CScene::GetScene();

    //while (InterpMoveLoc != 10.0f)
    //{
        InterpMoveLoc = FInterpConstantTo(InterpMoveLoc, 10.0f, TimerManager.m_pCoreClock->GetFDeltaTime(), 0.9f);
		//Logger.Log("InterpMoveCube Function, InterpMoveLoc = ", InterpMoveLoc);
		//MainBaseCube.SetLocationF(7.f, 0.0f, InterpMoveLoc);
    //}
    //InterpMoveLoc += 0.1f;

        //CScene& Scene = CScene::

    CScene& Scene = CScene::GetScene();
    for (auto& SceneIt : Scene.GetSceneList())
    {
		if (SceneIt.m_GameEntityTag == "TexturedCube")
		{
            Logger.Log("InterpMoveCube Function, InterpMoveLoc = ", InterpMoveLoc);
			SceneIt.SetLocationF(-6.f, 0.0f, InterpMoveLoc);
		}
    }




}

HRESULT CDX11Device::InitTexturedCube2()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;
    CGameEntity3D BaseCube = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::Cube);
    BaseCube.m_GameEntityTag = "TexturedCube";
    BaseCube.SetLocationF(0.0f, 0.0f, 2.0f);
    BaseCube.SetScale(0.5f, 0.5f, 0.5f);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"TextureShader.fxh", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the vertex shader
    ID3D11VertexShader* TempVertexShader{ nullptr };
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &TempVertexShader);
    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return m_HR;
    }
    BaseCube.SetVertexShader(TempVertexShader);
    auto VertexShaderLambda = [=]() {
        m_pImmediateContext->VSSetShader(TempVertexShader, nullptr, 0);
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
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &TempVertexLayout);
    pVSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;

    // Set the input layout
    BaseCube.SetInputLayout(TempVertexLayout);
    m_pImmediateContext->IASetInputLayout(TempVertexLayout);

    auto InputLayoutLambda = [=]() {
        m_pImmediateContext->IASetInputLayout(TempVertexLayout);
        };

    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"TextureShader.fxh", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the pixel shader
    ID3D11PixelShader* TempPixelShader{ nullptr };
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &TempPixelShader);
    pPSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;
    BaseCube.SetPixelShader(m_PixelShader);
    auto PixelShaderLambda = [=]() {
        m_pImmediateContext->PSSetShader(TempPixelShader, nullptr, 0);
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

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }
    };

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleVertex) * 25;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    ID3D11Buffer* TempVertexBuffer{ nullptr };
    D3D11_SUBRESOURCE_DATA InitData{};
    InitData.pSysMem = vertices;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &TempVertexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set vertex buffer
    UINT stride = sizeof(SSimpleVertex);
    UINT offset = 0;
    BaseCube.SetVertexBuffer(TempVertexBuffer);
    m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);

    auto VertexBufferLambda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);
        };
    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);


    // Create index buffer
    WORD indices[] =
    {
        3, 1, 0,
        2, 1, 3,

        6, 4, 5,
        7, 4, 6,

        11, 9, 8,
        10, 9, 11,

        14, 12, 13,
        15, 12, 14,

        19, 17, 16,
        18, 17, 19,

        22, 20, 21,
        23, 20, 22

    };

    ID3D11Buffer* TempIndexBuffer{ nullptr };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &TempIndexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set index buffer
    BaseCube.SetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    auto IndexBufferLambda = [=]() {
        m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        };

    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

    // Set primitive topology
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    ID3D11Buffer* TempConstantBuffer{ nullptr };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &TempConstantBuffer);
    if (FAILED(m_HR))
        return m_HR;
    BaseCube.SetConstantBuffer(TempConstantBuffer);
    auto ConstantBufferLambda = [=]()
        {
            m_pImmediateContext->VSSetConstantBuffers(0, 1, &TempConstantBuffer);
        };
    BaseCube.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    BaseCube.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);


    //const wchar_t* TextureName = L"grey_grid.dds";
    const wchar_t* TextureName = L"tex_stickman.dds";
    m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureRV);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
        return m_HR;
    }
    auto TextureLambda = [=]() {
        m_pImmediateContext->PSSetShaderResources(0, 1, &m_TextureRV);
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
    m_HR = m_pD3D11Device->CreateSamplerState(&sampDesc, &m_SamplerLinear);
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
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.01f, 100.0f);

    return S_OK;
}



void CDX11Device::RenderFinal(float RotX, float RotY, float EyeX, float EyeY, float EyeZ)
{
    CScene& SScene = CScene::GetScene();

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

    CDX11Device::m_ViewMatrix = XMMatrixIdentity();
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
    CDX11Device::m_ViewMatrix = XMMatrixTranslation(-EyeX, -EyeY, -EyeZ) * RotationMatrixXY;

    m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, Colors::MidnightBlue);

    //auto SceneEntityList = SScene.GetSceneList();

    //for (auto SceneEntityIt : SceneEntityList)
    //{
    //    if (SceneEntityIt.m_GameEntityTag == "BaseCube")
    //    {
    //        Constant_Buffer CB = SceneEntityIt.GetConstantBuffer();
    //        CB.mWorld = XMMatrixTranspose(CB.mWorld);
    //        CB.mView = XMMatrixTranspose(m_ViewMatrix);
    //        CB.mProjection = XMMatrixTranspose(m_ProjectionMatrix);

    //        ID3D11Buffer* CB2 = SceneEntityIt.m_DXResConfig.GetConstantBuffer();
    //        //g_pImmediateContext->UpdateSubresource(CB2, 0, nullptr, &CB, 0, 0);
    //        m_ImmediateContext->UpdateSubresource(CB2, 0, nullptr, &CB, 0, 0);

    //        ID3D11VertexShader* VertexShader = SceneEntityIt.m_DXResConfig.GetVertexShader();
    //        ID3D11PixelShader* PixelShader = SceneEntityIt.m_DXResConfig.GetPixelShader();

    //        m_ImmediateContext->VSSetShader(VertexShader, nullptr, 0);
    //        m_ImmediateContext->VSSetConstantBuffers(0, 1, &CB2);
    //        m_ImmediateContext->PSSetShader(PixelShader, nullptr, 0);
    //        m_ImmediateContext->DrawIndexed(36, 0, 0);
    //    }
    //    else if (SceneEntityIt.m_GameEntityTag == "TexturedCube")
    //    {
    //        Constant_Buffer CB = SceneEntityIt.GetConstantBuffer();
    //        CB.mWorld = XMMatrixTranspose(CB.mWorld);
    //        CB.mView = XMMatrixTranspose(m_ViewMatrix);
    //        CB.mProjection = XMMatrixTranspose(m_ProjectionMatrix);

    //        ID3D11Buffer* CB2 = SceneEntityIt.m_DXResConfig.GetConstantBuffer();
    //        //g_pImmediateContext->UpdateSubresource(CB2, 0, nullptr, &CB, 0, 0);
    //        m_ImmediateContext->UpdateSubresource(CB2, 0, nullptr, &CB, 0, 0);

    //        ID3D11VertexShader* VertexShader = SceneEntityIt.m_DXResConfig.GetVertexShader();
    //        ID3D11PixelShader* PixelShader = SceneEntityIt.m_DXResConfig.GetPixelShader();

    //        m_ImmediateContext->PSSetShaderResources(0, 1, &m_TextureRV);

    //        m_ImmediateContext->OMSetDepthStencilState(pDefDepthStencilState, 0);

    //        m_ImmediateContext->VSSetShader(VertexShader, nullptr, 0);
    //        m_ImmediateContext->VSSetConstantBuffers(0, 1, &CB2);
    //        m_ImmediateContext->PSSetShader(PixelShader, nullptr, 0);
    //        m_ImmediateContext->DrawIndexed(36, 0, 0);
    //    }
    //}

    //m_SwapChain->Present(0, 0);
}
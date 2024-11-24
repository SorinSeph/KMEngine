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
    //InterpMoveCube();InitTexturedCube2
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
    CGameEntity3D& SelectedEntity2 = SScene.GetSceneList().at(0);
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

HRESULT CDX11Device::InitTexturedCube()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;

	CubeEntity.m_GameEntityTag = "TexturedCube";

    CubeEntityComponent.m_GameEntityTag = "TexturedCubeComponent";
    CubeEntityComponent.SetLocationF(-1.f, 0.0f, 0.0f);
    CubeEntityComponent.SetScale(0.25f, 0.25f, 0.25f);

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
    auto VertexShaderLambda = [=]() {
        m_pImmediateContext->VSSetShader(TempVertexShader, nullptr, 0);
    };
    CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

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
    m_pImmediateContext->IASetInputLayout(TempVertexLayout);

    auto InputLayoutLambda = [=]() {
        m_pImmediateContext->IASetInputLayout(TempVertexLayout);
        };

    CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

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

    auto PixelShaderLambda = [=]() {
        m_pImmediateContext->PSSetShader(TempPixelShader, nullptr, 0);
        };
    CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(PixelShaderLambda);

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

            { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
            { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
            { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
            { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

        //{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
        //{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
        //{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
        //{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

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
    m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);

    auto VertexBufferLambda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);
        };
    CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);


    // Create index buffer
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

        //3,1,0,
        //2,1,3,

        //0,5,4,
        //1,5,0,

        //3,4,7,
        //0,4,3,

        //1,6,5,
        //2,6,1,

        //2,7,6,
        //3,7,2,

        //6,4,5,
        //7,4,6,
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
    m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    auto IndexBufferLambda = [=]() {
        m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        };
    CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

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

    auto ConstantBufferLambda = [=]() {
        m_pImmediateContext->VSSetConstantBuffers(0, 1, &TempConstantBuffer);
        };
    CubeEntityComponent.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);


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
    CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(TextureLambda);

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

    InterpMoveCubeRef = &CubeEntityComponent;
	TimerManager.SetTimer3<CDX11Device, void, &CDX11Device::InterpMoveCube>(this, 2.0f, 10.0f);

    // Initialize the projection matrix
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.01f, 100.0f);

    CSceneGraphNode<CGameEntity3DComponent>* CubeComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
    CubeComponentNode->Type = CubeEntityComponent;

	CubeEntity.m_SceneGraph.m_pRootNode = CubeComponentNode;

    SScene.AddEntityToScene(CubeEntity);
    //SScene.AddEntityToScene(CubeEntityComponent);

    return S_OK;
}

void CDX11Device::InterpMoveCube()
{
	CLogger& Logger = CLogger::GetLogger();
    Logger.Log("InterpMoveCube Called");
    CTimerManager& TimerManager = CTimerManager::GetTimerManager();

    InterpMoveLoc = FInterpConstantTo(InterpMoveLoc, 10.0f, TimerManager.m_pCoreClock->GetFDeltaTime(), 0.9f);

    CScene& Scene = CScene::GetScene();
    for (auto& SceneIt : Scene.GetSceneList())
    {
		if (SceneIt.m_GameEntityTag == "TexturedCube")
		{
			auto& EntityComponent = SceneIt.m_SceneGraph.m_pRootNode->Type;
            //auto& EntityComponent2 = SceneIt.m_SceneGraph.m_pRootNode->ChildNode.at(0)->Type;
            Logger.Log("InterpMoveCube Function, InterpMoveLoc = ", InterpMoveLoc);
            EntityComponent.SetLocationF(-1.f, 0.0f, InterpMoveLoc);
            //EntityComponent2.SetLocationF(-6.f, 0.0f, InterpMoveLoc);
            XMVECTOR quaternionRotation = XMQuaternionRotationRollPitchYaw(
                0.0f,
                XMConvertToRadians(InterpMoveLoc * 50),
                0.0f
            );
            EntityComponent.m_QuatRotationMatrix = XMMatrixRotationQuaternion(quaternionRotation);
		}
    }
}

HRESULT CDX11Device::InitTexturedCube2()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;

    CGameEntity3DComponent Cube;
    //Cube = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::Cube);
    Cube.m_GameEntityTag = "TexturedCube2";
    Cube.SetLocationF(-6.f, 3.0f, 0.0f);
    Cube.SetScale(0.25f, 0.25f, 0.25f);

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
    auto VertexShaderLambda = [=]() {
        m_pImmediateContext->VSSetShader(TempVertexShader, nullptr, 0);
    };
    Cube.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

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
    m_pImmediateContext->IASetInputLayout(TempVertexLayout);

    auto InputLayoutLambda = [=]() {
        m_pImmediateContext->IASetInputLayout(TempVertexLayout);
    };

    Cube.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

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

    auto PixelShaderLambda = [=]() {
        m_pImmediateContext->PSSetShader(TempPixelShader, nullptr, 0);
    };
    Cube.m_DXResConfig.m_pContextResourcePtr.push_back(PixelShaderLambda);

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
    m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);

    auto VertexBufferLambda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);
    };
    Cube.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);


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
    m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    auto IndexBufferLambda = [=]() {
        m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    };
    Cube.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

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

    auto ConstantBufferLambda = [=]() {
        m_pImmediateContext->VSSetConstantBuffers(0, 1, &TempConstantBuffer);
    };
    Cube.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    Cube.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);


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
    Cube.m_DXResConfig.m_pContextResourcePtr.push_back(TextureLambda);

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

    // Initialize the projection matrix
    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.01f, 100.0f);

	CSceneGraphNode<CGameEntity3DComponent>* CubeSubcomponent = new CSceneGraphNode<CGameEntity3DComponent>();
	CubeSubcomponent->Type = Cube;

	CubeEntity.m_SceneGraph.m_pRootNode->ChildNode.push_back(CubeSubcomponent);

    SScene.AddEntityToScene(CubeEntity);
    //SScene.AddEntityToScene(Cube);

    return S_OK;
}
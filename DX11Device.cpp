#include <bitset>
#include <locale>
#include <codecvt>

#include "Core/CoreClock.h"
#include "DX11Device.h"
#include "Renderer/Renderer.h"
#include "Modules/GraphicsModule.h"
#include "Modules/UIModule.h"
#include "Modules/PhysicsModule.h"

#define FLT_MAX          3.402823466e+38F

int gClicked = 0;

using namespace DirectX;
using namespace MathInternal;
using namespace std;

XMMATRIX CCamera::m_WorldMatrix{ XMMatrixIdentity() };
XMMATRIX CCamera::m_ViewMatrix{ XMMatrixIdentity() };
XMMATRIX CCamera::m_ProjectionMatrix;
XMMATRIX CCamera::m_MVPMatrix{ XMMatrixIdentity() };

int SceneLoc = 0;

/* Int for color highlight in gizmo shader */
int ArrowClicked = 0;

XMGLOBALCONST XMVECTORF32 _RayEpsilon = { { { 1e-20f, 1e-20f, 1e-20f, 1e-20f } } };
XMGLOBALCONST XMVECTORF32 _FltMin = { { { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX } } };
XMGLOBALCONST XMVECTORF32 _FltMax = { { { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX } } };

HRESULT CDX11Device::InitDX11Device()
{
	CLogger& Logger = CLogger::GetLogger();
    CCamera::m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.01f, 100.0f);

    InitDriveTypeAndFeatureLevelFinal();
    IDXGIFactory1* dxgiFactory = InitDXGIFactoryFinal();
    InitSwapChainFinal(dxgiFactory);
    InitRenderTargetViewFinal();
    InitViewport();
    //InitDisabledDepthStencil();
    InitDefaultDepthStencil();
    InitRasterizerState();
    InitFrustum();
    InitSolidColorCube();
    //InitPlane();
    //InitSingleCubeOutline();
    InitTexturedCube();

    if (m_pRenderer)
    {
        if (m_pRenderer->m_pGraphicsModule)
        {
			Logger.Log("CDX11Device::InitDX11Device(): m_pRenderer->m_pGraphicsModule is not null");
            m_pRenderer->m_pGraphicsModule->Notify([this]() {
                CLogger& Logger = CLogger::GetLogger();

				auto UIModule = static_cast<CUIModule*>(m_pRenderer->m_pGraphicsModule->m_pMediator->m_ModuleArray[0]);

                if (UIModule)
                {
					CScene& Scene = CScene::GetScene();
                    auto FirstEntity = Scene.GetSceneList().at(0).m_GameEntityTag; // Gets the frustum
                    const std::wstring newText = std::wstring(FirstEntity.begin(), FirstEntity.end());
                    SetWindowText(CRightSubwindow::m_OutlinerHwnd, newText.c_str());
                    InvalidateRect(CRightSubwindow::m_OutlinerHwnd, NULL, TRUE);
                    UpdateWindow(CRightSubwindow::m_OutlinerHwnd);
                }
            });
        }
    }

    return S_OK;
}

void CDX11Device::SetViewport(HWND InViewport)
{
    if (InViewport)
        m_Viewport = InViewport;
}

void CDX11Device::SetViewportSize(int ViewportWidth, int ViewportHeight)
{
	m_ViewportWidth = ViewportWidth;
	m_ViewportHeight = ViewportHeight;
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

void CDX11Device::InitDefaultDepthStencil()
{
    ID3D11Texture2D* pDepthStencil = NULL;
    DefDepthDesc.Width = m_ViewportWidth;
    DefDepthDesc.Height = m_ViewportHeight;
    DefDepthDesc.MipLevels = 1;
    DefDepthDesc.ArraySize = 1;
    DefDepthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    DefDepthDesc.SampleDesc.Count = 1;
    DefDepthDesc.SampleDesc.Quality = 0;
    DefDepthDesc.Usage = D3D11_USAGE_DEFAULT;
    DefDepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    DefDepthDesc.CPUAccessFlags = 0;
    DefDepthDesc.MiscFlags = 0;
    m_HR = m_pD3D11Device->CreateTexture2D(&DefDepthDesc, NULL, &pDepthStencil);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize depth stencil", L"Error", MB_OK);
        return;
    }

    DefDepthStencilViewDesc.Format = DefDepthDesc.Format;
    DefDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    DefDepthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view
    m_HR = m_pD3D11Device->CreateDepthStencilView(pDepthStencil, // Depth stencil texture
        &DefDepthStencilViewDesc, // Depth stencil desc
        &pDefDepthStencilView);  // [out] Depth stencil view
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize depth stencil view", L"Error", MB_OK);
        return;
    }

    // Depth test parameters
    DefDepthStencilDesc.DepthEnable = true;
    DefDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DefDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    // Stencil test parameters
    DefDepthStencilDesc.StencilEnable = true;
    DefDepthStencilDesc.StencilReadMask = 0xFF;
    DefDepthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    DefDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    DefDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    DefDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    DefDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    DefDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    DefDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    DefDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    DefDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create depth stencil state
    m_pD3D11Device->CreateDepthStencilState(&DefDepthStencilDesc, &pDefDepthStencilState);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize depth stencil state", L"Error", MB_OK);
        return;
    }

    // Bind the depth stencil view
    m_pImmediateContext->OMSetRenderTargets(1,          // One rendertarget view
        &m_pRenderTargetView,      // Render target view, created earlier
        pDefDepthStencilView);     // Depth stencil view for the render target


    m_pImmediateContext->OMSetDepthStencilState(pDefDepthStencilState, 0);
}

void CDX11Device::InitDisabledDepthStencil()
{
	CLogger& Logger = CLogger::GetLogger();

	Logger.Log("CDX11Device::InitDisabledDepthStencil()");

    // Depth test parameters
    DisabledDepthDesc.DepthEnable = true;
    DisabledDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DisabledDepthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil test parameters
    DisabledDepthDesc.StencilEnable = true;
    DisabledDepthDesc.StencilReadMask = 0xFF;
    DisabledDepthDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    DisabledDepthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    DisabledDepthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    DisabledDepthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    DisabledDepthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    DisabledDepthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    DisabledDepthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    DisabledDepthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    DisabledDepthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create depth stencil state
    m_pD3D11Device->CreateDepthStencilState(&DisabledDepthDesc, &pDisabledDepthStencilState);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize depth stencil state", L"Error", MB_OK);
        return;
    }

    // Bind the depth stencil view
    m_pImmediateContext->OMSetRenderTargets(1,          // One rendertarget view
        &m_pRenderTargetView,      // Render target view, created earlier
        pDisabledDepthStencilView);     // Depth stencil view for the render target


    //m_pImmediateContext->OMSetDepthStencilState(pDisabledDepthStencilState, 0);
}

void CDX11Device::InitOutlineDepthStencil()
{
    CLogger& Logger = CLogger::GetLogger();

    Logger.Log("CDX11Device::InitDisabledDepthStencil()");

    // Configure the depth stencil state for outlining
    OutlineDepthStencilDesc.DepthEnable = true;
    OutlineDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    OutlineDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    OutlineDepthStencilDesc.StencilEnable = true;
    OutlineDepthStencilDesc.StencilReadMask = 0xFF;
    OutlineDepthStencilDesc.StencilWriteMask = 0xFF;

    // Front face stencil operations
    OutlineDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    // Back face stencil operations
    OutlineDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    OutlineDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

    // Create depth stencil state
    m_pD3D11Device->CreateDepthStencilState(&OutlineDepthStencilDesc, &pOutlineDepthStencilState);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize depth stencil state", L"Error", MB_OK);
        return;
    }

    // Bind the depth stencil view
    m_pImmediateContext->OMSetRenderTargets(1,          // One rendertarget view
        &m_pRenderTargetView,      // Render target view, created earlier
        pOutlineDepthStencilView);     // Depth stencil view for the render target

    m_pImmediateContext->OMSetDepthStencilState(pOutlineDepthStencilState, 0);
}

void CDX11Device::InitRasterizerState()
{
    D3D11_RASTERIZER_DESC RasterDesc = {};
    RasterDesc.FillMode = D3D11_FILL_SOLID;
    RasterDesc.CullMode = D3D11_CULL_NONE;
    RasterDesc.FrontCounterClockwise = false;
    RasterDesc.DepthBias = 0;
    RasterDesc.DepthBiasClamp = 0.0f;
    RasterDesc.SlopeScaledDepthBias = 0.0f;
    RasterDesc.DepthClipEnable = true;
    RasterDesc.ScissorEnable = false;
    RasterDesc.MultisampleEnable = false;
    RasterDesc.AntialiasedLineEnable = false;

    m_HR = m_pD3D11Device->CreateRasterizerState(&RasterDesc, &m_RasterizerState);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to create rasterizer state", L"Error", MB_OK);
        return;
    }

    m_pImmediateContext->RSSetState(m_RasterizerState);
}

HRESULT CDX11Device::CompileShaderFromFile(const wchar_t* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG

    dwShaderFlags |= D3DCOMPILE_DEBUG;

    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

void CDX11Device::InitSingleCubeOutline()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;

    CGameEntity3DComponent OutlineCubeEntity;
    OutlineCubeEntity.m_GameEntityTag = "OutlineCubeEntity";
    OutlineCubeEntity.m_GameEntityType = EGameEntityType::Cube;

    CGameEntity3DComponent OutlineCubeComponent;
    OutlineCubeComponent.m_GameEntityTag = "OutlineCubeComponent";
    OutlineCubeComponent.SetLocationF(-5.f, 0.0f, 0.0f);
    OutlineCubeComponent.SetScale(0.30f, 0.30f, 0.30f);

    CTimerManager& TimerManager = CTimerManager::GetTimerManager();

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"SolidColorShader.fxh", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return;
    }

    // Create the vertex shader
    ID3D11VertexShader* VertexShader{ nullptr };
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &VertexShader);
    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return;
    }
    auto VertexShaderLambda = [=]() {
        m_pImmediateContext->VSSetShader(VertexShader, nullptr, 0);
    };
    OutlineCubeComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

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
    //m_pImmediateContext->IASetInputLayout(TempVertexLayout);

    auto InputLayoutLambda = [=]() {
        m_pImmediateContext->IASetInputLayout(TempVertexLayout);
    };

    OutlineCubeComponent.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"SolidColorShader.fxh", "PS", "ps_5_0", &pPSBlob);
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

    auto PixelShaderLambda = [=]() {
        m_pImmediateContext->PSSetShader(TempPixelShader, nullptr, 0);
    };
    OutlineCubeComponent.m_DXResConfig.m_pContextResourcePtr.push_back(PixelShaderLambda);

    SSimpleColorVertex vertices[] =
    {
        { XMFLOAT3(-1.f,  1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },   // V0
        { XMFLOAT3(1.f,  1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },    //V3
        { XMFLOAT3(1.f, -1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },    // V2
        { XMFLOAT3(-1.f, -1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },   // V1
        { XMFLOAT3(-1.f,  1.f, -1.f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },   //V4
        { XMFLOAT3(1.f,  1.f,  -1.f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },   //V5
        { XMFLOAT3(1.f,  -1.f,  -1.f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },  //V7
        { XMFLOAT3(-1.f,  -1.f, -1.f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },  //V6
    };

    D3D11_BUFFER_DESC BufferDescriptor{};
    BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    BufferDescriptor.ByteWidth = sizeof(SSimpleColorVertex) * 24;
    BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    BufferDescriptor.CPUAccessFlags = 0;

    ID3D11Buffer* TempVertexBuffer{ nullptr };
    D3D11_SUBRESOURCE_DATA InitData{};
    InitData.pSysMem = vertices;
    m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, &InitData, &TempVertexBuffer);
    if (FAILED(m_HR))
        return;

    // Set vertex buffer
    UINT stride = sizeof(SSimpleColorVertex);
    UINT offset = 0;
    m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);

    auto VertexBufferLambda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);
    };
    OutlineCubeComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);


    // Create index buffer
    WORD Indices[] =
    {
        0, 1, 2,
        0, 2, 3,

        4, 5, 6,
        4, 6, 7,

        0, 4, 5,
        0, 5, 1,

        1, 5, 6,
        1, 6, 2,

        2, 6, 7,
        2, 7, 3,

        3, 7, 4,
        3, 4, 0,
    };

    ID3D11Buffer* IndexBuffer{ nullptr };
    BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    BufferDescriptor.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    BufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
    BufferDescriptor.CPUAccessFlags = 0;
    InitData.pSysMem = Indices;
    m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, &InitData, &IndexBuffer);
    if (FAILED(m_HR))
        return;

    // Set index buffer
    m_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    auto IndexBufferLambda = [=]() {
        m_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    };
    OutlineCubeComponent.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

    // Set primitive topology
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    ID3D11Buffer* TempConstantBuffer{ nullptr };
    BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    BufferDescriptor.ByteWidth = sizeof(SConstantBuffer);
    BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    BufferDescriptor.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, nullptr, &TempConstantBuffer);
    if (FAILED(m_HR))
        return;

    auto ConstantBufferLambda = [=]() {
        m_pImmediateContext->VSSetConstantBuffers(0, 1, &TempConstantBuffer);
    };
    OutlineCubeComponent.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    OutlineCubeComponent.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);


    //const wchar_t* TextureName = L"tex_stickman.dds";
    //m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureColorGridRV);
    //if (FAILED(m_HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
    //    return m_HR;
    //}
    //auto TextureLambda = [=]() {
    //    m_pImmediateContext->PSSetShaderResources(0, 1, &m_TextureColorGridRV);
    //};
    //CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(TextureLambda);

    auto RasterizerStateLambda = [=]() {
        ID3D11RasterizerState* RasterizerState{ nullptr };
        D3D11_RASTERIZER_DESC RasterDesc = {};
        RasterDesc.FillMode = D3D11_FILL_SOLID;
        RasterDesc.CullMode = D3D11_CULL_NONE;
        RasterDesc.FrontCounterClockwise = false;
        RasterDesc.DepthBias = 0;
        RasterDesc.DepthBiasClamp = 0.0f;
        RasterDesc.SlopeScaledDepthBias = 0.0f;
        RasterDesc.DepthClipEnable = true;
        RasterDesc.ScissorEnable = false;
        RasterDesc.MultisampleEnable = false;
        RasterDesc.AntialiasedLineEnable = false;

        m_HR = m_pD3D11Device->CreateRasterizerState(&RasterDesc, &RasterizerState);
        if (FAILED(m_HR))
        {
            MessageBox(nullptr, L"Failed to create rasterizer state", L"Error", MB_OK);
            return;
        }

        m_pImmediateContext->RSSetState(RasterizerState);
    };

    OutlineCubeComponent.m_DXResConfig.m_pContextResourcePtr.push_back(RasterizerStateLambda);


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
        return;

    CSceneGraphNode<CGameEntity3DComponent>* CubeComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
    CubeComponentNode->m_tType = OutlineCubeComponent;

    OutlineCubeEntity.m_SceneGraph.m_pRootNode = CubeComponentNode;

    TimerManager.SetTimer3<CDX11Device, void, &CDX11Device::InterpMoveEntity>(this, 2.0f, 30.0f);



    SScene.AddEntityToScene(OutlineCubeEntity);
    //SScene.AddEntityToScene(CubeEntityComponent);

    //D3D11_RASTERIZER_DESC RasterDesc = {};
    //RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    //RasterDesc.CullMode = D3D11_CULL_NONE;
    //RasterDesc.FrontCounterClockwise = false;
    //RasterDesc.DepthBias = 0;
    //RasterDesc.DepthBiasClamp = 0.0f;
    //RasterDesc.SlopeScaledDepthBias = 0.0f;
    //RasterDesc.DepthClipEnable = true;
    //RasterDesc.ScissorEnable = false;
    //RasterDesc.MultisampleEnable = false;
    //RasterDesc.AntialiasedLineEnable = false;

    //m_HR = m_pD3D11Device->CreateRasterizerState(&RasterDesc, &m_RasterizerState);
    //if (FAILED(m_HR))
    //{
    //    MessageBox(nullptr, L"Failed to create rasterizer state", L"Error", MB_OK);
    //    return m_HR;
    //}


}



HRESULT CDX11Device::SpawnGizmo(const CGameEntity3D& SelectedEntity)
{
    //m_PostRenderPtr[0] = &CDX11Device::InitDefaultDepthStencil3;

    //InitDisabledDepthStencil();

    // Body of InitDisabledDepthStencicl

    /*defDepthStencilDesc3.DepthEnable = true;
    defDepthStencilDesc3.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;*/
    //defDepthStencilDesc3.DepthFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil test parameters
    //defDepthStencilDesc3.StencilEnable = true;
    //defDepthStencilDesc3.StencilReadMask = 0xFF;
    //defDepthStencilDesc3.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    //defDepthStencilDesc3.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc3.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    //defDepthStencilDesc3.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc3.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    //defDepthStencilDesc3.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc3.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    //defDepthStencilDesc3.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc3.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create depth stencil state
    //m_pD3D11Device->CreateDepthStencilState(&defDepthStencilDesc3, &pDefDepthStencilState3);
    //if (FAILED(m_HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize depth stencil state", L"Error", MB_OK);
    //    return m_HR;
    //}




    // Bind the depth stencil view
    //m_pImmediateContext->OMSetRenderTargets(1,          // One rendertarget view
    //    &m_pRenderTargetView,      // Render target view, created earlier
    //    pDefDepthStencilView3);     // Depth stencil view for the render target


    //m_pImmediateContext->OMSetDepthStencilState(pDefDepthStencilState3, 0);

    // End of body of InitDisabledDepthStencil

    //m_PreRenderPtr[0] = &CDX11Device::DisableDepthStencil;

    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;

	CLogger& Logger = CLogger::GetLogger();
    Logger.Log("CDX11Device::SpawnGizmo");

    CGameEntity3D GizmoEntity;// = GeometryFactory.CreateEntity3D(EGameEntityType::Arrow);
    GizmoEntity.m_GameEntityTag = "Gizmo";
    GizmoEntity.m_GameEntityType = EGameEntityType::Arrow;

    CGameEntity3DComponent GizmoComponent;
    GizmoComponent.m_GameEntityTag = "GizmoComponent";
    GeometryFactory.CreatePhysicalMesh(GizmoComponent.PhysicalMesh, EPrimitiveGeometryType::Arrow);

    float SelectedEntityLocationX = SelectedEntity.GetFloatLocationX();
    float SelectedEntityLocationY = SelectedEntity.GetFloatLocationY();
    float SelectedEntityLocationZ = SelectedEntity.GetFloatLocationZ();
    GizmoComponent.SetLocationF(SelectedEntityLocationX, SelectedEntityLocationY, SelectedEntityLocationZ);

    GizmoComponent.SetScale(0.25f, 0.25f, 0.25f);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"GizmoArrowShader.fxh", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the vertex shader
    ID3D11VertexShader* pVertexShader{ nullptr };
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pVertexShader);
    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return m_HR;
    }
    auto VertexShaderLambda = [=]() {
        m_pImmediateContext->VSSetShader(pVertexShader, nullptr, 0);
    };
    GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    ID3D11InputLayout* pVertexLayout{ nullptr };
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &pVertexLayout);
    pVSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;

    // Set the input layout
    m_pImmediateContext->IASetInputLayout(pVertexLayout);

    auto InputLayoutLambda = [=]() {
        m_pImmediateContext->IASetInputLayout(pVertexLayout);
    };

    GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"GizmoArrowShader.fxh", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the pixel shader
    ID3D11PixelShader* pPixelShader{ nullptr };
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pPixelShader);
    pPSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;

    auto PixelShaderLambda = [=]() {
        m_pImmediateContext->PSSetShader(pPixelShader, nullptr, 0);
    };
    GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(PixelShaderLambda);

    //SSimpleColorVertex vertices[] =
    //{
    //    { XMFLOAT3(-1.f,  1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   // V0
    //    { XMFLOAT3(1.f,  1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },    //V3
    //    { XMFLOAT3(1.f, -1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },    // V2
    //    { XMFLOAT3(-1.f, -1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   // V1
    //    { XMFLOAT3(-1.f,  1.f, -1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   //V4
    //    { XMFLOAT3(1.f,  1.f,  -1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   //V5
    //    { XMFLOAT3(1.f,  -1.f,  -1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },  //V7
    //    { XMFLOAT3(-1.f,  -1.f, -1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },  //V6
    //};

    auto TempSimpleColorVertices = GizmoComponent.PhysicalMesh.GetSimpleColorVerticesList();
    SSimpleColorVertex* pSimpleColorVertices = TempSimpleColorVertices.data();

    D3D11_BUFFER_DESC BufferDescriptor{};
    BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    BufferDescriptor.ByteWidth = sizeof(SSimpleColorVertex) * TempSimpleColorVertices.size();
    BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    BufferDescriptor.CPUAccessFlags = 0;

    ID3D11Buffer* pVertexBuffer{ nullptr };
    D3D11_SUBRESOURCE_DATA InitData{};
    InitData.pSysMem = pSimpleColorVertices;
    m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, &InitData, &pVertexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set vertex buffer
    UINT stride = sizeof(SSimpleColorVertex);
    UINT offset = 0;
    m_pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

    auto VertexBufferLambda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
    };
    GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);

    auto TempSimpleColorIndices = GizmoComponent.PhysicalMesh.GetIndicesList();
    WORD* pSimpleColorIndices = TempSimpleColorIndices.data();

    ID3D11Buffer* IndexBuffer{ nullptr };
    BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    BufferDescriptor.ByteWidth = sizeof(WORD) * TempSimpleColorIndices.size();        // 36 vertices needed for 12 triangles in a triangle list
    BufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
    BufferDescriptor.CPUAccessFlags = 0;
    InitData.pSysMem = pSimpleColorIndices;
    m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, &InitData, &IndexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set index buffer
    m_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    auto IndexBufferLambda = [=]() {
        m_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    };
    GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

    // Set primitive topology
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer


    //ID3D11Buffer* TempConstantBuffer{ nullptr };
    //bd.Usage = D3D11_USAGE_DEFAULT;
    //bd.ByteWidth = sizeof(SCollisionBuffer);
    //bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //bd.CPUAccessFlags = 0;
    //m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &TempConstantBuffer);
    //if (FAILED(m_HR))
    //    return m_HR;

	ID3D11Buffer* ArrowConstantBuffer{ nullptr };
    BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    BufferDescriptor.ByteWidth = sizeof(SArrowConstantBuffer);
    BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    BufferDescriptor.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, nullptr, &ArrowConstantBuffer);
    if (FAILED(m_HR))
        return m_HR;

    auto ConstantBufferLambda = [=]() {
        m_pImmediateContext->VSSetConstantBuffers(0, 1, &ArrowConstantBuffer);
    };
    GizmoComponent.m_DXResConfig.SetConstantBuffer(ArrowConstantBuffer);
    GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);

    // OLD constant buffer code
    //ID3D11Buffer* pConstantBuffer{ nullptr };
    //BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    //BufferDescriptor.ByteWidth = sizeof(SArrowConstantBuffer);
    //BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //BufferDescriptor.CPUAccessFlags = 0;
    //m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, nullptr, &pConstantBuffer);
    //if (FAILED(m_HR))
    //    return m_HR;

    //auto ConstantBufferLambda = [=]() {
    //    m_pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
    //};
    //GizmoComponent.m_DXResConfig.SetConstantBuffer(pConstantBuffer);
    //GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);


    //auto DisableDepthStencilLambda = [=]() {
    //    InitDisabledDepthStencil();
    //};
    //GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(DisableDepthStencilLambda);


    //const wchar_t* TextureName = L"tex_stickman.dds";
    //m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureColorGridRV);
    //if (FAILED(m_HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
    //    return m_HR;
    //}
    //auto TextureLambda = [=]() {
    //    m_pImmediateContext->PSSetShaderResources(0, 1, &m_TextureColorGridRV);
    //};
    //GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(TextureLambda);

    D3D11_SAMPLER_DESC SampDesc = {};
    SampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    SampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    SampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SampDesc.MinLOD = 0;
    SampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    m_HR = m_pD3D11Device->CreateSamplerState(&SampDesc, &m_SamplerLinear);
    if (FAILED(m_HR))
        return m_HR;

    //InterpMoveCubeRef = &GizmoComponent;

    CSceneGraphNode<CGameEntity3DComponent>* CubeComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
    CubeComponentNode->m_tType = GizmoComponent;

    GizmoEntity.m_SceneGraph.m_pRootNode = CubeComponentNode;

    SScene.AddEntityToScene(GizmoEntity);

    //m_PostRenderPtr[0] = &CDX11Device::EnableDepthStencil;
    //SScene.AddEntityToScene(GizmoComponent);

	CTimerManager& TimerManager = CTimerManager::GetTimerManager();
    
    //  !!
    //  Temporary fix, must remember to change in CoreClock.cpp to accept function pointers and tick for the entire runtime 
    //  !!

    TimerManager.SetTimer3<CDX11Device, void, &CDX11Device::SetGizmoTimer>(this, 0, 10000);

    return S_OK;
}

void CDX11Device::SetGizmoTimer()
{
    XMVECTOR CubeOriginProjected = XMVector3Project(
        XMVECTOR{ -5, 0, 0 },
        0,
        0,
        ViewportWidth,
        ViewportHeight,
        0,
        1,
        CCamera::m_ProjectionMatrix,
        CCamera::m_ViewMatrix,
        XMMatrixIdentity());

	XMFLOAT3 fCubeOriginProjected{ 0, 0, 0 };
	XMStoreFloat3(&fCubeOriginProjected, CubeOriginProjected);

	XMVECTOR CubeNormalProjected = XMVector3Project(
		XMVECTOR{ -5, 0, 10 },
		0,
		0,
		ViewportWidth,
		ViewportHeight,
		0,
		1,
        CCamera::m_ProjectionMatrix,
        CCamera::m_ViewMatrix,
		XMMatrixIdentity());

	XMFLOAT3 fCubeNormalProjected{ 0, 0, 0 };
	XMStoreFloat3(&fCubeNormalProjected, CubeNormalProjected);

    CLogger& Logger = CLogger::GetLogger();

	if (m_pRenderer)
	{
		CPhysicsModule* pPhysicsModule = static_cast<CPhysicsModule*>(m_pRenderer->m_pGraphicsModule->m_pMediator->m_ModuleArray[2]);
		CUIModule* pUIModule = static_cast<CUIModule*>(m_pRenderer->m_pGraphicsModule->m_pMediator->m_ModuleArray[0]);

        if (pUIModule)
        {
			CUIMessageQueue& ViewportMessage = CUIMessageQueue::GetUIMessageQueue();
            auto MouseX = ViewportMessage.m_MouseX;
            auto MouseY = ViewportMessage.m_MouseY;

			Logger.Log("CDX11Device::SetGizmoTimer: g_RaycastX2 = ", MouseX, ", g_RaycastY2 = ", MouseY, "\n");

            if (pUIModule->m_ViewportWindow.m_YKeyPressed)
            {
				this->m_YKeyPressed = true;
				Logger.Log("CDX11Device::SetGizmoTimer: Y key pressed");
            }
            else
            {
                this->m_YKeyPressed = false;
				Logger.Log("CDX11Device::SetGizmoTimer: Y key NOT pressed");
            }

            float fDist;

			XMMATRIX QuatMatrix = XMMatrixRotationRollPitchYaw(0, 0, 0);
            XMFLOAT4 Orientation;
            XMStoreFloat4(&Orientation, QuatMatrix.r[0]);

            XMVECTOR Origin = XMVector3Unproject(
                XMVECTOR{ 0, 0, 0 },
                0,
                0,
                ViewportWidth,
                ViewportHeight,
                0,
                1,
                CCamera::m_ProjectionMatrix,
                CCamera::m_ViewMatrix,
                CCamera::m_WorldMatrix);

            XMVECTOR Destination = XMVector3Unproject(
                XMVECTOR{ (float)MouseX, (float)MouseY, 1 },
                0,
                0,
                ViewportWidth,
                ViewportHeight,
                0,
                1,
                CCamera::m_ProjectionMatrix,
                CCamera::m_ViewMatrix,
                CCamera::m_WorldMatrix);

            XMFLOAT3 BoxCenter{ -5.f, 0, 0 };
            XMFLOAT3 BoxExtents{ 1.f, 1.f, 1.f};
			XMFLOAT4 BoxOrientation{ Orientation };

            //        FXMVECTOR RayOrigin, FXMVECTOR RayDirection, XMFLOAT3 BoxCenter,
                //XMFLOAT3 BoxExtents, XMFLOAT4 BoxOrientation, float& Dist)
            if (pPhysicsModule->DoesRayIntersectOBB(
                Origin,
				Destination,
                BoxCenter,
                BoxExtents,
				BoxOrientation,
                fDist
            ))

            {
				m_bGizmoHovered = 1;
				Logger.Log("CDX11Device::SetGizmoTimer: Ray intersects OBB");
            }
            else
            {
				m_bGizmoHovered = 0;
                Logger.Log("CDX11Device::SetGizmoTimer: Ray DOES NOT intersects OBB");
            }
		}
		else
		{
			Logger.Log("CDX11Device::SetGizmoTimer: pUIModule NOT valid");
        }

        if (pPhysicsModule)
        {
            Logger.Log("CDX11Device::SetGizmoTimer: pPhysicsModule valid");
        }
        else 
        {
            Logger.Log("CDX11Device::SetGizmoTimer: pPhysicsModule NOT valid");
        }

        Logger.Log("CDX11Device::SetGizmoTimer: m_pRenderer valid");
	}
    else
    {
        Logger.Log("CDX11Device::SetGizmoTimer: m_pRenderer NOT valid");
    }
}

HRESULT CDX11Device::AddGizmo()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;

    CGameEntity3D GizmoEntity;// = GeometryFactory.CreateEntity3D(EGameEntityType::Arrow);
    GizmoEntity.m_GameEntityTag = "Gizmo";
    GizmoEntity.m_GameEntityType = EGameEntityType::Arrow;

	CGameEntity3DComponent GizmoComponent;
    GizmoComponent.m_GameEntityTag = "GizmoComponent";
    GizmoComponent.SetLocationF(3.f, 0.0f, -4.0f);
    GeometryFactory.CreatePhysicalMesh(GizmoComponent.PhysicalMesh, EPrimitiveGeometryType::Arrow);
    //GizmoComponent.SetScale(15.25f, 15.25f, 15.25f);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"SolidColorShader.fxh", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the vertex shader
    ID3D11VertexShader* pVertexShader{ nullptr };
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pVertexShader);
    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return m_HR;
    }
    auto VertexShaderLambda = [=]() {
        m_pImmediateContext->VSSetShader(pVertexShader, nullptr, 0);
    };
    GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    ID3D11InputLayout* pVertexLayout{ nullptr };
    m_HR = m_pD3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &pVertexLayout);
    pVSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;

    // Set the input layout
    m_pImmediateContext->IASetInputLayout(pVertexLayout);

    auto InputLayoutLambda = [=]() {
        m_pImmediateContext->IASetInputLayout(pVertexLayout);
    };

    GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"SolidColorShader.fxh", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the pixel shader
    ID3D11PixelShader* pPixelShader{ nullptr };
    m_HR = m_pD3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pPixelShader);
    pPSBlob->Release();
    if (FAILED(m_HR))
        return m_HR;

    auto PixelShaderLambda = [=]() {
        m_pImmediateContext->PSSetShader(pPixelShader, nullptr, 0);
    };
    GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(PixelShaderLambda);

    //SSimpleColorVertex vertices[] =
    //{
    //    { XMFLOAT3(-1.f,  1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   // V0
    //    { XMFLOAT3(1.f,  1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },    //V3
    //    { XMFLOAT3(1.f, -1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },    // V2
    //    { XMFLOAT3(-1.f, -1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   // V1
    //    { XMFLOAT3(-1.f,  1.f, -1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   //V4
    //    { XMFLOAT3(1.f,  1.f,  -1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   //V5
    //    { XMFLOAT3(1.f,  -1.f,  -1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },  //V7
    //    { XMFLOAT3(-1.f,  -1.f, -1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },  //V6
    //};

	auto TempSimpleColorVertices = GizmoComponent.PhysicalMesh.GetSimpleColorVerticesList();
	SSimpleColorVertex* pSimpleColorVertices = TempSimpleColorVertices.data();

    D3D11_BUFFER_DESC BufferDescriptor{};
    BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    BufferDescriptor.ByteWidth = sizeof(SSimpleColorVertex) * 256;
    BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    BufferDescriptor.CPUAccessFlags = 0;

    ID3D11Buffer* pVertexBuffer{ nullptr };
    D3D11_SUBRESOURCE_DATA InitData{};
    InitData.pSysMem = pSimpleColorVertices;
    m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, &InitData, &pVertexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set vertex buffer
    UINT stride = sizeof(SSimpleColorVertex);
    UINT offset = 0;
    m_pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

    auto VertexBufferLambda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
    };
    GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);

    auto TempSimpleColorIndices = GizmoComponent.PhysicalMesh.GetIndicesList();
    WORD* pSimpleColorIndices = TempSimpleColorIndices.data();

    ID3D11Buffer* IndexBuffer{ nullptr };
    BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    BufferDescriptor.ByteWidth = sizeof(WORD) * 2000;        // 36 vertices needed for 12 triangles in a triangle list
    BufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
    BufferDescriptor.CPUAccessFlags = 0;
    InitData.pSysMem = pSimpleColorIndices;
    m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, &InitData, &IndexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set index buffer
    m_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    auto IndexBufferLambda = [=]() {
        m_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    };
    GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

    // Set primitive topology
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    ID3D11Buffer* pConstantBuffer{ nullptr };
    BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    BufferDescriptor.ByteWidth = sizeof(SArrowConstantBuffer);
    BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    BufferDescriptor.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, nullptr, &pConstantBuffer);
    if (FAILED(m_HR))
        return m_HR;

    auto ConstantBufferLambda = [=]() {
        m_pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
    };
    GizmoComponent.m_DXResConfig.SetConstantBuffer(pConstantBuffer);
    GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);


    //const wchar_t* TextureName = L"tex_stickman.dds";
    //m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureColorGridRV);
    //if (FAILED(m_HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
    //    return m_HR;
    //}
    //auto TextureLambda = [=]() {
    //    m_pImmediateContext->PSSetShaderResources(0, 1, &m_TextureColorGridRV);
    //};
    //GizmoComponent.m_DXResConfig.m_pContextResourcePtr.push_back(TextureLambda);

    D3D11_SAMPLER_DESC SampDesc = {};
    SampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    SampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    SampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SampDesc.MinLOD = 0;
    SampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    m_HR = m_pD3D11Device->CreateSamplerState(&SampDesc, &m_SamplerLinear);
    if (FAILED(m_HR))
        return m_HR;

    //InterpMoveCubeRef = &GizmoComponent;

    CSceneGraphNode<CGameEntity3DComponent>* CubeComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
    CubeComponentNode->m_tType = GizmoComponent;

    GizmoEntity.m_SceneGraph.m_pRootNode = CubeComponentNode;

    SScene.AddEntityToScene(GizmoEntity);
    //SScene.AddEntityToScene(GizmoComponent);

    return S_OK;
}



void CDX11Device::CleanupDX11Device()
{
    //if (m_pImmediateContext) m_pImmediateContext->ClearState();
    //if (m_ConstantBuffer) m_ConstantBuffer->Release();
    //if (m_VertexBuffer) m_VertexBuffer->Release();
    //if (m_VertexLayout) m_VertexLayout->Release();
    //if (m_VertexShader) m_VertexShader->Release();
    //if (m_PixelShader) m_PixelShader->Release();
    //if (m_pRenderTargetView) m_pRenderTargetView->Release();
    //if (m_SwapChain1) m_SwapChain1->Release();
    //if (m_SwapChain) m_SwapChain->Release();
    //if (m_pImmediateContext1) m_pImmediateContext1->Release();
    //if (m_pImmediateContext) m_pImmediateContext->Release();
    //if (m_pD3D11Device1) m_pD3D11Device1->Release();
    //if (m_pD3D11Device) m_pD3D11Device->Release();
    //if (m_TextureRV) m_TextureRV->Release();
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

        auto breakpoint = 1;
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

void CDX11Device::InitViewport()
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

	CGameEntity3D TexturedCubeEntity;
    TexturedCubeEntity.m_GameEntityTag = "TexturedCube";
	TexturedCubeEntity.m_GameEntityType = EGameEntityType::Cube;

    CGameEntity3DComponent TexturedCubeComponent;
    TexturedCubeComponent.m_GameEntityTag = "TexturedCubeComponent";
    TexturedCubeComponent.SetLocationF(-5.f, 0.0f, 0.0f);
    TexturedCubeComponent.SetScale(0.25f, 0.25f, 0.25f);

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
    TexturedCubeComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

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

    TexturedCubeComponent.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

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
    TexturedCubeComponent.m_DXResConfig.m_pContextResourcePtr.push_back(PixelShaderLambda);

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

    std::vector<SSimpleVertex> VerticesVector{
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
    };

    SSimpleVertex* Vertices{ VerticesVector.data() };

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
    };

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleVertex) * 36;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    ID3D11Buffer* TempVertexBuffer{ nullptr };
    D3D11_SUBRESOURCE_DATA InitData{};
    InitData.pSysMem = Vertices;
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
    TexturedCubeComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);


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
    TexturedCubeComponent.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

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
    TexturedCubeComponent.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    TexturedCubeComponent.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);

    auto image = std::make_unique<ScratchImage>();
    HRESULT hr = LoadFromTGAFile(L"grey_grid.tga", TGA_FLAGS_NONE, nullptr, *image);
    if (FAILED(hr))
    {
		MessageBox(nullptr, L"Failed to load texture from TGA file", L"Error", MB_OK);
		return hr;
    }

	m_HR = CreateShaderResourceView(m_pD3D11Device, image->GetImages(), image->GetImageCount(), image->GetMetadata(), &m_TextureColorGridRV);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
        return m_HR;
    }

    //const wchar_t* TextureName = L"tex_stickman.dds";
    //m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureColorGridRV);
    //if (FAILED(m_HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
    //    return m_HR;
    //}
    auto TextureLambda = [=]() {
        m_pImmediateContext->PSSetShaderResources(0, 1, &m_TextureColorGridRV);
    };
    TexturedCubeComponent.m_DXResConfig.m_pContextResourcePtr.push_back(TextureLambda);

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

    InterpMoveCubeRef = &TexturedCubeComponent;
	//TimerManager.SetTimer3<CDX11Device, void, &CDX11Device::InterpMoveEntity>(this, 2.0f, 10.0f);

    // Initialize the projection matrix
    CCamera::m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ViewportWidth / (FLOAT)m_ViewportHeight, 0.01f, 100.0f);

    CSceneGraphNode<CGameEntity3DComponent>* CubeComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
    CubeComponentNode->m_tType = TexturedCubeComponent;

    TexturedCubeEntity.m_SceneGraph.m_pRootNode = CubeComponentNode;

    SScene.AddEntityToScene(TexturedCubeEntity);

    return S_OK;
}

void CDX11Device::InterpMoveEntity()
{
	g_DoesFrustumContain = true;
    CGameEntity3DComponent* Cube{ nullptr };
    CGameEntity3DComponent* Frustum{ nullptr };

	CLogger& Logger = CLogger::GetLogger();
    Logger.Log("InterpMoveCube Called");
    CTimerManager& TimerManager = CTimerManager::GetTimerManager();

    InterpMoveLoc = FInterpConstantTo(InterpMoveLoc, -10.0f, TimerManager.m_pCoreClock->GetFDeltaTime(), 2.9f);

    CScene& Scene = CScene::GetScene();
    for (auto& SceneIt : Scene.GetSceneList())
    {
		if (SceneIt.m_GameEntityTag == "SolidColorCubeEntity")
		{
            Cube = &SceneIt.m_SceneGraph.m_pRootNode->m_tType;
		}
		else if (SceneIt.m_GameEntityTag == "FrustumEntity")
		{
			Frustum = &SceneIt.m_SceneGraph.m_pRootNode->m_tType;
		}
    }	
   // auto& EntityComponent = SceneIt.m_SceneGraph.m_pRootNode->m_tType;
    //auto& EntityComponent2 = SceneIt.m_SceneGraph.m_pRootNode->ChildNode.at(0)->Type;
    Logger.Log("InterpMoveCube Function, InterpMoveLoc = ", InterpMoveLoc);
    Cube->SetLocationF(InterpMoveLoc, 0.0f, 5.f);
    //EntityComponent2.SetLocationF(-6.f, 0.0f, InterpMoveLoc);
    XMVECTOR quaternionRotation = XMQuaternionRotationRollPitchYaw(
        0.0f,
        XMConvertToRadians(InterpMoveLoc * 50),
        0.0f
    );
    Cube->m_QuatRotationMatrix = XMMatrixRotationQuaternion(quaternionRotation);

	auto CollisionType = CollisionCheck(Frustum, Cube);
    SCollisionBuffer& CB = Frustum->m_CollisionBuffer;

    if (CollisionType == ContainmentType::DISJOINT)
        CB.mDoesFrustumContain = false;
    else if (CollisionType == ContainmentType::INTERSECTS)
        CB.mDoesFrustumContain = true;
    else if (CollisionType == ContainmentType::CONTAINS)
        CB.mDoesFrustumContain = true;
    

	Logger.Log("InterpMoveCube Function, ContainmentType = ", CollisionType);
}

HRESULT CDX11Device::InitSolidColorCube()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;


	CGameEntity3DComponent CubeEntity;
    CubeEntity.m_GameEntityTag = "SolidColorCubeEntity";
	CubeEntity.m_GameEntityType = EGameEntityType::Cube;

	CGameEntity3DComponent CubeEntityComponent;
    CubeEntityComponent.m_GameEntityTag = "SolidColorCubeComponent";
    CubeEntityComponent.SetLocationF(10.f, 0.0f, 5.0f);
    //CubeEntityComponent.SetScale(15.25f, 15.25f, 15.25f);

    CTimerManager& TimerManager = CTimerManager::GetTimerManager();

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"SolidColorShader.fxh", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the vertex shader
    ID3D11VertexShader* VertexShader{ nullptr };
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &VertexShader);
    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return m_HR;
    }
    auto VertexShaderLambda = [=]() {
        m_pImmediateContext->VSSetShader(VertexShader, nullptr, 0);
    };
    CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

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
        return m_HR;

    // Set the input layout
    //m_pImmediateContext->IASetInputLayout(TempVertexLayout);

    auto InputLayoutLambda = [=]() {
        m_pImmediateContext->IASetInputLayout(TempVertexLayout);
    };

    CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"SolidColorShader.fxh", "PS", "ps_5_0", &pPSBlob);
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

    SSimpleColorVertex vertices[] =
    {
		{ XMFLOAT3(-1.f,  1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   // V0
        { XMFLOAT3(1.f,  1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },    //V3
        { XMFLOAT3(1.f, -1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },    // V2
        { XMFLOAT3(-1.f, -1.f,  1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   // V1
        { XMFLOAT3(-1.f,  1.f, -1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   //V4
        { XMFLOAT3(1.f,  1.f,  -1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   //V5
        { XMFLOAT3(1.f,  -1.f,  -1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },  //V7
        { XMFLOAT3(-1.f,  -1.f, -1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },  //V6










            //{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },

            //{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },

            //{ XMFLOAT3(-1.0f, -1.0f, 1.0f),XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },

            //{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },

            //{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },

            //{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
            //{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }
    };

    D3D11_BUFFER_DESC BufferDescriptor{};
    BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    BufferDescriptor.ByteWidth = sizeof(SSimpleColorVertex) * 24;
    BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    BufferDescriptor.CPUAccessFlags = 0;

    ID3D11Buffer* TempVertexBuffer{ nullptr };
    D3D11_SUBRESOURCE_DATA InitData{};
    InitData.pSysMem = vertices;
    m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, &InitData, &TempVertexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set vertex buffer
    UINT stride = sizeof(SSimpleColorVertex);
    UINT offset = 0;
    m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);

    auto VertexBufferLambda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);
    };
    CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);


    // Create index buffer
    WORD Indices[] =
    {
	    0, 1, 2,
	    0, 2, 3,

	    4, 5, 6,
	    4, 6, 7,

	    0, 4, 5,
	    0, 5, 1,

	    1, 5, 6,
	    1, 6, 2,

	    2, 6, 7,
	    2, 7, 3,

	    3, 7, 4,
	    3, 4, 0,
    };

    ID3D11Buffer* IndexBuffer{ nullptr };
    BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    BufferDescriptor.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    BufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
    BufferDescriptor.CPUAccessFlags = 0;
    InitData.pSysMem = Indices;
    m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, &InitData, &IndexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set index buffer
    m_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    auto IndexBufferLambda = [=]() {
        m_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    };
    CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

    // Set primitive topology
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    ID3D11Buffer* TempConstantBuffer{ nullptr };
    BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    BufferDescriptor.ByteWidth = sizeof(SConstantBuffer);
    BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    BufferDescriptor.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, nullptr, &TempConstantBuffer);
    if (FAILED(m_HR))
        return m_HR;

    auto ConstantBufferLambda = [=]() {
        m_pImmediateContext->VSSetConstantBuffers(0, 1, &TempConstantBuffer);
    };
    CubeEntityComponent.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);


    //const wchar_t* TextureName = L"tex_stickman.dds";
    //m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureColorGridRV);
    //if (FAILED(m_HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
    //    return m_HR;
    //}
    //auto TextureLambda = [=]() {
    //    m_pImmediateContext->PSSetShaderResources(0, 1, &m_TextureColorGridRV);
    //};
    //CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(TextureLambda);

    auto RasterizerStateLambda = [=]() {
        ID3D11RasterizerState* RasterizerState{ nullptr };
        D3D11_RASTERIZER_DESC RasterDesc = {};
        RasterDesc.FillMode = D3D11_FILL_SOLID;
        RasterDesc.CullMode = D3D11_CULL_NONE;
        RasterDesc.FrontCounterClockwise = false;
        RasterDesc.DepthBias = 0;
        RasterDesc.DepthBiasClamp = 0.0f;
        RasterDesc.SlopeScaledDepthBias = 0.0f;
        RasterDesc.DepthClipEnable = true;
        RasterDesc.ScissorEnable = false;
        RasterDesc.MultisampleEnable = false;
        RasterDesc.AntialiasedLineEnable = false;

        m_HR = m_pD3D11Device->CreateRasterizerState(&RasterDesc, &RasterizerState);
        if (FAILED(m_HR))
        {
            MessageBox(nullptr, L"Failed to create rasterizer state", L"Error", MB_OK);
            return;
        }

        m_pImmediateContext->RSSetState(RasterizerState);
    };

    CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(RasterizerStateLambda);


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

    CSceneGraphNode<CGameEntity3DComponent>* CubeComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
    CubeComponentNode->m_tType = CubeEntityComponent;

    CubeEntity.m_SceneGraph.m_pRootNode = CubeComponentNode;

    TimerManager.SetTimer3<CDX11Device, void, &CDX11Device::InterpMoveEntity>(this, 2.0f, 30.0f);



    SScene.AddEntityToScene(CubeEntity);
    //SScene.AddEntityToScene(CubeEntityComponent);

    //D3D11_RASTERIZER_DESC RasterDesc = {};
    //RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    //RasterDesc.CullMode = D3D11_CULL_NONE;
    //RasterDesc.FrontCounterClockwise = false;
    //RasterDesc.DepthBias = 0;
    //RasterDesc.DepthBiasClamp = 0.0f;
    //RasterDesc.SlopeScaledDepthBias = 0.0f;
    //RasterDesc.DepthClipEnable = true;
    //RasterDesc.ScissorEnable = false;
    //RasterDesc.MultisampleEnable = false;
    //RasterDesc.AntialiasedLineEnable = false;

    //m_HR = m_pD3D11Device->CreateRasterizerState(&RasterDesc, &m_RasterizerState);
    //if (FAILED(m_HR))
    //{
    //    MessageBox(nullptr, L"Failed to create rasterizer state", L"Error", MB_OK);
    //    return m_HR;
    //}

    return S_OK;
}

HRESULT CDX11Device::InitPlane()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;


    CGameEntity3DComponent PlaneEntity;
    PlaneEntity.m_GameEntityTag = "PlaneEntity";
    PlaneEntity.m_GameEntityType = EGameEntityType::Plane;

    CGameEntity3DComponent PlaneEntityComponent;
    PlaneEntityComponent.m_GameEntityTag = "PlaneEntityComponent";
    PlaneEntityComponent.SetLocationF(2.f, 0.0f, 2.0f);
    //PlaneEntityComponent.SetScale(15.25f, 15.25f, 15.25f);

    CTimerManager& TimerManager = CTimerManager::GetTimerManager();

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"SolidColorShader.fxh", "VS", "vs_5_0", &pVSBlob);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return m_HR;
    }

    // Create the vertex shader
    ID3D11VertexShader* VertexShader{ nullptr };
    m_HR = m_pD3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &VertexShader);
    if (FAILED(m_HR))
    {
        pVSBlob->Release();
        return m_HR;
    }
    auto VertexShaderLambda = [=]() {
        m_pImmediateContext->VSSetShader(VertexShader, nullptr, 0);
    };
    PlaneEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

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
        return m_HR;

    // Set the input layout
    //m_pImmediateContext->IASetInputLayout(TempVertexLayout);

    auto InputLayoutLambda = [=]() {
        m_pImmediateContext->IASetInputLayout(TempVertexLayout);
    };

    PlaneEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"SolidColorShader.fxh", "PS", "ps_5_0", &pPSBlob);
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
    PlaneEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(PixelShaderLambda);

    SSimpleColorVertex vertices[] =
    {
        { XMFLOAT3(-1.f,  0.f,  1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   
        { XMFLOAT3(-1.f,  0.f,  -1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   
        { XMFLOAT3(1.f, 0.f,  -1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },    
        { XMFLOAT3(1.f, 0.f,  1.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },  
    };

    D3D11_BUFFER_DESC BufferDescriptor{};
    BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    BufferDescriptor.ByteWidth = sizeof(SSimpleColorVertex) * 4;
    BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    BufferDescriptor.CPUAccessFlags = 0;

    ID3D11Buffer* TempVertexBuffer{ nullptr };
    D3D11_SUBRESOURCE_DATA InitData{};
    InitData.pSysMem = vertices;
    m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, &InitData, &TempVertexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set vertex buffer
    UINT stride = sizeof(SSimpleColorVertex);
    UINT offset = 0;
    m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);

    auto VertexBufferLambda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);
    };
    PlaneEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);


    // Create index buffer
    WORD Indices[] =
    {
        0, 1, 2,
        2, 3, 0,
    };

    ID3D11Buffer* IndexBuffer{ nullptr };
    BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    BufferDescriptor.ByteWidth = sizeof(WORD) * 6;        
    BufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
    BufferDescriptor.CPUAccessFlags = 0;
    InitData.pSysMem = Indices;
    m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, &InitData, &IndexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set index buffer
    m_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    auto IndexBufferLambda = [=]() {
        m_pImmediateContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    };
    PlaneEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

    // Set primitive topology
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    ID3D11Buffer* TempConstantBuffer{ nullptr };
    BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    BufferDescriptor.ByteWidth = sizeof(SConstantBuffer);
    BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    BufferDescriptor.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&BufferDescriptor, nullptr, &TempConstantBuffer);
    if (FAILED(m_HR))
        return m_HR;

    auto ConstantBufferLambda = [=]() {
        m_pImmediateContext->VSSetConstantBuffers(0, 1, &TempConstantBuffer);
    };
    PlaneEntityComponent.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    PlaneEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);


    //D3D11_SAMPLER_DESC sampDesc = {};
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

    CSceneGraphNode<CGameEntity3DComponent>* CubeComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
    CubeComponentNode->m_tType = PlaneEntityComponent;

    PlaneEntity.m_SceneGraph.m_pRootNode = CubeComponentNode;

    SScene.AddEntityToScene(PlaneEntity);


    return S_OK;
}

HRESULT CDX11Device::InitFrustum()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;
    CGameEntity3D FrustumEntity;
    FrustumEntity.m_GameEntityTag = "FrustumEntity";
	FrustumEntity.m_GameEntityType = EGameEntityType::Frustum;

    CFrustumComponent FrustumComponent;
    XMMATRIX xmProj = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1.77778f, 0.5f, 10.0f);
    CreateFrustumFromMatrix(FrustumComponent, xmProj);

    FrustumComponent.m_GameEntityTag = "FrustumComponent";
    FrustumComponent.SetLocationF(0.f, 0.0f, 0.0f);

    CTimerManager& TimerManager = CTimerManager::GetTimerManager();

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"FrustumShader.fxh", "VS", "vs_5_0", &pVSBlob);
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
    FrustumComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

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
        return m_HR;

    // Set the input layout
    m_pImmediateContext->IASetInputLayout(TempVertexLayout);

    auto InputLayoutLambda = [=]() {
        m_pImmediateContext->IASetInputLayout(TempVertexLayout);
    };

    FrustumComponent.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"FrustumShader.fxh", "PS", "ps_5_0", &pPSBlob);
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
    FrustumComponent.m_DXResConfig.m_pContextResourcePtr.push_back(PixelShaderLambda);

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

    XMFLOAT3 verts[8];
	GetFrustumCorners(verts, FrustumComponent);

    SSimpleColorVertex vertices[] =
    {
        //{ XMFLOAT3(-5.f,  5.f,  15.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   // V0
        //{ XMFLOAT3(5.f,  5.f,  15.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },    //V3
        //{ XMFLOAT3(5.f, -5.f,  15.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },    // V2
        //{ XMFLOAT3(-5.f, -5.f,  15.f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   // V1
        //{ XMFLOAT3(-0.25f,  0.25f, -0.25f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   //V4
        //{ XMFLOAT3(0.25f,  0.25f,  -0.25f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   //V5
        //{ XMFLOAT3(0.25f,  -0.25f,  -0.25f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },  //V7
        //{ XMFLOAT3(-0.25f,  -0.25f, -0.25f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },  //V6

        { verts[7], XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   // V0
        { verts[6], XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   // V0
        { verts[5], XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },    //V3
        { verts[4], XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },    // V2
        { verts[3], XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   // V1
        { verts[2], XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   //V4
        { verts[1], XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },   //V5
        { verts[0], XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },  //V7
    };

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleColorVertex) * 8;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    ID3D11Buffer* TempVertexBuffer{ nullptr };
    D3D11_SUBRESOURCE_DATA InitData{};
    InitData.pSysMem = vertices;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &TempVertexBuffer);
    if (FAILED(m_HR))
        return m_HR;

    // Set vertex buffer
    UINT stride = sizeof(SSimpleColorVertex);
    UINT offset = 0;
    m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);

    auto VertexBufferLambda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);
    };
    FrustumComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);


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
        7,4,6,

        //4, 5, 6,
        //4, 6, 7,

        //0, 4, 5,
        //0, 5, 1,

        //1, 5, 6,
        //1, 6, 2,

        //2, 6, 7,
        //2, 7, 3,

        //3, 7, 4,
        //3, 4, 0,
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
    FrustumComponent.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

    // Set primitive topology
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    ID3D11Buffer* TempConstantBuffer{ nullptr };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SCollisionBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &TempConstantBuffer);
    if (FAILED(m_HR))
        return m_HR;

    auto ConstantBufferLambda = [=]() {
        m_pImmediateContext->VSSetConstantBuffers(0, 1, &TempConstantBuffer);
    };
    FrustumComponent.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    FrustumComponent.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);
    //FrustumComponent.m_CollisionBuffer = TempConstantBuffer;


    //const wchar_t* TextureName = L"tex_stickman.dds";
    //m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureColorGridRV);
    //if (FAILED(m_HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
    //    return m_HR;
    //}
    //auto TextureLambda = [=]() {
    //    m_pImmediateContext->PSSetShaderResources(0, 1, &m_TextureColorGridRV);
    //};
    //CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(TextureLambda);

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

    auto RasterizerStateLambda = [=]() {
        ID3D11RasterizerState* RasterizerState{ nullptr };
        D3D11_RASTERIZER_DESC RasterDesc = {};
        RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
        RasterDesc.CullMode = D3D11_CULL_NONE;
        RasterDesc.FrontCounterClockwise = false;
        RasterDesc.DepthBias = 0;
        RasterDesc.DepthBiasClamp = 0.0f;
        RasterDesc.SlopeScaledDepthBias = 0.0f;
        RasterDesc.DepthClipEnable = true;
        RasterDesc.ScissorEnable = false;
        RasterDesc.MultisampleEnable = false;
        RasterDesc.AntialiasedLineEnable = false;

        m_HR = m_pD3D11Device->CreateRasterizerState(&RasterDesc, &RasterizerState);
        if (FAILED(m_HR))
        {
            MessageBox(nullptr, L"Failed to create rasterizer state", L"Error", MB_OK);
            return;
        }

        m_pImmediateContext->RSSetState(RasterizerState);
    };

    FrustumComponent.m_DXResConfig.m_pContextResourcePtr.push_back(RasterizerStateLambda);

    CSceneGraphNode<CGameEntity3DComponent>* FrustumComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
    FrustumComponentNode->m_tType = FrustumComponent;

    FrustumEntity.m_SceneGraph.m_pRootNode = FrustumComponentNode;

    SScene.AddEntityToScene(FrustumEntity);
    //SScene.AddEntityToScene(CubeEntityComponent);

    return S_OK;
}

ContainmentType CDX11Device::CollisionCheck(CGameEntity3DComponent* Frustum, CGameEntity3DComponent* Cube)
{
    XMVECTOR FrustumOrigin = Frustum->GetLocationVector();
	XMFLOAT4 FrustumOrientationF4{ 0, 0, 0, 1 };
    XMVECTOR FrustumOrientation = XMLoadFloat4(&FrustumOrientationF4);

	XMFLOAT3 CubeOrigin{ Cube->GetLocationX(), Cube->GetLocationY(), Cube->GetLocationZ() };
    XMVECTOR BoxOrigin = XMLoadFloat3(&CubeOrigin);
	XMFLOAT3 CubeExtentsF3{ 1.0f, 1.0f, 1.0f };
    XMVECTOR BoxExtents = XMLoadFloat3(&CubeExtentsF3);
    XMVECTOR BoxOrientation = XMLoadFloat4(&FrustumOrientationF4);

    // Init frustum planes
    XMVECTOR NearPlane = XMVectorSet(0.0f, 0.0f, -1.0f, g_Near);
    NearPlane = DirectX::MathInternal::XMPlaneTransform(NearPlane, FrustumOrientation, FrustumOrigin);
    NearPlane = XMPlaneNormalize(NearPlane);

    XMVECTOR FarPlane = XMVectorSet(0.0f, 0.0f, 1.0f, -g_Far);
    FarPlane = DirectX::MathInternal::XMPlaneTransform(FarPlane, FrustumOrientation, FrustumOrigin);
    FarPlane = XMPlaneNormalize(FarPlane);

    XMVECTOR RightPlane = XMVectorSet(1.0f, 0.0f, -g_RightSlope, 0.0f);
    RightPlane = DirectX::MathInternal::XMPlaneTransform(RightPlane, FrustumOrientation, FrustumOrigin);
    RightPlane = XMPlaneNormalize(RightPlane);

    XMVECTOR LeftPlane = XMVectorSet(-1.0f, 0.0f, g_LeftSlope, 0.0f);
    LeftPlane = DirectX::MathInternal::XMPlaneTransform(LeftPlane, FrustumOrientation, FrustumOrigin);
    LeftPlane = XMPlaneNormalize(LeftPlane);

    XMVECTOR TopPlane = XMVectorSet(0.0f, 1.0f, -g_TopSlope, 0.0f);
    TopPlane = DirectX::MathInternal::XMPlaneTransform(TopPlane, FrustumOrientation, FrustumOrigin);
    TopPlane = XMPlaneNormalize(TopPlane);

    XMVECTOR BottomPlane = XMVectorSet(0.0f, -1.0f, g_BottomSlope, 0.0f);
    BottomPlane = DirectX::MathInternal::XMPlaneTransform(BottomPlane, FrustumOrientation, FrustumOrigin);
    BottomPlane = XMPlaneNormalize(BottomPlane);



    // Actual collision check
    assert(DirectX::MathInternal::XMQuaternionIsUnit(BoxOrientation));

    // Set w of the center to one so we can dot4 with a plane.
    BoxOrigin = XMVectorInsert<0, 0, 0, 0, 1>(BoxOrigin, XMVectorSplatOne());

    // Build the 3x3 rotation matrix that defines the box axes.
    XMMATRIX R = XMMatrixRotationQuaternion(BoxOrientation);

    XMVECTOR Outside, Inside;

    // Test against each plane.
    DirectX::MathInternal::FastIntersectOrientedBoxPlane(BoxOrigin, BoxExtents, R.r[0], R.r[1], R.r[2], NearPlane, Outside, Inside);

    XMVECTOR AnyOutside = Outside;
    XMVECTOR AllInside = Inside;

    DirectX::MathInternal::FastIntersectOrientedBoxPlane(BoxOrigin, BoxExtents, R.r[0], R.r[1], R.r[2], FarPlane, Outside, Inside);
    AnyOutside = XMVectorOrInt(AnyOutside, Outside);
    AllInside = XMVectorAndInt(AllInside, Inside);

    DirectX::MathInternal::FastIntersectOrientedBoxPlane(BoxOrigin, BoxExtents, R.r[0], R.r[1], R.r[2], RightPlane, Outside, Inside);
    AnyOutside = XMVectorOrInt(AnyOutside, Outside);
    AllInside = XMVectorAndInt(AllInside, Inside);

    DirectX::MathInternal::FastIntersectOrientedBoxPlane(BoxOrigin, BoxExtents, R.r[0], R.r[1], R.r[2], LeftPlane, Outside, Inside);
    AnyOutside = XMVectorOrInt(AnyOutside, Outside);
    AllInside = XMVectorAndInt(AllInside, Inside);

    DirectX::MathInternal::FastIntersectOrientedBoxPlane(BoxOrigin, BoxExtents, R.r[0], R.r[1], R.r[2], TopPlane, Outside, Inside);
    AnyOutside = XMVectorOrInt(AnyOutside, Outside);
    AllInside = XMVectorAndInt(AllInside, Inside);

    DirectX::MathInternal::FastIntersectOrientedBoxPlane(BoxOrigin, BoxExtents, R.r[0], R.r[1], R.r[2], BottomPlane, Outside, Inside);
    AnyOutside = XMVectorOrInt(AnyOutside, Outside);
    AllInside = XMVectorAndInt(AllInside, Inside);

    // If the box is outside any plane it is outside.
    if (XMVector4EqualInt(AnyOutside, XMVectorTrueInt()))
        return DISJOINT;

    // If the box is inside all planes it is inside.
    if (XMVector4EqualInt(AllInside, XMVectorTrueInt()))
        return CONTAINS;

    // The box is not inside all planes or outside a plane, it may intersect.
    return INTERSECTS;
}

_Use_decl_annotations_
inline void XM_CALLCONV CDX11Device::CreateFrustumFromMatrix(CFrustumComponent& Out, FXMMATRIX Projection, bool rhcoords) noexcept
{
    // Corners of the projection frustum in homogenous space.
    static XMVECTORF32 HomogenousPoints[6] =
    {
        { { {  1.0f,  0.0f, 1.0f, 1.0f } } },   // right (at far plane)
        { { { -1.0f,  0.0f, 1.0f, 1.0f } } },   // left
        { { {  0.0f,  1.0f, 1.0f, 1.0f } } },   // top
        { { {  0.0f, -1.0f, 1.0f, 1.0f } } },   // bottom

        { { { 0.0f, 0.0f, 0.0f, 1.0f } } },     // near
        { { { 0.0f, 0.0f, 1.0f, 1.0f } } }      // far
    };

    XMVECTOR Determinant;
    XMMATRIX matInverse = XMMatrixInverse(&Determinant, Projection);

    // Compute the frustum corners in world space.
    XMVECTOR Points[6];

    for (size_t i = 0; i < 6; ++i)
    {
        // Transform point.
        Points[i] = XMVector4Transform(HomogenousPoints[i], matInverse);
    }

    Out.Origin = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Out.Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

    // Compute the slopes.
    Points[0] = XMVectorMultiply(Points[0], XMVectorReciprocal(XMVectorSplatZ(Points[0])));
    Points[1] = XMVectorMultiply(Points[1], XMVectorReciprocal(XMVectorSplatZ(Points[1])));
    Points[2] = XMVectorMultiply(Points[2], XMVectorReciprocal(XMVectorSplatZ(Points[2])));
    Points[3] = XMVectorMultiply(Points[3], XMVectorReciprocal(XMVectorSplatZ(Points[3])));

    Out.RightSlope = XMVectorGetX(Points[0]);
    g_RightSlope = XMVectorGetX(Points[0]);
    Out.LeftSlope = XMVectorGetX(Points[1]);
    g_LeftSlope = XMVectorGetX(Points[1]);
    Out.TopSlope = XMVectorGetY(Points[2]);
    g_TopSlope = XMVectorGetY(Points[2]);
    Out.BottomSlope = XMVectorGetY(Points[3]);
    g_BottomSlope = XMVectorGetY(Points[3]);

    // Compute near and far.
    Points[4] = XMVectorMultiply(Points[4], XMVectorReciprocal(XMVectorSplatW(Points[4])));
    Points[5] = XMVectorMultiply(Points[5], XMVectorReciprocal(XMVectorSplatW(Points[5])));

    if (rhcoords)
    {
        Out.Near = XMVectorGetZ(Points[5]);
        g_Near = XMVectorGetZ(Points[5]);
        Out.Far = XMVectorGetZ(Points[4]);
        g_Far = XMVectorGetZ(Points[4]);
    }
    else
    {
        Out.Near = XMVectorGetZ(Points[4]);
		g_Near = XMVectorGetZ(Points[4]);
        Out.Far = XMVectorGetZ(Points[5]);
		g_Far = XMVectorGetZ(Points[5]);
    }
}

inline void CDX11Device::GetFrustumCorners(XMFLOAT3* Corners, CFrustumComponent& Out)
{
    assert(Corners != nullptr);

    // Load origin and orientation of the frustum.
    XMVECTOR vOrigin = XMLoadFloat3(&Out.Origin);
    XMVECTOR vOrientation = XMLoadFloat4(&Out.Orientation);

    assert(DirectX::MathInternal::XMQuaternionIsUnit(vOrientation));

    // Build the corners of the frustum.
    XMVECTOR vRightTop = XMVectorSet(Out.RightSlope, Out.TopSlope, 1.0f, 0.0f);
    XMVECTOR vRightBottom = XMVectorSet(Out.RightSlope, Out.BottomSlope, 1.0f, 0.0f);
    XMVECTOR vLeftTop = XMVectorSet(Out.LeftSlope, Out.TopSlope, 1.0f, 0.0f);
    XMVECTOR vLeftBottom = XMVectorSet(Out.LeftSlope, Out.BottomSlope, 1.0f, 0.0f);
    XMVECTOR vNear = XMVectorReplicatePtr(&Out.Near);
    XMVECTOR vFar = XMVectorReplicatePtr(&Out.Far);

    // Returns 8 corners position of bounding frustum.
    //     Near    Far
    //    0----1  4----5
    //    |    |  |    |
    //    |    |  |    |
    //    3----2  7----6

    XMVECTOR vCorners[8];
    vCorners[0] = XMVectorMultiply(vLeftTop, vNear);
    vCorners[1] = XMVectorMultiply(vRightTop, vNear);
    vCorners[2] = XMVectorMultiply(vRightBottom, vNear);
    vCorners[3] = XMVectorMultiply(vLeftBottom, vNear);
    vCorners[4] = XMVectorMultiply(vLeftTop, vFar);
    vCorners[5] = XMVectorMultiply(vRightTop, vFar);
    vCorners[6] = XMVectorMultiply(vRightBottom, vFar);
    vCorners[7] = XMVectorMultiply(vLeftBottom, vFar);

    for (size_t i = 0; i < 8; ++i)
    {
        XMVECTOR C = XMVectorAdd(XMVector3Rotate(vCorners[i], vOrientation), vOrigin);
        XMStoreFloat3(&Corners[i], C);
    }
}

void CDX11Device::CopyEntity(CGameEntity3D Entity)
{
    CScene& Scene = CScene::GetScene();

    CGameEntity3D NewEntity{ Entity };  
    CGameEntity3DComponent NewEntityComponent;
    NewEntityComponent = Entity.m_SceneGraph.m_pRootNode->m_tType;
	NewEntity.m_GameEntityTag = Entity.m_GameEntityTag + "_Copy";

    //NewEntityComponent = NewEntity.m_SceneGraph.m_pRootNode->m_tType;
    NewEntityComponent.m_GameEntityTag = Entity.m_GameEntityTag + "_Copy";
	NewEntityComponent.SetScale(5.5f, 5.5f, 5.5f);
	NewEntity.m_SceneGraph.m_pRootNode->m_tType = NewEntityComponent;

	Scene.AddEntityToScene(NewEntity);
}

void CDX11Device::SetRenderer(CRenderer* Renderer)
{
	m_pRenderer = Renderer;
}

void CDX11Device::OnPreRender()
{
	for (auto& funcPtr : m_PreRenderPtr)
	{
		if (funcPtr)
		{
			(this->*funcPtr)();
		}
	}
}

void CDX11Device::OnPostRender()
{
    for (auto& funcPtr : m_PostRenderPtr)
    {
        if (funcPtr)
        {
            (this->*funcPtr)();
        }
    }
}

void CDX11Device::RaycastLine(float OriginX, float OriginY, float OriginZ, float DestinationX, float DestinationY, float DestinationZ)
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;
    CGameEntity3D LinetraceEntity;
    LinetraceEntity.m_GameEntityTag = "LinetraceEntity";
    LinetraceEntity.m_GameEntityType = EGameEntityType::Frustum;

    CGameEntity3DComponent LinetraceComponent;
    LinetraceComponent.m_GameEntityTag = "LinetraceComponent";

    CTimerManager& TimerManager = CTimerManager::GetTimerManager();

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"FrustumShader.fxh", "VS", "vs_5_0", &pVSBlob);
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
    auto VertexShaderLambda = [=]() {
        m_pImmediateContext->VSSetShader(TempVertexShader, nullptr, 0);
    };
    LinetraceComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

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
    m_pImmediateContext->IASetInputLayout(TempVertexLayout);

    auto InputLayoutLambda = [=]() {
        m_pImmediateContext->IASetInputLayout(TempVertexLayout);
    };

    LinetraceComponent.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    m_HR = CompileShaderFromFile(L"FrustumShader.fxh", "PS", "ps_5_0", &pPSBlob);
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

    auto PixelShaderLambda = [=]() {
        m_pImmediateContext->PSSetShader(TempPixelShader, nullptr, 0);
    };
    LinetraceComponent.m_DXResConfig.m_pContextResourcePtr.push_back(PixelShaderLambda);

    SSimpleColorVertex vertices[] =
    {
        { XMFLOAT3(OriginX, OriginY, OriginZ), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },   
        { XMFLOAT3(DestinationX, DestinationY, DestinationZ), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }
    };

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SSimpleColorVertex) * 2;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    ID3D11Buffer* TempVertexBuffer{ nullptr };
    D3D11_SUBRESOURCE_DATA InitData{};
    InitData.pSysMem = vertices;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &TempVertexBuffer);
    if (FAILED(m_HR))
        return;

    // Set vertex buffer
    UINT stride = sizeof(SSimpleColorVertex);
    UINT offset = 0;
    m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);

    auto VertexBufferLambda = [=]() {
        m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);
    };
    LinetraceComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);


    // Create index buffer
    WORD indices[] =
    {
       0, 1
    };

    ID3D11Buffer* TempIndexBuffer{ nullptr };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 2;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, &InitData, &TempIndexBuffer);
    if (FAILED(m_HR))
        return;

    // Set index buffer
    m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    auto IndexBufferLambda = [=]() {
        m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    };
    LinetraceComponent.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

    // Set primitive topology
    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    ID3D11Buffer* TempConstantBuffer{ nullptr };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SCollisionBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    m_HR = m_pD3D11Device->CreateBuffer(&bd, nullptr, &TempConstantBuffer);
    if (FAILED(m_HR))
        return;

    auto ConstantBufferLambda = [=]() {
        m_pImmediateContext->VSSetConstantBuffers(0, 1, &TempConstantBuffer);
    };
    LinetraceComponent.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    LinetraceComponent.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);
    //FrustumComponent.m_CollisionBuffer = TempConstantBuffer;


    //const wchar_t* TextureName = L"tex_stickman.dds";
    //m_HR = CreateDDSTextureFromFile(m_pD3D11Device, TextureName, nullptr, &m_TextureColorGridRV);
    //if (FAILED(m_HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
    //    return m_HR;
    //}
    //auto TextureLambda = [=]() {
    //    m_pImmediateContext->PSSetShaderResources(0, 1, &m_TextureColorGridRV);
    //};
    //CubeEntityComponent.m_DXResConfig.m_pContextResourcePtr.push_back(TextureLambda);

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
        return;

    auto RasterizerStateLambda = [=]() {
        ID3D11RasterizerState* RasterizerState{ nullptr };
        D3D11_RASTERIZER_DESC RasterDesc = {};
        RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
        RasterDesc.CullMode = D3D11_CULL_NONE;
        RasterDesc.FrontCounterClockwise = false;
        RasterDesc.DepthBias = 0;
        RasterDesc.DepthBiasClamp = 0.0f;
        RasterDesc.SlopeScaledDepthBias = 0.0f;
        RasterDesc.DepthClipEnable = true;
        RasterDesc.ScissorEnable = false;
        RasterDesc.MultisampleEnable = false;
        RasterDesc.AntialiasedLineEnable = false;

        m_HR = m_pD3D11Device->CreateRasterizerState(&RasterDesc, &RasterizerState);
        if (FAILED(m_HR))
        {
            MessageBox(nullptr, L"Failed to create rasterizer state", L"Error", MB_OK);
            return;
        }

        m_pImmediateContext->RSSetState(RasterizerState);
    };

    LinetraceComponent.m_DXResConfig.m_pContextResourcePtr.push_back(RasterizerStateLambda);

    CSceneGraphNode<CGameEntity3DComponent>* pLinetraceComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
    pLinetraceComponentNode->m_tType = LinetraceComponent;

    LinetraceEntity.m_SceneGraph.m_pRootNode = pLinetraceComponentNode;

    SScene.AddEntityToScene(LinetraceEntity);
}
#include "GameEntityBuilder.h"

void GameEntityBuilder::InitDefaultEntities()
{
    Scene& SScene = Scene::GetScene();
    PrimitiveGeometryFactory GeometryFactory;
    GameEntity3D m_CubeEntity2{};
    HRESULT HR{};
    DXResourcesConfig DefaultDXResourceConfig;

    // Init resources for Cube Component Entity

    m_CubeEntity2 = GeometryFactory.CreateEntity3D(PrimitiveGeometryType::Cube);

    m_CubeEntity2.SetLocationF(3.0f, 0.0f, 0.0f);
    //m_CubeEntity.SetScale(0.1f, 0.1f, 0.1f);
    m_CubeEntity2.m_GameEntityTag = "SingleCubeComponentEntity";
    //m_CubeEntity2.m_DXResConfig.m_UID = "UID1234";
    m_CubeEntity2.PhysicalMesh.SetIndicesList(GeometryFactory.GetIndicesList());
    auto CubeEntityIndicesList = m_CubeEntity2.PhysicalMesh.GetIndicesList();
    auto CubeEntityIndicesList2 = GeometryFactory.GetIndicesList();
    m_CubeEntity2.PhysicalMesh.SetStride(sizeof(SimpleVertex));
    m_CubeEntity2.PhysicalMesh.SetSimpleVerticesList(m_CubeEntity2.GetVerticesList());

    XMFLOAT3 CubeEntityCenter{ 3, 0, 0 };
    XMFLOAT3 CubeEntityExtents{ 0.5f, 0.5f, 0.5f };

    CollisionComponent Collision = m_CubeEntity2.GetCollisionComponent();
    Collision.AABox.Center = { 3, 0, 0 };
    Collision.AABox.Extents = { 0.5f, 0.5f, 0.5f };
    Collision.CollisionType = DISJOINT;
    m_CubeEntity2.SetCollisionParams(CubeEntityCenter, CubeEntityExtents, DISJOINT);

    // Cube Entity Component 1
    GameEntity3DComponent CubeComponent1 = GeometryFactory.CreateEntity3DComponent(PrimitiveGeometryType::Cube);

    CubeComponent1.SetLocationF(3.0f, 0.0f, 0.0f);
    //CubeComponent1.SetScale(0.1f, 0.1f, 0.1f);
    CubeComponent1.m_GameEntityTag = "Cube Component1";
    //CubeComponent1.m_DXResConfig.m_UID = "UID1234";
    CubeComponent1.PhysicalMesh.SetIndicesList(GeometryFactory.GetIndicesList());
    CubeComponent1.PhysicalMesh.SetStride(sizeof(SimpleVertex));
    CubeComponent1.PhysicalMesh.SetSimpleVerticesList(CubeComponent1.GetVerticesList());

    CollisionComponent Collision2 = CubeComponent1.GetCollisionComponent();
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
    ID3D11VertexShader* VertexShader{ nullptr };
    ID3DBlob* pVSBlob = nullptr;
    HR = m_DX11Device->CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/texture.vs", "VS", "vs_5_0", &pVSBlob);

    if (FAILED(HR))
    {
        MessageBox(nullptr, L"The VS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the vertex shader
    HR = m_DX11Device->m_D3D11Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &VertexShader);

    if (FAILED(HR))
    {
        pVSBlob->Release();
        return;
    }

    CubeComponent1.m_DXResConfig.SetVertexShader(VertexShader);
    DefaultDXResourceConfig.SetVertexShader(VertexShader);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout

    ID3D11InputLayout* VertexLayout{ nullptr };
    HR = m_DX11Device->m_D3D11Device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &VertexLayout);
    pVSBlob->Release();

    if (FAILED(HR))
    {
        MessageBox(nullptr, L"Failed to initialize input layout", L"Error", MB_OK);
        return;
    }

    CubeComponent1.m_DXResConfig.SetInputLayout(VertexLayout);
    DefaultDXResourceConfig.SetInputLayout(VertexLayout);

    // Compile the pixel shader

    ID3D11PixelShader* PixelShader{ nullptr };
    ID3DBlob* pPSBlob = nullptr;
    //hr = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_4_0", &pPSBlob);
    HR = m_DX11Device->CompileShaderFromFile(L"C:/Users/sefce/source/repos/KMEngine/KMEngine/texture.ps", "PS", "ps_5_0", &pPSBlob);
    if (FAILED(HR))
    {
        MessageBox(nullptr, L"The PS file cannot be compiled.", L"Error", MB_OK);
        return;
    }

    // Create the pixel shader
    HR = m_DX11Device->m_D3D11Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &PixelShader);
    pPSBlob->Release();
    if (FAILED(HR))
    {
        MessageBox(nullptr, L"Failed to create pixel shader", L"Error", MB_OK);
        return;
    }

    CubeComponent1.m_DXResConfig.SetPixelShader(PixelShader);
    DefaultDXResourceConfig.SetPixelShader(PixelShader);

    // Create the vertex buffer

    ID3D11Buffer* VertexBuffer{};
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData{};

    std::vector<SimpleVertex> TotalVerticesVectorFinal{ m_CubeEntity2.PhysicalMesh.GetSimpleVerticesList() };

    //std::vector<SimpleVertex> TotalVerticesVector{ SScene.GetSceneList2().at(SceneLoc).GetVerticesList() };
    int TotalVerticesVectorSize = TotalVerticesVectorFinal.size();
    SimpleVertex* VerticesArray = new SimpleVertex[TotalVerticesVectorSize];
    for (int i = 0; i < TotalVerticesVectorSize; i++)
    {
        VerticesArray[i] = TotalVerticesVectorFinal.at(i);
    }

    // Dangling pointer
    //SimpleVertex* VerticesArray2 = SScene.GetSceneList2().at(0).GetVerticesList().data();

    InitData.pSysMem = VerticesArray;

    HR = m_DX11Device->m_D3D11Device->CreateBuffer(&bd, &InitData, &VertexBuffer);
    if (FAILED(HR))
    {
        MessageBox(nullptr, L"Failed to initialize vertex buffer", L"Error", MB_OK);
        return;
    }

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;

    CubeComponent1.m_DXResConfig.SetVertexBuffer(VertexBuffer);
    DefaultDXResourceConfig.SetVertexBuffer(VertexBuffer);

    // Create the index buffer

    ID3D11Buffer* IndexBuffer{};
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
    HR = m_DX11Device->m_D3D11Device->CreateBuffer(&bd, &InitData, &IndexBuffer);

    if (FAILED(HR))
    {
        MessageBox(nullptr, L"Failed to initialize index buffer", L"Error", MB_OK);
        return;
    }

    CubeComponent1.m_DXResConfig.SetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    DefaultDXResourceConfig.SetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Create the constant buffer
    ID3D11Buffer* mConstantBuffer{ };
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    HR = m_DX11Device->m_D3D11Device->CreateBuffer(&bd, nullptr, &mConstantBuffer);

    if (FAILED(HR))
    {
        MessageBox(nullptr, L"Failed to initialize constant buffer", L"Error", MB_OK);
        return;
    }

    CubeComponent1.m_DXResConfig.SetConstantBuffer(mConstantBuffer);
    DefaultDXResourceConfig.SetConstantBuffer(mConstantBuffer);

    // Create the texture resource view

    ID3D11ShaderResourceView* TextureRV{ nullptr };
    const wchar_t* TextureName = L"grey_grid.dds";
    HR = CreateDDSTextureFromFile(m_DX11Device->m_D3D11Device, TextureName, nullptr, &TextureRV);
    if (FAILED(HR))
    {
        MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
        return;
    }

    //D3D11_TEXTURE2D_DESC defDescDepth{ };
    //ID3D11Texture2D* pDefDepthStencil{ nullptr };

    //defDescDepth.Width = m_DX11Device->GetViewportWidth();
    //defDescDepth.Height = m_DX11Device->GetViewportHeight();
    //defDescDepth.MipLevels = 1;
    //defDescDepth.ArraySize = 1;
    //defDescDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    //defDescDepth.SampleDesc.Count = 1;
    //defDescDepth.SampleDesc.Quality = 0;
    //defDescDepth.Usage = D3D11_USAGE_DEFAULT;
    //defDescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    //defDescDepth.CPUAccessFlags = 0;
    //defDescDepth.MiscFlags = 0;

    //HR = m_DX11Device->m_D3D11Device->CreateTexture2D(&defDescDepth, NULL, &pDefDepthStencil);
    //if (FAILED(HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize depth stencil", L"Error", MB_OK);
    //    return;
    //}

    //D3D11_DEPTH_STENCIL_VIEW_DESC	defDescDepthStencilViewDesc{ };
    //ID3D11DepthStencilView* pDefDepthStencilView{ nullptr };

    //defDescDepthStencilViewDesc.Format = defDescDepth.Format;
    //defDescDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    //defDescDepthStencilViewDesc.Texture2D.MipSlice = 0;
    //HR = m_DX11Device->m_D3D11Device->CreateDepthStencilView(pDefDepthStencil, &defDescDepthStencilViewDesc, &pDefDepthStencilView);

    //if (FAILED(HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize depth stencil view", L"Error", MB_OK);
    //    return;
    //}

    //m_DX11Device->m_ImmediateContext->OMSetRenderTargets(1, &(m_DX11Device->m_RenderTargetView), pDefDepthStencilView);

    //D3D11_DEPTH_STENCIL_DESC defDepthStencilDesc{ };

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

    //defDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    //defDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

    //ID3D11DepthStencilState* DepthStencilState{ nullptr };

    //HR = m_DX11Device->m_D3D11Device->CreateDepthStencilState(&defDepthStencilDesc, &DepthStencilState);

    //if (FAILED(HR))
    //{
    //    MessageBox(nullptr, L"Failed to initialize depth stencil state", L"Error", MB_OK);
    //    return;
    //}

    //CubeComponent1.m_DXResConfig.SetDepthStencilState(DepthStencilState);
    //DefaultDXResourceConfig.SetDepthStencilState(DepthStencilState);

    SScene.InsertDXResourceConfig(2, DefaultDXResourceConfig);
}

#pragma once
#include <cassert>
#include "TerrainGenerator.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#define CBT_IMPLEMENTATION
#include "cbt.h"

HRESULT CTerrainGenerator::GenerateTestTerrain()
{
    CScene& SScene = CScene::GetScene();
    CPrimitiveGeometryFactory GeometryFactory;

    CLogger& SLogger = CLogger::GetLogger();

    CGameEntity3D Terrain = GeometryFactory.CreateEntity3D(EPrimitiveGeometryType::Cube);

    CGameEntity3DComponent TerrainComponent;
    TerrainComponent.m_GameEntityTag = "TerrainComponent";
    TerrainComponent.SetLocationF(0.0f, 0.0f, 0.0f);
    TerrainComponent.SetScale(2.5f, 2.5f, 2.5f);

    TerrainComponent.m_DXResConfig.m_pDX11Device = this->m_pDX11Device;

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
    auto VertexShaderLambda = [=]() {
        TerrainComponent.m_DXResConfig.m_pDX11Device->m_pImmediateContext->VSSetShader(TempVertexShader, nullptr, 0);
        };
    TerrainComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexShaderLambda);

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
    m_pDX11Device->m_pImmediateContext->IASetInputLayout(TempVertexLayout);

    auto InputLayoutLambda = [=]() {
        TerrainComponent.m_DXResConfig.m_pDX11Device->m_pImmediateContext->IASetInputLayout(TempVertexLayout);
        };

    TerrainComponent.m_DXResConfig.m_pContextResourcePtr.push_back(InputLayoutLambda);

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
    auto PixelShaderLambda = [=]() mutable {
        TerrainComponent.m_DXResConfig.m_pDX11Device->m_pImmediateContext->PSSetShader(TempPixelShader, nullptr, 0);
        };
    TerrainComponent.m_DXResConfig.m_pContextResourcePtr.push_back(PixelShaderLambda);

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
    m_pDX11Device->m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);

    auto VertexBufferLambda = [=]() mutable {
        TerrainComponent.m_DXResConfig.m_pDX11Device->m_pImmediateContext->IASetVertexBuffers(0, 1, &TempVertexBuffer, &stride, &offset);
        };
    TerrainComponent.m_DXResConfig.m_pContextResourcePtr.push_back(VertexBufferLambda);


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
    m_pDX11Device->m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    auto IndexBufferLambda = [=]() mutable {
        TerrainComponent.m_DXResConfig.m_pDX11Device->m_pImmediateContext->IASetIndexBuffer(TempIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        };

    TerrainComponent.m_DXResConfig.m_pContextResourcePtr.push_back(IndexBufferLambda);

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
    auto ConstantBufferLambda = [=]()
        {
        TerrainComponent.m_DXResConfig.m_pDX11Device->m_pImmediateContext->VSSetConstantBuffers(0, 1, &TempConstantBuffer);
        };
    TerrainComponent.m_DXResConfig.SetConstantBuffer(TempConstantBuffer);
    TerrainComponent.m_DXResConfig.m_pContextResourcePtr.push_back(ConstantBufferLambda);


    const wchar_t* TextureName = L"grey_grid.jpg";
    m_HR = CreateDDSTextureFromFile(m_pDX11Device->m_pD3D11Device, TextureName, nullptr, &m_pDX11Device->m_TextureRV);
    if (FAILED(m_HR))
    {
        MessageBox(nullptr, L"Failed to initialize texture from file", L"Error", MB_OK);
        return m_HR;
    }
    auto TextureLambda = [=]() mutable {
        TerrainComponent.m_DXResConfig.m_pDX11Device->m_pImmediateContext->PSSetShaderResources(0, 1, &TerrainComponent.m_DXResConfig.m_pDX11Device->m_TextureRV);
        };
    TerrainComponent.m_DXResConfig.m_pContextResourcePtr.push_back(TextureLambda);

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

    CSceneGraphNode<CGameEntity3DComponent>* TerrainComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
    TerrainComponentNode->m_tType = TerrainComponent;

    Terrain.m_SceneGraph.m_pRootNode = TerrainComponentNode;

    SScene.AddEntityToScene(Terrain);

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

void CTerrainGenerator::GenerateTerrain()
{
    CScene& Scene = CScene::GetScene();
    CGameEntity3D Terrain{};
    Terrain.m_GameEntityTag = "Terrain";
    CGameEntity3DComponent TerrainComponent;
    TerrainComponent.m_GameEntityTag = "TerrainComponent";

    uint32_t& ShaderProgram{ TerrainComponent.m_OpenGLResource.m_ShaderProgram };
    uint32_t& VAO{ TerrainComponent.m_OpenGLResource.m_VAO };
    uint32_t& VBO{ TerrainComponent.m_OpenGLResource.m_VBO };
    uint32_t& EBO{ TerrainComponent.m_OpenGLResource.m_EBO };
    uint32_t& Texture{ TerrainComponent.m_OpenGLResource.m_Texture };

    CShaderGenerator ShaderGenerator;
    //ShaderGenerator.GenerateBaseShaders(&TerrainComponent.m_OpenGLResource);
    ShaderGenerator.GenerateLightShaders(&TerrainComponent.m_OpenGLResource);

    float Vertices[] = {
        // positions       // normals           // texture coords
        -0.5f, -0.5f,  0.f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    };

    uint32_t Indices[] =
    {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    TerrainComponent.SetLocationF(0.f, 0.f, -10.5f);
    TerrainComponent.m_CollisionComponent.m_Center = glm::vec3{ 0.f, 0.f, -10.5f };
    TerrainComponent.m_CollisionComponent.m_Extents = glm::vec3{ 0.5f, 0.5f, 0.5f };

	// Previous OpenGL setup code
    
    ////glGenVertexArrays(1, &VAO);
    //glGenVertexArrays(1, &VAO);
    //glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);
    //// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    //glBindVertexArray(VAO);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    //// position attribute
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);
    //// index attribute
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load("grey_grid.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glUniform1i(glGetUniformLocation(ShaderProgram, "material.diffuse"), 0);
    glUseProgram(ShaderProgram);
    
    if (m_pOpenGLDevice)
    {
        glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, std::string{ "projection" }.c_str()), 1, GL_FALSE, &COpenGLDevice::g_ProjectionMatrix[0][0]);
    }

	TerrainComponent.m_OpenGLResource.m_DrawMode = GL_TRIANGLES;

    auto DrawLambda = []() {
        glDrawArrays(GL_TRIANGLES, 0, 36);
    };
	TerrainComponent.m_OpenGLResource.m_pContextResourcePtr.push_back(DrawLambda);

    CSceneGraphNode<CGameEntity3DComponent>* TerrainComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
    TerrainComponentNode->m_tType = TerrainComponent;
    Terrain.m_SceneGraph.m_pRootNode = TerrainComponentNode;
    std::vector<uint32_t> TempIndices{0, 1, 3, 1, 2, 3};
    TerrainComponent.m_OpenGLResource.m_Indices.insert(TerrainComponent.m_OpenGLResource.m_Indices.end(), TempIndices.begin(), TempIndices.end());

    Scene.AddEntityToScene(Terrain);
}
#include "Renderer.h"
#include "CoreTimer.h"
#include "Scene.h"
#include "Logger.h"

Renderer::Renderer(HWND Viewport)
{
    m_DX11Device.SetViewport(m_Viewport);
}

void Renderer::SetViewport(HWND InViewport)
{
    m_DX11Device.SetViewport(InViewport);
}

void Renderer::InitRenderer()
{
    m_DX11Device.InitDX11Device();

    //m_DX11Device.AddTestLine();
}

DX11Device* Renderer::GetDX11Device()
{
    return &m_DX11Device;
}

void Renderer::Render(float RotX, float RotY, float EyeX, float EyeY, float EyeZ)
{
    //m_DX11Device.Render(RotX, RotY, LocX, LocY, LocZ);

    
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

        DX11Device::m_ViewMatrix = XMMatrixIdentity();
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
        DX11Device::m_ViewMatrix = XMMatrixTranslation(-EyeX, -EyeY, -EyeZ) * RotationMatrixXY;

        m_DX11Device.m_ImmediateContext->ClearRenderTargetView(m_DX11Device.m_RenderTargetView, Colors::MidnightBlue);
        m_DX11Device.m_ImmediateContext->ClearDepthStencilView(m_DX11Device.pDefDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        m_DX11Device.m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        UINT stride = sizeof(SimpleVertex);
        UINT stride2 = sizeof(SimpleColorVertex);
        UINT offset = 0;

        int SceneEntityIndex = 0;

        auto Entity = SScene.GetSceneList().at(0);

        // Main rendering loop

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
        //                cb.mView = XMMatrixTranspose(DX11Device::m_ViewMatrix);
        //                cb.mProjection = XMMatrixTranspose(DX11Device::m_ProjectionMatrix);

        //                ID3D11Buffer* ConstantBuffer = SceneEntityComponentIt.m_DXResConfig.m_ConstantBuffer;
        //                m_DX11Device.m_ImmediateContext->UpdateSubresource(ConstantBuffer, 0, nullptr, &cb, 0, 0);

        //                ID3D11Buffer* VertexBuffer = SceneEntityComponentIt.m_DXResConfig.m_VertexBuffer;
        //                assert(VertexBuffer != nullptr);

        //                m_DX11Device.m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

        //                ID3D11Buffer* IndexBuffer = SceneEntityComponentIt.m_DXResConfig.m_IndexBuffer;
        //                DXGI_FORMAT IndexBufferFormat = SceneEntityComponentIt.m_DXResConfig.m_IndexBufferFormat;
        //                UINT IndexBufferOffset = SceneEntityComponentIt.m_DXResConfig.m_IndexBufferOffset;
        //                m_DX11Device.m_ImmediateContext->IASetIndexBuffer(IndexBuffer, IndexBufferFormat, IndexBufferOffset);

        //                ID3D11InputLayout* InputLayout = SceneEntityComponentIt.m_DXResConfig.GetInputLayout();
        //                m_DX11Device.m_ImmediateContext->IASetInputLayout(InputLayout);

        //                ID3D11VertexShader* VertexShader = SceneEntityComponentIt.m_DXResConfig.m_VertexShader;
        //                ID3D11PixelShader* PixelShader = SceneEntityComponentIt.m_DXResConfig.m_PixelShader;

        //                m_DX11Device.m_ImmediateContext->VSSetShader(VertexShader, nullptr, 0);
        //                m_DX11Device.m_ImmediateContext->PSSetShader(PixelShader, nullptr, 0);

        //                m_DX11Device.m_ImmediateContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
        //                m_DX11Device.m_ImmediateContext->OMSetDepthStencilState(m_DX11Device.pDefDepthStencilState, 0);

        //                m_DX11Device.m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //                m_DX11Device.m_ImmediateContext->DrawIndexed(512, 0, 0);
        //            }
        //        }
        //    }
        //    // Render Cube yellow outline
        //    else if (SceneEntityIt.m_GameEntityTag != "Linetrace")
        //    {
        //        ConstantBuffer cb = SceneEntityIt.GetConstantBuffer();
        //        cb.mWorld = XMMatrixTranspose(cb.mWorld);
        //        cb.mView = XMMatrixTranspose(DX11Device::m_ViewMatrix);
        //        cb.mProjection = XMMatrixTranspose(DX11Device::m_ProjectionMatrix);

        //        ID3D11Buffer* CB = SceneEntityIt.m_DXResConfig.m_ConstantBuffer;
        //        m_DX11Device.m_ImmediateContext->UpdateSubresource(CB, 0, nullptr, &cb, 0, 0);

        //        ID3D11Buffer* VertexBuffer = SceneEntityIt.m_DXResConfig.m_VertexBuffer;
        //        assert(VertexBuffer != nullptr);

        //        m_DX11Device.m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride2, &offset);

        //        ID3D11Buffer* IndexBuffer = SceneEntityIt.m_DXResConfig.m_IndexBuffer;
        //        DXGI_FORMAT IndexBufferFormat = SceneEntityIt.m_DXResConfig.m_IndexBufferFormat;
        //        UINT IndexBufferOffset = SceneEntityIt.m_DXResConfig.m_IndexBufferOffset;
        //        m_DX11Device.m_ImmediateContext->IASetIndexBuffer(IndexBuffer, IndexBufferFormat, IndexBufferOffset);

        //        ID3D11InputLayout* InputLayout = SceneEntityIt.m_DXResConfig.GetInputLayout();
        //        m_DX11Device.m_ImmediateContext->IASetInputLayout(InputLayout);

        //        ID3D11VertexShader* VertexShader = SceneEntityIt.m_DXResConfig.m_VertexShader;
        //        ID3D11PixelShader* PixelShader = SceneEntityIt.m_DXResConfig.m_PixelShader;

        //        m_DX11Device.m_ImmediateContext->VSSetShader(VertexShader, nullptr, 0);
        //        m_DX11Device.m_ImmediateContext->PSSetShader(PixelShader, nullptr, 0);

        //        m_DX11Device.m_ImmediateContext->VSSetConstantBuffers(0, 1, &CB);
        //        m_DX11Device.m_ImmediateContext->OMSetDepthStencilState(m_DX11Device.pDepthStencilStateOutline, 0);

        //        m_DX11Device.m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //        m_DX11Device.m_ImmediateContext->DrawIndexed(512, 0, 0);
        //    }
        //    else
        //    {
        //        ID3D11Buffer* VertexBuffer = SceneEntityIt.m_DXResConfig.GetVertexBuffer();
        //        m_DX11Device.m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
        //        m_DX11Device.m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

        //        ConstantBuffer cb = SceneEntityIt.GetConstantBuffer();
        //        cb.mWorld = XMMatrixTranspose(cb.mWorld);
        //        cb.mView = XMMatrixTranspose(DX11Device::m_ViewMatrix);
        //        cb.mProjection = XMMatrixTranspose(DX11Device::m_ProjectionMatrix);

        //        ID3D11Buffer* ConstantBuffer = SceneEntityIt.m_DXResConfig.m_ConstantBuffer;
        //        m_DX11Device.m_ImmediateContext->UpdateSubresource(ConstantBuffer, 0, nullptr, &cb, 0, 0);

        //        m_DX11Device.m_ImmediateContext->Draw(2, 0);
        //    }
        //}



            for (auto SceneEntityIt : SScene.GetSceneList())
            {
                // Render Cube 
                if (SceneEntityIt.m_GameEntityTag == "SingleCubeComponentEntity")
                {
                    if (SceneEntityIt.m_GameEntity3DComponent.size())
                    {
                        for (auto SceneEntityComponentIt : SceneEntityIt.m_GameEntity3DComponent)
                        {
                            auto TempConfigMap = SScene.DXResourceConfigMap;
                            int HashKey = SceneEntityComponentIt.DXResourceHashKey;
                            DXResourcesConfig DXConfig = SScene.GetDXResourceConfig(HashKey);

                            ConstantBuffer cb = SceneEntityComponentIt.GetConstantBuffer();
                            cb.mWorld = XMMatrixTranspose(cb.mWorld);
                            cb.mView = XMMatrixTranspose(DX11Device::m_ViewMatrix);
                            cb.mProjection = XMMatrixTranspose(DX11Device::m_ProjectionMatrix);

                            //ID3D11Buffer* ConstantBuffer = SceneEntityComponentIt.m_DXResConfig.GetConstantBuffer();
                            ID3D11Buffer* ConstantBuffer = DXConfig.GetConstantBuffer();
                            m_DX11Device.m_ImmediateContext->UpdateSubresource(ConstantBuffer, 0, nullptr, &cb, 0, 0);

                            //ID3D11Buffer* VertexBuffer = SceneEntityComponentIt.m_DXResConfig.GetVertexBuffer();
                            ID3D11Buffer* VertexBuffer = DXConfig.GetVertexBuffer();
                            assert(VertexBuffer != nullptr);
                            m_DX11Device.m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

                            //ID3D11Buffer* IndexBuffer = SceneEntityComponentIt.m_DXResConfig.GetIndexBuffer();
                            //DXGI_FORMAT IndexBufferFormat = SceneEntityComponentIt.m_DXResConfig.GetIndexBufferFormat();
                            //UINT IndexBufferOffset = SceneEntityComponentIt.m_DXResConfig.GetIndexBufferOffset();
                            ID3D11Buffer* IndexBuffer = DXConfig.GetIndexBuffer();                        
                            DXGI_FORMAT IndexBufferFormat = DXConfig.GetIndexBufferFormat();
                            UINT IndexBufferOffset = DXConfig.GetIndexBufferOffset();
                            m_DX11Device.m_ImmediateContext->IASetIndexBuffer(IndexBuffer, IndexBufferFormat, IndexBufferOffset);

                            //ID3D11InputLayout* InputLayout = SceneEntityComponentIt.m_DXResConfig.GetInputLayout();
                            ID3D11InputLayout* InputLayout = DXConfig.GetInputLayout();
                            m_DX11Device.m_ImmediateContext->IASetInputLayout(InputLayout);

                            //ID3D11VertexShader* VertexShader = SceneEntityComponentIt.m_DXResConfig.GetVertexShader();
                            //ID3D11PixelShader* PixelShader = SceneEntityComponentIt.m_DXResConfig.GetPixelShader();
                            ID3D11VertexShader* VertexShader = DXConfig.GetVertexShader();
                            ID3D11PixelShader* PixelShader = DXConfig.GetPixelShader();

                            m_DX11Device.m_ImmediateContext->VSSetShader(VertexShader, nullptr, 0);
                            m_DX11Device.m_ImmediateContext->PSSetShader(PixelShader, nullptr, 0);

                            m_DX11Device.m_ImmediateContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);

                            //ID3D11DepthStencilState* DepthStencilState = DXConfig.GetDepthStencilState();
                            //m_DX11Device.m_ImmediateContext->OMSetDepthStencilState(DepthStencilState, 0);
                            m_DX11Device.m_ImmediateContext->OMSetDepthStencilState(m_DX11Device.pDefDepthStencilState, 0);

                            m_DX11Device.m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                            m_DX11Device.m_ImmediateContext->DrawIndexed(512, 0, 0);
                        }
                    }
                }
                 //Render Cube yellow outline
                else if (SceneEntityIt.m_GameEntityTag != "Linetrace")
                {
                    ConstantBuffer cb = SceneEntityIt.GetConstantBuffer();
                    cb.mWorld = XMMatrixTranspose(cb.mWorld);
                    cb.mView = XMMatrixTranspose(DX11Device::m_ViewMatrix);
                    cb.mProjection = XMMatrixTranspose(DX11Device::m_ProjectionMatrix);

                    ID3D11Buffer* CB = SceneEntityIt.m_DXResConfig.GetConstantBuffer();
                    m_DX11Device.m_ImmediateContext->UpdateSubresource(CB, 0, nullptr, &cb, 0, 0);

                    ID3D11Buffer* VertexBuffer = SceneEntityIt.m_DXResConfig.GetVertexBuffer();
                    assert(VertexBuffer != nullptr);

                    m_DX11Device.m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride2, &offset);

                    ID3D11Buffer* IndexBuffer = SceneEntityIt.m_DXResConfig.GetIndexBuffer();
                    DXGI_FORMAT IndexBufferFormat = SceneEntityIt.m_DXResConfig.GetIndexBufferFormat();
                    UINT IndexBufferOffset = SceneEntityIt.m_DXResConfig.GetIndexBufferOffset();
                    m_DX11Device.m_ImmediateContext->IASetIndexBuffer(IndexBuffer, IndexBufferFormat, IndexBufferOffset);

                    ID3D11InputLayout* InputLayout = SceneEntityIt.m_DXResConfig.GetInputLayout();
                    m_DX11Device.m_ImmediateContext->IASetInputLayout(InputLayout);

                    ID3D11VertexShader* VertexShader = SceneEntityIt.m_DXResConfig.GetVertexShader();
                    ID3D11PixelShader* PixelShader = SceneEntityIt.m_DXResConfig.GetPixelShader();

                    m_DX11Device.m_ImmediateContext->VSSetShader(VertexShader, nullptr, 0);
                    m_DX11Device.m_ImmediateContext->PSSetShader(PixelShader, nullptr, 0);

                    m_DX11Device.m_ImmediateContext->VSSetConstantBuffers(0, 1, &CB);
                    m_DX11Device.m_ImmediateContext->OMSetDepthStencilState(m_DX11Device.pDepthStencilStateOutline, 0);

                    m_DX11Device.m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                    m_DX11Device.m_ImmediateContext->DrawIndexed(512, 0, 0);
                }
                else
                {
                    ID3D11Buffer* VertexBuffer = SceneEntityIt.m_DXResConfig.GetVertexBuffer();
                    m_DX11Device.m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
                    m_DX11Device.m_ImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

                    ConstantBuffer cb = SceneEntityIt.GetConstantBuffer();
                    cb.mWorld = XMMatrixTranspose(cb.mWorld);
                    cb.mView = XMMatrixTranspose(DX11Device::m_ViewMatrix);
                    cb.mProjection = XMMatrixTranspose(DX11Device::m_ProjectionMatrix);

                    ID3D11Buffer* ConstantBuffer = SceneEntityIt.m_DXResConfig.GetConstantBuffer();
                    m_DX11Device.m_ImmediateContext->UpdateSubresource(ConstantBuffer, 0, nullptr, &cb, 0, 0);

                    m_DX11Device.m_ImmediateContext->Draw(2, 0);
                }
            }

            m_DX11Device.m_SwapChain->Present(0, 0);
        //}
    
}

void Renderer::Raycast(float OriginX, float OriginY, float OriginZ, float DestX, float DestY, float DestZ)
{
    FTimerManager& TimerManager = FTimerManager::GetTimerManager();
    Scene& SScene = Scene::GetScene();
    Logger& SLogger = Logger::GetLogger();

    //m_DX11Device.InitLine(OriginX, OriginY, OriginZ, DestX, DestY, DestZ);

    SLogger.Log("Renderer.cpp, Raycast() : ",
        "\nOriginX = ", OriginX,
        "\nOriginY = ", OriginY,
        "\nOriginZ = ", OriginZ,
        "\nDestX = ", DestX,
        "\nDestY = ", DestY,
        "\nDestZ = ", DestZ
    );

    m_DX11Device.CheckCollision(OriginX, OriginY, OriginZ, DestX, DestY, DestZ);

    //SceneList& TempSceneList = SScene.m_SceneList; 

    //TimerManager.SetTimer<SceneList, void, &SceneList::RemoveLastElement>(TempSceneList, 3.0f);
    //TimerManager.SetTimer<SceneList, void, &SceneList::BreakpointTest>(SScene.m_SceneList, 3.0f);
     
    //TimerManager.SetTimer<DX11Device, void, &DX11Device::RemoveLastElementFromScene>(m_DX11Device, 3.0f); // Working

    //TimerManager.SetTimer<Scene, void, >
    //TimerManager.SetTimer<Scene, void, &Scene::RemoveLastEntityFromScene>(&Scene::GetScene());
}

void Renderer::AddOutline()
{
    //m_DX11Device.CheckCollision();
}

void Renderer::AddGizmo()
{
    m_DX11Device.AddGizmo();
}

void Renderer::CleanupRenderer()
{
    m_DX11Device.CleanupDX11Device();
}
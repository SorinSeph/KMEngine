#include "Renderer.h"
#include "CoreClock.h"
#include "Scene.h"
#include "Logger.h"
#include "Math.h"

CRenderer::CRenderer(HWND Viewport)
{
    CLogger& Logger = CLogger::GetLogger();
    Logger.Log("CRenderer constructor from", __FILE__);
    m_DX11evice.SetViewport(m_Viewport);
}

void CRenderer::SetViewport(HWND InViewport)
{
    m_DX11evice.SetViewport(InViewport);
}

void CRenderer::InitRenderer()
{
    m_DX11evice.InitDX11Device();

    //m_DX11Device.AddTestLine();
}

CDX11Device* CRenderer::GetDX11Device()
{
    return &m_DX11evice;
}

void CRenderer::Render(float RotX, float RotY, float EyeX, float EyeY, float EyeZ)
{
    CScene& Scene = CScene::GetScene();

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

    m_DX11evice.m_pImmediateContext->ClearRenderTargetView(m_DX11evice.m_pRenderTargetView, Colors::MidnightBlue);
    m_DX11evice.m_pImmediateContext->ClearDepthStencilView(m_DX11evice.pDefDepthStencilView3, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_DX11evice.m_pImmediateContext->OMSetDepthStencilState(m_DX11evice.pDefDepthStencilState3, 0);

    auto SceneEntityList = Scene.GetSceneList();

    /**
    * Render loop
    */

    CTimerManager& TimerManager = CTimerManager::GetTimerManager();
	m_CubeLocZ = FInterpConstantTo(m_CubeLocZ, 10.0f, TimerManager.m_pCoreClock->GetFDeltaTime(), 2.0f);

	for (auto SceneEntityIt : SceneEntityList)
    {
        if (SceneEntityIt.m_GameEntityTag == "TexturedCube")
        {
			SceneEntityIt.SetLocationF(0.0f, 0.0f, m_CubeLocZ);
        }
    }

    CLogger& Logger = CLogger::GetLogger();  

    Logger.Log();

    for (auto& SceneEntityIt : SceneEntityList)
    {
        if (SceneEntityIt.m_GameEntityTag == "TexturedCube")
        {
            SConstantBuffer CB = SceneEntityIt.GetConstantBuffer();
            ID3D11Buffer* CB2 = SceneEntityIt.m_DXResConfig.GetConstantBuffer();
			//SceneEntityIt.SetLocationF(-6.0f, 0.0f, m_CubeLocZ);
			auto LocationMatrix = SceneEntityIt.GetLocation();

            CB.mWorld = LocationMatrix;
			//CB.mWorld = XMMatrixTranslation(LocX, LocY, LocZ);

            CB.mWorld = XMMatrixTranspose(CB.mWorld);
            CB.mView = XMMatrixTranspose(CDX11Device::m_ViewMatrix);
            CB.mProjection = XMMatrixTranspose(CDX11Device::m_ProjectionMatrix);
            m_DX11evice.m_pImmediateContext->UpdateSubresource(CB2, 0, nullptr, &CB, 0, 0);

            SceneEntityIt.m_DXResConfig.Execute();

            m_DX11evice.m_pImmediateContext->OMSetDepthStencilState(m_DX11evice.pDefDepthStencilState3, 0);

            m_DX11evice.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            m_DX11evice.m_pImmediateContext->DrawIndexed(36, 0, 0);
        }
        //else if (SceneEntityIt.m_GameEntityTag == "TexturedCube2")
        //{
        //    SConstantBuffer CB = SceneEntityIt.GetConstantBuffer();
        //    ID3D11Buffer* CB2 = SceneEntityIt.m_DXResConfig.GetConstantBuffer();
        //    CB.mWorld = XMMatrixTranspose(CB.mWorld);
        //    CB.mView = XMMatrixTranspose(CDX11Device::m_ViewMatrix);
        //    CB.mProjection = XMMatrixTranspose(CDX11Device::m_ProjectionMatrix);
        //    m_DX11Device.m_ImmediateContext->UpdateSubresource(CB2, 0, nullptr, &CB, 0, 0);

        //    SceneEntityIt.m_DXResConfig.Execute();

        //    m_DX11Device.m_ImmediateContext->OMSetDepthStencilState(m_DX11Device.pDefDepthStencilState3, 0);

        //    m_DX11Device.m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        //    m_DX11Device.m_ImmediateContext->DrawIndexed(36, 0, 0);
        //}
        else if (SceneEntityIt.m_GameEntityTag == "Terrain")
        {
            SConstantBuffer CB = SceneEntityIt.GetConstantBuffer();
            ID3D11Buffer* CB2 = SceneEntityIt.m_DXResConfig.GetConstantBuffer();
            CB.mWorld = XMMatrixTranspose(CB.mWorld);
            CB.mView = XMMatrixTranspose(CDX11Device::m_ViewMatrix);
            CB.mProjection = XMMatrixTranspose(CDX11Device::m_ProjectionMatrix);
            m_DX11evice.m_pImmediateContext->UpdateSubresource(CB2, 0, nullptr, &CB, 0, 0);

            UINT Stride = sizeof(SSimpleVertex);
            UINT Offset = 0;

            ID3D11Buffer* VertexBuffer = SceneEntityIt.m_DXResConfig.m_VertexBuffer;
            m_DX11evice.m_pImmediateContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);

            ID3D11Buffer* IndexBuffer = SceneEntityIt.m_DXResConfig.m_IndexBuffer;
            DXGI_FORMAT IndexBufferFormat = SceneEntityIt.m_DXResConfig.m_IndexBufferFormat;
            UINT IndexBufferOffset = SceneEntityIt.m_DXResConfig.m_IndexBufferOffset;
            m_DX11evice.m_pImmediateContext->IASetIndexBuffer(IndexBuffer, IndexBufferFormat, IndexBufferOffset);

            ID3D11InputLayout* InputLayout = SceneEntityIt.m_DXResConfig.GetInputLayout();
            m_DX11evice.m_pImmediateContext->IASetInputLayout(InputLayout);

            //ID3D11VertexShader* VertexShader = SceneEntityIt.m_DXResConfig.m_VertexShader;
            //ID3D11PixelShader* PixelShader = SceneEntityIt.m_DXResConfig.m_PixelShader;
            //m_DX11Device.m_ImmediateContext->VSSetShader(VertexShader, nullptr, 0);
            //m_DX11Device.m_ImmediateContext->PSSetShader(PixelShader, nullptr, 0);
            SceneEntityIt.m_DXResConfig.Execute();

            m_DX11evice.m_pImmediateContext->VSSetConstantBuffers(0, 1, &CB2);
            m_DX11evice.m_pImmediateContext->OMSetDepthStencilState(m_DX11evice.pDefDepthStencilState3, 0);

            m_DX11evice.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
            //m_DX11Device.m_ImmediateContext->DrawIndexed(512, 0, 0);
            m_DX11evice.m_pImmediateContext->DrawIndexed(6, 0, 0);
        }
        else if (SceneEntityIt.m_GameEntityTag == "Raycast")
        {
            SConstantBuffer CB = SceneEntityIt.GetConstantBuffer();
            ID3D11Buffer* CB2 = SceneEntityIt.m_DXResConfig.GetConstantBuffer();
            CB.mWorld = XMMatrixTranspose(CB.mWorld);
            CB.mView = XMMatrixTranspose(CDX11Device::m_ViewMatrix);
            CB.mProjection = XMMatrixTranspose(CDX11Device::m_ProjectionMatrix);
            m_DX11evice.m_pImmediateContext->UpdateSubresource(CB2, 0, nullptr, &CB, 0, 0);

            m_DX11evice.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

            SceneEntityIt.m_DXResConfig.Execute();

            m_DX11evice.m_pImmediateContext->Draw(2, 0);
        }
    }

    m_DX11evice.m_SwapChain->Present(0, 0);
}

void CRenderer::Raycast(float OriginX, float OriginY, float OriginZ, float DestX, float DestY, float DestZ)
{
    CTimerManager& TimerManager = CTimerManager::GetTimerManager();
    CScene& SScene = CScene::GetScene();
    CLogger& SLogger = CLogger::GetLogger();

    m_DX11evice.InitRaycast(OriginX, OriginY, OriginZ, DestX, DestY, DestZ);

    SLogger.Log(
        "Renderer_.cpp, Raycast() : ",
        "\nOriginX = ", OriginX,
        "\nOriginY = ", OriginY,
        "\nOriginZ = ", OriginZ,
        "\nDestX = ", DestX,
        "\nDestY = ", DestY,
        "\nDestZ = ", DestZ
    );

    //m_DX11Device.CheckCollision(OriginX, OriginY, OriginZ, DestX, DestY, DestZ);
}

void CRenderer::AddOutline()
{
    //m_DX11Device.CheckCollision();
}

void CRenderer::AddGizmo()
{
    m_DX11evice.AddGizmo();
}

void CRenderer::CleanupRenderer()
{
    m_DX11evice.CleanupDX11Device();
}
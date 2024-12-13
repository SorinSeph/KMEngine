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

    /**
    * WIP Collision checking section, to be refactored into its own function
    */

    

    for (auto& SceneEntityIt : SceneEntityList)
    {
        //auto EntityComponent = SceneEntityIt.m_SceneGraph.m_pRootNode->Type;
        std::vector<CSceneGraphNode<CGameEntity3DComponent>*> EntityComponentVector;
		//SceneEntityIt.m_SceneGraph.Traverse(SceneEntityIt.m_SceneGraph.m_pRootNode, EntityComponentVector);
		EntityComponentVector.push_back(SceneEntityIt.m_SceneGraph.m_pRootNode);
		//EntityComponentVector.push_back(SceneEntityIt.m_SceneGraph.m_pRootNode->ChildNode[0]);

        for (auto& EntityComponent : EntityComponentVector)
        {
            //if (EntityComponent->Type.m_GameEntityTag == "TexturedCubeComponent" || "TexturedCubeComponent2")
            //{
            if (EntityComponent->m_TType.m_GameEntityTag == "FrustumComponent")
            {
                SCollisionBuffer CB = EntityComponent->m_TType.m_CollisionBuffer;
                ID3D11Buffer* CB2 = EntityComponent->m_TType.m_DXResConfig.GetConstantBuffer();
                //SceneEntityIt.SetLocationF(-6.0f, 0.0f, m_CubeLocZ);
                auto LocationMatrix = EntityComponent->m_TType.GetLocation();
                auto RotationMatrix = EntityComponent->m_TType.m_QuatRotationMatrix;
                auto ScaleMatrix = EntityComponent->m_TType.GetScale();

                CB.mWorld = /* ScaleMatrix * RotationMatrix * */LocationMatrix;

                CB.mWorld = XMMatrixTranspose(CB.mWorld);
                CB.mView = XMMatrixTranspose(CDX11Device::m_ViewMatrix);
                CB.mProjection = XMMatrixTranspose(CDX11Device::m_ProjectionMatrix);
                //CB.mDoesFrustumContain = g_DoesFrustumContain;
                m_DX11evice.m_pImmediateContext->UpdateSubresource(CB2, 0, nullptr, &CB, 0, 0);
				Logger.Log("Renderer.cpp, Render() : g_DoesFrustumContain = ", g_DoesFrustumContain);
            }
            else 
            {
                SConstantBuffer CB = EntityComponent->m_TType.GetConstantBuffer();
                ID3D11Buffer* CB2 = EntityComponent->m_TType.m_DXResConfig.GetConstantBuffer();
                //SceneEntityIt.SetLocationF(-6.0f, 0.0f, m_CubeLocZ);
                auto LocationMatrix = EntityComponent->m_TType.GetLocation();
                auto RotationMatrix = EntityComponent->m_TType.m_QuatRotationMatrix;
                auto ScaleMatrix = EntityComponent->m_TType.GetScale();

                CB.mWorld = /* ScaleMatrix * RotationMatrix * */LocationMatrix;

                CB.mWorld = XMMatrixTranspose(CB.mWorld);
                CB.mView = XMMatrixTranspose(CDX11Device::m_ViewMatrix);
                CB.mProjection = XMMatrixTranspose(CDX11Device::m_ProjectionMatrix);
                m_DX11evice.m_pImmediateContext->UpdateSubresource(CB2, 0, nullptr, &CB, 0, 0);
            }


                EntityComponent->m_TType.m_DXResConfig.Execute();

                m_DX11evice.m_pImmediateContext->OMSetDepthStencilState(m_DX11evice.pDefDepthStencilState3, 0);

                m_DX11evice.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                m_DX11evice.m_pImmediateContext->DrawIndexed(36, 0, 0);
            //}
        }
    }

    m_DX11evice.m_SwapChain->Present(0, 0);
}

void CRenderer::Raycast(float DestinationX, float DestinationY)
{
	CLogger& Logger = CLogger::GetLogger();

    XMVECTOR Origin = XMVector3Unproject(
        XMVECTOR{0, 0, 0},
        0,
        0,
        518,
        825,
        0,
        1,
        CDX11Device::m_ProjectionMatrix,
        CDX11Device::m_ViewMatrix,
        CDX11Device::m_WorldMatrix);

	XMVECTOR Destination = XMVector3Unproject(
		XMVECTOR{ DestinationX, DestinationY, 1 },
		0,
		0,
		518,
		825,
		0,
		1,
		CDX11Device::m_ProjectionMatrix,
		CDX11Device::m_ViewMatrix,
		CDX11Device::m_WorldMatrix);

	XMVECTOR Direction = XMVector3Normalize(Destination - Origin);
    XMFLOAT3 OriginF;
	XMStoreFloat3(&OriginF, Origin);
	XMFLOAT3 DirectionF;
	XMStoreFloat3(&DirectionF, Direction);

    m_DX11evice.Raycast(0, 0, 0, DirectionF.x, DirectionF.y, DirectionF.z);
    Logger.Log("Renderer.cpp, Raycast2()");
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
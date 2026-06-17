#include "RendererDirectX11.h"
#include "../modules/GraphicsModule.h"

void CRendererDirectX11::SetViewport(HWND InViewport)
{
    m_DX11Device.SetViewport(InViewport);
}

void CRendererDirectX11::SetViewportSize(int Width, int Height)
{
    m_ViewportWidth = Width;
    m_ViewportHeight = Height;
    m_DX11Device.SetViewportSize(Width, Height);
}

void CRendererDirectX11::InitRenderer()
{
    m_DX11Device.InitDX11Device();

    //m_DX11Device.AddTestLine();
}

void CRendererDirectX11::SetCameraParams(float RotX, float RotY, float EyeX, float EyeY, float EyeZ)
{
	m_RotX = RotX;
	m_RotY = RotY;
	m_EyeX = EyeX;
	m_EyeY = EyeY;
	m_EyeZ = EyeZ;
}

CDX11Device* CRendererDirectX11::GetDX11Device()
{
    return &m_DX11Device;
}

void CRendererDirectX11::Render()
{
    CScene& Scene = CScene::GetScene();
    CLogger& Logger = CLogger::GetLogger();

    XMFLOAT3 CameraPos(m_EyeX, m_EyeY, m_EyeZ);
    XMVECTOR CameraVec = XMLoadFloat3(&CameraPos);

    Logger.Log("CRenderer::Render: EyeX = ", m_EyeX, "EyeY = ", m_EyeY, "EyeZ = ", m_EyeZ, "\n");

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

    //CDX11Device::m_ViewMatrix = XMMatrixIdentity();
    XMMATRIX RotationMatrixX
    (
        1.f, 0.f, 0.f, 0.f,
        0.f, std::cosf(XMConvertToRadians(m_RotX)), -std::sinf(XMConvertToRadians(m_RotX)), 0.f,
        0.f, std::sinf(XMConvertToRadians(m_RotX)), std::cosf(XMConvertToRadians(m_RotX)), 0.f,
        0.f, 0.f, 0.f, 1.f
    );

    XMMATRIX RotationMatrixY
    (
        std::cosf(XMConvertToRadians(m_RotY)), 0.f, std::sinf(XMConvertToRadians(m_RotY)), 0.f,
        0.f, 1.f, 0.f, 0.f,
        -std::sinf(XMConvertToRadians(m_RotY)), 0.f, std::cosf(XMConvertToRadians(m_RotY)), 0.f,
        0.f, 0.f, 0.f, 1.f
    );

    XMMATRIX RotationMatrixXY = RotationMatrixY * RotationMatrixX;
    //CDX11Device::m_ViewMatrix = XMMatrixTranslation(-EyeX, -EyeY, -EyeZ) * RotationMatrixXY;
    CCamera::m_ViewMatrix = XMMatrixTranslation(-m_EyeX, -m_EyeY, -m_EyeZ) * (XMMatrixRotationY(XMConvertToRadians(-m_RotY)) * XMMatrixRotationX(XMConvertToRadians(-m_RotX)));

    m_DX11Device.m_pImmediateContext->ClearRenderTargetView(m_DX11Device.m_pRenderTargetView, Colors::MidnightBlue);
    m_DX11Device.m_pImmediateContext->ClearDepthStencilView(m_DX11Device.pDefDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    //m_DX11Device.m_pImmediateContext->OMSetDepthStencilState(m_DX11Device.pDefDepthStencilState3, 0);

    auto& SceneEntityList = Scene.GetSceneList();

    /**
    * WIP Collision checking section, to be refactored into its own function
    */

    //CDX11Device::m_ViewMatrix = XMMatrixRotationY(XMConvertToRadians(90)) * CDX11Device::m_ViewMatrix;

    for (auto& SceneEntityIt : SceneEntityList)
    {
        //auto EntityComponent = SceneEntityIt.m_SceneGraph.m_pRootNode->Type;
        std::vector<CSceneGraphNode<CGameEntity3DComponent>*> EntityComponentVector;
        //SceneEntityIt.m_SceneGraph.Traverse(SceneEntityIt.m_SceneGraph.m_pRootNode, EntityComponentVector);
        EntityComponentVector.push_back(SceneEntityIt.m_SceneGraph.m_pRootNode);
        //EntityComponentVector.push_back(SceneEntityIt.m_SceneGraph.m_pRootNode->ChildNode[0]);

        if (SceneEntityIt.m_GameEntityTag == "Gizmo" || "OutlineCubeEntity")
        {
            m_DX11Device.m_pImmediateContext->OMSetDepthStencilState(m_DX11Device.pDisabledDepthStencilState, 0);
        }
        //else if (SceneEntityIt.m_GameEntityTag == "OutlineCubeEntity")
        //{
        //    m_DX11Device.m_pImmediateContext->OMSetDepthStencilState(m_DX11Device.pOutlineDepthStencilState, 0);
        //}
        else
        {
            m_DX11Device.m_pImmediateContext->OMSetDepthStencilState(m_DX11Device.pDefDepthStencilState, 0);
        }

        for (auto& EntityComponent : EntityComponentVector)
        {
            //m_DX11Device.OnPreRender();
            //if (EntityComponent->Type.m_GameEntityTag == "TexturedCubeComponent" || "TexturedCubeComponent2")
            //{
            if (EntityComponent->m_tType.m_GameEntityTag == "FrustumComponent")
            {
                SCollisionBuffer CB = EntityComponent->m_tType.m_CollisionBuffer;
                ID3D11Buffer* CB2 = EntityComponent->m_tType.m_DXResConfig.GetConstantBuffer();
                //SceneEntityIt.SetLocationF(-6.0f, 0.0f, m_CubeLocZ);
                auto LocationMatrix = EntityComponent->m_tType.GetLocation();
                auto RotationMatrix = EntityComponent->m_tType.m_QuatRotationMatrix;
                auto ScaleMatrix = EntityComponent->m_tType.GetScale();

                CB.mWorld = ScaleMatrix * RotationMatrix * LocationMatrix;

                CB.mWorld = XMMatrixTranspose(CB.mWorld);
                CB.mView = XMMatrixTranspose(CCamera::m_ViewMatrix);
                CB.mProjection = XMMatrixTranspose(CCamera::m_ProjectionMatrix);
                //CB.mDoesFrustumContain = g_DoesFrustumContain;
                m_DX11Device.m_pImmediateContext->UpdateSubresource(CB2, 0, nullptr, &CB, 0, 0);
                Logger.Log("Renderer.cpp, Render() : g_DoesFrustumContain = ", g_DoesFrustumContain);
            }
            else if (EntityComponent->m_tType.m_GameEntityTag == "GizmoComponent")
            {
                SArrowConstantBuffer CB;// = EntityComponent->m_tType.GetConstantBuffer();
                ID3D11Buffer* CB2 = EntityComponent->m_tType.m_DXResConfig.GetConstantBuffer();
                //SceneEntityIt.SetLocationF(-6.0f, 0.0f, m_CubeLocZ);
                auto LocationMatrix = EntityComponent->m_tType.GetLocation();
                auto RotationMatrix = EntityComponent->m_tType.m_QuatRotationMatrix;
                auto ScaleMatrix = EntityComponent->m_tType.GetScale();

                CB.mWorld = ScaleMatrix * RotationMatrix * LocationMatrix;

                CB.mWorld = XMMatrixTranspose(CB.mWorld);
                CB.mView = XMMatrixTranspose(CCamera::m_ViewMatrix);
                CB.mProjection = XMMatrixTranspose(CCamera::m_ProjectionMatrix);
                CB.mIsHovered = m_DX11Device.m_bGizmoHovered;
                m_DX11Device.m_pImmediateContext->UpdateSubresource(CB2, 0, nullptr, &CB, 0, 0);

                Logger.Log("Renderer.cpp, Render() : bGizmoHovered: ", m_DX11Device.m_bGizmoHovered);
            }
            else
            {
                SConstantBuffer CB = EntityComponent->m_tType.GetConstantBuffer();
                ID3D11Buffer* CB2 = EntityComponent->m_tType.m_DXResConfig.GetConstantBuffer();
                //SceneEntityIt.SetLocationF(-6.0f, 0.0f, m_CubeLocZ);
                auto LocationMatrix = EntityComponent->m_tType.GetLocation();
                auto RotationMatrix = EntityComponent->m_tType.m_QuatRotationMatrix;
                auto ScaleMatrix = EntityComponent->m_tType.GetScale();

                CB.mWorld = ScaleMatrix * RotationMatrix * LocationMatrix;

                CB.mWorld = XMMatrixTranspose(CB.mWorld);
                CB.mView = XMMatrixTranspose(CCamera::m_ViewMatrix);
                CB.mProjection = XMMatrixTranspose(CCamera::m_ProjectionMatrix);
                m_DX11Device.m_pImmediateContext->UpdateSubresource(CB2, 0, nullptr, &CB, 0, 0);

                Logger.Log("Renderer.cpp, Render() : GameEntity3DComponent Tag is:  ", EntityComponent->m_tType.m_GameEntityTag);
            }

            EntityComponent->m_tType.m_DXResConfig.Execute();

            m_DX11Device.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_DX11Device.m_pImmediateContext->DrawIndexed(512, 0, 0);


            //m_DX11Device.OnPostRender();
        //}
        }
    }

    m_DX11Device.m_SwapChain->Present(0, 0);

}

void CRendererDirectX11::AddOutline()
{
    //m_DX11Device.CheckCollision();
}

void CRendererDirectX11::AddGizmo()
{
    m_DX11Device.AddGizmo();
}

void CRendererDirectX11::CleanupRenderer()
{
    m_DX11Device.CleanupDX11Device();
}

void CRendererDirectX11::TestGraphicsModuleLog()
{
    CLogger& Logger = CLogger::GetLogger();

    if (m_pGraphicsModule)
    {
        m_pGraphicsModule->TestLog();
    }
    else
    {
        Logger.Log("Renderer.cpp, Raycast2() : m_pGraphicsModule is nullptr");
    }
}
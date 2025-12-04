#include <any>
#include "UIMessageQueue.h"
#include "Modules/UIModule.h"
#include "Modules/GraphicsModule.h"
#include "Modules/PhysicsModule.h"
#include "Core/CoreClock.h"

void CUIMessageQueue::RayPicking(int MouseX, int MouseY)
{
    float m_InitialMousePosX = MouseX;
    float m_InitialMousePosY = MouseY;
    //float m_InitialMousePosZ = MouseZ;

    CPhysicsModule* pPhysicsModule = static_cast<CPhysicsModule*>(m_pUIModule->m_pMediator->m_ModuleArray[2]);
    CGraphicsModule* pGraphicsModule = static_cast<CGraphicsModule*>(m_pUIModule->m_pMediator->m_ModuleArray[1]);

    CTimerManager& TimerManager = CTimerManager::GetTimerManager();
	CCoreClock* Clock = TimerManager.m_pCoreClock;

    CScene& Scene = CScene::GetScene();
    auto& SceneEntityList = Scene.GetSceneList();
    CLogger& Logger = CLogger::GetLogger();

    CGameEntity3DComponent* pGameEntity3DComponent{ nullptr };

    for (auto& SceneEntity : SceneEntityList)
    {
        if (SceneEntity.m_GameEntityTag == "TexturedCube")
		{
            pGameEntity3DComponent = &SceneEntity.m_SceneGraph.m_pRootNode->m_tType;
			break;
        }
    }

    if (m_pUIModule)
    {
        if (!m_bIsEntitySelected)
        {
			m_bIsEntitySelected = true;

                if (pGameEntity3DComponent->m_GameEntityTag == "TexturedCubeComponent")
                {
                    m_InitialMousePosX = MouseX;
                    m_InitialMousePosY = MouseY;

                    float X = pGameEntity3DComponent->GetLocationX();
                    float Y = pGameEntity3DComponent->GetLocationY();
                    float Z = pGameEntity3DComponent->GetLocationZ();

                    auto QuatMatrix = pGameEntity3DComponent->m_QuatRotationMatrix;
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

                    XMFLOAT3 BoxCenter{ X, Y, Z };
                    XMFLOAT3 BoxExtents{ 1.0f, 1.0f, 1.0f };
                    XMFLOAT4 BoxOrientation{ Orientation.x, Orientation.y, Orientation.z, Orientation.w };

                    float fDist;

                    if (pPhysicsModule->DoesRayIntersectOBB(
                        Origin,
                        Destination,
                        BoxCenter,
                        BoxExtents,
                        BoxOrientation,
                        fDist))
                    {
                        if (m_pUIModule->m_pMediator)
                        {
                            if (pGraphicsModule)
                            {
                                pGraphicsModule->m_Renderer.GetDX11Device()->SpawnGizmo(*pGameEntity3DComponent);
                                pGraphicsModule->m_Renderer.GetDX11Device()->InitPlane();
                                //GraphicsModule->m_Renderer.m_DX11Device.CopyEntity(SceneEntity);
                                //EntityComponent.SetScale(5.f, 5.f, 5.f);
                                //GraphicsModule->m_Renderer.TestGraphicsModuleLog();
                                Logger.Log("ViewportMessage.cpp, SendToUIModule(): Cast to GraphicsModule succeeded");
                            }
                        }
                    }
                    
                    //auto LocationMatrix = EntityComponent->m_TType.GetLocation();

                    // Buffer to hold the formatted string
                    wchar_t buffer[256];

                    // Format the float values into the buffer
                    swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"X: %.2f, Y: %.2f, Z: %.2f", X, Y, Z);

                    // Display the formatted string in a MessageBox
                    //MessageBox(nullptr, buffer, L"Entity Location", MB_OK);
                }
            
        }
        else
        {
            if (pGraphicsModule->m_Renderer.GetDX11Device()->m_bGizmoHovered && m_bIsLeftMouseButtonDown)
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

                //XMFLOAT3 fCubeOriginProjected{ 0, 0, 0 };
                //XMStoreFloat3(&fCubeOriginProjected, CubeOriginProjected);

                XMVECTOR CubeNormalProjected = XMVector3Project(
                    XMVECTOR{ -5, 0, 1 },
                    0,
                    0,
                    ViewportWidth,
                    ViewportHeight,
                    0,
                    1,
                    CCamera::m_ProjectionMatrix,
                    CCamera::m_ViewMatrix,
                    XMMatrixIdentity());

                //XMFLOAT3 fCubeNormalProjected{ 0, 0, 0 };
                //XMStoreFloat3(&fCubeNormalProjected, CubeNormalProjected);

				XMVECTOR ProjectedNormal = XMVectorSubtract(CubeOriginProjected, CubeNormalProjected);

				PreviousX += MouseX;
				PreviousY += MouseY;

                Logger.Log("ViewportMessage.cpp, SendToUIModule(): pGameEntity3DComponent Z BEFORE lambda: ", pGameEntity3DComponent->GetFloatLocationZ());

                const XMFLOAT3 HardcodedNormal{ 0,0,1 };

                XMVECTOR cubeOriginWS = XMVectorSet(-5, 0, 0, 1);
                XMVECTOR cubeNormalWS = cubeOriginWS + XMLoadFloat3(&HardcodedNormal);
                
                XMFLOAT2 o, n;
                XMStoreFloat2(&o, CubeOriginProjected);
                XMStoreFloat2(&n, CubeNormalProjected);
                XMFLOAT2 ScreenProjection = { n.x - o.x, n.y - o.y };
                

                // capture this axis and your normal
                auto worldNormalV = XMLoadFloat3(&HardcodedNormal);



                TimerManager.SetTimerVariadicArgsLambda("TranslationTimer", 0.0f, 10000.0f,
                    [=](float a, float b)
                {
                    CUIMessageQueue& ViewportMessage = CUIMessageQueue::GetUIMessageQueue();
                    // current & previous mouse
                    XMFLOAT2 cur{ ViewportMessage.m_MouseX, ViewportMessage.m_MouseY };
                    XMFLOAT2 prev{ ViewportMessage.PreviousX, ViewportMessage.PreviousY };
                    XMFLOAT2 delta{ cur.x - prev.x, cur.y - prev.y };

                    // how many world-units along normal?
                    float denom = ScreenProjection.x * ScreenProjection.x + ScreenProjection.y * ScreenProjection.y;
                    if (denom < 1e-6f) return;
                    float numer = delta.x * ScreenProjection.x + delta.y * ScreenProjection.y;
                    float worldOffset = (numer / denom);// *1.5;

                    // fetch current 3D pos
                    XMFLOAT3 curPos3D{
                      pGameEntity3DComponent->GetFloatLocationX(),
                      pGameEntity3DComponent->GetFloatLocationY(),
                      pGameEntity3DComponent->GetFloatLocationZ()
                    };
                    XMVECTOR posV = XMLoadFloat3(&curPos3D);

                    // move along world-normal
                    XMVECTOR newPosV = posV + worldNormalV * worldOffset;
                    XMFLOAT3 newPosF; XMStoreFloat3(&newPosF, newPosV);

                    pGameEntity3DComponent->SetLocationF(
                        newPosF.x, newPosF.y, newPosF.z);

                    // update for next frame
                    ViewportMessage.PreviousX = cur.x;
                    ViewportMessage.PreviousY = cur.y;
                }, 0, 0);
                //MessageBox(nullptr, L"Gizmo dragging", L"ViewportMessage", MB_OK);
            }
            else if (!m_bIsLeftMouseButtonDown)
            {
			    m_bIsEntitySelected = false;
				TimerManager.RemoveTimer("TranslationTimer");
                MessageBox(nullptr, L"Deselected entity", L"ViewportMessage", MB_OK);
            }
        }
    }
}

void CUIMessageQueue::ImportGLTF(std::string& FilePath, const std::string& FileContent)
{
    CGraphicsModule* pGraphicsModule = static_cast<CGraphicsModule*>(m_pUIModule->m_pMediator->m_ModuleArray[1]);

    if (pGraphicsModule)
    {
        //pGraphicsModule->m_EntityBuilder.ImportGLTF(FilePath, FileContent);
	}
}
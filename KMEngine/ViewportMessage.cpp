#include <any>
#include "ViewportMessage.h"
#include "UIModule.h"
#include "GraphicsModule.h"
#include "PhysicsModule.h"
#include "CoreClock.h"

void CViewportMessage::SendToUIModule(int MouseX, int MouseY)
{
    float m_InitialMousePosX = MouseX;
    float m_InitialMousePosY = MouseY;
    //float m_InitialMousePosZ = MouseZ;

    CPhysicsModule* pPhysicsModule = static_cast<CPhysicsModule*>(m_pUIModule->m_pMediator->m_ModuleArray[2]);
    CGraphicsModule* pGraphicsModule = static_cast<CGraphicsModule*>(m_pUIModule->m_pMediator->m_ModuleArray[1]);

    CScene& Scene = CScene::GetScene();
    auto& SceneEntityList = Scene.GetSceneList();
    CLogger& Logger = CLogger::GetLogger();

    CGameEntity3DComponent* pGameEntity3DComponent{ nullptr };

    for (auto& SceneEntity : SceneEntityList)
    {
        if (SceneEntity.m_GameEntityTag == "TexturedCube")
		{
            pGameEntity3DComponent = &SceneEntity.m_SceneGraph.m_pRootNode->m_TType;
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
                        CDX11Device::m_ProjectionMatrix,
                        CDX11Device::m_ViewMatrix,
                        CDX11Device::m_WorldMatrix);

                    XMVECTOR Destination = XMVector3Unproject(
                        XMVECTOR{ (float)MouseX, (float)MouseY, 1 },
                        0,
                        0,
                        ViewportWidth,
                        ViewportHeight,
                        0,
                        1,
                        CDX11Device::m_ProjectionMatrix,
                        CDX11Device::m_ViewMatrix,
                        CDX11Device::m_WorldMatrix);

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
            if (pGraphicsModule->m_Renderer.GetDX11Device()->m_bGizmoHovered)
            {

               // float DeltaMouseX = static_cast<float>(MouseX - m_InitialMousePosX) / 1.0f;
               // float DeltaMouseY = static_cast<float>(MouseY - m_InitialMousePosY) / 1.0f;

               //  //Update entity position based on delta
               // pGameEntity3DComponent->SetLocationF(
               //     pGameEntity3DComponent->GetLocationX() + DeltaMouseX,
               //     pGameEntity3DComponent->GetLocationY(),
               //     pGameEntity3DComponent->GetLocationZ());

               // Logger.Log("Entity position updated:", "X:", pGameEntity3DComponent->GetLocationX() + DeltaMouseX);
               // Logger.Log("Entity position updated:", "DeltaMouse is: ", DeltaMouseX);

               //  //Optionally, update initial positions for smooth continuous dragging
               // m_InitialMousePosX = MouseX;
               // m_InitialMousePosY = MouseY;
               ////m_InitialEntityPos.x += DeltaMouseX;
               // //m_InitialEntityPos.y += DeltaMouseY;


				PreviousX += MouseX;
				PreviousY += MouseY;

                Logger.Log("ViewportMessage.cpp, SendToUIModule(): pGameEntity3DComponent Z BEFORE lambda: ", pGameEntity3DComponent->GetFloatLocationZ());

                CTimerManager& TimerManager = CTimerManager::GetTimerManager();
				CCoreClock* Clock = TimerManager.m_pCoreClock;
                TimerManager.SetTimerVariadicArgsLambda(0.0f, 10000.0f,
                    [pGameEntity3DComponent](float a, float b)
                {
					CViewportMessage& ViewportMessage = CViewportMessage::GetViewportMessage();
                    float X = ViewportMessage.m_MouseX;
                    float Y = ViewportMessage.m_MouseY;

                    // Check if the mouse has moved since the last update
                    //if (X != ViewportMessage.PreviousX || Y != ViewportMessage.PreviousY)
                    //{
                        float DeltaMouseX = (X - ViewportMessage.PreviousX) / 100.0f; // Adjust divisor as needed
                        float DeltaMouseY = (Y - ViewportMessage.PreviousY) / 100.0f; // Adjust divisor as needed

                        // Update the entity's position based on mouse movement
                        pGameEntity3DComponent->SetLocationF(
                            pGameEntity3DComponent->GetLocationX(),
                            pGameEntity3DComponent->GetLocationY(),
                            pGameEntity3DComponent->GetLocationZ() + DeltaMouseY
                        );

                        // Update PreviousX and PreviousY for the next comparison
                        ViewportMessage.PreviousX = X;
                        ViewportMessage.PreviousY = Y;

                        // Logging for debugging
                        CLogger& Logger = CLogger::GetLogger();
                        Logger.Log("ViewportMessage.cpp, SendToUIModule(): pGameEntity3DComponent Z AFTER lambda: ", pGameEntity3DComponent->GetFloatLocationZ());
                        Logger.Log("ViewportMessage.cpp, SendToUIModule(): DeltaMouseY is: ", DeltaMouseY);
                        Logger.Log("ViewportMessage.cpp, SendToUIModule(): DeltaMouseY is: ", DeltaMouseY);
                        Logger.Log("ViewportMessage.cpp, SendToUIModule(): X is: ", X);
                        Logger.Log("ViewportMessage.cpp, SendToUIModule(): Y is: ", Y);
                    //}
                }, 0, 0);
                //MessageBox(nullptr, L"Gizmo dragging", L"ViewportMessage", MB_OK);
            }
            else
            {
			    m_bIsEntitySelected = false;
                MessageBox(nullptr, L"Deselected entity", L"ViewportMessage", MB_OK);
            }
        }
    }
}
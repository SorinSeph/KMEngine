#include <any>
#include "ViewportMessage.h"
#include "UIModule.h"
#include "GraphicsModule.h"
#include "PhysicsModule.h"

void CViewportMessage::SendToUIModule(int MouseX, int MouseY)
{		
    CLogger& Logger = CLogger::GetLogger();
    if (m_pUIModule)
    {
        m_pUIModule->TestLog(MouseX, MouseY);
        Logger.Log("ViewportMessage.cpp, SendToUIModule(): Pointer to UI Module for test logging true");
    }
    else
    {
        Logger.Log("ViewportMessage.cpp, SendToUIModule(): Pointer to UI Module for test logging is null");
    }

    if (m_pUIModule)
    {
        CScene& Scene = CScene::GetScene();
        auto& SceneEntityList = Scene.GetSceneList();

        //MessageBox(nullptr, L"LMB Pressed", L"Message", MB_OK);

        for (auto& SceneEntity : SceneEntityList)
        {
            if (SceneEntity.m_GameEntityType == EGameEntityType::Cube)
            {
                //float X = SceneEntity.GetLocationX();
                //float Y = SceneEntity.GetLocationY();
                //float Z = SceneEntity.GetLocationZ();

				auto& EntityComponent = SceneEntity.m_SceneGraph.m_pRootNode->m_TType;

				float X = EntityComponent.GetLocationX();
                float Y = EntityComponent.GetLocationY();
                float Z = EntityComponent.GetLocationZ();
                
                auto QuatMatrix = EntityComponent.m_QuatRotationMatrix;
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

				CPhysicsModule* pPhysicsModule = static_cast<CPhysicsModule*>(m_pUIModule->m_pMediator->m_ModuleArray[2]);

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
						CGraphicsModule* GraphicsModule = static_cast<CGraphicsModule*>(m_pUIModule->m_pMediator->m_ModuleArray[1]);

                        if (GraphicsModule)
                        {
                            GraphicsModule->m_Renderer.GetDX11Device()->SpawnGizmo(EntityComponent);
                            //GraphicsModule->m_Renderer.m_DX11Device.CopyEntity(SceneEntity);
                            //EntityComponent.SetScale(5.f, 5.f, 5.f);
                            //GraphicsModule->m_Renderer.TestGraphicsModuleLog();
                            Logger.Log("ViewportMessage.cpp, SendToUIModule(): Cast to GraphicsModule succeeded");
                        }

                        else
                        {
                             Logger.Log("ViewportMessage.cpp, SendToUIModule(): Cast to GraphicsModule failed");
                        }
                    }
                    else
                    {
			            Logger.Log("ViewportMessage.cpp, SendToUIModule(): Pointer to Mediator in UI Module is null");
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

        
    }
}
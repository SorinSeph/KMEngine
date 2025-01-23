#include <any>
#include "ViewportMessage.h"
#include "UIModule.h"
#include "GraphicsModule.h"

bool DoesRayIntersectOBB(
    FXMVECTOR RayOrigin, 
    FXMVECTOR RayDirection, 
	XMFLOAT3 BoxCenter,
    XMFLOAT3 BoxExtents,
	XMFLOAT4 BoxOrientation,
    float& Dist)
{
    static const XMVECTORU32 SelectY = { { { XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0 } } };
    static const XMVECTORU32 SelectZ = { { { XM_SELECT_0, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0 } } };

    // Load the box.
    XMVECTOR vCenter = XMLoadFloat3(&BoxCenter);
    XMVECTOR vExtents = XMLoadFloat3(&BoxExtents);
    XMVECTOR vOrientation = XMLoadFloat4(&BoxOrientation);

    assert(DirectX::Internal::XMQuaternionIsUnit(vOrientation));

    // Get the boxes normalized side directions.
    XMMATRIX R = XMMatrixRotationQuaternion(vOrientation);

    // Adjust ray origin to be relative to center of the box.
    XMVECTOR TOrigin = XMVectorSubtract(vCenter, RayOrigin);

    // Compute the dot product againt each axis of the box.
    XMVECTOR AxisDotOrigin = XMVector3Dot(R.r[0], TOrigin);
    AxisDotOrigin = XMVectorSelect(AxisDotOrigin, XMVector3Dot(R.r[1], TOrigin), SelectY);
    AxisDotOrigin = XMVectorSelect(AxisDotOrigin, XMVector3Dot(R.r[2], TOrigin), SelectZ);

    XMVECTOR AxisDotDirection = XMVector3Dot(R.r[0], RayDirection);
    AxisDotDirection = XMVectorSelect(AxisDotDirection, XMVector3Dot(R.r[1], RayDirection), SelectY);
    AxisDotDirection = XMVectorSelect(AxisDotDirection, XMVector3Dot(R.r[2], RayDirection), SelectZ);

    // if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
    XMVECTOR IsParallel = XMVectorLessOrEqual(XMVectorAbs(AxisDotDirection), g_RayEpsilon);

    // Test against all three axes simultaneously.
    XMVECTOR InverseAxisDotDirection = XMVectorReciprocal(AxisDotDirection);
    XMVECTOR t1 = XMVectorMultiply(XMVectorSubtract(AxisDotOrigin, vExtents), InverseAxisDotDirection);
    XMVECTOR t2 = XMVectorMultiply(XMVectorAdd(AxisDotOrigin, vExtents), InverseAxisDotDirection);

    // Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
    // use the results from any directions parallel to the slab.
    XMVECTOR t_min = XMVectorSelect(XMVectorMin(t1, t2), g_FltMin, IsParallel);
    XMVECTOR t_max = XMVectorSelect(XMVectorMax(t1, t2), g_FltMax, IsParallel);

    // t_min.x = maximum( t_min.x, t_min.y, t_min.z );
    // t_max.x = minimum( t_max.x, t_max.y, t_max.z );
    t_min = XMVectorMax(t_min, XMVectorSplatY(t_min));  // x = max(x,y)
    t_min = XMVectorMax(t_min, XMVectorSplatZ(t_min));  // x = max(max(x,y),z)
    t_max = XMVectorMin(t_max, XMVectorSplatY(t_max));  // x = min(x,y)
    t_max = XMVectorMin(t_max, XMVectorSplatZ(t_max));  // x = min(min(x,y),z)

    // if ( t_min > t_max ) return false;
    XMVECTOR NoIntersection = XMVectorGreater(XMVectorSplatX(t_min), XMVectorSplatX(t_max));

    // if ( t_max < 0.0f ) return false;
    NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(XMVectorSplatX(t_max), XMVectorZero()));

    // if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return false;
    XMVECTOR ParallelOverlap = XMVectorInBounds(AxisDotOrigin, vExtents);
    NoIntersection = XMVectorOrInt(NoIntersection, XMVectorAndCInt(IsParallel, ParallelOverlap));

    if (!DirectX::Internal::XMVector3AnyTrue(NoIntersection))
    {
        // Store the x-component to *pDist
        XMStoreFloat(&Dist, t_min);
        return true;
    }

    Dist = 0.f;
    return false;
}

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
                    518,
                    825,
                    0,
                    1,
                    CDX11Device::m_ProjectionMatrix,
                    CDX11Device::m_ViewMatrix,
                    CDX11Device::m_WorldMatrix);

                XMVECTOR Destination = XMVector3Unproject(
                    XMVECTOR{ (float)MouseX, (float)MouseY, 1 },
                    0,
                    0,
                    518,
                    825,
                    0,
                    1,
                    CDX11Device::m_ProjectionMatrix,
                    CDX11Device::m_ViewMatrix,
                    CDX11Device::m_WorldMatrix);

				XMFLOAT3 BoxCenter{ X, Y, Z };
				XMFLOAT3 BoxExtents{ 1.0f, 1.0f, 1.0f };
				XMFLOAT4 BoxOrientation{ Orientation.x, Orientation.y, Orientation.z, Orientation.w };

                float fDist;

                if (DoesRayIntersectOBB(
                    Origin,
                    Destination,
                    BoxCenter,
                    BoxExtents,
                    BoxOrientation,
                    fDist))
                {
                    if (m_pUIModule->m_Mediator)
                    {
                        try
                        {
                            auto GraphicsModule = std::any_cast<CGraphicsModule*>(m_pUIModule->m_Mediator->m_ModuleVector[1]);
                            if (GraphicsModule)
                            {
								GraphicsModule->m_Renderer.m_DX11Device.CopyEntity(SceneEntity);
                                //EntityComponent.SetScale(5.f, 5.f, 5.f);
                            }
                            else
                            {
                                Logger.Log("ViewportMessage.cpp, SendToUIModule(): Cast to GraphicsModule failed");
                            }
                        }
                        catch (const std::bad_any_cast& e)
                        {
                            Logger.Log("ViewportMessage.cpp, SendToUIModule(): Bad any cast: ", e.what());
                        }
                    }
                    else
                    {
			            Logger.Log("ViewportMessage.cpp, SendToUIModule(): Pointer to Mediator in UI Module is null");
                    }
					MessageBox(nullptr, L"Ray intersects OBB", L"Message", MB_OK);
				}
                else
                {
                    MessageBox(nullptr, L"Ray does not intersect OBB", L"Message", MB_OK);
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
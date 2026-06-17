#include <any>
#include "UIMessageQueue.h"
#include "Modules/UIModule.h"
#include "Modules/GraphicsModule.h"
#include "Modules/PhysicsModule.h"
#include "Core/CoreClock.h"

//for (int ChildIt = Anim.m_Nodes.size() - 1; ChildIt >= 0; --ChildIt)


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

                Logger.Log("ViewportMessage.cpp, SendToUIModule(): pGameEntity3DComponent Z BEFORE lambda: ", pGameEntity3DComponent->GetLocationZ());

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
                          pGameEntity3DComponent->GetLocationX(),
                          pGameEntity3DComponent->GetLocationY(),
                          pGameEntity3DComponent->GetLocationZ()
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

void CUIMessageQueue::RaycastOpenGL(int MouseX, int MouseY)
{
    // 1) Convert mouse to Normalized Device Coordinates [-1,1]
    float ndcX = (2.0f * MouseX) / static_cast<float>(COpenGLDevice::m_ViewportWidth) - 1.0f;
    float ndcY = 1.0f - (2.0f * MouseY) / static_cast<float>(COpenGLDevice::m_ViewportHeight); // flip Y

    glm::vec4 rayStartNDC{ ndcX, ndcY, -1.0f, 1.0f }; // near plane
    glm::vec4 rayEndNDC{ ndcX, ndcY,  1.0f, 1.0f }; // far plane

    // 2) Unproject to world space
    glm::mat4 invVP = glm::inverse(COpenGLDevice::g_ProjectionMatrix * COpenGLDevice::g_ViewMatrix);

    glm::vec4 rayStartWorld4 = invVP * rayStartNDC;
    glm::vec4 rayEndWorld4 = invVP * rayEndNDC;

    // perspective divide
    rayStartWorld4 /= rayStartWorld4.w;
    rayEndWorld4 /= rayEndWorld4.w;

    glm::vec3 rayOrigin = glm::vec3(rayStartWorld4);
    glm::vec3 rayDirection = glm::normalize(glm::vec3(rayEndWorld4) - rayOrigin);

    // 3) Raycast destination along forward by 50 units from the origin
    glm::vec3 rayDest = rayOrigin + rayDirection * 50.0f;

    CUIModule* pUIModule = static_cast<CUIModule*>(m_pUIModule->m_pMediator->m_ModuleArray[0]);
    CGraphicsModule* pGraphicsModule = static_cast<CGraphicsModule*>(m_pUIModule->m_pMediator->m_ModuleArray[1]);
    CPhysicsModule* pPhysicsModule = static_cast<CPhysicsModule*>(m_pUIModule->m_pMediator->m_ModuleArray[2]);

    if (pGraphicsModule)
    {
        //pGraphicsModule->m_EntityBuilder.AddLinetrace(rayOrigin, rayDest);
        if (pGraphicsModule->m_Renderer.m_pRendererOpenGL)
        {
            pGraphicsModule->m_EntityBuilder.AddLinetrace(glm::vec3{ 0.f, 0.f, 0.f }, rayDest);
        }
    }

    glm::vec3 BoxCenter{ 0.f, 0.f, -5.f };
    glm::vec3 BoxExtents{ 0.5f, 0.5f, 0.5f };
    glm::quat OrientationQuat{ glm::quat(1.0f, 0.0f, 0.0f, 0.0f) };  // (w, x, y, z)
    float Dist{};

    // Currently iterate through all objects in the scene. In the future, get the entities from the current world partition
    CScene& Scene = CScene::GetScene();
    auto& SceneEntityList = Scene.GetSceneList();
    for (auto& SceneEntityIt : SceneEntityList)
    {
        std::vector<CSceneGraphNode<CGameEntity3DComponent>*> EntityComponentVector;
        EntityComponentVector.push_back(SceneEntityIt.m_SceneGraph.m_pRootNode);
        for (auto& EntityComponent : EntityComponentVector)
        {
            if (pPhysicsModule->DoesRayIntersectOBBOpenGL(
                rayOrigin,
                rayDirection,
                EntityComponent->m_tType.m_CollisionComponent.m_Center,
                EntityComponent->m_tType.m_CollisionComponent.m_Extents,
                OrientationQuat,
                Dist))
            {
                if (pUIModule)
                {
                    pUIModule->UpdateSelectedEntityString(EntityComponent->m_tType.m_GameEntityTag);
                    glm::vec3 LocationVector{ EntityComponent->m_tType.m_LocationX, EntityComponent->m_tType.m_LocationY, EntityComponent->m_tType.m_LocationZ };
                    pGraphicsModule->SpawnGizmoOpenGL(EntityComponent->m_tType);
                }
                //MessageBox(nullptr, L"Raycast hit!", L"ViewportMessage", MB_OK);
                break;
            }
        }
    }
}

void CUIMessageQueue::ImportGLTF(std::string& FilePath, const std::string& FileContent)
{
    CGraphicsModule* pGraphicsModule = static_cast<CGraphicsModule*>(m_pUIModule->m_pMediator->m_ModuleArray[1]);

    if (pGraphicsModule)
    {
        pGraphicsModule->m_EntityBuilder.ImportGLTF(FilePath, FileContent);
        //pGraphicsModule->m_EntityBuilder.TestReplicateEntity();
    }
}

// No longer used
void CUIMessageQueue::TestPlayGLTFAnimation()
{
}


void CUIMessageQueue::PlayGLTFAnimation(std::string& FilePath, const std::string& FileContent)
{
    //TestPlayGLTFAnimation();

    CGraphicsModule* pGraphicsModule = static_cast<CGraphicsModule*>(m_pUIModule->m_pMediator->m_ModuleArray[1]);
    CTimerManager& TimerManager = CTimerManager::GetTimerManager();
    CGLTFAnimation Anim = pGraphicsModule->m_EntityBuilder.GetGLTFAnimation();

    CScene& Scene = CScene::GetScene();
    std::vector<CGameEntity3D>& SceneEntityList = Scene.GetSceneList();
    CLogger& Logger = CLogger::GetLogger();

    CGameEntity3D* pGameEntity{ nullptr };

    for (auto& SceneEntityIt : SceneEntityList)
    {
        if (SceneEntityIt.m_GameEntityTag == "Knight")
        {
            pGameEntity = &SceneEntityIt;
        }
    }
    uint32_t ShaderProgram = pGameEntity->m_SceneGraph.m_pRootNode->m_tType.m_OpenGLResource.m_ShaderProgram;

    //// Testing #1: Previous working example

    //for (uint16_t KeyframeIt = 0; KeyframeIt < Anim.m_AnimKeyframes.size(); KeyframeIt++)
    //{
    //    auto AnimLambda = [=]() {
    //        glm::quat BoneTopQuat{ Anim.m_Samplers.at(4).m_Rotation.at(KeyframeIt).w,
    //            Anim.m_Samplers.at(4).m_Rotation.at(KeyframeIt).x,
    //            Anim.m_Samplers.at(4).m_Rotation.at(KeyframeIt).y,
    //            Anim.m_Samplers.at(4).m_Rotation.at(KeyframeIt).z,
    //        };

    //        // Local transforms
    //        glm::mat4 BoneTopTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0, 3, 0));
    //        glm::mat4 BoneTopRotation = glm::mat4_cast(BoneTopQuat);
    //        glm::mat4 LocalTransformBoneTop = BoneTopTranslation * BoneTopRotation;

    //        glm::mat4 LocalTransformBoneRoot = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

    //        // Global transforms
    //        glm::mat4 globalTransformBoneRoot = LocalTransformBoneRoot;
    //        glm::mat4 globalTransformBoneTop = globalTransformBoneRoot * LocalTransformBoneTop;

    //        glm::mat4 Anim2InverseBindMatriceBoneRoot{
    //            1, -0, 0, -0,
    //            -0, 1, -0, 0,
    //            0, -0, 1, -0,
    //            -0, 0, -0, 1
    //        };

    //        glm::mat4 Anim2InverseBindMatriceBoneTop{
    //            1, -0, 0, -0,
    //            -0, 1, -0, 0,
    //            0, -0, 1, -0,
    //            -0, -3, -0, 1
    //        };

    //        glm::mat4 IdentityMatrix{ 1.f };

    //        glm::mat4 BoneMatrixRoot = globalTransformBoneRoot * Anim2InverseBindMatriceBoneRoot;
    //        glm::mat4 BoneMatrixTop = globalTransformBoneTop * Anim2InverseBindMatriceBoneTop;

    //        glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, "bones[0]"), 1, GL_FALSE, &BoneMatrixRoot[0][0]);
    //        glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, "bones[1]"), 1, GL_FALSE, &BoneMatrixTop[0][0]);

    //        if (KeyframeIt == 29)
    //            __debugbreak();
    //    };

    //    float CurrentTime = TimerManager.m_pCoreClock->GetFDeltaTime();
    //    float AnimTime = CurrentTime + Anim.m_AnimKeyframes.at(KeyframeIt);
    //    TimerManager.SetTimerVariadicArgsLambda("AnimationTimer", AnimTime, 0.0f, AnimLambda);
    //}




    //--------------------------------------------------------------------------------------
    // Testing 2
    //--------------------------------------------------------------------------------------

    glm::mat4 Anim2InverseBindMatriceBoneRoot{
        1, -0, 0, -0,
        -0, 1, -0, 0,
        0, -0, 1, -0,
        -0, 0, -0, 1
    };

    glm::mat4 Anim2InverseBindMatriceBoneTop{
        1, -0, 0, -0,
        -0, 1, -0, 0,
        0, -0, 1, -0,
        -0, -3, -0, 1
    };

    std::vector<glm::mat4> NodeLocalTransform;
    std::vector<glm::mat4> NodeGlobalTransform;
    std::vector<glm::mat4> InverseBindMatrices;

    InverseBindMatrices.push_back(Anim2InverseBindMatriceBoneRoot);
    InverseBindMatrices.push_back(Anim2InverseBindMatriceBoneTop);

    for (auto& NodeIt : Anim.m_Nodes)
    {
        glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1.0f), NodeIt.m_PoseTranslation);
        glm::mat4 RotationMatrix = glm::mat4_cast(NodeIt.m_PoseRotation);
        glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0f), NodeIt.m_PoseScale);
        glm::mat4 GlobalTransform = TranslationMatrix * RotationMatrix * ScaleMatrix;
        NodeLocalTransform.push_back(GlobalTransform);
    }

    for (uint16_t KeyframeIt = 0; KeyframeIt < Anim.m_AnimKeyframes.size(); KeyframeIt++)
    {
        Logger.Log("UIMessageQueue.cpp, PlayGLTFAnimation");

        auto AnimLambda = [=]() mutable {
            CLogger& Logger = CLogger::GetLogger();

            float KeyframeTime = Anim.m_AnimKeyframes.at(KeyframeIt);

            const uint16_t TranslationFlag = (uint16_t)EChannelTransformType::Translation;
            const uint16_t RotationFlag = (uint16_t)EChannelTransformType::Rotation;
            const uint16_t ScaleFlag = (uint16_t)EChannelTransformType::Scale;

            for (int32_t NodeIt = static_cast<int32_t>(Anim.m_Nodes.size()) - 1; NodeIt >= 0; NodeIt--)
            {
                glm::vec3 Translation{ Anim.m_Nodes.at(NodeIt).m_PoseTranslation };
                glm::quat Rotation = Anim.m_Nodes.at(NodeIt).m_PoseRotation;
                glm::vec3 Scale = Anim.m_Nodes.at(NodeIt).m_PoseScale;

                if (Anim.m_Nodes.at(NodeIt).m_KeyframeMap.contains(KeyframeTime))
                {
                    const CKeyframe& Keyframe = Anim.m_Nodes.at(NodeIt).m_KeyframeMap.at(KeyframeTime);
                    const uint16_t KeyframeFlag = Keyframe.m_TransformTypeFlags;

                    if ((KeyframeFlag & TranslationFlag) == TranslationFlag)
                    {
                        Translation = Keyframe.m_Translation;
                    }
                    if ((KeyframeFlag & RotationFlag) == RotationFlag)
                    {
                        Rotation = Keyframe.m_Rotation;
                    }
                    if ((KeyframeFlag & ScaleFlag) == ScaleFlag)
                    {
                        Scale = Keyframe.m_Scale;
                    }
                }

                glm::mat4 LocalTranslation{ 1.0f};
                //PoseTranslation = glm::translate(PoseTranslation, Anim.m_Nodes.at(NodeIt).m_PoseTranslation);
                LocalTranslation = glm::translate(LocalTranslation, Translation);
                glm::mat4 LocalRotation{ glm::mat4(Rotation) };
                NodeLocalTransform.at(NodeIt) = LocalTranslation * LocalRotation * glm::scale(glm::mat4(1.0f), Scale);
            }

            std::vector<glm::mat4> NodeGlobalTransform(Anim.m_Nodes.size());
            NodeGlobalTransform.at(Anim.m_Nodes.size() - 1) = NodeLocalTransform.at(Anim.m_Nodes.size() - 1);
            for (int32_t NodeIt = static_cast<int32_t>(Anim.m_Nodes.size()) - 2; NodeIt >= 0; NodeIt--)
            {
                auto ParentGlobalTransform = NodeGlobalTransform.at(NodeIt + 1);
                auto CurrentLocalTransform = NodeLocalTransform.at(NodeIt);
                NodeGlobalTransform.at(NodeIt) = ParentGlobalTransform * CurrentLocalTransform;
                if (KeyframeIt == 100 && NodeIt == 24)
                    __debugbreak();
            }
            for (int32_t ShaderNodeIt = 0; ShaderNodeIt < static_cast<int32_t>(NodeGlobalTransform.size()); ShaderNodeIt++)
            {
                int32_t NodeIndex = static_cast<int32_t>(NodeGlobalTransform.size()) - 1 - ShaderNodeIt;
                auto FinalGlobalTransform = NodeGlobalTransform.at(ShaderNodeIt);
                auto FinalInverseMatrix = Anim.m_InverseBindMatrixMap.at(ShaderNodeIt);
                glm::mat4 BoneMatrix = FinalGlobalTransform * FinalInverseMatrix;

                std::string BoneString{ "bones[" + std::to_string(NodeIndex) + "]" };
                glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, BoneString.c_str()), 1, GL_FALSE, &BoneMatrix[0][0]);

                if (KeyframeIt == 100 && ShaderNodeIt == 24)
                    __debugbreak();

                //if (KeyframeIt == 29 && ShaderNodeIt == 0)
                //    __debugbreak();
            }


        }; // End of lambda

        if (KeyframeIt == 0)
        {
            AnimLambda();
        }

        float CurrentTime = TimerManager.m_pCoreClock->GetFDeltaTime();
        float AnimTime = CurrentTime + Anim.m_AnimKeyframes.at(KeyframeIt);
        TimerManager.SetTimerVariadicArgsLambda("AnimationTimer", AnimTime, 0.0f, AnimLambda);
    }
}
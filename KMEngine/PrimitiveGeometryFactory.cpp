#include "PrimitiveGeometryFactory.h"
#include "EntityPhysicalMesh.h"

int CPrimitiveGeometryFactory::m_Entity3D_UID = 1;

void CPrimitiveGeometryFactory::SetUID(CGameEntity3D& Entity3D)
{
    std::string UID{ };

    if (CPrimitiveGeometryFactory::m_Entity3D_UID <= 10)
    {
        std::string uid = "e3d00000";
        std::string counter = std::to_string(CPrimitiveGeometryFactory::m_Entity3D_UID);

        UID = uid + counter;
        Entity3D.SetUID(UID);
    }
    else if (CPrimitiveGeometryFactory::m_Entity3D_UID / 10 != 0)
    {
        std::string uid = "dxr0000";
        std::string counter = std::to_string(CPrimitiveGeometryFactory::m_Entity3D_UID);

        UID = uid + counter;
        Entity3D.SetUID(UID);
    }
    else if (CPrimitiveGeometryFactory::m_Entity3D_UID / 100 != 0)
    {
        std::string uid = "e3d000";
        std::string counter = std::to_string(CPrimitiveGeometryFactory::m_Entity3D_UID);

        UID = uid + counter;
        Entity3D.SetUID(UID);
    }
    else if (CPrimitiveGeometryFactory::m_Entity3D_UID / 1000 != 0)
    {
        std::string uid = "e3d00";
        std::string counter = std::to_string(CPrimitiveGeometryFactory::m_Entity3D_UID);

        UID = uid + counter;
        Entity3D.SetUID(UID);
    }
    else if (CPrimitiveGeometryFactory::m_Entity3D_UID / 10000 != 0)
    {
        std::string uid = "e3d0";
        std::string counter = std::to_string(CPrimitiveGeometryFactory::m_Entity3D_UID);

        UID = uid + counter;
        Entity3D.SetUID(UID);
    }
    else if (CPrimitiveGeometryFactory::m_Entity3D_UID / 100000 != 0)
    {
        std::string uid = "e3d";
        std::string counter = std::to_string(CPrimitiveGeometryFactory::m_Entity3D_UID);

        UID = uid + counter;
        Entity3D.SetUID(UID);
    }
    CPrimitiveGeometryFactory::m_Entity3D_UID++;
}

void CPrimitiveGeometryFactory::SetUIDTest(CGameEntity3D& Entity3D)
{
    //Entity3D.SetUID(CPrimitiveGeometryFactory::m_Entity3D_UID);
    CPrimitiveGeometryFactory::m_Entity3D_UID++;
}

CGameEntity3D CPrimitiveGeometryFactory::CreateEntity3D(EPrimitiveGeometryType m_TType)
{
    CGameEntity3D Entity;

    switch (m_TType)
    {
        case EPrimitiveGeometryType::Plane:
        {
            std::vector<SSimpleVertex> VerticesList = {
                { XMFLOAT3(-5.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(5.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
                { XMFLOAT3(5.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
                { XMFLOAT3(-5.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
            };

            std::vector<WORD> IndicesVector = {
                3,1,0,
                2,1,3,

                6,4,5,
                7,4,6,

                11,9,8,
                10,9,11,

                14,12,13,
                15,12,14,

                19,17,16,
                18,17,19,

                22,20,21,
                23,20,22
            };

            Entity.PhysicalMesh.SetSimpleVerticesList(VerticesList);
            Entity.PhysicalMesh.SetIndicesList(IndicesVector);
            return Entity;
            break;
        }
        case EPrimitiveGeometryType::PlaneColored:
        {
            std::vector<SSimpleColorVertex> VerticesList = {
                { XMFLOAT3(-5.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                { XMFLOAT3(5.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                { XMFLOAT3(5.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                { XMFLOAT3(-5.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
            };

            return Entity;
            break;
        }
        case EPrimitiveGeometryType::Cube:
        {
            std::vector<SSimpleVertex> VerticesList = {
                { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f) },
                { XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },

                { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f) },
                { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },
                { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },

                { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f) },
                { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f) },

                { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f) },
                { XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f) },

                { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f) },

                // Front face

                { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f) },
                { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f) },
            };

            //Entity.SetVerticesList(VerticesList);
            SetUID(Entity);
            SetUIDTest(Entity);
            return Entity;
            break;
        }
        case EPrimitiveGeometryType::Pyramid:
        {
            std::vector<SSimpleVertex> VerticesList = {
                // Base
                { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

                // Front Face
                { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(-1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

                // Right Face
                { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(1.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },

                // Back face
                { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(1.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },

                // Left face
                { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(-1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
            };

            //Entity.SetVerticesList(VerticesList);
            return Entity;
            break;
        }
        case EPrimitiveGeometryType::CubeTest:
        {
            std::vector<SSimpleColorVertex> VerticesList = {
                { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                // Front face

                { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                //{ XMFLOAT3(-0.25f, 0.25f, -0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(0.25f, 0.25f, -0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(0.25f, 0.25f, 0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(-0.25f, 0.25f, 0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },

                //{ XMFLOAT3(-0.25f, -0.25f, -0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(0.25f, -0.25f, -0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(0.25f, -0.25f, 0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(-0.25f, -0.25f, 0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },

                //{ XMFLOAT3(-0.25f, -0.25f, 0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(-0.25f, -0.25f, -0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(-0.25f, 0.25f, -0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(-0.25f, 0.25f, 0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },

                //{ XMFLOAT3(0.25f, -0.25f, 0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(0.25f, -0.25f, -0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(0.25f, 0.25f, -0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(0.25f, 0.25f, 0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },

                //{ XMFLOAT3(-0.25f, -0.25f, -0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(0.25f, -0.25f, -0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(0.25f, 0.25f, -0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(-0.25f, 0.25f, -0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },

                //// Front face

                //{ XMFLOAT3(-0.25f, -0.25f, 0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(0.25f, -0.25f, 0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(0.25f, 0.25f, 0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(-0.25f, 0.25f, 0.25f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
            };

            return Entity;
            break;
        }
        case EPrimitiveGeometryType::Arrow:
        {
            std::vector<SSimpleColorVertex> VerticesList = {
                { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
                //{ XMFLOAT3(cosf(XMConvertToRadians(0)), 0.0f, sinf(XMConvertToRadians(0))), XMFLOAT2(1.0f, 1.0f) },
                //{ XMFLOAT3(cosf(XMConvertToRadians(22.5)), 0.0f, sinf(XMConvertToRadians(22.5))), XMFLOAT2(1.0f, 1.0f) },
                //{ XMFLOAT3(cosf(XMConvertToRadians(22.5 * 2)), 0.0f, sinf(XMConvertToRadians(22.5 * 2))), XMFLOAT2(1.0f, 1.0f) },
                //{ XMFLOAT3(cosf(XMConvertToRadians(22.5 * 3)), 0.0f, sinf(XMConvertToRadians(22.5 * 3))), XMFLOAT2(1.0f, 1.0f) },
                //{ XMFLOAT3(cosf(XMConvertToRadians(22.5 * 4)), 0.0f, sinf(XMConvertToRadians(22.5 * 4))), XMFLOAT2(1.0f, 1.0f) },
            };

            const float Increment = 22.5f;
            const float BodyLength = 15.0f;
            const float TipLength = 2.0f + BodyLength;

            for (int i = 0; i < 16; i++)
            {
                VerticesList.push_back({
                    XMFLOAT3(cosf(XMConvertToRadians(i * Increment)), sinf(XMConvertToRadians(i * Increment)), 0.0f),
                    XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) });
            }


            for (int i = 0; i < 16; i++)
            {
                VerticesList.push_back({
                    XMFLOAT3(cosf(XMConvertToRadians(i * Increment)), sinf(XMConvertToRadians(i * Increment)), BodyLength),
                    XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) });
            }

            int Size = VerticesList.size();

            for (int i = 32; i > 16; i--)
            {
                XMFLOAT3 Position = VerticesList.at(i).Pos;
                Position.x *= 1.5f;
                Position.y *= 1.5f;
                XMFLOAT4 VertColor = VerticesList.at(i).Col;
                VerticesList.push_back({ Position, VertColor });
            }

            VerticesList.push_back({ XMFLOAT3(0.0f, 0.0f, TipLength), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) });

            std::vector<WORD> ArrowIndices = {
                //3,1,0,
                //2,1,3,

                0, 1, 2,
                0, 2, 3,
                0, 3, 4,
                0, 4, 5,
                0, 5, 6,
                0, 6, 7,
                0, 7, 8,
                0, 8, 9,
                0, 9, 10,
                0, 10, 11,
                0, 11, 12,
                0, 12, 13,
                0, 13, 14,
                0, 14, 15,
                0, 15, 16,
                0, 1, 16,

                1, 2, 17,
                17, 18, 2,
                3, 2, 18,
                18, 19, 3,
                4, 3, 19,
                19, 20, 4,
                5, 4, 20,
                20, 21, 5,
                6, 5, 21,
                21, 22, 6,
                7, 6, 22,
                22, 23, 7,
                8, 7, 23,
                23, 24, 8,
                9, 8, 24,
                24, 25, 9,
                10, 9, 25,
                25, 26, 10,
                11, 10, 26,
                26, 27, 11,
                12, 11, 27,
                27, 28, 12,
                13, 12, 28,
                28, 29, 13,
                13, 14, 29,
                29, 30, 14,
                15, 14, 30,
                30, 31, 15,
                16, 15, 31,
                31, 32, 16,
                1, 16, 32,
                32, 17, 1,

                17, 18, 33,
                33, 34, 18,
                18, 19, 34,
                34, 35, 19,
                19, 20, 35,
                35, 36, 20,
                20, 21, 36,
                36, 37, 21,
                21, 22, 37,
                37, 38, 22,
                22, 23, 38,
                38, 39, 23,
                23, 24, 39,
                39, 40, 24,
                24, 25, 40,
                40, 41, 25,
                25, 26, 41,
                41, 42, 26,
                26, 27, 42,
                42, 43, 27,
                27, 28, 43,
                43, 44, 28,
                28, 29, 44,
                44, 45, 29,
                29, 30, 45,
                45, 46, 30,
                30, 31, 46,
                46, 47, 31,
                31, 32, 47,
                47, 48, 32,
                32, 17, 48,
                48, 33, 17,

                49, 33, 34,
                49, 34, 35,
                49, 35, 36,
                49, 36, 37,
                49, 37, 38,
                49, 38, 39,
                49, 39, 40,
                49, 41, 42,
                49, 42, 43,
                49, 43, 44,
                49, 44, 45,
                49, 45, 46,
                49, 46, 47,
                49, 47, 48
            };
            Entity.PhysicalMesh.SetIndicesList(ArrowIndices);

            TEntityPhysicalMesh ArrowPhysicalMesh;

            //Entity.m_PhysicalMeshVector.push_back(ArrowPhysicalMesh);
            //Entity.m_PhysicalMeshVector.at(0).SetSimpleColorVerticesList(VerticesList);
            //Entity.m_PhysicalMeshVector.at(0).SetIndicesList(ArrowIndices);
            //Entity.m_PhysicalMeshVector.at(0).SetStride(sizeof(SSimpleColorVertex));
            return Entity;
            break;
        }
    }
}

std::vector<WORD> CPrimitiveGeometryFactory::GetIndicesList()
{
    std::vector<WORD> IndicesVector {
                3,1,0,
                2,1,3,

                6,4,5,
                7,4,6,

                11,9,8,
                10,9,11,

                14,12,13,
                15,12,14,

                19,17,16,
                18,17,19,

                22,20,21,
                23,20,22
    };

    return IndicesVector;
}

CGameEntity3DComponent CPrimitiveGeometryFactory::CreateEntity3DComponent(EPrimitiveGeometryType m_TType)
{
    CGameEntity3DComponent EntityComponent;

    switch (m_TType)
    {
        case EPrimitiveGeometryType::Cube:
        {
            std::vector<SSimpleVertex> VerticesList = {
                { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f) },
                { XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },

                { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f) },
                { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },
                { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },

                { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },
                { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f) },
                { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f) },

                { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f) },
                { XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f) },

                { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f) },
                { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f) },

                // Front face

                { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f) },
                { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f) },
                { XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f) },
                { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f) },
            };

            //EntityComponent.SetVerticesList(VerticesList);
            return EntityComponent;
            break;
        }
    }
}

CGameEntity3D CPrimitiveGeometryFactory::CreateLinetrace(float X_Origin, float Y_Origin, float Z_Origin, float X_Destination, float Y_Destination, float Z_Destination)
{
    CGameEntity3D LineEntity;

    std::vector<SSimpleColorVertex> VerticesList = {
        { XMFLOAT3(X_Origin, Y_Origin, Z_Origin), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(X_Destination, Y_Destination, Z_Destination), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
    };

    return LineEntity;
}
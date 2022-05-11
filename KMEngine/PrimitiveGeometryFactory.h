#pragma once

#include "GameEntity3D.h"
#include <math.h>

enum class PrimitiveGeometryType
{
    Line,
	Plane,
	Cube,
    CubeTest,
	Cylinder,
    Arrow
};

class PrimitiveGeometryFactory
{
public:
	GameEntity3D CreateEntity3D(PrimitiveGeometryType Type)
	{
        GameEntity3D Entity; 

		switch (Type)
		{
            case PrimitiveGeometryType::Plane:
            {
                std::vector<SimpleVertex>VerticesList =
                {
                    { XMFLOAT3(-5.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
                    { XMFLOAT3(5.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
                    { XMFLOAT3(5.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
                    { XMFLOAT3(-5.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
                };

                Entity.SetVerticesList(VerticesList);
                return Entity;
            }

		    case PrimitiveGeometryType::Cube:
		    {
                std::vector<SimpleVertex>VerticesList = 
                {
                    { XMFLOAT3(-0.24f, 0.24f, -0.24f), XMFLOAT2(1.0f, 0.0f) },
                    { XMFLOAT3(0.24f, 0.24f, -0.24f), XMFLOAT2(0.0f, 0.0f) },
                    { XMFLOAT3(0.24f, 0.24f, 0.24f), XMFLOAT2(0.0f, 1.0f) },
                    { XMFLOAT3(-0.24f, 0.24f, 0.24f), XMFLOAT2(1.0f, 1.0f) },

                    { XMFLOAT3(-0.24f, -0.24f, -0.24f), XMFLOAT2(0.0f, 0.0f) },
                    { XMFLOAT3(0.24f, -0.24f, -0.24f), XMFLOAT2(1.0f, 0.0f) },
                    { XMFLOAT3(0.24f, -0.24f, 0.24f), XMFLOAT2(1.0f, 1.0f) },
                    { XMFLOAT3(-0.24f, -0.24f, 0.24f), XMFLOAT2(0.0f, 1.0f) },

                    { XMFLOAT3(-0.24f, -0.24f, 0.24f), XMFLOAT2(0.0f, 1.0f) },
                    { XMFLOAT3(-0.24f, -0.24f, -0.24f), XMFLOAT2(1.0f, 1.0f) },
                    { XMFLOAT3(-0.24f, 0.24f, -0.24f), XMFLOAT2(1.0f, 0.0f) },
                    { XMFLOAT3(-0.24f, 0.24f, 0.24f), XMFLOAT2(0.0f, 0.0f) },

                    { XMFLOAT3(0.24f, -0.24f, 0.24f), XMFLOAT2(1.0f, 1.0f) },
                    { XMFLOAT3(0.24f, -0.24f, -0.24f), XMFLOAT2(0.0f, 1.0f) },
                    { XMFLOAT3(0.24f, 0.24f, -0.24f), XMFLOAT2(0.0f, 0.0f) },
                    { XMFLOAT3(0.24f, 0.24f, 0.24f), XMFLOAT2(1.0f, 0.0f) },

                    { XMFLOAT3(-0.24f, -0.24f, -0.24f), XMFLOAT2(0.0f, 1.0f) },
                    { XMFLOAT3(0.24f, -0.24f, -0.24f), XMFLOAT2(1.0f, 1.0f) },
                    { XMFLOAT3(0.24f, 0.24f, -0.24f), XMFLOAT2(1.0f, 0.0f) },
                    { XMFLOAT3(-0.24f, 0.24f, -0.24f), XMFLOAT2(0.0f, 0.0f) },

                    // Front face

                    { XMFLOAT3(-0.24f, -0.24f, 0.24f), XMFLOAT2(1.0f, 1.0f) },
                    { XMFLOAT3(0.24f, -0.24f, 0.24f), XMFLOAT2(0.0f, 1.0f) },
                    { XMFLOAT3(0.24f, 0.24f, 0.24f), XMFLOAT2(0.0f, 0.0f) },
                    { XMFLOAT3(-0.24f, 0.24f, 0.24f), XMFLOAT2(1.0f, 0.0f) },
                };

                Entity.SetVerticesList(VerticesList);
                return Entity;
		    }
            case PrimitiveGeometryType::CubeTest:
            {
                std::vector<SimpleColorVertex> VerticesList =
                {
                    { XMFLOAT3(-0.25f, 0.25f, -0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(0.25f, 0.25f, -0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(0.25f, 0.25f, 0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(-0.25f, 0.25f, 0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                    { XMFLOAT3(-0.25f, -0.25f, -0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(0.25f, -0.25f, -0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(0.25f, -0.25f, 0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(-0.25f, -0.25f, 0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                    { XMFLOAT3(-0.25f, -0.25f, 0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(-0.25f, -0.25f, -0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(-0.25f, 0.25f, -0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(-0.25f, 0.25f, 0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                    { XMFLOAT3(0.25f, -0.25f, 0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(0.25f, -0.25f, -0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(0.25f, 0.25f, -0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(0.25f, 0.25f, 0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                    { XMFLOAT3(-0.25f, -0.25f, -0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(0.25f, -0.25f, -0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(0.25f, 0.25f, -0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(-0.25f, 0.25f, -0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

                    // Front face

                    { XMFLOAT3(-0.25f, -0.25f, 0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(0.25f, -0.25f, 0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(0.25f, 0.25f, 0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                    { XMFLOAT3(-0.25f, 0.25f, 0.25f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
                };

                Entity.SetSimpleColorVerticesList(VerticesList);
                return Entity;
            }
            case PrimitiveGeometryType::Arrow:
            {
                std::vector<SimpleColorVertex>VerticesList =
                {
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
                

                Entity.SetSimpleColorVerticesList(VerticesList);
                return Entity;
            }
		}
	}

    GameEntity3D CreateLinetrace(float X_Origin, float Y_Origin, float Z_Origin, float X_Destination, float Y_Destination, float Z_Destination)
    {
        GameEntity3D LineEntity;

        std::vector<SimpleColorVertex>VerticesList =
        {
            { XMFLOAT3(X_Origin, Y_Origin, Z_Origin), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
            { XMFLOAT3(X_Destination, Y_Destination, Z_Destination), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        };

        LineEntity.SetSimpleColorVerticesList(VerticesList);
        return LineEntity;
    }
	
};
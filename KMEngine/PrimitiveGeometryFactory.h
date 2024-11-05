#pragma once

#include "GameEntity3D.h"
#include <math.h>

enum class PrimitiveGeometryType
{
    Line,
    Plane,
    PlaneColored,
    Cube,
    Pyramid,
    CubeTest,
    Cylinder,
    Arrow
};

class PrimitiveGeometryFactory
{
public:
    static int m_Entity3D_UID;

    void SetUID(GameEntity3D& Entity3D);

    void SetUIDTest(GameEntity3D& Entity3D);

    GameEntity3D CreateEntity3D(PrimitiveGeometryType Type);

    GameEntity3DComponent CreateEntity3DComponent(PrimitiveGeometryType Type);

    GameEntity3D CreateLinetrace(float X_Origin, float Y_Origin, float Z_Origin, float X_Destination, float Y_Destination, float Z_Destination);

    std::vector<WORD> GetIndicesList();
};
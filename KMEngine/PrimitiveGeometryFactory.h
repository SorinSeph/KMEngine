#pragma once

#include "GameEntity3D.h"
#include <math.h>

enum class EPrimitiveGeometryType
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

class CPrimitiveGeometryFactory
{
public:
    static int m_Entity3D_UID;

    void SetUID(CGameEntity3D& Entity3D);

    void SetUIDTest(CGameEntity3D& Entity3D);

    CGameEntity3D CreateEntity3D(EPrimitiveGeometryType m_TType);

    CGameEntity3DComponent CreateEntity3DComponent(EPrimitiveGeometryType m_TType);

    CGameEntity3D CreateLinetrace(float X_Origin, float Y_Origin, float Z_Origin, float X_Destination, float Y_Destination, float Z_Destination);

    std::vector<WORD> GetIndicesList();
};
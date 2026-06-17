#pragma once
#include "BaseModule.h"
#include "GameEntity3D.h"

class CPhysicsModule : public CBaseModule
{
public:
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

        assert(DirectX::MathInternal::XMQuaternionIsUnit(vOrientation));

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

        if (!DirectX::MathInternal::XMVector3AnyTrue(NoIntersection))
        {
            // Store the x-component to *pDist
            XMStoreFloat(&Dist, t_min);
            return true;
        }

        Dist = 0.f;
        return false;
    }

    bool DoesRayIntersectOBBOpenGL(
    glm::vec3 RayOrigin,
    glm::vec3 RayDirection,
    glm::vec3 BoxCenter,
    glm::vec3 BoxExtents,
    glm::quat BoxOrientation,
    float& Dist)
    {
        constexpr float EPSILON = 1e-6f;

        // Build rotation matrix from quaternion
        glm::mat3 R = glm::mat3_cast(BoxOrientation);

        // Ray origin relative to box center
        glm::vec3 Delta = BoxCenter - RayOrigin;

        float TMin = -FLT_MAX;
        float TMax = FLT_MAX;

        // Test each axis (local X, Y, Z of the OBB)
        for (int i = 0; i < 3; ++i)
        {
            glm::vec3 Axis = R[i]; // i-th column = i-th local axis
            float E = glm::dot(Axis, Delta);
            float F = glm::dot(Axis, RayDirection);

            if (std::abs(F) > EPSILON)
            {
                float T1 = (E - BoxExtents[i]) / F;
                float T2 = (E + BoxExtents[i]) / F;

                if (T1 > T2) std::swap(T1, T2);
                TMin = std::max(TMin, T1);
                TMax = std::min(TMax, T2);

                if (TMin > TMax) return false;
                if (TMax < 0.0f) return false;
            }
            else
            {
                // Ray parallel to slab; check if origin is within slab
                if (-E - BoxExtents[i] > 0.0f || -E + BoxExtents[i] < 0.0f)
                    return false;
            }
        }

        Dist = (TMin > 0.0f) ? TMin : TMax;
        return true;
    }
};
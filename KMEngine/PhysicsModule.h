#pragma once
#include "BaseModule.h"
#include "GameEntity3D.h"

class CPhysicsModule : public CBaseModule
{
public:
	void RayAndOBBIntersection(CGameEntity3D Ray, CGameEntity3D OBB)
	{
        //CLogger& SLogger = CLogger::GetLogger();

        //XMFLOAT3 fDirection{ 0, 0, 0 };
        //XMStoreFloat3(&fDirection, Direction);
        //SLogger.Log("DX11_Device::DoesIntersect, Ray Direction is: ", fDirection.x, " ", fDirection.y, " ", fDirection.z);

        //// Load the box.
        //XMVECTOR vCenter = XMLoadFloat3(&Center);
        ///*XMFLOAT3 fCenter{ 0, 0, 0 };
        //XMStoreFloat3(&fCenter, vCenter);
        //SLogger.Log("DX11_Device::DoesIntersect, Box Center is: ", fCenter.x, " ", fCenter.y, " ", fCenter.z);*/

        //XMVECTOR vExtents = XMLoadFloat3(&Extents);
        ///*XMFLOAT3 fExtents{ 0, 0, 0 };
        //XMStoreFloat3(&fExtents, vExtents);
        //SLogger.Log("DX11_Device::DoesIntersect, Box Extents is: ", fExtents.x, " ", fExtents.y, " ", fExtents.z);*/

        //// Adjust ray origin to be relative to center of the box.
        ////XMVECTOR TOrigin = XMVectorSubtract(vCenter, Origin);
        ////XMFLOAT3 fOrigin{ 0, 0, 0 };
        ////XMStoreFloat3(&fOrigin, TOrigin);
        ////SLogger.Log("DX11_Device::DoesIntersect, TOrigin is: ", fOrigin.x, " ", fOrigin.y, " ", fOrigin.z);

        ////XMFLOAT3 AxisDotDirectionFloat{ sinf(XMConvertToRadians(92)), 0, cosf(XMConvertToRadians(92)) };
        ////XMVECTOR AxisDotDirection{ XMLoadFloat3(&AxisDotDirectionFloat) };
        //XMVECTOR AxisDotDirection{ Direction };

        //// Compute the dot product againt each axis of the box.
        //// Since the axii are (1,0,0), (0,1,0), (0,0,1) no computation is necessary.
        //XMVECTOR AxisDotOrigin = XMVectorSubtract(vCenter, Origin);
        //XMFLOAT3 fAxisDotOrigin{ 0, 0, 0 };
        //XMStoreFloat3(&fAxisDotOrigin, AxisDotOrigin);
        //SLogger.Log("DX11_Device::DoesIntersect, Axis Dot Origin is: ", fAxisDotOrigin.x, " ", fAxisDotOrigin.y, " ", fAxisDotOrigin.z);

        ////XMVECTOR AxisDotDirection = Direction;

        //// Test against all three axii simultaneously.
        //XMVECTOR InverseAxisDotDirection = XMVectorReciprocal(AxisDotDirection);
        //XMFLOAT3 fInverseAxisDotDirection{ 0, 0, 0 };
        //XMStoreFloat3(&fInverseAxisDotDirection, InverseAxisDotDirection);
        //SLogger.Log("DX11_Device::DoesIntersect, Inverse Axis Dot Direction is: ", fInverseAxisDotDirection.x, " ", fInverseAxisDotDirection.y, " ", fInverseAxisDotDirection.z);

        //// if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
        //XMVECTOR IsParallel = XMVectorLessOrEqual(XMVectorAbs(AxisDotDirection), _RayEpsilon);
        //XMFLOAT3 fIsParallel{ 0, 0, 0 };
        //XMStoreFloat3(&fIsParallel, IsParallel);
        //SLogger.Log("DX11_Device::DoesIntersect, Is Parallel is: ", fIsParallel.x, " ", fIsParallel.y, " ", fIsParallel.z);

        //XMVECTOR t1 = XMVectorMultiply(XMVectorSubtract(AxisDotOrigin, vExtents), InverseAxisDotDirection);
        //XMFLOAT3 ft1{ 0, 0, 0 };
        //XMStoreFloat3(&ft1, t1);
        //SLogger.Log("DX11_Device::DoesIntersect, t1 is: ", ft1.x, " ", ft1.y, " ", ft1.z);

        //XMVECTOR t2 = XMVectorMultiply(XMVectorAdd(AxisDotOrigin, vExtents), InverseAxisDotDirection);
        //XMFLOAT3 ft2{ 0, 0, 0 };
        //XMStoreFloat3(&ft2, t2);
        //SLogger.Log("DX11_Device::DoesIntersect, t2 is: ", ft2.x, " ", ft2.y, " ", ft2.z);

        //// Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
        //// use the results from any directions parallel to the slab.
        //XMVECTOR t_min = XMVectorSelect(XMVectorMin(t1, t2), _FltMin, IsParallel);
        //XMFLOAT3 ft_min{ 0, 0, 0 };
        //XMStoreFloat3(&ft_min, t_min);
        //SLogger.Log("DX11_Device::DoesIntersect, t_min is: ", ft_min.x, " ", ft_min.y, " ", ft_min.z);

        //XMVECTOR t_max = XMVectorSelect(XMVectorMax(t1, t2), _FltMax, IsParallel);
        //XMFLOAT3 ft_max{ 0, 0, 0 };
        //XMStoreFloat3(&ft_max, t_max);
        //SLogger.Log("DX11_Device::DoesIntersect, t_max is: ", ft_max.x, " ", ft_max.y, " ", ft_max.z);

        //// t_min.x = maximum( t_min.x, t_min.y, t_min.z );
        //// t_max.x = minimum( t_max.x, t_max.y, t_max.z );
        //XMVECTOR new_t_min = XMVectorMax(t_min, XMVectorSplatY(t_min));  // x = max(x,y)
        //XMFLOAT3 fnew_t_min{ 0, 0, 0 };
        //XMStoreFloat3(&fnew_t_min, new_t_min);
        //SLogger.Log("DX11_Device::DoesIntersect, t_min: ", fnew_t_min.x, " ", fnew_t_min.y, " ", fnew_t_min.z);

        //XMVECTOR new_t_min2 = XMVectorMax(new_t_min, XMVectorSplatZ(new_t_min));  // x = max(max(x,y),z)
        //XMFLOAT3 fnew_t_min2{ 0, 0, 0 };
        //XMStoreFloat3(&fnew_t_min2, new_t_min2);
        //SLogger.Log("DX11_Device::DoesIntersect, t_min: ", fnew_t_min2.x, " ", fnew_t_min2.y, " ", fnew_t_min2.z);

        //XMVECTOR new_t_max = XMVectorMin(t_max, XMVectorSplatY(t_max));  // x = min(x,y)
        //XMFLOAT3 fnew_t_max{ 0, 0, 0 };
        //XMStoreFloat3(&fnew_t_max, new_t_max);
        //SLogger.Log("DX11_Device::DoesIntersect, t_max: ", fnew_t_max.x, " ", fnew_t_max.y, " ", fnew_t_max.z);

        //XMVECTOR new_t_max2 = XMVectorMin(new_t_max, XMVectorSplatZ(new_t_max));  // x = min(min(x,y),z)
        //XMFLOAT3 fnew_t_max2{ 0, 0, 0 };
        //XMStoreFloat3(&fnew_t_max2, new_t_max2);
        //SLogger.Log("DX11_Device::DoesIntersect, t_max: ", fnew_t_max2.x, " ", fnew_t_max2.y, " ", fnew_t_max2.z);

        //// if ( t_min > t_max ) return false;
        //XMVECTOR NoIntersection = XMVectorGreater(XMVectorSplatX(new_t_min2), XMVectorSplatX(new_t_max2));
        //XMFLOAT3 fNoIntersection{ 0, 0, 0 };
        //XMStoreFloat3(&fNoIntersection, NoIntersection);
        //SLogger.Log("DX11_Device::DoesIntersect, No Intersection is: ", fNoIntersection.x, " ", fNoIntersection.y, " ", fNoIntersection.z);

        //// if ( t_max < 0.0f ) return false;
        //XMVECTOR NoIntersection2 = XMVectorOrInt(NoIntersection, XMVectorLess(XMVectorSplatX(t_max), XMVectorZero()));
        //XMFLOAT3 fNoIntersection2{ 0, 0, 0 };
        //XMStoreFloat3(&fNoIntersection2, NoIntersection2);
        //SLogger.Log("DX11_Device::DoesIntersect, No Intersection2 is: ", fNoIntersection2.x, " ", fNoIntersection2.y, " ", fNoIntersection2.z);

        //// if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return false;
        //XMVECTOR ParallelOverlap = XMVectorInBounds(AxisDotOrigin, vExtents);
        //XMFLOAT3 fParallelOverlap{ 0, 0, 0 };
        //XMStoreFloat3(&fParallelOverlap, ParallelOverlap);
        //SLogger.Log("DX11_Device::DoesIntersect, Parallel overlap: ", fParallelOverlap.x, " ", fParallelOverlap.y, " ", fParallelOverlap.z);

        //XMVECTOR NoIntersection3 = XMVectorOrInt(NoIntersection2, XMVectorAndCInt(IsParallel, ParallelOverlap));
        //XMFLOAT3 fNoIntersection3{ 0, 0, 0 };
        //XMStoreFloat3(&fNoIntersection3, NoIntersection3);
        //SLogger.Log("DX11_Device::DoesIntersect, NoIntersection3 is: ", fNoIntersection3.x, " ", fNoIntersection3.y, " ", fNoIntersection3.z);

        //if (!DirectX::Internal::XMVector3AnyTrue(NoIntersection))
        //{
        //    // Store the x-component to *pDist
        //    XMStoreFloat(&Dist, t_min);
        //    SLogger.Log("Intersection TRUE");
        //    return true;
        //}

        //Dist = 0.f;
        //SLogger.Log("Intersection FALSE");
        //return false;
	}
};
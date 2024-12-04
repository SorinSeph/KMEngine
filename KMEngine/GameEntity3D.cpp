#include "GameEntity3D.h"

XMMATRIX CGameEntity3D::TransformationMatrix()
{
	return m_ScaleMatrix * m_RotationMatrix * m_LocationMatrix;
}


void CGameEntity3D::SetSimpleColorVerticesList(std::vector<SSimpleColorVertex> VerticesList)
{
	m_SimpleColorVerticesList = VerticesList;
}

std::vector<SSimpleVertex> CGameEntity3D::GetVerticesList()
{
	return m_VerticesList;
}

std::vector<SSimpleColorVertex> CGameEntity3D::GetSimpleColorVerticesList()
{
	return m_SimpleColorVerticesList;
}

std::vector<SColorVertex> CGameEntity3D::GetColorVerticesList()
{
	return m_ColorVerticesList;
}

void CGameEntity3D::SetLocation(XMMATRIX Location)
{
	m_ConstantBuffer.mWorld = Location;
}

void CGameEntity3D::SetLocationF(float InX, float InY, float InZ)
{
	m_LocationX = InX;
	m_LocationY = InY;
	m_LocationZ = InZ;
	m_LocationMatrix = XMMatrixTranslation(InX, InY, InZ);
	m_ConstantBuffer.mWorld = TransformationMatrix();
}

XMMATRIX CGameEntity3D::GetLocation()
{
	return m_LocationMatrix;
}

float CGameEntity3D::GetLocationX()
{
	return m_LocationX;
}

float CGameEntity3D::GetLocationY()
{
	return m_LocationY;
}

float CGameEntity3D::GetLocationZ()
{
	return m_LocationZ;
}

float CGameEntity3D::GetRotationX()
{
	return m_RotationX;
}

float CGameEntity3D::GetRotationY()
{
	return m_RotationY;
}

float CGameEntity3D::GetRotationZ()
{
	return m_RotationZ;
}

XMVECTOR CGameEntity3D::GetLocationVector()
{
	return m_LocationVector;
}

XMMATRIX CGameEntity3D::GetRotation()
{
	return m_RotationMatrix;
}

XMMATRIX CGameEntity3D::GetScale()
{
	return m_ScaleMatrix;
}

void CGameEntity3D::SetRotation(float InPitch, float InYaw, float InRoll)
{
	m_RotationMatrix = XMMatrixRotationRollPitchYaw(InPitch, InYaw, InRoll);
	m_ConstantBuffer.mWorld = TransformationMatrix();
}

void CGameEntity3D::SetScale(float InX, float InY, float InZ)
{
	m_ScaleMatrix = XMMatrixScaling(InX, InY, InZ);
	m_ConstantBuffer.mWorld = TransformationMatrix();
}

SConstantBuffer CGameEntity3D::GetConstantBuffer()
{
	return m_ConstantBuffer;
}

SArrowConstantBuffer CGameEntity3D::GetArrowConstantBuffer()
{
	return m_ArrowConstantBuffer;
}

void CGameEntity3D::SetCollisionBoxCenter(XMFLOAT3 NewCenter)
{
	m_Collision.AABox.Center = NewCenter;
	m_Collision.AABox.Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);
	m_Collision.CollisionType = DISJOINT;
}

void CGameEntity3D::SetUID(std::string uid)
{
	m_UID = uid;
}

std::string CGameEntity3D::GetUID()
{
	return m_UID;
}

SCollisionComponent CGameEntity3D::GetCollisionComponent()
{
	return m_Collision;
}

// Test

void CGameEntity3D::SetUIDTest(int uid)
{
	m_UIDTest = uid;
}

int CGameEntity3D::GetUIDTest()
{
	return m_UIDTest;
}

void CGameEntity3D::SetCollisionParams(XMFLOAT3 Center, XMFLOAT3 Extents, ContainmentType CollisionType)
{
	m_Collision.AABox.Center = Center;
	m_Collision.AABox.Extents = Extents;
	m_Collision.CollisionType = CollisionType;
}

DXGI_FORMAT CGameEntity3D::GetOwnFormat()
{
	return m_Format;
}

int CGameEntity3D::GetOwnOffset()
{
	return m_Offset;
}

_Use_decl_annotations_
inline CFrustumComponent::CFrustumComponent(CXMMATRIX Projection, bool rhcoords) noexcept
{
    CreateFrustumFromMatrix(*this, Projection, rhcoords);
}


//-----------------------------------------------------------------------------
// Transform a frustum by an angle preserving transform.
//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline void XM_CALLCONV CFrustumComponent::Transform(CFrustumComponent& Out, FXMMATRIX M) const noexcept
{
    // Load the frustum.
    XMVECTOR vOrigin = XMLoadFloat3(&Origin);
    XMVECTOR vOrientation = XMLoadFloat4(&Orientation);

    assert(DirectX::Internal::XMQuaternionIsUnit(vOrientation));

    // Composite the frustum rotation and the transform rotation
    XMMATRIX nM;
    nM.r[0] = XMVector3Normalize(M.r[0]);
    nM.r[1] = XMVector3Normalize(M.r[1]);
    nM.r[2] = XMVector3Normalize(M.r[2]);
    nM.r[3] = g_XMIdentityR3;
    XMVECTOR Rotation = XMQuaternionRotationMatrix(nM);
    vOrientation = XMQuaternionMultiply(vOrientation, Rotation);

    // Transform the center.
    vOrigin = XMVector3Transform(vOrigin, M);

    // Store the frustum.
    XMStoreFloat3(&Out.Origin, vOrigin);
    XMStoreFloat4(&Out.Orientation, vOrientation);

    // Scale the near and far distances (the slopes remain the same).
    XMVECTOR dX = XMVector3Dot(M.r[0], M.r[0]);
    XMVECTOR dY = XMVector3Dot(M.r[1], M.r[1]);
    XMVECTOR dZ = XMVector3Dot(M.r[2], M.r[2]);

    XMVECTOR d = XMVectorMax(dX, XMVectorMax(dY, dZ));
    float Scale = sqrtf(XMVectorGetX(d));

    Out.Near = Near * Scale;
    Out.Far = Far * Scale;

    // Copy the slopes.
    Out.RightSlope = RightSlope;
    Out.LeftSlope = LeftSlope;
    Out.TopSlope = TopSlope;
    Out.BottomSlope = BottomSlope;
}

_Use_decl_annotations_
inline void XM_CALLCONV CFrustumComponent::Transform(CFrustumComponent& Out, float Scale, FXMVECTOR Rotation, FXMVECTOR Translation) const noexcept
{
    assert(DirectX::Internal::XMQuaternionIsUnit(Rotation));

    // Load the frustum.
    XMVECTOR vOrigin = XMLoadFloat3(&Origin);
    XMVECTOR vOrientation = XMLoadFloat4(&Orientation);

    assert(DirectX::Internal::XMQuaternionIsUnit(vOrientation));

    // Composite the frustum rotation and the transform rotation.
    vOrientation = XMQuaternionMultiply(vOrientation, Rotation);

    // Transform the origin.
    vOrigin = XMVectorAdd(XMVector3Rotate(XMVectorScale(vOrigin, Scale), Rotation), Translation);

    // Store the frustum.
    XMStoreFloat3(&Out.Origin, vOrigin);
    XMStoreFloat4(&Out.Orientation, vOrientation);

    // Scale the near and far distances (the slopes remain the same).
    Out.Near = Near * Scale;
    Out.Far = Far * Scale;

    // Copy the slopes.
    Out.RightSlope = RightSlope;
    Out.LeftSlope = LeftSlope;
    Out.TopSlope = TopSlope;
    Out.BottomSlope = BottomSlope;
}


//-----------------------------------------------------------------------------
// Get the corner points of the frustum
//-----------------------------------------------------------------------------
inline void CFrustumComponent::GetFrustumCorners(XMFLOAT3* Corners)
{
    assert(Corners != nullptr);

    // Load origin and orientation of the frustum.
    XMVECTOR vOrigin = XMLoadFloat3(&Origin);
    XMVECTOR vOrientation = XMLoadFloat4(&Orientation);

    assert(DirectX::Internal::XMQuaternionIsUnit(vOrientation));

    // Build the corners of the frustum.
    XMVECTOR vRightTop = XMVectorSet(RightSlope, TopSlope, 1.0f, 0.0f);
    XMVECTOR vRightBottom = XMVectorSet(RightSlope, BottomSlope, 1.0f, 0.0f);
    XMVECTOR vLeftTop = XMVectorSet(LeftSlope, TopSlope, 1.0f, 0.0f);
    XMVECTOR vLeftBottom = XMVectorSet(LeftSlope, BottomSlope, 1.0f, 0.0f);
    XMVECTOR vNear = XMVectorReplicatePtr(&Near);
    XMVECTOR vFar = XMVectorReplicatePtr(&Far);

    // Returns 8 corners position of bounding frustum.
    //     Near    Far
    //    0----1  4----5
    //    |    |  |    |
    //    |    |  |    |
    //    3----2  7----6

    XMVECTOR vCorners[CORNER_COUNT];
    vCorners[0] = XMVectorMultiply(vLeftTop, vNear);
    vCorners[1] = XMVectorMultiply(vRightTop, vNear);
    vCorners[2] = XMVectorMultiply(vRightBottom, vNear);
    vCorners[3] = XMVectorMultiply(vLeftBottom, vNear);
    vCorners[4] = XMVectorMultiply(vLeftTop, vFar);
    vCorners[5] = XMVectorMultiply(vRightTop, vFar);
    vCorners[6] = XMVectorMultiply(vRightBottom, vFar);
    vCorners[7] = XMVectorMultiply(vLeftBottom, vFar);

    for (size_t i = 0; i < CORNER_COUNT; ++i)
    {
        XMVECTOR C = XMVectorAdd(XMVector3Rotate(vCorners[i], vOrientation), vOrigin);
        XMStoreFloat3(&Corners[i], C);
    }
}


//-----------------------------------------------------------------------------
// Point in frustum test.
//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline ContainmentType XM_CALLCONV CFrustumComponent::Contains(FXMVECTOR Point) const noexcept
{
    // Build frustum planes.
    XMVECTOR Planes[6];
    Planes[0] = XMVectorSet(0.0f, 0.0f, -1.0f, Near);
    Planes[1] = XMVectorSet(0.0f, 0.0f, 1.0f, -Far);
    Planes[2] = XMVectorSet(1.0f, 0.0f, -RightSlope, 0.0f);
    Planes[3] = XMVectorSet(-1.0f, 0.0f, LeftSlope, 0.0f);
    Planes[4] = XMVectorSet(0.0f, 1.0f, -TopSlope, 0.0f);
    Planes[5] = XMVectorSet(0.0f, -1.0f, BottomSlope, 0.0f);

    // Load origin and orientation.
    XMVECTOR vOrigin = XMLoadFloat3(&Origin);
    XMVECTOR vOrientation = XMLoadFloat4(&Orientation);

    assert(DirectX::Internal::XMQuaternionIsUnit(vOrientation));

    // Transform point into local space of frustum.
    XMVECTOR TPoint = XMVector3InverseRotate(XMVectorSubtract(Point, vOrigin), vOrientation);

    // Set w to one.
    TPoint = XMVectorInsert<0, 0, 0, 0, 1>(TPoint, XMVectorSplatOne());

    XMVECTOR Zero = XMVectorZero();
    XMVECTOR Outside = Zero;

    // Test point against each plane of the frustum.
    for (size_t i = 0; i < 6; ++i)
    {
        XMVECTOR Dot = XMVector4Dot(TPoint, Planes[i]);
        Outside = XMVectorOrInt(Outside, XMVectorGreater(Dot, Zero));
    }

    return XMVector4NotEqualInt(Outside, XMVectorTrueInt()) ? CONTAINS : DISJOINT;
}


//-----------------------------------------------------------------------------
// Triangle vs frustum test.
//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline ContainmentType XM_CALLCONV CFrustumComponent::Contains(FXMVECTOR V0, FXMVECTOR V1, FXMVECTOR V2) const noexcept
{
    // Load origin and orientation of the frustum.
    XMVECTOR vOrigin = XMLoadFloat3(&Origin);
    XMVECTOR vOrientation = XMLoadFloat4(&Orientation);

    // Create 6 planes (do it inline to encourage use of registers)
    XMVECTOR NearPlane = XMVectorSet(0.0f, 0.0f, -1.0f, Near);
    NearPlane = DirectX::Internal::XMPlaneTransform(NearPlane, vOrientation, vOrigin);
    NearPlane = XMPlaneNormalize(NearPlane);

    XMVECTOR FarPlane = XMVectorSet(0.0f, 0.0f, 1.0f, -Far);
    FarPlane = DirectX::Internal::XMPlaneTransform(FarPlane, vOrientation, vOrigin);
    FarPlane = XMPlaneNormalize(FarPlane);

    XMVECTOR RightPlane = XMVectorSet(1.0f, 0.0f, -RightSlope, 0.0f);
    RightPlane = DirectX::Internal::XMPlaneTransform(RightPlane, vOrientation, vOrigin);
    RightPlane = XMPlaneNormalize(RightPlane);

    XMVECTOR LeftPlane = XMVectorSet(-1.0f, 0.0f, LeftSlope, 0.0f);
    LeftPlane = DirectX::Internal::XMPlaneTransform(LeftPlane, vOrientation, vOrigin);
    LeftPlane = XMPlaneNormalize(LeftPlane);

    XMVECTOR TopPlane = XMVectorSet(0.0f, 1.0f, -TopSlope, 0.0f);
    TopPlane = DirectX::Internal::XMPlaneTransform(TopPlane, vOrientation, vOrigin);
    TopPlane = XMPlaneNormalize(TopPlane);

    XMVECTOR BottomPlane = XMVectorSet(0.0f, -1.0f, BottomSlope, 0.0f);
    BottomPlane = DirectX::Internal::XMPlaneTransform(BottomPlane, vOrientation, vOrigin);
    BottomPlane = XMPlaneNormalize(BottomPlane);

    return TriangleTests::ContainedBy(V0, V1, V2, NearPlane, FarPlane, RightPlane, LeftPlane, TopPlane, BottomPlane);
}


//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline ContainmentType CFrustumComponent::Contains(const BoundingSphere& sh) const noexcept
{
    // Load origin and orientation of the frustum.
    XMVECTOR vOrigin = XMLoadFloat3(&Origin);
    XMVECTOR vOrientation = XMLoadFloat4(&Orientation);

    // Create 6 planes (do it inline to encourage use of registers)
    XMVECTOR NearPlane = XMVectorSet(0.0f, 0.0f, -1.0f, Near);
    NearPlane = DirectX::Internal::XMPlaneTransform(NearPlane, vOrientation, vOrigin);
    NearPlane = XMPlaneNormalize(NearPlane);

    XMVECTOR FarPlane = XMVectorSet(0.0f, 0.0f, 1.0f, -Far);
    FarPlane = DirectX::Internal::XMPlaneTransform(FarPlane, vOrientation, vOrigin);
    FarPlane = XMPlaneNormalize(FarPlane);

    XMVECTOR RightPlane = XMVectorSet(1.0f, 0.0f, -RightSlope, 0.0f);
    RightPlane = DirectX::Internal::XMPlaneTransform(RightPlane, vOrientation, vOrigin);
    RightPlane = XMPlaneNormalize(RightPlane);

    XMVECTOR LeftPlane = XMVectorSet(-1.0f, 0.0f, LeftSlope, 0.0f);
    LeftPlane = DirectX::Internal::XMPlaneTransform(LeftPlane, vOrientation, vOrigin);
    LeftPlane = XMPlaneNormalize(LeftPlane);

    XMVECTOR TopPlane = XMVectorSet(0.0f, 1.0f, -TopSlope, 0.0f);
    TopPlane = DirectX::Internal::XMPlaneTransform(TopPlane, vOrientation, vOrigin);
    TopPlane = XMPlaneNormalize(TopPlane);

    XMVECTOR BottomPlane = XMVectorSet(0.0f, -1.0f, BottomSlope, 0.0f);
    BottomPlane = DirectX::Internal::XMPlaneTransform(BottomPlane, vOrientation, vOrigin);
    BottomPlane = XMPlaneNormalize(BottomPlane);

    return sh.ContainedBy(NearPlane, FarPlane, RightPlane, LeftPlane, TopPlane, BottomPlane);
}


//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline ContainmentType CFrustumComponent::Contains(const BoundingBox& box) const noexcept
{
    // Load origin and orientation of the frustum.
    XMVECTOR vOrigin = XMLoadFloat3(&Origin);
    XMVECTOR vOrientation = XMLoadFloat4(&Orientation);

    // Create 6 planes (do it inline to encourage use of registers)
    XMVECTOR NearPlane = XMVectorSet(0.0f, 0.0f, -1.0f, Near);
    //std::cout << "DirectXCollision.inl: Near = " << Near << "\n";
    NearPlane = DirectX::Internal::XMPlaneTransform(NearPlane, vOrientation, vOrigin);
    NearPlane = XMPlaneNormalize(NearPlane);

    XMVECTOR FarPlane = XMVectorSet(0.0f, 0.0f, 1.0f, -Far);
    FarPlane = DirectX::Internal::XMPlaneTransform(FarPlane, vOrientation, vOrigin);
    FarPlane = XMPlaneNormalize(FarPlane);

    XMVECTOR RightPlane = XMVectorSet(1.0f, 0.0f, -RightSlope, 0.0f);
    RightPlane = DirectX::Internal::XMPlaneTransform(RightPlane, vOrientation, vOrigin);
    RightPlane = XMPlaneNormalize(RightPlane);

    XMVECTOR LeftPlane = XMVectorSet(-1.0f, 0.0f, LeftSlope, 0.0f);
    LeftPlane = DirectX::Internal::XMPlaneTransform(LeftPlane, vOrientation, vOrigin);
    LeftPlane = XMPlaneNormalize(LeftPlane);

    XMVECTOR TopPlane = XMVectorSet(0.0f, 1.0f, -TopSlope, 0.0f);
    TopPlane = DirectX::Internal::XMPlaneTransform(TopPlane, vOrientation, vOrigin);
    TopPlane = XMPlaneNormalize(TopPlane);

    XMVECTOR BottomPlane = XMVectorSet(0.0f, -1.0f, BottomSlope, 0.0f);
    BottomPlane = DirectX::Internal::XMPlaneTransform(BottomPlane, vOrientation, vOrigin);
    BottomPlane = XMPlaneNormalize(BottomPlane);

    return box.ContainedBy(NearPlane, FarPlane, RightPlane, LeftPlane, TopPlane, BottomPlane);
}


//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline ContainmentType CFrustumComponent::Contains(const BoundingOrientedBox& box) const noexcept
{
    // Load origin and orientation of the frustum.
    XMVECTOR vOrigin = XMLoadFloat3(&Origin);
    XMVECTOR vOrientation = XMLoadFloat4(&Orientation);

    // Create 6 planes (do it inline to encourage use of registers)
    XMVECTOR NearPlane = XMVectorSet(0.0f, 0.0f, -1.0f, Near);
    NearPlane = DirectX::Internal::XMPlaneTransform(NearPlane, vOrientation, vOrigin);
    XMFLOAT4 NearPlaneFloat;
    XMStoreFloat4(&NearPlaneFloat, NearPlane);
    NearPlane = XMPlaneNormalize(NearPlane);


    XMVECTOR FarPlane = XMVectorSet(0.0f, 0.0f, 1.0f, -Far);
    FarPlane = DirectX::Internal::XMPlaneTransform(FarPlane, vOrientation, vOrigin);
    XMFLOAT4 FarPlaneFloat;
    XMStoreFloat4(&FarPlaneFloat, FarPlane);
    FarPlane = XMPlaneNormalize(FarPlane);


    XMVECTOR RightPlane = XMVectorSet(1.0f, 0.0f, -RightSlope, 0.0f);
    RightPlane = DirectX::Internal::XMPlaneTransform(RightPlane, vOrientation, vOrigin);
    XMFLOAT4 RightPlaneFloat;
    XMStoreFloat4(&RightPlaneFloat, RightPlane);
    RightPlane = XMPlaneNormalize(RightPlane);


    XMVECTOR LeftPlane = XMVectorSet(-1.0f, 0.0f, LeftSlope, 0.0f);
    LeftPlane = DirectX::Internal::XMPlaneTransform(LeftPlane, vOrientation, vOrigin);
    XMFLOAT4 LeftPlaneFloat;
    XMStoreFloat4(&NearPlaneFloat, LeftPlane);
    LeftPlane = XMPlaneNormalize(LeftPlane);


    XMVECTOR TopPlane = XMVectorSet(0.0f, 1.0f, -TopSlope, 0.0f);
    TopPlane = DirectX::Internal::XMPlaneTransform(TopPlane, vOrientation, vOrigin);
    XMFLOAT4 TopPlaneFloat;
    XMStoreFloat4(&TopPlaneFloat, TopPlane);
    TopPlane = XMPlaneNormalize(TopPlane);


    XMVECTOR BottomPlane = XMVectorSet(0.0f, -1.0f, BottomSlope, 0.0f);
    BottomPlane = DirectX::Internal::XMPlaneTransform(BottomPlane, vOrientation, vOrigin);
    XMFLOAT4 BottomPlaneFloat;
    XMStoreFloat4(&BottomPlaneFloat, BottomPlane);
    BottomPlane = XMPlaneNormalize(BottomPlane);


    return box.ContainedBy(NearPlane, FarPlane, RightPlane, LeftPlane, TopPlane, BottomPlane);
}


//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline ContainmentType CFrustumComponent::Contains(const CFrustumComponent& fr) const noexcept
{
    // Load origin and orientation of the frustum.
    XMVECTOR vOrigin = XMLoadFloat3(&Origin);
    XMVECTOR vOrientation = XMLoadFloat4(&Orientation);

    // Create 6 planes (do it inline to encourage use of registers)
    XMVECTOR NearPlane = XMVectorSet(0.0f, 0.0f, -1.0f, Near);
    NearPlane = DirectX::Internal::XMPlaneTransform(NearPlane, vOrientation, vOrigin);
    NearPlane = XMPlaneNormalize(NearPlane);

    XMVECTOR FarPlane = XMVectorSet(0.0f, 0.0f, 1.0f, -Far);
    FarPlane = DirectX::Internal::XMPlaneTransform(FarPlane, vOrientation, vOrigin);
    FarPlane = XMPlaneNormalize(FarPlane);

    XMVECTOR RightPlane = XMVectorSet(1.0f, 0.0f, -RightSlope, 0.0f);
    RightPlane = DirectX::Internal::XMPlaneTransform(RightPlane, vOrientation, vOrigin);
    RightPlane = XMPlaneNormalize(RightPlane);

    XMVECTOR LeftPlane = XMVectorSet(-1.0f, 0.0f, LeftSlope, 0.0f);
    LeftPlane = DirectX::Internal::XMPlaneTransform(LeftPlane, vOrientation, vOrigin);
    LeftPlane = XMPlaneNormalize(LeftPlane);

    XMVECTOR TopPlane = XMVectorSet(0.0f, 1.0f, -TopSlope, 0.0f);
    TopPlane = DirectX::Internal::XMPlaneTransform(TopPlane, vOrientation, vOrigin);
    TopPlane = XMPlaneNormalize(TopPlane);

    XMVECTOR BottomPlane = XMVectorSet(0.0f, -1.0f, BottomSlope, 0.0f);
    BottomPlane = DirectX::Internal::XMPlaneTransform(BottomPlane, vOrientation, vOrigin);
    BottomPlane = XMPlaneNormalize(BottomPlane);

    return fr.ContainedBy(NearPlane, FarPlane, RightPlane, LeftPlane, TopPlane, BottomPlane);
}


//-----------------------------------------------------------------------------
// Exact sphere vs frustum test.  The algorithm first checks the sphere against
// the planes of the frustum, then if the plane checks were indeterminate finds
// the nearest feature (plane, line, point) on the frustum to the center of the
// sphere and compares the distance to the nearest feature to the radius of the
// sphere
//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline bool CFrustumComponent::Intersects(const BoundingSphere& sh) const noexcept
{
    XMVECTOR Zero = XMVectorZero();

    // Build the frustum planes.
    XMVECTOR Planes[6];
    Planes[0] = XMVectorSet(0.0f, 0.0f, -1.0f, Near);
    Planes[1] = XMVectorSet(0.0f, 0.0f, 1.0f, -Far);
    Planes[2] = XMVectorSet(1.0f, 0.0f, -RightSlope, 0.0f);
    Planes[3] = XMVectorSet(-1.0f, 0.0f, LeftSlope, 0.0f);
    Planes[4] = XMVectorSet(0.0f, 1.0f, -TopSlope, 0.0f);
    Planes[5] = XMVectorSet(0.0f, -1.0f, BottomSlope, 0.0f);

    // Normalize the planes so we can compare to the sphere radius.
    Planes[2] = XMVector3Normalize(Planes[2]);
    Planes[3] = XMVector3Normalize(Planes[3]);
    Planes[4] = XMVector3Normalize(Planes[4]);
    Planes[5] = XMVector3Normalize(Planes[5]);

    // Load origin and orientation of the frustum.
    XMVECTOR vOrigin = XMLoadFloat3(&Origin);
    XMVECTOR vOrientation = XMLoadFloat4(&Orientation);

    assert(DirectX::Internal::XMQuaternionIsUnit(vOrientation));

    // Load the sphere.
    XMVECTOR vCenter = XMLoadFloat3(&sh.Center);
    XMVECTOR vRadius = XMVectorReplicatePtr(&sh.Radius);

    // Transform the center of the sphere into the local space of frustum.
    vCenter = XMVector3InverseRotate(XMVectorSubtract(vCenter, vOrigin), vOrientation);

    // Set w of the center to one so we can dot4 with the plane.
    vCenter = XMVectorInsert<0, 0, 0, 0, 1>(vCenter, XMVectorSplatOne());

    // Check against each plane of the frustum.
    XMVECTOR Outside = XMVectorFalseInt();
    XMVECTOR InsideAll = XMVectorTrueInt();
    XMVECTOR CenterInsideAll = XMVectorTrueInt();

    XMVECTOR Dist[6];

    for (size_t i = 0; i < 6; ++i)
    {
        Dist[i] = XMVector4Dot(vCenter, Planes[i]);

        // Outside the plane?
        Outside = XMVectorOrInt(Outside, XMVectorGreater(Dist[i], vRadius));

        // Fully inside the plane?
        InsideAll = XMVectorAndInt(InsideAll, XMVectorLessOrEqual(Dist[i], XMVectorNegate(vRadius)));

        // Check if the center is inside the plane.
        CenterInsideAll = XMVectorAndInt(CenterInsideAll, XMVectorLessOrEqual(Dist[i], Zero));
    }

    // If the sphere is outside any of the planes it is outside.
    if (XMVector4EqualInt(Outside, XMVectorTrueInt()))
        return false;

    // If the sphere is inside all planes it is fully inside.
    if (XMVector4EqualInt(InsideAll, XMVectorTrueInt()))
        return true;

    // If the center of the sphere is inside all planes and the sphere intersects
    // one or more planes then it must intersect.
    if (XMVector4EqualInt(CenterInsideAll, XMVectorTrueInt()))
        return true;

    // The sphere may be outside the frustum or intersecting the frustum.
    // Find the nearest feature (face, edge, or corner) on the frustum
    // to the sphere.

    // The faces adjacent to each face are:
    static const size_t adjacent_faces[6][4] =
    {
        { 2, 3, 4, 5 },    // 0
        { 2, 3, 4, 5 },    // 1
        { 0, 1, 4, 5 },    // 2
        { 0, 1, 4, 5 },    // 3
        { 0, 1, 2, 3 },    // 4
        { 0, 1, 2, 3 }
    };  // 5

    XMVECTOR Intersects = XMVectorFalseInt();

    // Check to see if the nearest feature is one of the planes.
    for (size_t i = 0; i < 6; ++i)
    {
        // Find the nearest point on the plane to the center of the sphere.
        XMVECTOR Point = XMVectorNegativeMultiplySubtract(Planes[i], Dist[i], vCenter);

        // Set w of the point to one.
        Point = XMVectorInsert<0, 0, 0, 0, 1>(Point, XMVectorSplatOne());

        // If the point is inside the face (inside the adjacent planes) then
        // this plane is the nearest feature.
        XMVECTOR InsideFace = XMVectorTrueInt();

        for (size_t j = 0; j < 4; j++)
        {
            size_t plane_index = adjacent_faces[i][j];

            InsideFace = XMVectorAndInt(InsideFace,
                XMVectorLessOrEqual(XMVector4Dot(Point, Planes[plane_index]), Zero));
        }

        // Since we have already checked distance from the plane we know that the
        // sphere must intersect if this plane is the nearest feature.
        Intersects = XMVectorOrInt(Intersects,
            XMVectorAndInt(XMVectorGreater(Dist[i], Zero), InsideFace));
    }

    if (XMVector4EqualInt(Intersects, XMVectorTrueInt()))
        return true;

    // Build the corners of the frustum.
    XMVECTOR vRightTop = XMVectorSet(RightSlope, TopSlope, 1.0f, 0.0f);
    XMVECTOR vRightBottom = XMVectorSet(RightSlope, BottomSlope, 1.0f, 0.0f);
    XMVECTOR vLeftTop = XMVectorSet(LeftSlope, TopSlope, 1.0f, 0.0f);
    XMVECTOR vLeftBottom = XMVectorSet(LeftSlope, BottomSlope, 1.0f, 0.0f);
    XMVECTOR vNear = XMVectorReplicatePtr(&Near);
    XMVECTOR vFar = XMVectorReplicatePtr(&Far);

    XMVECTOR Corners[CORNER_COUNT];
    Corners[0] = XMVectorMultiply(vRightTop, vNear);
    Corners[1] = XMVectorMultiply(vRightBottom, vNear);
    Corners[2] = XMVectorMultiply(vLeftTop, vNear);
    Corners[3] = XMVectorMultiply(vLeftBottom, vNear);
    Corners[4] = XMVectorMultiply(vRightTop, vFar);
    Corners[5] = XMVectorMultiply(vRightBottom, vFar);
    Corners[6] = XMVectorMultiply(vLeftTop, vFar);
    Corners[7] = XMVectorMultiply(vLeftBottom, vFar);

    // The Edges are:
    static const size_t edges[12][2] =
    {
        { 0, 1 }, { 2, 3 }, { 0, 2 }, { 1, 3 },    // Near plane
        { 4, 5 }, { 6, 7 }, { 4, 6 }, { 5, 7 },    // Far plane
        { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
    }; // Near to far

    XMVECTOR RadiusSq = XMVectorMultiply(vRadius, vRadius);

    // Check to see if the nearest feature is one of the edges (or corners).
    for (size_t i = 0; i < 12; ++i)
    {
        size_t ei0 = edges[i][0];
        size_t ei1 = edges[i][1];

        // Find the nearest point on the edge to the center of the sphere.
        // The corners of the frustum are included as the endpoints of the edges.
        XMVECTOR Point = DirectX::Internal::PointOnLineSegmentNearestPoint(Corners[ei0], Corners[ei1], vCenter);

        XMVECTOR Delta = XMVectorSubtract(vCenter, Point);

        XMVECTOR DistSq = XMVector3Dot(Delta, Delta);

        // If the distance to the center of the sphere to the point is less than
        // the radius of the sphere then it must intersect.
        Intersects = XMVectorOrInt(Intersects, XMVectorLessOrEqual(DistSq, RadiusSq));
    }

    if (XMVector4EqualInt(Intersects, XMVectorTrueInt()))
        return true;

    // The sphere must be outside the frustum.
    return false;
}


//-----------------------------------------------------------------------------
// Exact axis aligned box vs frustum test.  Constructs an oriented box and uses
// the oriented box vs frustum test.
//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline bool CFrustumComponent::Intersects(const BoundingBox& box) const noexcept
{
    // Make the axis aligned box oriented and do an OBB vs frustum test.
    BoundingOrientedBox obox(box.Center, box.Extents, XMFLOAT4(0.f, 0.f, 0.f, 1.f));
    return Intersects(obox);
}


//-----------------------------------------------------------------------------
// Exact oriented box vs frustum test.
//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline bool CFrustumComponent::Intersects(const BoundingOrientedBox& box) const noexcept
{
    static const XMVECTORU32 SelectY = { { { XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0 } } };
    static const XMVECTORU32 SelectZ = { { { XM_SELECT_0, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0 } } };

    XMVECTOR Zero = XMVectorZero();

    // Build the frustum planes.
    XMVECTOR Planes[6];
    Planes[0] = XMVectorSet(0.0f, 0.0f, -1.0f, Near);
    Planes[1] = XMVectorSet(0.0f, 0.0f, 1.0f, -Far);
    Planes[2] = XMVectorSet(1.0f, 0.0f, -RightSlope, 0.0f);
    Planes[3] = XMVectorSet(-1.0f, 0.0f, LeftSlope, 0.0f);
    Planes[4] = XMVectorSet(0.0f, 1.0f, -TopSlope, 0.0f);
    Planes[5] = XMVectorSet(0.0f, -1.0f, BottomSlope, 0.0f);

    // Load origin and orientation of the frustum.
    XMVECTOR vOrigin = XMLoadFloat3(&Origin);
    XMVECTOR FrustumOrientation = XMLoadFloat4(&Orientation);

    assert(DirectX::Internal::XMQuaternionIsUnit(FrustumOrientation));

    // Load the box.
    XMVECTOR Center = XMLoadFloat3(&box.Center);
    XMVECTOR Extents = XMLoadFloat3(&box.Extents);
    XMVECTOR BoxOrientation = XMLoadFloat4(&box.Orientation);

    assert(DirectX::Internal::XMQuaternionIsUnit(BoxOrientation));

    // Transform the oriented box into the space of the frustum in order to
    // minimize the number of transforms we have to do.
    Center = XMVector3InverseRotate(XMVectorSubtract(Center, vOrigin), FrustumOrientation);
    BoxOrientation = XMQuaternionMultiply(BoxOrientation, XMQuaternionConjugate(FrustumOrientation));

    // Set w of the center to one so we can dot4 with the plane.
    Center = XMVectorInsert<0, 0, 0, 0, 1>(Center, XMVectorSplatOne());

    // Build the 3x3 rotation matrix that defines the box axes.
    XMMATRIX R = XMMatrixRotationQuaternion(BoxOrientation);

    // Check against each plane of the frustum.
    XMVECTOR Outside = XMVectorFalseInt();
    XMVECTOR InsideAll = XMVectorTrueInt();
    XMVECTOR CenterInsideAll = XMVectorTrueInt();

    for (size_t i = 0; i < 6; ++i)
    {
        // Compute the distance to the center of the box.
        XMVECTOR Dist = XMVector4Dot(Center, Planes[i]);

        // Project the axes of the box onto the normal of the plane.  Half the
        // length of the projection (sometime called the "radius") is equal to
        // h(u) * abs(n dot b(u))) + h(v) * abs(n dot b(v)) + h(w) * abs(n dot b(w))
        // where h(i) are extents of the box, n is the plane normal, and b(i) are the
        // axes of the box.
        XMVECTOR Radius = XMVector3Dot(Planes[i], R.r[0]);
        Radius = XMVectorSelect(Radius, XMVector3Dot(Planes[i], R.r[1]), SelectY);
        Radius = XMVectorSelect(Radius, XMVector3Dot(Planes[i], R.r[2]), SelectZ);
        Radius = XMVector3Dot(Extents, XMVectorAbs(Radius));

        // Outside the plane?
        Outside = XMVectorOrInt(Outside, XMVectorGreater(Dist, Radius));

        // Fully inside the plane?
        InsideAll = XMVectorAndInt(InsideAll, XMVectorLessOrEqual(Dist, XMVectorNegate(Radius)));

        // Check if the center is inside the plane.
        CenterInsideAll = XMVectorAndInt(CenterInsideAll, XMVectorLessOrEqual(Dist, Zero));
    }

    // If the box is outside any of the planes it is outside.
    if (XMVector4EqualInt(Outside, XMVectorTrueInt()))
        return false;

    // If the box is inside all planes it is fully inside.
    if (XMVector4EqualInt(InsideAll, XMVectorTrueInt()))
        return true;

    // If the center of the box is inside all planes and the box intersects
    // one or more planes then it must intersect.
    if (XMVector4EqualInt(CenterInsideAll, XMVectorTrueInt()))
        return true;

    // Build the corners of the frustum.
    XMVECTOR vRightTop = XMVectorSet(RightSlope, TopSlope, 1.0f, 0.0f);
    XMVECTOR vRightBottom = XMVectorSet(RightSlope, BottomSlope, 1.0f, 0.0f);
    XMVECTOR vLeftTop = XMVectorSet(LeftSlope, TopSlope, 1.0f, 0.0f);
    XMVECTOR vLeftBottom = XMVectorSet(LeftSlope, BottomSlope, 1.0f, 0.0f);
    XMVECTOR vNear = XMVectorReplicatePtr(&Near);
    XMVECTOR vFar = XMVectorReplicatePtr(&Far);

    XMVECTOR Corners[CORNER_COUNT];
    Corners[0] = XMVectorMultiply(vRightTop, vNear);
    Corners[1] = XMVectorMultiply(vRightBottom, vNear);
    Corners[2] = XMVectorMultiply(vLeftTop, vNear);
    Corners[3] = XMVectorMultiply(vLeftBottom, vNear);
    Corners[4] = XMVectorMultiply(vRightTop, vFar);
    Corners[5] = XMVectorMultiply(vRightBottom, vFar);
    Corners[6] = XMVectorMultiply(vLeftTop, vFar);
    Corners[7] = XMVectorMultiply(vLeftBottom, vFar);

    // Test against box axes (3)
    {
        // Find the min/max values of the projection of the frustum onto each axis.
        XMVECTOR FrustumMin, FrustumMax;

        FrustumMin = XMVector3Dot(Corners[0], R.r[0]);
        FrustumMin = XMVectorSelect(FrustumMin, XMVector3Dot(Corners[0], R.r[1]), SelectY);
        FrustumMin = XMVectorSelect(FrustumMin, XMVector3Dot(Corners[0], R.r[2]), SelectZ);
        FrustumMax = FrustumMin;

        for (size_t i = 1; i < BoundingOrientedBox::CORNER_COUNT; ++i)
        {
            XMVECTOR Temp = XMVector3Dot(Corners[i], R.r[0]);
            Temp = XMVectorSelect(Temp, XMVector3Dot(Corners[i], R.r[1]), SelectY);
            Temp = XMVectorSelect(Temp, XMVector3Dot(Corners[i], R.r[2]), SelectZ);

            FrustumMin = XMVectorMin(FrustumMin, Temp);
            FrustumMax = XMVectorMax(FrustumMax, Temp);
        }

        // Project the center of the box onto the axes.
        XMVECTOR BoxDist = XMVector3Dot(Center, R.r[0]);
        BoxDist = XMVectorSelect(BoxDist, XMVector3Dot(Center, R.r[1]), SelectY);
        BoxDist = XMVectorSelect(BoxDist, XMVector3Dot(Center, R.r[2]), SelectZ);

        // The projection of the box onto the axis is just its Center and Extents.
        // if (min > box_max || max < box_min) reject;
        XMVECTOR Result = XMVectorOrInt(XMVectorGreater(FrustumMin, XMVectorAdd(BoxDist, Extents)),
            XMVectorLess(FrustumMax, XMVectorSubtract(BoxDist, Extents)));

        if (DirectX::Internal::XMVector3AnyTrue(Result))
            return false;
    }

    // Test against edge/edge axes (3*6).
    XMVECTOR FrustumEdgeAxis[6];

    FrustumEdgeAxis[0] = vRightTop;
    FrustumEdgeAxis[1] = vRightBottom;
    FrustumEdgeAxis[2] = vLeftTop;
    FrustumEdgeAxis[3] = vLeftBottom;
    FrustumEdgeAxis[4] = XMVectorSubtract(vRightTop, vLeftTop);
    FrustumEdgeAxis[5] = XMVectorSubtract(vLeftBottom, vLeftTop);

    for (size_t i = 0; i < 3; ++i)
    {
        for (size_t j = 0; j < 6; j++)
        {
            // Compute the axis we are going to test.
            XMVECTOR Axis = XMVector3Cross(R.r[i], FrustumEdgeAxis[j]);

            // Find the min/max values of the projection of the frustum onto the axis.
            XMVECTOR FrustumMin, FrustumMax;

            FrustumMin = FrustumMax = XMVector3Dot(Axis, Corners[0]);

            for (size_t k = 1; k < CORNER_COUNT; k++)
            {
                XMVECTOR Temp = XMVector3Dot(Axis, Corners[k]);
                FrustumMin = XMVectorMin(FrustumMin, Temp);
                FrustumMax = XMVectorMax(FrustumMax, Temp);
            }

            // Project the center of the box onto the axis.
            XMVECTOR Dist = XMVector3Dot(Center, Axis);

            // Project the axes of the box onto the axis to find the "radius" of the box.
            XMVECTOR Radius = XMVector3Dot(Axis, R.r[0]);
            Radius = XMVectorSelect(Radius, XMVector3Dot(Axis, R.r[1]), SelectY);
            Radius = XMVectorSelect(Radius, XMVector3Dot(Axis, R.r[2]), SelectZ);
            Radius = XMVector3Dot(Extents, XMVectorAbs(Radius));

            // if (center > max + radius || center < min - radius) reject;
            Outside = XMVectorOrInt(Outside, XMVectorGreater(Dist, XMVectorAdd(FrustumMax, Radius)));
            Outside = XMVectorOrInt(Outside, XMVectorLess(Dist, XMVectorSubtract(FrustumMin, Radius)));
        }
    }

    if (XMVector4EqualInt(Outside, XMVectorTrueInt()))
        return false;

    // If we did not find a separating plane then the box must intersect the frustum.
    return true;
}


//-----------------------------------------------------------------------------
// Exact frustum vs frustum test.
//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline bool CFrustumComponent::Intersects(const CFrustumComponent& fr) const noexcept
{
    // Load origin and orientation of frustum B.
    XMVECTOR OriginB = XMLoadFloat3(&Origin);
    XMVECTOR OrientationB = XMLoadFloat4(&Orientation);

    assert(DirectX::Internal::XMQuaternionIsUnit(OrientationB));

    // Build the planes of frustum B.
    XMVECTOR AxisB[6];
    AxisB[0] = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
    AxisB[1] = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    AxisB[2] = XMVectorSet(1.0f, 0.0f, -RightSlope, 0.0f);
    AxisB[3] = XMVectorSet(-1.0f, 0.0f, LeftSlope, 0.0f);
    AxisB[4] = XMVectorSet(0.0f, 1.0f, -TopSlope, 0.0f);
    AxisB[5] = XMVectorSet(0.0f, -1.0f, BottomSlope, 0.0f);

    XMVECTOR PlaneDistB[6];
    PlaneDistB[0] = XMVectorNegate(XMVectorReplicatePtr(&Near));
    PlaneDistB[1] = XMVectorReplicatePtr(&Far);
    PlaneDistB[2] = XMVectorZero();
    PlaneDistB[3] = XMVectorZero();
    PlaneDistB[4] = XMVectorZero();
    PlaneDistB[5] = XMVectorZero();

    // Load origin and orientation of frustum A.
    XMVECTOR OriginA = XMLoadFloat3(&fr.Origin);
    XMVECTOR OrientationA = XMLoadFloat4(&fr.Orientation);

    assert(DirectX::Internal::XMQuaternionIsUnit(OrientationA));

    // Transform frustum A into the space of the frustum B in order to
    // minimize the number of transforms we have to do.
    OriginA = XMVector3InverseRotate(XMVectorSubtract(OriginA, OriginB), OrientationB);
    OrientationA = XMQuaternionMultiply(OrientationA, XMQuaternionConjugate(OrientationB));

    // Build the corners of frustum A (in the local space of B).
    XMVECTOR RightTopA = XMVectorSet(fr.RightSlope, fr.TopSlope, 1.0f, 0.0f);
    XMVECTOR RightBottomA = XMVectorSet(fr.RightSlope, fr.BottomSlope, 1.0f, 0.0f);
    XMVECTOR LeftTopA = XMVectorSet(fr.LeftSlope, fr.TopSlope, 1.0f, 0.0f);
    XMVECTOR LeftBottomA = XMVectorSet(fr.LeftSlope, fr.BottomSlope, 1.0f, 0.0f);
    XMVECTOR NearA = XMVectorReplicatePtr(&fr.Near);
    XMVECTOR FarA = XMVectorReplicatePtr(&fr.Far);

    RightTopA = XMVector3Rotate(RightTopA, OrientationA);
    RightBottomA = XMVector3Rotate(RightBottomA, OrientationA);
    LeftTopA = XMVector3Rotate(LeftTopA, OrientationA);
    LeftBottomA = XMVector3Rotate(LeftBottomA, OrientationA);

    XMVECTOR CornersA[CORNER_COUNT];
    CornersA[0] = XMVectorMultiplyAdd(RightTopA, NearA, OriginA);
    CornersA[1] = XMVectorMultiplyAdd(RightBottomA, NearA, OriginA);
    CornersA[2] = XMVectorMultiplyAdd(LeftTopA, NearA, OriginA);
    CornersA[3] = XMVectorMultiplyAdd(LeftBottomA, NearA, OriginA);
    CornersA[4] = XMVectorMultiplyAdd(RightTopA, FarA, OriginA);
    CornersA[5] = XMVectorMultiplyAdd(RightBottomA, FarA, OriginA);
    CornersA[6] = XMVectorMultiplyAdd(LeftTopA, FarA, OriginA);
    CornersA[7] = XMVectorMultiplyAdd(LeftBottomA, FarA, OriginA);

    // Check frustum A against each plane of frustum B.
    XMVECTOR Outside = XMVectorFalseInt();
    XMVECTOR InsideAll = XMVectorTrueInt();

    for (size_t i = 0; i < 6; ++i)
    {
        // Find the min/max projection of the frustum onto the plane normal.
        XMVECTOR Min, Max;

        Min = Max = XMVector3Dot(AxisB[i], CornersA[0]);

        for (size_t j = 1; j < CORNER_COUNT; j++)
        {
            XMVECTOR Temp = XMVector3Dot(AxisB[i], CornersA[j]);
            Min = XMVectorMin(Min, Temp);
            Max = XMVectorMax(Max, Temp);
        }

        // Outside the plane?
        Outside = XMVectorOrInt(Outside, XMVectorGreater(Min, PlaneDistB[i]));

        // Fully inside the plane?
        InsideAll = XMVectorAndInt(InsideAll, XMVectorLessOrEqual(Max, PlaneDistB[i]));
    }

    // If the frustum A is outside any of the planes of frustum B it is outside.
    if (XMVector4EqualInt(Outside, XMVectorTrueInt()))
        return false;

    // If frustum A is inside all planes of frustum B it is fully inside.
    if (XMVector4EqualInt(InsideAll, XMVectorTrueInt()))
        return true;

    // Build the corners of frustum B.
    XMVECTOR RightTopB = XMVectorSet(RightSlope, TopSlope, 1.0f, 0.0f);
    XMVECTOR RightBottomB = XMVectorSet(RightSlope, BottomSlope, 1.0f, 0.0f);
    XMVECTOR LeftTopB = XMVectorSet(LeftSlope, TopSlope, 1.0f, 0.0f);
    XMVECTOR LeftBottomB = XMVectorSet(LeftSlope, BottomSlope, 1.0f, 0.0f);
    XMVECTOR NearB = XMVectorReplicatePtr(&Near);
    XMVECTOR FarB = XMVectorReplicatePtr(&Far);

    XMVECTOR CornersB[CFrustumComponent::CORNER_COUNT];
    CornersB[0] = XMVectorMultiply(RightTopB, NearB);
    CornersB[1] = XMVectorMultiply(RightBottomB, NearB);
    CornersB[2] = XMVectorMultiply(LeftTopB, NearB);
    CornersB[3] = XMVectorMultiply(LeftBottomB, NearB);
    CornersB[4] = XMVectorMultiply(RightTopB, FarB);
    CornersB[5] = XMVectorMultiply(RightBottomB, FarB);
    CornersB[6] = XMVectorMultiply(LeftTopB, FarB);
    CornersB[7] = XMVectorMultiply(LeftBottomB, FarB);

    // Build the planes of frustum A (in the local space of B).
    XMVECTOR AxisA[6];
    XMVECTOR PlaneDistA[6];

    AxisA[0] = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
    AxisA[1] = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    AxisA[2] = XMVectorSet(1.0f, 0.0f, -fr.RightSlope, 0.0f);
    AxisA[3] = XMVectorSet(-1.0f, 0.0f, fr.LeftSlope, 0.0f);
    AxisA[4] = XMVectorSet(0.0f, 1.0f, -fr.TopSlope, 0.0f);
    AxisA[5] = XMVectorSet(0.0f, -1.0f, fr.BottomSlope, 0.0f);

    AxisA[0] = XMVector3Rotate(AxisA[0], OrientationA);
    AxisA[1] = XMVectorNegate(AxisA[0]);
    AxisA[2] = XMVector3Rotate(AxisA[2], OrientationA);
    AxisA[3] = XMVector3Rotate(AxisA[3], OrientationA);
    AxisA[4] = XMVector3Rotate(AxisA[4], OrientationA);
    AxisA[5] = XMVector3Rotate(AxisA[5], OrientationA);

    PlaneDistA[0] = XMVector3Dot(AxisA[0], CornersA[0]);  // Re-use corner on near plane.
    PlaneDistA[1] = XMVector3Dot(AxisA[1], CornersA[4]);  // Re-use corner on far plane.
    PlaneDistA[2] = XMVector3Dot(AxisA[2], OriginA);
    PlaneDistA[3] = XMVector3Dot(AxisA[3], OriginA);
    PlaneDistA[4] = XMVector3Dot(AxisA[4], OriginA);
    PlaneDistA[5] = XMVector3Dot(AxisA[5], OriginA);

    // Check each axis of frustum A for a seperating plane (5).
    for (size_t i = 0; i < 6; ++i)
    {
        // Find the minimum projection of the frustum onto the plane normal.
        XMVECTOR Min;

        Min = XMVector3Dot(AxisA[i], CornersB[0]);

        for (size_t j = 1; j < CORNER_COUNT; j++)
        {
            XMVECTOR Temp = XMVector3Dot(AxisA[i], CornersB[j]);
            Min = XMVectorMin(Min, Temp);
        }

        // Outside the plane?
        Outside = XMVectorOrInt(Outside, XMVectorGreater(Min, PlaneDistA[i]));
    }

    // If the frustum B is outside any of the planes of frustum A it is outside.
    if (XMVector4EqualInt(Outside, XMVectorTrueInt()))
        return false;

    // Check edge/edge axes (6 * 6).
    XMVECTOR FrustumEdgeAxisA[6];
    FrustumEdgeAxisA[0] = RightTopA;
    FrustumEdgeAxisA[1] = RightBottomA;
    FrustumEdgeAxisA[2] = LeftTopA;
    FrustumEdgeAxisA[3] = LeftBottomA;
    FrustumEdgeAxisA[4] = XMVectorSubtract(RightTopA, LeftTopA);
    FrustumEdgeAxisA[5] = XMVectorSubtract(LeftBottomA, LeftTopA);

    XMVECTOR FrustumEdgeAxisB[6];
    FrustumEdgeAxisB[0] = RightTopB;
    FrustumEdgeAxisB[1] = RightBottomB;
    FrustumEdgeAxisB[2] = LeftTopB;
    FrustumEdgeAxisB[3] = LeftBottomB;
    FrustumEdgeAxisB[4] = XMVectorSubtract(RightTopB, LeftTopB);
    FrustumEdgeAxisB[5] = XMVectorSubtract(LeftBottomB, LeftTopB);

    for (size_t i = 0; i < 6; ++i)
    {
        for (size_t j = 0; j < 6; j++)
        {
            // Compute the axis we are going to test.
            XMVECTOR Axis = XMVector3Cross(FrustumEdgeAxisA[i], FrustumEdgeAxisB[j]);

            // Find the min/max values of the projection of both frustums onto the axis.
            XMVECTOR MinA, MaxA;
            XMVECTOR MinB, MaxB;

            MinA = MaxA = XMVector3Dot(Axis, CornersA[0]);
            MinB = MaxB = XMVector3Dot(Axis, CornersB[0]);

            for (size_t k = 1; k < CORNER_COUNT; k++)
            {
                XMVECTOR TempA = XMVector3Dot(Axis, CornersA[k]);
                MinA = XMVectorMin(MinA, TempA);
                MaxA = XMVectorMax(MaxA, TempA);

                XMVECTOR TempB = XMVector3Dot(Axis, CornersB[k]);
                MinB = XMVectorMin(MinB, TempB);
                MaxB = XMVectorMax(MaxB, TempB);
            }

            // if (MinA > MaxB || MinB > MaxA) reject
            Outside = XMVectorOrInt(Outside, XMVectorGreater(MinA, MaxB));
            Outside = XMVectorOrInt(Outside, XMVectorGreater(MinB, MaxA));
        }
    }

    // If there is a seperating plane, then the frustums do not intersect.
    if (XMVector4EqualInt(Outside, XMVectorTrueInt()))
        return false;

    // If we did not find a separating plane then the frustums intersect.
    return true;
}


//-----------------------------------------------------------------------------
// Triangle vs frustum test.
//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline bool XM_CALLCONV CFrustumComponent::Intersects(FXMVECTOR V0, FXMVECTOR V1, FXMVECTOR V2) const noexcept
{
    // Build the frustum planes (NOTE: D is negated from the usual).
    XMVECTOR Planes[6];
    Planes[0] = XMVectorSet(0.0f, 0.0f, -1.0f, -Near);
    Planes[1] = XMVectorSet(0.0f, 0.0f, 1.0f, Far);
    Planes[2] = XMVectorSet(1.0f, 0.0f, -RightSlope, 0.0f);
    Planes[3] = XMVectorSet(-1.0f, 0.0f, LeftSlope, 0.0f);
    Planes[4] = XMVectorSet(0.0f, 1.0f, -TopSlope, 0.0f);
    Planes[5] = XMVectorSet(0.0f, -1.0f, BottomSlope, 0.0f);

    // Load origin and orientation of the frustum.
    XMVECTOR vOrigin = XMLoadFloat3(&Origin);
    XMVECTOR vOrientation = XMLoadFloat4(&Orientation);

    assert(DirectX::Internal::XMQuaternionIsUnit(vOrientation));

    // Transform triangle into the local space of frustum.
    XMVECTOR TV0 = XMVector3InverseRotate(XMVectorSubtract(V0, vOrigin), vOrientation);
    XMVECTOR TV1 = XMVector3InverseRotate(XMVectorSubtract(V1, vOrigin), vOrientation);
    XMVECTOR TV2 = XMVector3InverseRotate(XMVectorSubtract(V2, vOrigin), vOrientation);

    // Test each vertex of the triangle against the frustum planes.
    XMVECTOR Outside = XMVectorFalseInt();
    XMVECTOR InsideAll = XMVectorTrueInt();

    for (size_t i = 0; i < 6; ++i)
    {
        XMVECTOR Dist0 = XMVector3Dot(TV0, Planes[i]);
        XMVECTOR Dist1 = XMVector3Dot(TV1, Planes[i]);
        XMVECTOR Dist2 = XMVector3Dot(TV2, Planes[i]);

        XMVECTOR MinDist = XMVectorMin(Dist0, Dist1);
        MinDist = XMVectorMin(MinDist, Dist2);
        XMVECTOR MaxDist = XMVectorMax(Dist0, Dist1);
        MaxDist = XMVectorMax(MaxDist, Dist2);

        XMVECTOR PlaneDist = XMVectorSplatW(Planes[i]);

        // Outside the plane?
        Outside = XMVectorOrInt(Outside, XMVectorGreater(MinDist, PlaneDist));

        // Fully inside the plane?
        InsideAll = XMVectorAndInt(InsideAll, XMVectorLessOrEqual(MaxDist, PlaneDist));
    }

    // If the triangle is outside any of the planes it is outside.
    if (XMVector4EqualInt(Outside, XMVectorTrueInt()))
        return false;

    // If the triangle is inside all planes it is fully inside.
    if (XMVector4EqualInt(InsideAll, XMVectorTrueInt()))
        return true;

    // Build the corners of the frustum.
    XMVECTOR vRightTop = XMVectorSet(RightSlope, TopSlope, 1.0f, 0.0f);
    XMVECTOR vRightBottom = XMVectorSet(RightSlope, BottomSlope, 1.0f, 0.0f);
    XMVECTOR vLeftTop = XMVectorSet(LeftSlope, TopSlope, 1.0f, 0.0f);
    XMVECTOR vLeftBottom = XMVectorSet(LeftSlope, BottomSlope, 1.0f, 0.0f);
    XMVECTOR vNear = XMVectorReplicatePtr(&Near);
    XMVECTOR vFar = XMVectorReplicatePtr(&Far);

    XMVECTOR Corners[CORNER_COUNT];
    Corners[0] = XMVectorMultiply(vRightTop, vNear);
    Corners[1] = XMVectorMultiply(vRightBottom, vNear);
    Corners[2] = XMVectorMultiply(vLeftTop, vNear);
    Corners[3] = XMVectorMultiply(vLeftBottom, vNear);
    Corners[4] = XMVectorMultiply(vRightTop, vFar);
    Corners[5] = XMVectorMultiply(vRightBottom, vFar);
    Corners[6] = XMVectorMultiply(vLeftTop, vFar);
    Corners[7] = XMVectorMultiply(vLeftBottom, vFar);

    // Test the plane of the triangle.
    XMVECTOR Normal = XMVector3Cross(XMVectorSubtract(V1, V0), XMVectorSubtract(V2, V0));
    XMVECTOR Dist = XMVector3Dot(Normal, V0);

    XMVECTOR MinDist, MaxDist;
    MinDist = MaxDist = XMVector3Dot(Corners[0], Normal);
    for (size_t i = 1; i < CORNER_COUNT; ++i)
    {
        XMVECTOR Temp = XMVector3Dot(Corners[i], Normal);
        MinDist = XMVectorMin(MinDist, Temp);
        MaxDist = XMVectorMax(MaxDist, Temp);
    }

    Outside = XMVectorOrInt(XMVectorGreater(MinDist, Dist), XMVectorLess(MaxDist, Dist));
    if (XMVector4EqualInt(Outside, XMVectorTrueInt()))
        return false;

    // Check the edge/edge axes (3*6).
    XMVECTOR TriangleEdgeAxis[3];
    TriangleEdgeAxis[0] = XMVectorSubtract(V1, V0);
    TriangleEdgeAxis[1] = XMVectorSubtract(V2, V1);
    TriangleEdgeAxis[2] = XMVectorSubtract(V0, V2);

    XMVECTOR FrustumEdgeAxis[6];
    FrustumEdgeAxis[0] = vRightTop;
    FrustumEdgeAxis[1] = vRightBottom;
    FrustumEdgeAxis[2] = vLeftTop;
    FrustumEdgeAxis[3] = vLeftBottom;
    FrustumEdgeAxis[4] = XMVectorSubtract(vRightTop, vLeftTop);
    FrustumEdgeAxis[5] = XMVectorSubtract(vLeftBottom, vLeftTop);

    for (size_t i = 0; i < 3; ++i)
    {
        for (size_t j = 0; j < 6; j++)
        {
            // Compute the axis we are going to test.
            XMVECTOR Axis = XMVector3Cross(TriangleEdgeAxis[i], FrustumEdgeAxis[j]);

            // Find the min/max of the projection of the triangle onto the axis.
            XMVECTOR MinA, MaxA;

            XMVECTOR Dist0 = XMVector3Dot(V0, Axis);
            XMVECTOR Dist1 = XMVector3Dot(V1, Axis);
            XMVECTOR Dist2 = XMVector3Dot(V2, Axis);

            MinA = XMVectorMin(Dist0, Dist1);
            MinA = XMVectorMin(MinA, Dist2);
            MaxA = XMVectorMax(Dist0, Dist1);
            MaxA = XMVectorMax(MaxA, Dist2);

            // Find the min/max of the projection of the frustum onto the axis.
            XMVECTOR MinB, MaxB;

            MinB = MaxB = XMVector3Dot(Axis, Corners[0]);

            for (size_t k = 1; k < CORNER_COUNT; k++)
            {
                XMVECTOR Temp = XMVector3Dot(Axis, Corners[k]);
                MinB = XMVectorMin(MinB, Temp);
                MaxB = XMVectorMax(MaxB, Temp);
            }

            // if (MinA > MaxB || MinB > MaxA) reject;
            Outside = XMVectorOrInt(Outside, XMVectorGreater(MinA, MaxB));
            Outside = XMVectorOrInt(Outside, XMVectorGreater(MinB, MaxA));
        }
    }

    if (XMVector4EqualInt(Outside, XMVectorTrueInt()))
        return false;

    // If we did not find a separating plane then the triangle must intersect the frustum.
    return true;
}


//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline PlaneIntersectionType XM_CALLCONV CFrustumComponent::Intersects(FXMVECTOR Plane) const noexcept
{
    assert(DirectX::Internal::XMPlaneIsUnit(Plane));

    // Load origin and orientation of the frustum.
    XMVECTOR vOrigin = XMLoadFloat3(&Origin);
    XMVECTOR vOrientation = XMLoadFloat4(&Orientation);

    assert(DirectX::Internal::XMQuaternionIsUnit(vOrientation));

    // Set w of the origin to one so we can dot4 with a plane.
    vOrigin = XMVectorInsert<0, 0, 0, 0, 1>(vOrigin, XMVectorSplatOne());

    // Build the corners of the frustum (in world space).
    XMVECTOR RightTop = XMVectorSet(RightSlope, TopSlope, 1.0f, 0.0f);
    XMVECTOR RightBottom = XMVectorSet(RightSlope, BottomSlope, 1.0f, 0.0f);
    XMVECTOR LeftTop = XMVectorSet(LeftSlope, TopSlope, 1.0f, 0.0f);
    XMVECTOR LeftBottom = XMVectorSet(LeftSlope, BottomSlope, 1.0f, 0.0f);
    XMVECTOR vNear = XMVectorReplicatePtr(&Near);
    XMVECTOR vFar = XMVectorReplicatePtr(&Far);

    RightTop = XMVector3Rotate(RightTop, vOrientation);
    RightBottom = XMVector3Rotate(RightBottom, vOrientation);
    LeftTop = XMVector3Rotate(LeftTop, vOrientation);
    LeftBottom = XMVector3Rotate(LeftBottom, vOrientation);

    XMVECTOR Corners0 = XMVectorMultiplyAdd(RightTop, vNear, vOrigin);
    XMVECTOR Corners1 = XMVectorMultiplyAdd(RightBottom, vNear, vOrigin);
    XMVECTOR Corners2 = XMVectorMultiplyAdd(LeftTop, vNear, vOrigin);
    XMVECTOR Corners3 = XMVectorMultiplyAdd(LeftBottom, vNear, vOrigin);
    XMVECTOR Corners4 = XMVectorMultiplyAdd(RightTop, vFar, vOrigin);
    XMVECTOR Corners5 = XMVectorMultiplyAdd(RightBottom, vFar, vOrigin);
    XMVECTOR Corners6 = XMVectorMultiplyAdd(LeftTop, vFar, vOrigin);
    XMVECTOR Corners7 = XMVectorMultiplyAdd(LeftBottom, vFar, vOrigin);

    XMVECTOR Outside, Inside;
    DirectX::Internal::FastIntersectFrustumPlane(Corners0, Corners1, Corners2, Corners3,
        Corners4, Corners5, Corners6, Corners7,
        Plane, Outside, Inside);

    // If the frustum is outside any plane it is outside.
    if (XMVector4EqualInt(Outside, XMVectorTrueInt()))
        return FRONT;

    // If the frustum is inside all planes it is inside.
    if (XMVector4EqualInt(Inside, XMVectorTrueInt()))
        return BACK;

    // The frustum is not inside all planes or outside a plane it intersects.
    return INTERSECTING;
}


//-----------------------------------------------------------------------------
// Ray vs. frustum test
//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline bool XM_CALLCONV CFrustumComponent::Intersects(FXMVECTOR rayOrigin, FXMVECTOR Direction, float& Dist) const noexcept
{
    // If ray starts inside the frustum, return a distance of 0 for the hit
    if (Contains(rayOrigin) == CONTAINS)
    {
        Dist = 0.0f;
        return true;
    }

    // Build the frustum planes.
    XMVECTOR Planes[6];
    Planes[0] = XMVectorSet(0.0f, 0.0f, -1.0f, Near);
    Planes[1] = XMVectorSet(0.0f, 0.0f, 1.0f, -Far);
    Planes[2] = XMVectorSet(1.0f, 0.0f, -RightSlope, 0.0f);
    Planes[3] = XMVectorSet(-1.0f, 0.0f, LeftSlope, 0.0f);
    Planes[4] = XMVectorSet(0.0f, 1.0f, -TopSlope, 0.0f);
    Planes[5] = XMVectorSet(0.0f, -1.0f, BottomSlope, 0.0f);

    // Load origin and orientation of the frustum.
    XMVECTOR frOrigin = XMLoadFloat3(&Origin);
    XMVECTOR frOrientation = XMLoadFloat4(&Orientation);

    // This algorithm based on "Fast Ray-Convex Polyhedron Intersectin," in James Arvo, ed., Graphics Gems II pp. 247-250
    float tnear = -FLT_MAX;
    float tfar = FLT_MAX;

    for (size_t i = 0; i < 6; ++i)
    {
        XMVECTOR Plane = DirectX::Internal::XMPlaneTransform(Planes[i], frOrientation, frOrigin);
        Plane = XMPlaneNormalize(Plane);

        XMVECTOR AxisDotOrigin = XMPlaneDotCoord(Plane, rayOrigin);
        XMVECTOR AxisDotDirection = XMVector3Dot(Plane, Direction);

        if (XMVector3LessOrEqual(XMVectorAbs(AxisDotDirection), g_RayEpsilon))
        {
            // Ray is parallel to plane - check if ray origin is inside plane's
            if (XMVector3Greater(AxisDotOrigin, g_XMZero))
            {
                // Ray origin is outside half-space.
                Dist = 0.f;
                return false;
            }
        }
        else
        {
            // Ray not parallel - get distance to plane.
            float vd = XMVectorGetX(AxisDotDirection);
            float vn = XMVectorGetX(AxisDotOrigin);
            float t = -vn / vd;
            if (vd < 0.0f)
            {
                // Front face - T is a near point.
                if (t > tfar)
                {
                    Dist = 0.f;
                    return false;
                }
                if (t > tnear)
                {
                    // Hit near face.
                    tnear = t;
                }
            }
            else
            {
                // back face - T is far point.
                if (t < tnear)
                {
                    Dist = 0.f;
                    return false;
                }
                if (t < tfar)
                {
                    // Hit far face.
                    tfar = t;
                }
            }
        }
    }

    // Survived all tests.
    // Note: if ray originates on polyhedron, may want to change 0.0f to some
    // epsilon to avoid intersecting the originating face.
    float distance = (tnear >= 0.0f) ? tnear : tfar;
    if (distance >= 0.0f)
    {
        Dist = distance;
        return true;
    }

    Dist = 0.f;
    return false;
}


//-----------------------------------------------------------------------------
// Test a frustum vs 6 planes (typically forming another frustum).
//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline ContainmentType XM_CALLCONV CFrustumComponent::ContainedBy(
    FXMVECTOR Plane0, FXMVECTOR Plane1, FXMVECTOR Plane2,
    GXMVECTOR Plane3,
    HXMVECTOR Plane4, HXMVECTOR Plane5) const noexcept
{
    // Load origin and orientation of the frustum.
    XMVECTOR vOrigin = XMLoadFloat3(&Origin);
    XMVECTOR vOrientation = XMLoadFloat4(&Orientation);

    assert(DirectX::Internal::XMQuaternionIsUnit(vOrientation));

    // Set w of the origin to one so we can dot4 with a plane.
    vOrigin = XMVectorInsert<0, 0, 0, 0, 1>(vOrigin, XMVectorSplatOne());

    // Build the corners of the frustum (in world space).
    XMVECTOR RightTop = XMVectorSet(RightSlope, TopSlope, 1.0f, 0.0f);
    XMVECTOR RightBottom = XMVectorSet(RightSlope, BottomSlope, 1.0f, 0.0f);
    XMVECTOR LeftTop = XMVectorSet(LeftSlope, TopSlope, 1.0f, 0.0f);
    XMVECTOR LeftBottom = XMVectorSet(LeftSlope, BottomSlope, 1.0f, 0.0f);
    XMVECTOR vNear = XMVectorReplicatePtr(&Near);
    XMVECTOR vFar = XMVectorReplicatePtr(&Far);

    RightTop = XMVector3Rotate(RightTop, vOrientation);
    RightBottom = XMVector3Rotate(RightBottom, vOrientation);
    LeftTop = XMVector3Rotate(LeftTop, vOrientation);
    LeftBottom = XMVector3Rotate(LeftBottom, vOrientation);

    XMVECTOR Corners0 = XMVectorMultiplyAdd(RightTop, vNear, vOrigin);
    XMVECTOR Corners1 = XMVectorMultiplyAdd(RightBottom, vNear, vOrigin);
    XMVECTOR Corners2 = XMVectorMultiplyAdd(LeftTop, vNear, vOrigin);
    XMVECTOR Corners3 = XMVectorMultiplyAdd(LeftBottom, vNear, vOrigin);
    XMVECTOR Corners4 = XMVectorMultiplyAdd(RightTop, vFar, vOrigin);
    XMVECTOR Corners5 = XMVectorMultiplyAdd(RightBottom, vFar, vOrigin);
    XMVECTOR Corners6 = XMVectorMultiplyAdd(LeftTop, vFar, vOrigin);
    XMVECTOR Corners7 = XMVectorMultiplyAdd(LeftBottom, vFar, vOrigin);

    XMVECTOR Outside, Inside;

    // Test against each plane.
    DirectX::Internal::FastIntersectFrustumPlane(Corners0, Corners1, Corners2, Corners3,
        Corners4, Corners5, Corners6, Corners7,
        Plane0, Outside, Inside);

    XMVECTOR AnyOutside = Outside;
    XMVECTOR AllInside = Inside;

    DirectX::Internal::FastIntersectFrustumPlane(Corners0, Corners1, Corners2, Corners3,
        Corners4, Corners5, Corners6, Corners7,
        Plane1, Outside, Inside);

    AnyOutside = XMVectorOrInt(AnyOutside, Outside);
    AllInside = XMVectorAndInt(AllInside, Inside);

    DirectX::Internal::FastIntersectFrustumPlane(Corners0, Corners1, Corners2, Corners3,
        Corners4, Corners5, Corners6, Corners7,
        Plane2, Outside, Inside);

    AnyOutside = XMVectorOrInt(AnyOutside, Outside);
    AllInside = XMVectorAndInt(AllInside, Inside);

    DirectX::Internal::FastIntersectFrustumPlane(Corners0, Corners1, Corners2, Corners3,
        Corners4, Corners5, Corners6, Corners7,
        Plane3, Outside, Inside);

    AnyOutside = XMVectorOrInt(AnyOutside, Outside);
    AllInside = XMVectorAndInt(AllInside, Inside);

    DirectX::Internal::FastIntersectFrustumPlane(Corners0, Corners1, Corners2, Corners3,
        Corners4, Corners5, Corners6, Corners7,
        Plane4, Outside, Inside);

    AnyOutside = XMVectorOrInt(AnyOutside, Outside);
    AllInside = XMVectorAndInt(AllInside, Inside);

    DirectX::Internal::FastIntersectFrustumPlane(Corners0, Corners1, Corners2, Corners3,
        Corners4, Corners5, Corners6, Corners7,
        Plane5, Outside, Inside);

    AnyOutside = XMVectorOrInt(AnyOutside, Outside);
    AllInside = XMVectorAndInt(AllInside, Inside);

    // If the frustum is outside any plane it is outside.
    if (XMVector4EqualInt(AnyOutside, XMVectorTrueInt()))
        return DISJOINT;

    // If the frustum is inside all planes it is inside.
    if (XMVector4EqualInt(AllInside, XMVectorTrueInt()))
        return CONTAINS;

    // The frustum is not inside all planes or outside a plane, it may intersect.
    return INTERSECTS;
}


//-----------------------------------------------------------------------------
// Build the 6 frustum planes from a frustum.
//
// The intended use for these routines is for fast culling to a view frustum.
// When the volume being tested against a view frustum is small relative to the
// view frustum it is usually either inside all six planes of the frustum
// (CONTAINS) or outside one of the planes of the frustum (DISJOINT). If neither
// of these cases is true then it may or may not be intersecting the frustum
// (INTERSECTS)
//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline void CFrustumComponent::GetPlanes(XMVECTOR* NearPlane, XMVECTOR* FarPlane, XMVECTOR* RightPlane,
    XMVECTOR* LeftPlane, XMVECTOR* TopPlane, XMVECTOR* BottomPlane) const noexcept
{
    // Load origin and orientation of the frustum.
    XMVECTOR vOrigin = XMLoadFloat3(&Origin);
    XMVECTOR vOrientation = XMLoadFloat4(&Orientation);

    if (NearPlane)
    {
        XMVECTOR vNearPlane = XMVectorSet(0.0f, 0.0f, -1.0f, Near);
        vNearPlane = DirectX::Internal::XMPlaneTransform(vNearPlane, vOrientation, vOrigin);
        *NearPlane = XMPlaneNormalize(vNearPlane);
    }

    if (FarPlane)
    {
        XMVECTOR vFarPlane = XMVectorSet(0.0f, 0.0f, 1.0f, -Far);
        vFarPlane = DirectX::Internal::XMPlaneTransform(vFarPlane, vOrientation, vOrigin);
        *FarPlane = XMPlaneNormalize(vFarPlane);
    }

    if (RightPlane)
    {
        XMVECTOR vRightPlane = XMVectorSet(1.0f, 0.0f, -RightSlope, 0.0f);
        vRightPlane = DirectX::Internal::XMPlaneTransform(vRightPlane, vOrientation, vOrigin);
        *RightPlane = XMPlaneNormalize(vRightPlane);
    }

    if (LeftPlane)
    {
        XMVECTOR vLeftPlane = XMVectorSet(-1.0f, 0.0f, LeftSlope, 0.0f);
        vLeftPlane = DirectX::Internal::XMPlaneTransform(vLeftPlane, vOrientation, vOrigin);
        *LeftPlane = XMPlaneNormalize(vLeftPlane);
    }

    if (TopPlane)
    {
        XMVECTOR vTopPlane = XMVectorSet(0.0f, 1.0f, -TopSlope, 0.0f);
        vTopPlane = DirectX::Internal::XMPlaneTransform(vTopPlane, vOrientation, vOrigin);
        *TopPlane = XMPlaneNormalize(vTopPlane);
    }

    if (BottomPlane)
    {
        XMVECTOR vBottomPlane = XMVectorSet(0.0f, -1.0f, BottomSlope, 0.0f);
        vBottomPlane = DirectX::Internal::XMPlaneTransform(vBottomPlane, vOrientation, vOrigin);
        *BottomPlane = XMPlaneNormalize(vBottomPlane);
    }
}


//-----------------------------------------------------------------------------
// Build a frustum from a persepective projection matrix.  The matrix may only
// contain a projection; any rotation, translation or scale will cause the
// constructed frustum to be incorrect.
//-----------------------------------------------------------------------------
_Use_decl_annotations_
inline void XM_CALLCONV CFrustumComponent::CreateFrustumFromMatrix(CFrustumComponent& Out, FXMMATRIX Projection, bool rhcoords) noexcept
{
    // Corners of the projection frustum in homogenous space.
    static XMVECTORF32 HomogenousPoints[6] =
    {
        { { {  1.0f,  0.0f, 1.0f, 1.0f } } },   // right (at far plane)
        { { { -1.0f,  0.0f, 1.0f, 1.0f } } },   // left
        { { {  0.0f,  1.0f, 1.0f, 1.0f } } },   // top
        { { {  0.0f, -1.0f, 1.0f, 1.0f } } },   // bottom

        { { { 0.0f, 0.0f, 0.0f, 1.0f } } },     // near
        { { { 0.0f, 0.0f, 1.0f, 1.0f } } }      // far
    };

    XMVECTOR Determinant;
    XMMATRIX matInverse = XMMatrixInverse(&Determinant, Projection);

    // Compute the frustum corners in world space.
    XMVECTOR Points[6];

    for (size_t i = 0; i < 6; ++i)
    {
        // Transform point.
        Points[i] = XMVector4Transform(HomogenousPoints[i], matInverse);
    }

    Out.Origin = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Out.Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

    // Compute the slopes.
    Points[0] = XMVectorMultiply(Points[0], XMVectorReciprocal(XMVectorSplatZ(Points[0])));
    Points[1] = XMVectorMultiply(Points[1], XMVectorReciprocal(XMVectorSplatZ(Points[1])));
    Points[2] = XMVectorMultiply(Points[2], XMVectorReciprocal(XMVectorSplatZ(Points[2])));
    Points[3] = XMVectorMultiply(Points[3], XMVectorReciprocal(XMVectorSplatZ(Points[3])));

    Out.RightSlope = XMVectorGetX(Points[0]);
    Out.LeftSlope = XMVectorGetX(Points[1]);
    Out.TopSlope = XMVectorGetY(Points[2]);
    Out.BottomSlope = XMVectorGetY(Points[3]);

    // Compute near and far.
    Points[4] = XMVectorMultiply(Points[4], XMVectorReciprocal(XMVectorSplatW(Points[4])));
    Points[5] = XMVectorMultiply(Points[5], XMVectorReciprocal(XMVectorSplatW(Points[5])));

    if (rhcoords)
    {
        Out.Near = XMVectorGetZ(Points[5]);
        Out.Far = XMVectorGetZ(Points[4]);
    }
    else
    {
        Out.Near = XMVectorGetZ(Points[4]);
        Out.Far = XMVectorGetZ(Points[5]);
    }
}
#pragma once

#include <d3d11_1.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include "GameEntity.h"
#include "DXResourcesConfig.h"
#include "EntityPhysicalMesh.h"
#include <DirectXCollision.h>
#include <DirectXCollision.inl>
#include <vector>
#include "SceneGraph.h"

class CDXResourcesConfig;

struct SConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

struct SCollisionBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	BOOL mDoesFrustumContain;
	BOOL mPadding[15];
};

struct SArrowConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	int mIsHovered;
};

struct SCollisionComponent
{
	BoundingBox AABox;
	ContainmentType CollisionType;
};

struct SColorVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class CGameEntity3DComponent;

class CGameEntity3D : public CGameEntity
{
public:
	CGameEntity3D()
		: m_ConstantBuffer{ }
		, m_LocationMatrix{ XMMatrixIdentity() }
		, m_RotationMatrix{ XMMatrixIdentity() }
		, m_QuatRotationMatrix{ XMMatrixIdentity() }
		, m_ScaleMatrix{ XMMatrixIdentity() }
		, m_Collision{ }
		, m_DXResConfig{ }
		, m_LocationX{ 0.0f }
		, m_SceneGraph{ }
	{ }

	XMMATRIX TransformationMatrix();

	void SetLocation(XMMATRIX Location);

	void SetLocationF(float InX, float InY, float InZ);

	XMMATRIX GetLocation();

	float GetLocationX();

	float GetLocationY();

	float GetLocationZ();

	XMVECTOR GetLocationVector();

	XMMATRIX GetRotation();

	float GetRotationX();

	float GetRotationY();

	float GetRotationZ();

	XMMATRIX GetScale();

	void SetRotation(float InPitch, float InYaw, float InRoll);

	void SetScale(float InX, float InY, float InZ);

	void SetUID(std::string uid);

	std::string GetUID();

	SCollisionComponent GetCollisionComponent();

	CDXResourcesConfig m_DXResConfig{ };

	std::string m_UID;

	TEntityPhysicalMesh PhysicalMesh;

	CSceneGraph<CGameEntity3DComponent> m_SceneGraph;

	SConstantBuffer GetConstantBuffer();

	XMMATRIX m_QuatRotationMatrix;
	SCollisionBuffer m_CollisionBuffer;

protected:
	//SceneGraph<CGameEntity3DComponent*> m_SceneGraph;

	SCollisionComponent m_Collision;

	XMVECTOR m_LocationVector;

	XMMATRIX m_LocationMatrix;
	float m_LocationX;
	float m_LocationY;
	float m_LocationZ;

	XMMATRIX m_RotationMatrix;
	float m_RotationX;
	float m_RotationY;
	float m_RotationZ;

	XMMATRIX m_ScaleMatrix;
	float m_ScaleX;
	float m_ScaleY;
	float m_ScaleZ;

	SConstantBuffer m_ConstantBuffer;
};


class CGameEntity3DComponent : public CGameEntity3D
{
public:

	uint16_t DXResourceHashKey;
};

class CFrustumComponent : public CGameEntity3DComponent
{
public:
	static constexpr size_t CORNER_COUNT = 8;

	XMFLOAT3 Origin;            // Origin of the frustum (and projection).
	XMFLOAT4 Orientation;       // Quaternion representing rotation.

	float RightSlope;           // Positive X (X/Z)
	float LeftSlope;            // Negative X
	float TopSlope;             // Positive Y (Y/Z)
	float BottomSlope;          // Negative Y
	float Near, Far;            // Z of the near plane and far plane.

	// Creators
	CFrustumComponent() noexcept :
		Origin(0, 0, 0), Orientation(0, 0, 0, 1.f), RightSlope(1.f), LeftSlope(-1.f),
		TopSlope(1.f), BottomSlope(-1.f), Near(0), Far(1.f) {}

	CFrustumComponent(const CFrustumComponent&) = default;
	CFrustumComponent& operator=(const CFrustumComponent&) = default;

	CFrustumComponent(CFrustumComponent&&) = default;
	CFrustumComponent& operator=(CFrustumComponent&&) = default;

	//constexpr FrustumComponent(_In_ const XMFLOAT3& origin, _In_ const XMFLOAT4& orientation,
	//	_In_ float rightSlope, _In_ float leftSlope, _In_ float topSlope, _In_ float bottomSlope,
	//	_In_ float nearPlane, _In_ float farPlane) noexcept
	//	: Origin(origin), Orientation(orientation),
	//	RightSlope(rightSlope), LeftSlope(leftSlope), TopSlope(topSlope), BottomSlope(bottomSlope),
	//	Near(nearPlane), Far(farPlane) {}
	CFrustumComponent(_In_ CXMMATRIX Projection, bool rhcoords = false) noexcept;

	// Methods
	void    XM_CALLCONV     Transform(_Out_ CFrustumComponent& Out, _In_ FXMMATRIX M) const noexcept;
	void    XM_CALLCONV     Transform(_Out_ CFrustumComponent& Out, _In_ float Scale, _In_ FXMVECTOR Rotation, _In_ FXMVECTOR Translation) const noexcept;

	void GetFrustumCorners(XMFLOAT3* Corners);
	// Gets the 8 corners of the frustum

	ContainmentType    XM_CALLCONV     Contains(_In_ FXMVECTOR Point) const noexcept;
	ContainmentType    XM_CALLCONV     Contains(_In_ FXMVECTOR V0, _In_ FXMVECTOR V1, _In_ FXMVECTOR V2) const noexcept;
	ContainmentType Contains(_In_ const BoundingSphere& sp) const noexcept;
	ContainmentType Contains(_In_ const BoundingBox& box) const noexcept;
	ContainmentType Contains(_In_ const BoundingOrientedBox& box) const noexcept;
	ContainmentType Contains(_In_ const CFrustumComponent& fr) const noexcept;
	// Frustum-Frustum test

	bool Intersects(_In_ const BoundingSphere& sh) const noexcept;
	bool Intersects(_In_ const BoundingBox& box) const noexcept;
	bool Intersects(_In_ const BoundingOrientedBox& box) const noexcept;
	bool Intersects(_In_ const CFrustumComponent& fr) const noexcept;

	bool    XM_CALLCONV     Intersects(_In_ FXMVECTOR V0, _In_ FXMVECTOR V1, _In_ FXMVECTOR V2) const noexcept;
	// Triangle-Frustum test

	PlaneIntersectionType    XM_CALLCONV     Intersects(_In_ FXMVECTOR Plane) const noexcept;
	// Plane-Frustum test

	bool    XM_CALLCONV     Intersects(_In_ FXMVECTOR rayOrigin, _In_ FXMVECTOR Direction, _Out_ float& Dist) const noexcept;
	// Ray-Frustum test

	ContainmentType     XM_CALLCONV     ContainedBy(_In_ FXMVECTOR Plane0, _In_ FXMVECTOR Plane1, _In_ FXMVECTOR Plane2,
		_In_ GXMVECTOR Plane3, _In_ HXMVECTOR Plane4, _In_ HXMVECTOR Plane5) const noexcept;
	// Test frustum against six planes (see BoundingFrustum::GetPlanes)

	void GetPlanes(_Out_opt_ XMVECTOR* NearPlane, _Out_opt_ XMVECTOR* FarPlane, _Out_opt_ XMVECTOR* RightPlane,
		_Out_opt_ XMVECTOR* LeftPlane, _Out_opt_ XMVECTOR* TopPlane, _Out_opt_ XMVECTOR* BottomPlane) const noexcept;
	// Create 6 Planes representation of Frustum

	// Static methods
	static void     XM_CALLCONV     CreateFrustumFromMatrix(_Out_ CFrustumComponent& Out, _In_ FXMMATRIX Projection, bool rhcoords = false) noexcept;
};
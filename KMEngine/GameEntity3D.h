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

//struct Simple_Vertex
//{
//	XMFLOAT3 Pos;
//	XMFLOAT2 Tex;
//};
//
//struct Simple_Color_Vertex
//{
//	XMFLOAT3 Pos;
//	XMFLOAT4 Col;
//};

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
		, m_VerticesList{ 0 }
		, m_LocationMatrix{ XMMatrixIdentity() }
		, m_RotationMatrix{ XMMatrixIdentity() }
		, m_ScaleMatrix{ XMMatrixIdentity() }
		, m_Collision{ }
		, m_DXResConfig{ }
		, m_LocationX{ 0.0f }
		, m_SceneGraph{ }
	{ }

	XMMATRIX TransformationMatrix();

	void SetSimpleColorVerticesList(std::vector<SSimpleColorVertex> VerticesList);

	/**
	* Sets a vertex list
	*/

	void SetColorVerticesList(std::vector<SColorVertex> VerticesList);

	std::vector<SSimpleVertex> GetVerticesList();

	std::vector<SSimpleColorVertex> GetSimpleColorVerticesList();

	std::vector<SColorVertex> GetColorVerticesList();

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

	DXGI_FORMAT GetOwnFormat();

	int GetOwnOffset();

	SConstantBuffer GetConstantBuffer();

	SArrowConstantBuffer GetArrowConstantBuffer();

	void SetCollisionBoxCenter(XMFLOAT3 NewCenter);

	void SetUID(std::string uid);

	std::string GetUID();

	SCollisionComponent GetCollisionComponent();

	CDXResourcesConfig m_DXResConfig{ };

	std::string m_UID;

	// Test

	void SetUIDTest(int uid);

	int GetUIDTest();

	void SetCollisionParams(XMFLOAT3 Center, XMFLOAT3 Extents, ContainmentType CollisionType);

	TEntityPhysicalMesh PhysicalMesh;

	std::vector<TEntityPhysicalMesh> m_PhysicalMeshVector;

	std::vector<CGameEntity3DComponent> m_GameEntity3DComponent;

	SceneGraph<CGameEntity3DComponent> m_SceneGraph;
	//CollisionComponent m_Collision;

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

	std::vector<SSimpleVertex> m_VerticesList;
	std::vector<SColorVertex> m_ColorVerticesList;
	std::vector<SSimpleColorVertex> m_SimpleColorVerticesList;
	SConstantBuffer m_ConstantBuffer;
	SArrowConstantBuffer m_ArrowConstantBuffer;
	int m_UIDTest{ };

	ID3D11Buffer* m_IndexBuffer;
	ID3D11Buffer* m_VertexBuffer;
	DXGI_FORMAT m_Format;
	int m_Offset;

	std::uint16_t m_MaterialHashIndex;
};


class CGameEntity3DComponent : public CGameEntity3D
{
public:

	uint16_t DXResourceHashKey;
};
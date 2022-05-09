#pragma once

#include "GameEntity.h"
#include <DirectXCollision.h>
#include <DirectXCollision.inl>
#include <vector>

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

struct CollisionComponent
{
	BoundingBox AABox;
	ContainmentType CollisionType;
};

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
};

struct SimpleColorVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Col;
};

struct ColorVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class GameEntity3D : public GameEntity
{
public:
	GameEntity3D() :
		m_ConstantBuffer{ },
		m_VerticesList{ 0 },
		m_LocationMatrix{ XMMatrixIdentity() },
		m_RotationMatrix{ XMMatrixIdentity() },
		m_ScaleMatrix{ XMMatrixIdentity() },
		m_Collision{ }
	{ }

	void SetVerticesList(std::vector<SimpleVertex> VerticesList) 
	{
		m_VerticesList = VerticesList; 
	}

	void SetSimpleColorVerticesList(std::vector<SimpleColorVertex> VerticesList)
	{
		m_SimpleColorVerticesList = VerticesList;
	}

	/**
	* Sets a vertex list
	*/

	void SetColorVerticesList(std::vector<ColorVertex> VerticesList)
	{
		m_ColorVerticesList = VerticesList;
	}

	std::vector<SimpleVertex> GetVerticesList() 
	{
		return m_VerticesList; 
	}

	std::vector<SimpleColorVertex> GetSimpleColorVerticesList()
	{
		return m_SimpleColorVerticesList;
	}

	std::vector<ColorVertex> GetColorVerticesList()
	{
		return m_ColorVerticesList;
	}

	void SetLocation(XMMATRIX Location)
	{
		m_ConstantBuffer.mWorld = Location;
	}

	void SetLocationF(float InX, float InY, float InZ)
	{
		m_LocationVector = { InX, InY, InZ };
		m_LocationMatrix = XMMatrixTranslation(InX, InY, InZ);
		m_ConstantBuffer.mWorld = TransformationMatrix();
	}

	XMMATRIX GetLocation()
	{
		return m_LocationMatrix;
	}

	XMVECTOR GetLocationVector()
	{
		return m_LocationVector;
	}

	XMMATRIX GetRotation()
	{
		return m_RotationMatrix;
	}

	XMMATRIX GetScale()
	{
		return m_ScaleMatrix;
	}

	void SetRotation(float InPitch, float InYaw, float InRoll)
	{
		m_RotationMatrix = XMMatrixRotationRollPitchYaw(InPitch, InYaw, InRoll);
		m_ConstantBuffer.mWorld = TransformationMatrix();
	}

	void SetScale(float InX, float InY, float InZ)
	{
		m_ScaleMatrix = XMMatrixScaling(InX, InY, InZ);
		m_ConstantBuffer.mWorld = TransformationMatrix();
	}

	ConstantBuffer GetConstantBuffer()
	{
		return m_ConstantBuffer;
	}

	void SetCollisionBoxCenter(XMFLOAT3 NewCenter)
	{
		m_Collision.AABox.Center = NewCenter;
		m_Collision.AABox.Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);
		m_Collision.CollisionType = DISJOINT;
	}

	CollisionComponent GetCollisionComponent()
	{
		return m_Collision;
	}

protected:
	CollisionComponent m_Collision;
	XMVECTOR m_LocationVector;
	XMMATRIX m_LocationMatrix;
	XMMATRIX m_RotationMatrix;
	XMMATRIX m_ScaleMatrix;
	std::vector<SimpleVertex> m_VerticesList;
	std::vector<ColorVertex> m_ColorVerticesList;
	std::vector<SimpleColorVertex> m_SimpleColorVerticesList;
	ConstantBuffer m_ConstantBuffer;

	XMMATRIX TransformationMatrix()
	{
		return m_ScaleMatrix * m_RotationMatrix * m_LocationMatrix;
	}
};


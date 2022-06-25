#include "GameEntity3D.h"

XMMATRIX GameEntity3D::TransformationMatrix()
{
	return m_ScaleMatrix * m_RotationMatrix * m_LocationMatrix;
}

void  GameEntity3D::SetVerticesList(std::vector<SimpleVertex> VerticesList)
{
	m_VerticesList = VerticesList;
}

void  GameEntity3D::SetSimpleColorVerticesList(std::vector<SimpleColorVertex> VerticesList)
{
	m_SimpleColorVerticesList = VerticesList;
}

/**
* Sets a vertex list
*/

void  GameEntity3D::SetColorVerticesList(std::vector<ColorVertex> VerticesList)
{
	m_ColorVerticesList = VerticesList;
}

void GameEntity3D::SetVertexbuffer(ID3D11Buffer* vb)
{
	m_DXResConfig.SetVertexBuffer(vb);
}

std::vector<SimpleVertex> GameEntity3D::GetVerticesList()
{
	return m_VerticesList;
}

std::vector<SimpleColorVertex> GameEntity3D::GetSimpleColorVerticesList()
{
	return m_SimpleColorVerticesList;
}

std::vector<ColorVertex> GameEntity3D::GetColorVerticesList()
{
	return m_ColorVerticesList;
}

void GameEntity3D::SetLocation(XMMATRIX Location)
{
	m_ConstantBuffer.mWorld = Location;
}

void GameEntity3D::SetLocationF(float InX, float InY, float InZ)
{
	m_LocationVector = { InX, InY, InZ };
	m_LocationMatrix = XMMatrixTranslation(InX, InY, InZ);
	m_ConstantBuffer.mWorld = TransformationMatrix();
}

XMMATRIX GameEntity3D::GetLocation()
{
	return m_LocationMatrix;
}

XMVECTOR GameEntity3D::GetLocationVector()
{
	return m_LocationVector;
}

XMMATRIX GameEntity3D::GetRotation()
{
	return m_RotationMatrix;
}

XMMATRIX GameEntity3D::GetScale()
{
	return m_ScaleMatrix;
}

void GameEntity3D::SetRotation(float InPitch, float InYaw, float InRoll)
{
	m_RotationMatrix = XMMatrixRotationRollPitchYaw(InPitch, InYaw, InRoll);
	m_ConstantBuffer.mWorld = TransformationMatrix();
}

void GameEntity3D::SetScale(float InX, float InY, float InZ)
{
	m_ScaleMatrix = XMMatrixScaling(InX, InY, InZ);
	m_ConstantBuffer.mWorld = TransformationMatrix();
}

ConstantBuffer GameEntity3D::GetConstantBuffer()
{
	return m_ConstantBuffer;
}

void GameEntity3D::SetCollisionBoxCenter(XMFLOAT3 NewCenter)
{
	m_Collision.AABox.Center = NewCenter;
	m_Collision.AABox.Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);
	m_Collision.CollisionType = DISJOINT;
}

CollisionComponent GameEntity3D::GetCollisionComponent()
{
	return m_Collision;
}
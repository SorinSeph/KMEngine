#include "GameEntity3D.h"

XMMATRIX GameEntity3D::TransformationMatrix()
{
	return m_ScaleMatrix * m_RotationMatrix * m_LocationMatrix;
}

void GameEntity3D::SetVerticesList(std::vector<SimpleVertex> VerticesList)
{
	m_VerticesList = VerticesList;
}

void GameEntity3D::SetSimpleColorVerticesList(std::vector<SimpleColorVertex> VerticesList)
{
	m_SimpleColorVerticesList = VerticesList;
}

/**
* Sets a vertex list
*/

void GameEntity3D::SetColorVerticesList(std::vector<ColorVertex> VerticesList)
{
	m_ColorVerticesList = VerticesList;
}

void GameEntity3D::SetVertexBuffer(ID3D11Buffer* vb)
{
	m_DXResConfig.SetVertexBuffer(vb);
}

void GameEntity3D::SetIndexBuffer(ID3D11Buffer* ib, DXGI_FORMAT Format, int Offset)
{
	m_DXResConfig.SetIndexBuffer(ib, Format, Offset);
}

void GameEntity3D::SetInputLayout(ID3D11InputLayout* InputLayout)
{
	m_DXResConfig.SetInputLayout(InputLayout);
}

void GameEntity3D::SetVertexShader(ID3D11VertexShader* VS)
{
	m_DXResConfig.SetVertexShader(VS);
}

void GameEntity3D::SetPixelShader(ID3D11PixelShader* PS)
{
	m_DXResConfig.SetPixelShader(PS);
}

void GameEntity3D::SetConstantBuffer(ID3D11Buffer* cb)
{
	m_DXResConfig.SetConstantBuffer(cb);
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

float GameEntity3D::GetLocationX()
{
	return m_LocationX;
}

float GameEntity3D::GetLocationY()
{
	return m_LocationY;
}

float GameEntity3D::GetLocationZ()
{
	return m_LocationZ;
}

float GameEntity3D::GetRotationX()
{
	return m_RotationX;
}

float GameEntity3D::GetRotationY()
{
	return m_RotationY;
}

float GameEntity3D::GetRotationZ()
{
	return m_RotationZ;
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

ArrowConstantBuffer GameEntity3D::GetArrowConstantBuffer()
{
	return m_ArrowConstantBuffer;
}

void GameEntity3D::SetCollisionBoxCenter(XMFLOAT3 NewCenter)
{
	m_Collision.AABox.Center = NewCenter;
	m_Collision.AABox.Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);
	m_Collision.CollisionType = DISJOINT;
}

void GameEntity3D::SetUID(std::string uid)
{
	m_UID = uid;
}

std::string GameEntity3D::GetUID()
{
	return m_UID;
}

CollisionComponent GameEntity3D::GetCollisionComponent()
{
	return m_Collision;
}

// Test

void GameEntity3D::SetUIDTest(int uid)
{
	m_UIDTest = uid;
}

int GameEntity3D::GetUIDTest()
{
	return m_UIDTest;
}

void GameEntity3D::SetCollisionParams(XMFLOAT3 Center, XMFLOAT3 Extents, ContainmentType CollisionType)
{
	m_Collision.AABox.Center = Center;
	m_Collision.AABox.Extents = Extents;
	m_Collision.CollisionType = CollisionType;
}
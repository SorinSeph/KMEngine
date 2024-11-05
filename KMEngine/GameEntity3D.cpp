#include "GameEntity3D.h"

XMMATRIX CGameEntity3D::TransformationMatrix()
{
	return m_ScaleMatrix * m_RotationMatrix * m_LocationMatrix;
}

void CGameEntity3D::SetVerticesList(std::vector<SSimpleVertex> VerticesList)
{
	m_VerticesList = VerticesList;
}

void CGameEntity3D::SetSimpleColorVerticesList(std::vector<SSimpleColorVertex> VerticesList)
{
	m_SimpleColorVerticesList = VerticesList;
}

/**
* Sets a vertex list
*/

void CGameEntity3D::SetColorVerticesList(std::vector<SColorVertex> VerticesList)
{
	m_ColorVerticesList = VerticesList;
}

void CGameEntity3D::SetVertexBuffer(ID3D11Buffer* vb)
{
	m_DXResConfig.SetVertexBuffer(vb);
}

void CGameEntity3D::SetIndexBuffer(ID3D11Buffer* ib, DXGI_FORMAT Format, int Offset)
{
	m_DXResConfig.SetIndexBuffer(ib, Format, Offset);
}

void CGameEntity3D::SetInputLayout(ID3D11InputLayout* InputLayout)
{
	m_DXResConfig.SetInputLayout(InputLayout);
}

void CGameEntity3D::SetVertexShader(ID3D11VertexShader* VS)
{
	m_DXResConfig.SetVertexShader(VS);
}

void CGameEntity3D::SetPixelShader(ID3D11PixelShader* PS)
{
	m_DXResConfig.SetPixelShader(PS);
}

void CGameEntity3D::SetConstantBuffer(ID3D11Buffer* cb)
{
	m_DXResConfig.SetConstantBuffer(cb);
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

void CGameEntity3D::SetOwnVertexBuffer(ID3D11Buffer* InVertexBuffer)
{
	m_VertexBuffer = InVertexBuffer;
}

void CGameEntity3D::SetIndexOwnBuffer(ID3D11Buffer* InIndexBuffer, DXGI_FORMAT InFormat, int InOffset)
{
	m_IndexBuffer = InIndexBuffer;
	m_Format = InFormat;
	m_Offset = InOffset;
}

ID3D11Buffer* CGameEntity3D::GetOwnVertexBuffer()
{
	return m_VertexBuffer;
}

ID3D11Buffer* CGameEntity3D::GetOwnIndexBuffer()
{
	return m_IndexBuffer;
}

DXGI_FORMAT CGameEntity3D::GetOwnFormat()
{
	return m_Format;
}

int CGameEntity3D::GetOwnOffset()
{
	return m_Offset;
}
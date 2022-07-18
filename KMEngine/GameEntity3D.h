#pragma once

#include <d3d11_1.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include "GameEntity.h"
#include "DXResourcesConfig.h"
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
	GameEntity3D() 
		:	m_ConstantBuffer{ }
		,	m_VerticesList{ 0 }
		,	m_LocationMatrix{ XMMatrixIdentity() }
		,	m_RotationMatrix{ XMMatrixIdentity() }
		,	m_ScaleMatrix{ XMMatrixIdentity() }
		,	m_Collision{ }
		,	m_DXResConfig{ }
	{ }

	XMMATRIX TransformationMatrix();

	void SetVertexbuffer(ID3D11Buffer* vb);

	void SetIndexbuffer(ID3D11Buffer* ib, DXGI_FORMAT Format, int Offset);

	void SetVerticesList(std::vector<SimpleVertex> VerticesList);

	void SetInputLayout(ID3D11InputLayout* InputLayout);

	void SetVertexShader(ID3D11VertexShader* VS);

	void SetPixelShader(ID3D11PixelShader* VS);

	void SetConstantBuffer(ID3D11Buffer* cb);

	void SetSimpleColorVerticesList(std::vector<SimpleColorVertex> VerticesList);

	/**
	* Sets a vertex list
	*/

	void SetColorVerticesList(std::vector<ColorVertex> VerticesList);

	std::vector<SimpleVertex> GetVerticesList();

	std::vector<SimpleColorVertex> GetSimpleColorVerticesList();

	std::vector<ColorVertex> GetColorVerticesList();

	void SetLocation(XMMATRIX Location);

	void SetLocationF(float InX, float InY, float InZ);

	XMMATRIX GetLocation();

	XMVECTOR GetLocationVector();

	XMMATRIX GetRotation();

	XMMATRIX GetScale();

	void SetRotation(float InPitch, float InYaw, float InRoll);

	void SetScale(float InX, float InY, float InZ);

	ConstantBuffer GetConstantBuffer();

	void SetCollisionBoxCenter(XMFLOAT3 NewCenter);

	void SetUID(std::string uid);

	std::string GetUID();

	CollisionComponent GetCollisionComponent();

	DXResourcesConfig m_DXResConfig{ };

	std::string m_UID;

	// Test

	void SetUIDTest(int uid);

	int GetUIDTest();

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
	int m_UIDTest{ };
};


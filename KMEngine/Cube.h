#pragma once

#include "GameEntity.h"
#include "GameEntity3D.h"
#include <vector>

class GameEntity3D;
struct ConstantBuffer;

class Cube : public GameEntity3D
{
public:
	Cube()
	{
		Location = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		ModelMatrix = XMMatrixIdentity();
	};

private:
	XMVECTOR Location;
	XMMATRIX ModelMatrix;

public:
	void SetConstantBuffer(XMMATRIX World, XMMATRIX View, XMMATRIX Projection)
	{
		m_ConstantBuffer.mWorld = World;
		m_ConstantBuffer.mView = View;
		m_ConstantBuffer.mProjection = Projection;
	}

	void SetConstantBuffer(XMMATRIX World)
	{
		m_ConstantBuffer.mWorld = World;
	}

	void SetLocation(XMMATRIX Location)
	{
		m_ConstantBuffer.mWorld = Location;
	}

	void SetLocationF(float X, float Y, float Z)
	{
		XMMATRIX LocationMatrix;
		LocationMatrix = XMMatrixIdentity();
		LocationMatrix = XMMatrixTranslation(X, Y, Z);
		m_ConstantBuffer.mWorld = LocationMatrix;
	}

	XMMATRIX GetLocation()
	{
		return m_ConstantBuffer.mWorld;
	}

	void SetView(XMMATRIX View)
	{
		m_ConstantBuffer.mView = View;
	}

	XMMATRIX GetView()
	{
		return m_ConstantBuffer.mView;
	}
};

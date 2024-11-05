#pragma once

#include "GameEntity3D.h"
#include "DX11Device.h"
#include "Logger.h"

class CTerrain : public CGameEntity3D
{
public:
	void SetDX11Device(CDX11Device* InDevice)
	{
		m_pDX11Device = InDevice;
	}

	CDX11Device* m_pDX11Device{ nullptr };
};

class CTerrainGenerator
{
public:
	CTerrainGenerator(CDX11Device* InDX11Device)
		: m_pDX11Device{ InDX11Device }
	{
		CLogger& Logger = CLogger::GetLogger();
		Logger.Log("Terrain generator constructed");
	}

	~CTerrainGenerator()
	{
		CLogger& Logger = CLogger::GetLogger();
		Logger.Log("Terrain generator destroyed");
	}

	HRESULT GenerateTestTerrain();

	CDX11Device* GetDX11Device();

private:
	CDX11Device* m_pDX11Device{ nullptr };
};
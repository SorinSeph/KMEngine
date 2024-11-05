#pragma once

#include <cassert>
#include "DX11Device.h"
#include "DXResourcesConfig.h"
#include "PrimitiveGeometryFactory.h"
#include "Scene.h"

class GameEntityBuilder
{
public:
	GameEntityBuilder(DX11Device *InDX11Device)
		: m_DX11Device{ InDX11Device }
	{}

	void InitDefaultEntities();

	void TestDevice()
	{
		assert(m_DX11Device != nullptr);
	}

private:
	PrimitiveGeometryFactory m_PrimitiveGeometryFactory;
	DX11Device* m_DX11Device{ nullptr };
};
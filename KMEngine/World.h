#pragma once

#include <unordered_map>
#include "DX11Device.h"
#include "DXResourcesConfig.h"

class CWorld
{
public:
	CWorld();

	void Init();

	CScene& m_Scene{ CScene::GetScene() };

private:
	std::unordered_map<uint16_t, CDXResourcesConfig> DXResourceConfigMap;
};
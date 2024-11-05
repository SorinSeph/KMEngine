#pragma once

#include <unordered_map>
#include "DX11Device.h"
#include "DXResourcesConfig.h"

class TWorld
{
public:
	void Init();

private:
	std::unordered_map<uint16_t, DXResourcesConfig> DXResourceConfigMap;
};
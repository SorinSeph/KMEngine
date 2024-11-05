#pragma once

#include <directxmath.h>
#include <string>

using namespace DirectX;

class CGameEntity
{
public:
	CGameEntity()
		: m_GameEntityTag{ }
	{}

	std::string m_GameEntityTag;
};


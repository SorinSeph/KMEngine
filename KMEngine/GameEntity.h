#pragma once

#include <directxmath.h>
#include <string>

using namespace DirectX;

class GameEntity
{
public:
	GameEntity()
		: m_GameEntityTag{ }
	{}

	std::string m_GameEntityTag;
};
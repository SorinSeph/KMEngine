#pragma once

#include <directxmath.h>
#include <string>
#include "Core/CoreObject.h"

using namespace DirectX;

class CGameEntity : public CCoreObject
{
public:
	CGameEntity()
		: m_GameEntityTag{ }
	{}

	std::string m_GameEntityTag;
};


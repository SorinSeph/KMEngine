#pragma once

#include "BaseModule.h"
#include "../GLTFImporter.h"

class CGameplayModule : public CBaseModule
{
public:
	CGameplayModule() = default;

	void ReadInput() {};

	CGLTFImporter m_GLTFImporter;
};

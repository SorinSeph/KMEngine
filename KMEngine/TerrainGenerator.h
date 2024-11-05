#pragma once

#include "GameEntity3D.h"

class Terrain : public GameEntity3D {};

class TerrainGenerator
{
public:

	Terrain* GenerateTerrain()
	{
		Terrain mTerrain;

		return new Terrain;
	}
};
#pragma once

#include "cube.h"
#include "PerlinNoise.h"
#include <vector>

class CubeTerrainGenerator
{
public:
	// A terrain chunk consists of a 4 x 4 cubes	
	CubeTerrainGenerator() : m_ChunkList(0), m_CubeTerrain(0)
	{
		int i{ 0 };
		int j{ 0 };
		int counter{ 0 };
		int counter2{ 4 };
		int counter3{ 8 };
		int counter4{ 12 };
		float SpawnLocX{ -3 };
		float SpawnLocY{ -2 };
		float SpawnLocZ{ 3 };

		// Middle chunk

		for (i = 0; i < 4; i++)
		{
			m_ChunkList.push_back(new Cube());
			m_ChunkList.at(i)->SetLocationF(SpawnLocX, SpawnLocY, SpawnLocZ);

			SpawnLocX += 2;
			counter++;
		}

		SpawnLocX = -3;
		SpawnLocZ = 1;

		for (i = 4; i < 8; i++)
		{
			m_ChunkList.push_back(new Cube());
			m_ChunkList.at(i)->SetLocationF(SpawnLocX, SpawnLocY, SpawnLocZ);

			SpawnLocX += 2;
		}

		SpawnLocX = -3;
		SpawnLocZ = -1;

		for (i = 8; i < 12; i++)
		{
			m_ChunkList.push_back(new Cube());
			m_ChunkList.at(i)->SetLocationF(SpawnLocX, SpawnLocY, SpawnLocZ);

			SpawnLocX += 2;
		}

		SpawnLocX = -3;
		SpawnLocZ = -3;

		for (i = 12; i < 16; i++)
		{
			m_ChunkList.push_back(new Cube());
			m_ChunkList.at(i)->SetLocationF(SpawnLocX, SpawnLocY, SpawnLocZ);
			SpawnLocX += 2;
		}

		m_CubeTerrain.push_back(m_ChunkList);
	};

private:
	std::vector<Cube*> m_ChunkList;
	std::vector<std::vector<Cube*>> m_CubeTerrain;

public:
	std::vector<std::vector<Cube*>> GetCubeTerrain()
	{
		return m_CubeTerrain;
	}

	std::vector<Cube*> GetChunkListAt(int Location)
	{
		return m_CubeTerrain.at(Location);
	}
};

class TerrainGenerator
{
public:
	TerrainGenerator(float width, float height) :
		m_Width(width),
		m_Height(height)
	{}

private:
	float m_Width;
	float m_Height;
};

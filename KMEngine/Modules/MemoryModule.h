#pragma once

#include <string>
#include "BaseModule.h"
#include <fstream>
#include "../Core/CoreObject.h"
#include "../GLTFImporter.h"
#include "../GameEntity3D.h"
#include "../EntityBuilder.h"

#define PERMANENT_MEMORY_SIZE 2048

class CMemoryModule : public CBaseModule
{
public:
	CMemoryModule() = default;

	void Init();

	void SerializeGameEntity(CCoreObject& Object);

	void LoadPlayer();

	void LoadPlayer(std::string FilePath, const std::string FileContent);

	template <typename Type>
	void AddToPermanentMemory(Type tObject)
	{
		uint64_t Size = sizeof(tObject);
		if (m_ArenaCurrentOffset + Size <= PERMANENT_MEMORY_SIZE)
		{
			std::memcpy(&m_ArenaAllocator[m_ArenaCurrentOffset], &tObject, Size);
			m_ArenaPreviousOffset = m_ArenaCurrentOffset;
			m_ArenaCurrentOffset += Size;
		}
	};

	std::string GenerateUID();

	uint8_t m_ArenaAllocator[PERMANENT_MEMORY_SIZE];
	uint64_t m_ArenaCurrentOffset{ 0 };
	uint64_t m_ArenaPreviousOffset{ 0 };
	std::ofstream m_MapFileStream;

	CEntityBuilder m_EntityBuilder;
};
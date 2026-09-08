#pragma once

enum class ECoreObjectType
{
	GAME_ENTITY,
	GAME_ENTITY_3D
};

class CCoreObject 
{
public:
	CCoreObject() = default;

	ECoreObjectType ObjectType;
};
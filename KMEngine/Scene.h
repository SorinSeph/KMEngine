#pragma once

#include <vector>
#include "GameEntity3D.h"

class Scene
{
private:
	Scene() {}
	//static Scene* GScene;
	std::vector<GameEntity3D> m_GameEntityList;

public:
	//static Scene* GetScene();
	static Scene& GetScene();
	void AddEntityToScene(GameEntity3D Entity);
	void AddEntityToScene(int Location, GameEntity3D Entity);
	std::vector<GameEntity3D> GetSceneList();
};
#include "Scene.h"

Scene* Scene::GScene = new Scene;

Scene* Scene::GetScene()
{
	return GScene;
}

void Scene::AddEntityToScene(GameEntity3D Entity)
{
	m_GameEntityList.push_back(Entity);
}

void Scene::AddEntityToScene(int Location, GameEntity3D Entity)
{
	auto Loc = m_GameEntityList.begin() + Location - 1;
	m_GameEntityList.emplace(Loc, Entity);
}

std::vector<GameEntity3D> Scene::GetSceneList()
{
	return m_GameEntityList;
}
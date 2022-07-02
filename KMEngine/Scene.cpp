#include "Scene.h"

//Scene* Scene::GScene = new Scene;

Scene& Scene::GetScene()
{
	static Scene GScene;
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

void Scene::SetVertexbuffer(int Location, ID3D11Buffer* vb)
{
	m_GameEntityList.at(Location).SetVertexbuffer(vb);
}

std::vector<GameEntity3D> Scene::GetSceneList()
{
	return m_GameEntityList;
}
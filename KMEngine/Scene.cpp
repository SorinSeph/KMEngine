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

void Scene::SetIndexbuffer(int Location, ID3D11Buffer* ib, DXGI_FORMAT Format, int Offset)
{
	m_GameEntityList.at(Location).SetIndexbuffer(ib, Format, Offset);
}

void Scene::SetInputLayout(int Location, ID3D11InputLayout* InputLayout)
{
	m_GameEntityList.at(Location).SetInputLayout(InputLayout);
}

void Scene::SetVertexShader(int Location, ID3D11VertexShader* VS)
{
	m_GameEntityList.at(Location).SetVertexShader(VS);
}

void Scene::SetPixelShader(int Location, ID3D11PixelShader* PS)
{
	m_GameEntityList.at(Location).SetPixelShader(PS);
}

void Scene::SetConstantBuffer(int Location, ID3D11Buffer* cb)
{
	m_GameEntityList.at(Location).SetConstantBuffer(cb);
}

void Scene::SetCollisionParams(int Location, XMFLOAT3 Center, XMFLOAT3 Extents, ContainmentType CollisionType)
{
	m_GameEntityList.at(Location).SetCollisionParams(Center, Extents, CollisionType);
}

std::vector<GameEntity3D> Scene::GetSceneList()
{
	return m_GameEntityList;
}
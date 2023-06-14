#include "Scene.h"
#include "Logger.h"

//Scene* Scene::GScene = new Scene;

void SceneList::RemoveLastElement()
{
	Logger& Logger_ = Logger::GetLogger();
	m_GameEntityList.pop_back();

	Logger_.Log("Scene.cpp, RemoveLastElement: Pop back on GameEntityList called");
}



void SceneList::TestRemoveLinetrace()
{
	Logger& Logger_ = Logger::GetLogger();

	for (int It = 0; It < m_GameEntityList.size(); It++)
	{
		if (m_GameEntityList.at(It).m_GameEntityTag == "Linetrace");
		{
			m_GameEntityList.at(It).m_GameEntityTag = "LinetraceNEW";
			m_GameEntityList.erase(m_GameEntityList.begin() + It);
			Logger_.Log("Scene.cpp, TestRemoveLinetrace: Linetrace removed");
		}
	}

	Logger_.Log("Scene.cpp, TestRemoveLinetrace: TestRemoveLinetrace called");
}

void SceneList::BreakpointTest()
{
	Logger& Logger_ = Logger::GetLogger();
	Logger_.Log("Scene.cpp, BreakpointTest");
}

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

void Scene::RemoveEntityFromScene(std::string EntityTag)
{
	for (int It = 0; It < m_GameEntityList.size(); It++)
	{
		if (m_GameEntityList.at(It).m_GameEntityTag == EntityTag)
		{
			m_GameEntityList.erase(m_GameEntityList.begin() + It);
		}
	}
}

void Scene::RemoveLastEntityFromScene() 
{

	m_GameEntityList.pop_back();
	Logger& Logger_ = Logger::GetLogger();
	Logger_.Log ( "Scene.cpp, RemoveLastEntityFromScene: Removing ", m_GameEntityList.at(m_GameEntityList.size() - 1).m_GameEntityTag );

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



////////


void Scene::AddEntityToScene2(GameEntity3D Entity)
{
	m_SceneList.m_GameEntityList.push_back(Entity);
}

void Scene::AddEntityToScene2(int Location, GameEntity3D Entity)
{
	auto Loc = m_SceneList.m_GameEntityList.begin() + Location - 1;
	m_SceneList.m_GameEntityList.emplace(Loc, Entity);
}

void Scene::RemoveEntityFromScene2(std::string EntityTag)
{
	for (int It = 0; It < m_SceneList.m_GameEntityList.size(); It++)
	{
		if (m_SceneList.m_GameEntityList.at(It).m_GameEntityTag == EntityTag)
		{
			m_SceneList.m_GameEntityList.erase(m_SceneList.m_GameEntityList.begin() + It);
		}
	}
}

void Scene::RemoveLastEntityFromScene2()
{
	int Resize = m_SceneList.m_GameEntityList.size() - 1;
	m_SceneList.m_GameEntityList.pop_back();
	Logger& Logger_ = Logger::GetLogger();
	Logger_.Log("Scene.cpp, RemoveLastEntityFromScene2()\n");
}

void Scene::SetVertexbuffer2(int Location, ID3D11Buffer* vb)
{
	m_SceneList.m_GameEntityList.at(Location).SetVertexbuffer(vb);
}

void Scene::SetIndexbuffer2(int Location, ID3D11Buffer* ib, DXGI_FORMAT Format, int Offset)
{
	m_SceneList.m_GameEntityList.at(Location).SetIndexbuffer(ib, Format, Offset);
}

void Scene::SetInputLayout2(int Location, ID3D11InputLayout* InputLayout)
{
	m_SceneList.m_GameEntityList.at(Location).SetInputLayout(InputLayout);
}

void Scene::SetVertexShader2(int Location, ID3D11VertexShader* VS)
{
	m_SceneList.m_GameEntityList.at(Location).SetVertexShader(VS);
}

void Scene::SetPixelShader2(int Location, ID3D11PixelShader* PS)
{
	m_SceneList.m_GameEntityList.at(Location).SetPixelShader(PS);
}

void Scene::SetConstantBuffer2(int Location, ID3D11Buffer* cb)
{
	m_SceneList.m_GameEntityList.at(Location).SetConstantBuffer(cb);
}

void Scene::SetCollisionParams2(int Location, XMFLOAT3 Center, XMFLOAT3 Extents, ContainmentType CollisionType)
{
	m_SceneList.m_GameEntityList.at(Location).SetCollisionParams(Center, Extents, CollisionType);
}

std::vector<GameEntity3D> Scene::GetSceneList2()
{
	return m_SceneList.m_GameEntityList;
}

////// Setting GameEntity3DComponent objects in Scene

void Scene::SetComponentConstantBuffer(int Location, ID3D11Buffer* cb)
{
	m_SceneList.m_GameEntityList.at(Location).m_GameEntity3DComponent.at(0).SetConstantBuffer(cb);
}

void Scene::SetComponentVertexShader(int Location, ID3D11VertexShader* VS)
{
	m_SceneList.m_GameEntityList.at(Location).m_GameEntity3DComponent.at(0).SetVertexShader(VS);
}

void Scene::SetComponentPixelShader(int Location, ID3D11PixelShader* PS)
{
	m_SceneList.m_GameEntityList.at(Location).m_GameEntity3DComponent.at(0).SetPixelShader(PS);
}

void Scene::SetComponentInputLayout(int Location, ID3D11InputLayout* InputLayout)
{
	m_SceneList.m_GameEntityList.at(Location).m_GameEntity3DComponent.at(0).SetInputLayout(InputLayout);
}

void Scene::SetComponentVertexbuffer(int Location, ID3D11Buffer* vb)
{
	m_SceneList.m_GameEntityList.at(Location).m_GameEntity3DComponent.at(0).SetVertexbuffer(vb);
}

void Scene::SetComponentIndexbuffer(int Location, ID3D11Buffer* ib, DXGI_FORMAT Format, int Offset)
{
	m_SceneList.m_GameEntityList.at(Location).m_GameEntity3DComponent.at(0).SetIndexbuffer(ib, Format, Offset);
}
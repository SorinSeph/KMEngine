#include "Scene.h"
#include "Logger.h"

//Scene* Scene::GScene = new Scene;

void CSceneList::RemoveLastElement()
{
	CLogger& Logger = CLogger::GetLogger();
	m_GameEntityList.pop_back();

	Logger.Log("Scene.cpp, RemoveLastElement: Pop back on GameEntityList called");
}



void CSceneList::TestRemoveLinetrace()
{
	CLogger& Logger = CLogger::GetLogger();

	for (int It = 0; It < m_GameEntityList.size(); It++)
	{
		if (m_GameEntityList.at(It).m_GameEntityTag == "Linetrace");
		{
			m_GameEntityList.at(It).m_GameEntityTag = "LinetraceNEW";
			m_GameEntityList.erase(m_GameEntityList.begin() + It);
			Logger.Log("Scene.cpp, TestRemoveLinetrace: Linetrace removed");
		}
	}

	Logger.Log("Scene.cpp, TestRemoveLinetrace: TestRemoveLinetrace called");
}

void CSceneList::BreakpointTest()
{
	CLogger& Logger = CLogger::GetLogger();
	Logger.Log("Scene.cpp, BreakpointTest");
}

CScene& CScene::GetScene()
{
	static CScene Scene;
	return Scene;
}

void CScene::AddEntityToScene(CGameEntity3D Entity)
{
	m_SceneList.m_GameEntityList.push_back(Entity);
}

void CScene::AddEntityToScene(int Location, CGameEntity3D Entity)
{
	auto Loc = m_SceneList.m_GameEntityList.begin() + Location - 1;
	m_SceneList.m_GameEntityList.emplace(Loc, Entity);
}

void CScene::RemoveEntityFromScene(std::string EntityTag)
{
	for (int It = 0; It < m_SceneList.m_GameEntityList.size(); It++)
	{
		if (m_SceneList.m_GameEntityList.at(It).m_GameEntityTag == EntityTag)
		{
			m_SceneList.m_GameEntityList.erase(m_SceneList.m_GameEntityList.begin() + It);
		}
	}
}

void CScene::RemoveLastEntityFromScene()
{
	int Resize = m_SceneList.m_GameEntityList.size() - 1;
	m_SceneList.m_GameEntityList.pop_back();
	CLogger& Logger = CLogger::GetLogger();
	Logger.Log("Scene.cpp, RemoveLastEntityFromScene2()\n");
}

void CScene::SetVertexbuffer(int Location, ID3D11Buffer* vb)
{
	m_SceneList.m_GameEntityList.at(Location).SetVertexBuffer(vb);
}

void CScene::SetIndexbuffer(int Location, ID3D11Buffer* ib, DXGI_FORMAT Format, int Offset)
{
	m_SceneList.m_GameEntityList.at(Location).SetIndexBuffer(ib, Format, Offset);
}

void CScene::SetInputLayout(int Location, ID3D11InputLayout* InputLayout)
{
	m_SceneList.m_GameEntityList.at(Location).SetInputLayout(InputLayout);
}

void CScene::SetVertexShader(int Location, ID3D11VertexShader* VS)
{
	m_SceneList.m_GameEntityList.at(Location).SetVertexShader(VS);
}

void CScene::SetPixelShader(int Location, ID3D11PixelShader* PS)
{
	m_SceneList.m_GameEntityList.at(Location).SetPixelShader(PS);
}

void CScene::SetConstantBuffer(int Location, ID3D11Buffer* cb)
{
	m_SceneList.m_GameEntityList.at(Location).SetConstantBuffer(cb);
}

void CScene::SetCollisionParams(int Location, XMFLOAT3 Center, XMFLOAT3 Extents, ContainmentType CollisionType)
{
	m_SceneList.m_GameEntityList.at(Location).SetCollisionParams(Center, Extents, CollisionType);
}

std::vector<CGameEntity3D> CScene::GetSceneList()
{
	return m_SceneList.m_GameEntityList;
}

////// Setting GameEntity3DComponent objects in Scene

void CScene::SetComponentConstantBuffer(int Location, int ComponentLocation, ID3D11Buffer* cb)
{
	m_SceneList.m_GameEntityList.at(Location).m_GameEntity3DComponent.at(ComponentLocation).SetConstantBuffer(cb);
}

void CScene::SetComponentVertexShader(int Location, int ComponentLocation, ID3D11VertexShader* VS)
{
	m_SceneList.m_GameEntityList.at(Location).m_GameEntity3DComponent.at(ComponentLocation).SetVertexShader(VS);
}

void CScene::SetComponentPixelShader(int Location, int ComponentLocation, ID3D11PixelShader* PS)
{
	m_SceneList.m_GameEntityList.at(Location).m_GameEntity3DComponent.at(ComponentLocation).SetPixelShader(PS);
}

void CScene::SetComponentInputLayout(int Location, int ComponentLocation, ID3D11InputLayout* InputLayout)
{
	m_SceneList.m_GameEntityList.at(Location).m_GameEntity3DComponent.at(ComponentLocation).SetInputLayout(InputLayout);
}

void CScene::SetComponentVertexbuffer(int Location, int ComponentLocation, ID3D11Buffer* vb)
{
	m_SceneList.m_GameEntityList.at(Location).m_GameEntity3DComponent.at(ComponentLocation).SetVertexBuffer(vb);
}

void CScene::SetComponentIndexbuffer(int Location, int ComponentLocation, ID3D11Buffer* ib, DXGI_FORMAT Format, int Offset)
{
	m_SceneList.m_GameEntityList.at(Location).m_GameEntity3DComponent.at(ComponentLocation).SetIndexBuffer(ib, Format, Offset);
}

void CScene::InsertDXResourceConfig(int Key, CDXResourcesConfig Config)
{
	DXResourceConfigMap.emplace(std::make_pair(Key, Config));
}

CDXResourcesConfig CScene::GetDXResourceConfig(int Key)
{
	return DXResourceConfigMap.at(Key);
}

void CScene::SetOwnVertexShader(ID3D11VertexShader* VertexShader)
{
	m_VertexShader = VertexShader;
}

ID3D11VertexShader* CScene::GetOwnVertexShader()
{
	return m_VertexShader;
}

void CScene::SetOwnPixelShader(ID3D11PixelShader* PixelShader)
{
	m_PixelShader = PixelShader;
}

ID3D11PixelShader* CScene::GetOwnPixelShader()
{
	return m_PixelShader;
}
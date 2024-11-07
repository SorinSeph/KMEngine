#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "GameEntity3D.h"

class CSceneList
{
public:
	std::vector<CGameEntity3D> m_GameEntityList{ };

	void RemoveLastElement();

	void TestRemoveLinetrace();

	void BreakpointTest();

	//void TestOutput();
};

class CScene
{
private:
	CScene() {}
	//static Scene* GScene;
	std::vector<CGameEntity3D> m_GameEntityList{ };



	// Temp members, to delete later

	ID3D11VertexShader* m_VertexShader{ nullptr };

	ID3D11PixelShader* m_PixelShader{ nullptr };

public:
	//static Scene* GetScene();
	static CScene& GetScene();

	SConstantBuffer m_SceneConstantBuffer;

	std::unordered_map<uint16_t, CDXResourcesConfig> DXResourceConfigMap;

	void InsertDXResourceConfig(int Key, CDXResourcesConfig Config);

	CDXResourcesConfig GetDXResourceConfig(int Key);

	void AddEntityToScene(CGameEntity3D& Entity);

	void AddEntityToScene(int Location, CGameEntity3D Entity);

	void RemoveLastEntityFromScene();

	void RemoveEntityFromScene(std::string EntityTag);

	void SetCollisionParams(int Location, XMFLOAT3 Center, XMFLOAT3 Extents, ContainmentType CollisionType);

	CSceneList m_SceneList;

	std::vector<CGameEntity3D>& GetSceneList();

	void SetOwnVertexShader(ID3D11VertexShader* VertexShader);

	ID3D11VertexShader* GetOwnVertexShader();

	void SetOwnPixelShader(ID3D11PixelShader* PixelShader);

	ID3D11PixelShader* GetOwnPixelShader();
};
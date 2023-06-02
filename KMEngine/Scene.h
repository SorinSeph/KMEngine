#pragma once

#include <vector>
#include <string>
#include "GameEntity3D.h"

class SceneList
{
public:
	std::vector<GameEntity3D> m_GameEntityList{ };

	void RemoveLastElement();

	void TestRemoveLinetrace();

	void BreakpointTest();

	//void TestOutput();
};

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

	void RemoveLastEntityFromScene();

	void RemoveEntityFromScene(std::string EntityTag);

	void SetConstantBuffer(int Location, ID3D11Buffer* cb);

	void SetVertexbuffer(int Location, ID3D11Buffer* vb);

	void SetIndexbuffer(int Location, ID3D11Buffer* ib, DXGI_FORMAT Format, int Offset);

	void SetInputLayout(int Location, ID3D11InputLayout* InputLayout);

	void SetVertexShader(int Location, ID3D11VertexShader* VS);

	void SetPixelShader(int Location, ID3D11PixelShader* PS);

	void SetCollisionParams(int Location, XMFLOAT3 Center, XMFLOAT3 Extents, ContainmentType CollisionType);

	SceneList m_SceneList;

	std::vector<GameEntity3D> GetSceneList();


	////////


	void AddEntityToScene2(GameEntity3D Entity);

	void AddEntityToScene2(int Location, GameEntity3D Entity);

	void RemoveLastEntityFromScene2();

	void RemoveEntityFromScene2(std::string EntityTag);

	void SetConstantBuffer2(int Location, ID3D11Buffer* cb);

	void SetVertexbuffer2(int Location, ID3D11Buffer* vb);

	void SetIndexbuffer2(int Location, ID3D11Buffer* ib, DXGI_FORMAT Format, int Offset);

	void SetInputLayout2(int Location, ID3D11InputLayout* InputLayout);

	void SetVertexShader2(int Location, ID3D11VertexShader* VS);

	void SetPixelShader2(int Location, ID3D11PixelShader* PS);

	void SetCollisionParams2(int Location, XMFLOAT3 Center, XMFLOAT3 Extents, ContainmentType CollisionType);

	std::vector<GameEntity3D> GetSceneList2();

	void SetComponentConstantBuffer(int Location, ID3D11Buffer* cb);
};
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
	void SetConstantBuffer(int Location, ID3D11Buffer* cb);
	void SetVertexbuffer(int Location, ID3D11Buffer* vb);
	void SetIndexbuffer(int Location, ID3D11Buffer* ib, DXGI_FORMAT Format, int Offset);
	void SetInputLayout(int Location, ID3D11InputLayout* InputLayout);
	void SetVertexShader(int Location, ID3D11VertexShader* VS);
	void SetPixelShader(int Location, ID3D11PixelShader* PS);
	void SetCollisionParams(int Location, XMFLOAT3 Center, XMFLOAT3 Extents, ContainmentType CollisionType);
};
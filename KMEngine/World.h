#pragma once

#include "DX11Device.h"
#include "OpenGLDevice.h"
#include "DXResourcesConfig.h"
#include "GLTFImporter.h"
#include "Modules/GraphicsModule.h"

class CWorld
{
public:
	CWorld() = default;

	void Init();

	void SetOpenGLDevice(COpenGLDevice* pOpenGLDevice);

	void SetGraphicsModule(CGraphicsModule* pGraphicsModule);

	void AddPlayer();

	void MovePlayerStartTimer();

	void MovePlayer();

	CScene& m_Scene{ CScene::GetScene() };

	CGameEntity3D* m_pPlayer{ nullptr };

private:
	COpenGLDevice* m_pOpenGLDevice{ nullptr };
	CGLTFImporter m_GLTFImporter;
	CGraphicsModule* m_pGraphicsModule{ nullptr };

};
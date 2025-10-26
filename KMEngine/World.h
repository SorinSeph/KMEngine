#pragma once

#include "DX11Device.h"
#include "OpenGLDevice.h"
#include "DXResourcesConfig.h"

class CWorld
{
public:
	CWorld() = default;

	void Init();

	void SetOpenGLDevice(COpenGLDevice* pOpenGLDevice);

	void AddTestEntity();

	CScene& m_Scene{ CScene::GetScene() };

private:
	COpenGLDevice* m_pOpenGLDevice{ nullptr };
};
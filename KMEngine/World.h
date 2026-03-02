#pragma once

#include "DX11Device.h"
#include "OpenGLDevice.h"
#include "DXResourcesConfig.h"
#include "ImporterGLTF.h"

class CWorld
{
public:
	CWorld() = default;

	void Init();

	void SetOpenGLDevice(COpenGLDevice* pOpenGLDevice);

	CScene& m_Scene{ CScene::GetScene() };

private:
	COpenGLDevice* m_pOpenGLDevice{ nullptr };
	CImporterGLTF m_ImporterGLTF;
};
#pragma once

#include "GameEntity3D.h"
#include "Scene.h"
#include "stb_image.h"
#include "ImporterGLTF.h"
#include "OpenGLDevice.h"

class CGraphicsModule;

class CEntityBuilder
{
public:
	CEntityBuilder() = default;

	void ImportGLTF(std::string& FilePath, const std::string& FileContent);

	void CreateModel(const std::vector<float>& Vertices, const std::vector<uint32_t>& Indices);

	void SetGraphicsModule(CGraphicsModule* GraphicsModule);

	void SetOpenGLDevice(COpenGLDevice* pOpenGLDevice);

	void AddTestEntity();

	CImporterGLTF m_ImporterGLTF;
	CGraphicsModule* m_pGraphicsModule{ nullptr };
	COpenGLDevice* m_pOpenGLDevice{ nullptr };
};
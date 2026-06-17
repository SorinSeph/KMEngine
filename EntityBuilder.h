#pragma once

#include "GameEntity3D.h"
#include "Scene.h"
#include "stb_image.h"
#include "GLTFImporter.h"
#include "OpenGLDevice.h"

class CGraphicsModule;

class CEntityBuilder
{
public:
	CEntityBuilder() = default;

	void ImportGLTF(std::string& FilePath, const std::string& FileContent);

	void ImportGLTFAnimation();

	// @Temporary for testing a single animation imported on button press
	CGLTFAnimation GetGLTFAnimation();

	void CreateModel(const std::vector<SSkeletalVertex>& Vertices, const std::vector<uint32_t>& Indices);

	void CreateGizmoOpenGL(glm::vec3 Location);
	
	void CreateLight();

	void SetGraphicsModule(CGraphicsModule* GraphicsModule);

	void SetOpenGLDevice(COpenGLDevice* pOpenGLDevice);

	void AddLinetrace(glm::vec3 StartLocation, glm::vec3 EndLocation);

	CGLTFImporter m_GLTFImporter;
	CGraphicsModule* m_pGraphicsModule{ nullptr };
	COpenGLDevice* m_pOpenGLDevice{ nullptr };

	// Temporary, to delete 
	std::string m_FilePath;
	std::string m_FileContent;
	CGameEntity3D* m_pEntity;

	void InterpMoveTestEntity();
	float m_InterpMoveLoc{ 0 };
};
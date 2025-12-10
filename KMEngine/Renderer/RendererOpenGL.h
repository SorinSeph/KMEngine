#pragma once
#include "Renderer.h"  
#include "../OpenGLDevice.h"
#include "../Scene.h"
#include <source_location>

//static glm::mat4 g_ViewMatrix;// = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

class COpenGLCamera
{
public:
	COpenGLCamera()
	{
		COpenGLDevice::g_ViewMatrix = glm::lookAt(m_CameraLocation, m_CameraLocation + m_CameraFront, m_CameraUp);
	}

	glm::vec3 m_CameraLocation{ 0.0f, 0.0f, 3.0f };
	glm::vec3 m_CameraFront = { 0.0f, 0.0f, -1.0f };
	glm::vec3 m_CameraUp = {0.0f, 1.0f, 0.0f};
};

class CRendererOpenGL
{
public:
	CRendererOpenGL() = default;

	//explicit CRendererOpenGL(const std::source_location& loc = std::source_location::current())
	//{
	//	CLogger& Logger = CLogger::GetLogger();
	//	Logger.Log("CRendererOpenGL constructed at ", loc.file_name(), ":", (int)loc.line());
	//}

	void SetWindowHandle(HWND hwnd)
	{
		m_OpenGLDevice.SetViewportHandle(hwnd);
	} 

	void SetViewportWidthAndHeight(int Width, int Height)
	{
		m_OpenGLDevice.SetViewportWidthAndHeight(Width, Height);
	}

	void Render(float EyeX, float EyeY, float EyeZ, float RotX, float RotY)
	{
		CTimerManager& TimerManager = CTimerManager::GetTimerManager();
		CScene& Scene = CScene::GetScene();

		if (!m_OpenGLDevice.m_HDC || !m_OpenGLDevice.m_HGLRC)
			return;

		glViewport(0, 0, m_OpenGLDevice.m_ViewportWidth, m_OpenGLDevice.m_ViewportHeight);

		glClearColor(0.12f, 0.f, 0.35f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto& SceneEntityList = Scene.GetSceneList();
		for (auto& SceneEntityIt : SceneEntityList)
		{
			std::vector<CSceneGraphNode<CGameEntity3DComponent>*> EntityComponentVector;
			EntityComponentVector.push_back(SceneEntityIt.m_SceneGraph.m_pRootNode);
			for (auto& EntityComponent : EntityComponentVector)
			{
				glUseProgram(EntityComponent->m_tType.m_OpenGLResource.m_ShaderProgram);
				glBindVertexArray(EntityComponent->m_tType.m_OpenGLResource.m_VAO);

				m_ViewportCamera.m_CameraLocation = glm::vec3(EyeX, EyeY, -EyeZ);
				// Create rotation matrices
				glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(-RotX), glm::vec3(1.0f, 0.0f, 0.0f));
				glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(-RotY), glm::vec3(0.0f, 1.0f, 0.0f));

				// Combine rotations (note: order matters - Y then X to match DirectX)
				glm::mat4 rotation = rotationY * rotationX;

				// Apply rotation to the default forward vector
				glm::vec4 front4 = rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
				m_ViewportCamera.m_CameraFront = glm::normalize(glm::vec3(front4));
				COpenGLDevice::g_ViewMatrix = glm::lookAt(m_ViewportCamera.m_CameraLocation, m_ViewportCamera.m_CameraLocation + m_ViewportCamera.m_CameraFront, m_ViewportCamera.m_CameraUp);
				glUniformMatrix4fv(glGetUniformLocation(EntityComponent->m_tType.m_OpenGLResource.m_ShaderProgram, std::string{"view"}.c_str()), 1, GL_FALSE, &COpenGLDevice::g_ViewMatrix[0][0]);

				glm::mat4 modelMatrix = glm::mat4(1.0f);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(EntityComponent->m_tType.GetLocationX(),
																	EntityComponent->m_tType.GetLocationY(),
																	EntityComponent->m_tType.GetLocationZ()));

				glUniformMatrix4fv(glGetUniformLocation(EntityComponent->m_tType.m_OpenGLResource.m_ShaderProgram, std::string{ "model" }.c_str()), 1, GL_FALSE, &modelMatrix[0][0]);

				if (EntityComponent->m_tType.m_GameEntityTag == "TerrainComponent")
				{
					glDrawElements(EntityComponent->m_tType.m_OpenGLResource.m_DrawMode, 111408, GL_UNSIGNED_INT, nullptr);
				}
				else if (EntityComponent->m_tType.m_GameEntityTag == "LinetraceComponent")
				{
					auto prog = EntityComponent->m_tType.m_OpenGLResource.m_ShaderProgram;
					GLint locPos = glGetAttribLocation(prog, "aPos");
					GLint locCol = glGetAttribLocation(prog, "aColor");

					// Temporarily disable depth to rule out occlusion
					glDisable(GL_DEPTH_TEST);
					glLineWidth(2.0f);

					glDrawElements(EntityComponent->m_tType.m_OpenGLResource.m_DrawMode, 2, GL_UNSIGNED_INT, nullptr);

					glEnable(GL_DEPTH_TEST);
				}

				auto breakpoint = 1;
			}
		}
		SwapBuffers(m_OpenGLDevice.m_HDC);
	}

	COpenGLDevice m_OpenGLDevice;
	COpenGLCamera m_ViewportCamera;
};
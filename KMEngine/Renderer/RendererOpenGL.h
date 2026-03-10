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
		// Set initial pitch (negative = looking down)
		float pitchDegrees = -30.0f;
		float pitchRadians = glm::radians(pitchDegrees);
		
		m_CameraFront = glm::vec3(
			0.0f,                        // no yaw, so x = 0
			glm::sin(pitchRadians),      // y component from pitch
			-glm::cos(pitchRadians)      // z component (forward is -Z in OpenGL)
		);
		m_CameraFront = glm::normalize(m_CameraFront);
		
		COpenGLDevice::g_ViewMatrix = glm::lookAt(m_CameraLocation, m_CameraLocation + m_CameraFront, m_CameraUp);
	}

	glm::vec3 m_CameraLocation{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_CameraFront{ 0.0f, 0.0f, -1.0f };
	glm::vec3 m_CameraUp{ 0.0f, 1.0f, 0.0f };
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
		CLogger& Logger = CLogger::GetLogger();

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
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, EntityComponent->m_tType.m_OpenGLResource.m_Texture);
				glBindVertexArray(EntityComponent->m_tType.m_OpenGLResource.m_VAO);

				glUseProgram(EntityComponent->m_tType.m_OpenGLResource.m_ShaderProgram);

				m_ViewportCamera.m_CameraLocation = glm::vec3(EyeX, EyeY, -EyeZ);
				glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(-RotX), glm::vec3(1.0f, 0.0f, 0.0f));
				glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(-RotY), glm::vec3(0.0f, 1.0f, 0.0f));
				glm::mat4 rotation = rotationY * rotationX;
				glm::vec4 front4 = rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
				m_ViewportCamera.m_CameraFront = glm::normalize(glm::vec3(front4));

				COpenGLDevice::g_ViewMatrix = glm::lookAt(m_ViewportCamera.m_CameraLocation, m_ViewportCamera.m_CameraLocation + m_ViewportCamera.m_CameraFront, m_ViewportCamera.m_CameraUp);
				glUniformMatrix4fv(glGetUniformLocation(EntityComponent->m_tType.m_OpenGLResource.m_ShaderProgram, std::string{"view"}.c_str()), 1, GL_FALSE, &COpenGLDevice::g_ViewMatrix[0][0]);

				//glm::mat4 modelMatrix = glm::mat4(1.0f);
				//modelMatrix = glm::translate(modelMatrix, glm::vec3(m_TestLocationX, 0.0f, -4.0f));
				//m_TestLocationX -= 0.001f;

				glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(EntityComponent->m_tType.GetLocationX(), EntityComponent->m_tType.GetLocationY(), EntityComponent->m_tType.GetLocationZ()));
				//modelMatrix = glm::translate(modelMatrix, glm::vec3 (EntityComponent->m_tType.GetLocationX(), EntityComponent->m_tType.GetLocationY(), EntityComponent->m_tType.GetLocationZ()));

				glUniformMatrix4fv(glGetUniformLocation(EntityComponent->m_tType.m_OpenGLResource.m_ShaderProgram, std::string{ "model" }.c_str()), 1, GL_FALSE, &modelMatrix[0][0]);

				glm::vec3 LightPos = glm::vec3(0.0f, 1.0f, -3.0f);
				glUniform3fv(glGetUniformLocation(EntityComponent->m_tType.m_OpenGLResource.m_ShaderProgram, "light.position"), 1, &LightPos[0]);
				glUniform3fv(glGetUniformLocation(EntityComponent->m_tType.m_OpenGLResource.m_ShaderProgram, "viewPos"), 1, &m_ViewportCamera.m_CameraLocation[0]);

				glm::vec3 lightColor;
				lightColor.x = 1.0f;
				lightColor.y = 1.0f;
				lightColor.z = 1.0f;
				glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); 
				glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); 
				glUniform3fv(glGetUniformLocation(EntityComponent->m_tType.m_OpenGLResource.m_ShaderProgram, "light.diffuse"), 1, &diffuseColor[0]);
				glUniform3fv(glGetUniformLocation(EntityComponent->m_tType.m_OpenGLResource.m_ShaderProgram, "light.ambient"), 1, &ambientColor[0]);
				glUniform3f(glGetUniformLocation(EntityComponent->m_tType.m_OpenGLResource.m_ShaderProgram, "light.specular"), 1.0f, 1.0f, 1.0f);

				glUniform3f(glGetUniformLocation(EntityComponent->m_tType.m_OpenGLResource.m_ShaderProgram, "material.specular"), 0.5f, 0.5f, 0.5f);
				glUniform1f(glGetUniformLocation(EntityComponent->m_tType.m_OpenGLResource.m_ShaderProgram, "material.shininess"), 32.0f);

				if (SceneEntityList.size() > 1)
				{
					auto breakpoint = 1;
				}

				EntityComponent->m_tType.m_OpenGLResource.Execute();

				if (EntityComponent->m_tType.m_GameEntityTag == "TerrainComponent")

				{
					Logger.Log("RendererOpenGL.h, Render(): rendering terrain component");
					//glDrawElements(EntityComponent->m_tType.m_OpenGLResource.m_DrawMode, 111408, GL_UNSIGNED_INT, nullptr);
					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
				//else if (EntityComponent->m_tType.m_GameEntityTag == "LinetraceComponent")
				//{
				//	glDisable(GL_DEPTH_TEST);
				//	glLineWidth(2.0f);

				//	glDrawElements(EntityComponent->m_tType.m_OpenGLResource.m_DrawMode, 2, GL_UNSIGNED_INT, nullptr);

				//	glEnable(GL_DEPTH_TEST);
				//}
				else if (EntityComponent->m_tType.m_GameEntityTag == "KnightComponent")
				{
					Logger.Log("RendererOpenGL.h, void Render(): Entity Component tag is \"KnightComponent\"");
					Logger.Log("RendererOpenGL.h, void Render(): Entity Location X: ", EntityComponent->m_tType.GetLocationX(), " Y: ", EntityComponent->m_tType.GetLocationY(), " Z: ", EntityComponent->m_tType.GetLocationZ(), "\n");
					glDrawElements(EntityComponent->m_tType.m_OpenGLResource.m_DrawMode, 111408, GL_UNSIGNED_INT, nullptr);
					auto breakpoint = 1;
				}

				auto breakpoint = 1;
			}
		}
		SwapBuffers(m_OpenGLDevice.m_HDC);
	}

	COpenGLDevice m_OpenGLDevice;
	COpenGLCamera m_ViewportCamera;

	// Temporary 
	float m_TestLocationX{ 0.0f };
};
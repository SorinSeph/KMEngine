#pragma once
#include "Renderer.h"  
#include "../OpenGLDevice.h"

// Temporary, to be moved in the camera class
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

class CRendererOpenGL
{
public:
	CRendererOpenGL() = default;

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

		if (!m_OpenGLDevice.m_HDC || !m_OpenGLDevice.m_HGLRC)
			return;

		glViewport(0, 0, m_OpenGLDevice.m_ViewportWidth, m_OpenGLDevice.m_ViewportHeight);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(m_OpenGLDevice.m_ShaderProgram);
		glBindVertexArray(m_OpenGLDevice.m_VAO);

		cameraPos = glm::vec3(EyeX, EyeY, -EyeZ);
		// Create rotation matrices
		glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(-RotX), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(-RotY), glm::vec3(0.0f, 1.0f, 0.0f));

		// Combine rotations (note: order matters - Y then X to match DirectX)
		glm::mat4 rotation = rotationY * rotationX;

		// Apply rotation to the default forward vector
		glm::vec4 front4 = rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
		cameraFront = glm::normalize(glm::vec3(front4));
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(glGetUniformLocation(m_OpenGLDevice.m_ShaderProgram, std::string{"view"}.c_str()), 1, GL_FALSE, &view[0][0]);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
		glUniformMatrix4fv(glGetUniformLocation(m_OpenGLDevice.m_ShaderProgram, std::string{ "model" }.c_str()), 1, GL_FALSE, &model[0][0]);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		SwapBuffers(m_OpenGLDevice.m_HDC);
	}

	COpenGLDevice m_OpenGLDevice;
};
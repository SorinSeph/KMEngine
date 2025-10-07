#pragma once
#include "Renderer.h"  
#include "../OpenGLDevice.h"

class CRendererOpenGL
{
public:
	CRendererOpenGL() = default;

	void SetWindowHandle(HWND hwnd)
	{
		m_OpenGLDevice.SetViewportHandle(hwnd);
	}

	void Render()
	{
		if (!m_OpenGLDevice.m_HDC || !m_OpenGLDevice.m_HGLRC)
			return;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(m_OpenGLDevice.g_ShaderProgram);
		glBindVertexArray(m_OpenGLDevice.g_VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		SwapBuffers(m_OpenGLDevice.m_HDC);
	}

	COpenGLDevice m_OpenGLDevice;
};
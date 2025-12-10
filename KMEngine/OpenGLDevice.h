#pragma once

#include <windows.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "stb_image.h"

#define FOV 45.0f

class COpenGLDevice
{
public:
	COpenGLDevice() = default;

    void SetViewportHandle(HWND hwnd);

    void SetViewportWidthAndHeight(int Width, int Height);

    void SetShader(const char* VertexPath, const char* FragmentPath);

    void InitOpenGLDevice();

    void CheckCompileErrors(unsigned int Shader, std::string Type);

    HWND m_Hwnd{};
    HDC m_HDC = nullptr;
    HGLRC m_HGLRC = nullptr;

    static unsigned int m_ViewportWidth;
    static unsigned int m_ViewportHeight;
    static glm::mat4 g_ProjectionMatrix;
    static glm::mat4 g_ViewMatrix;
    unsigned int m_VBO, m_VAO, m_EBO;
    unsigned int m_VertexShader;
    unsigned int m_FragmentShader;
    unsigned int m_ShaderProgram;
};
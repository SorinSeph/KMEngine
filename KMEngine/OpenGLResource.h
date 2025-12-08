#pragma once

#include <vector>
#include <functional>
#include <glad/glad.h>

class COpenGLResource
{
public:
    COpenGLResource() = default;

    uint32_t m_ShaderProgram{ 0 };
    uint32_t m_VBO{ 0 };
    uint32_t m_VAO{ 0 };
    uint32_t m_EBO{ 0 };
    uint32_t m_DrawMode{ 0 };
    std::vector<uint32_t> m_Indices;
    std::vector<std::function<void()>> m_pContextResourcePtr;

    void Execute();
};

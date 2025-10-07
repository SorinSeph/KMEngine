#pragma once

#include <windows.h>
#include <glad/glad.h>

class COpenGLDevice
{
public:
	COpenGLDevice() = default;

	void SetViewportHandle(HWND hwnd)
	{
		m_Hwnd = hwnd;
	}

    void SetShaders()
    {
        g_VertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(g_VertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(g_VertexShader);

        int success;
        char infoLog[512];
        glGetShaderiv(g_VertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(g_VertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // fragment shader
        g_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(g_FragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(g_FragmentShader);

        glGetShaderiv(g_FragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(g_FragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        g_ShaderProgram = glCreateProgram();
        glAttachShader(g_ShaderProgram, g_VertexShader);
        glAttachShader(g_ShaderProgram, g_FragmentShader);
        glLinkProgram(g_ShaderProgram);

        glGetProgramiv(g_ShaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(g_ShaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDeleteShader(g_VertexShader);
        glDeleteShader(g_FragmentShader);

        float vertices[] = {
            -0.5f, -0.5f, 0.0f, 
             0.5f, -0.5f, 0.0f, 
             0.0f,  0.5f, 0.0f  
        };

        glGenVertexArrays(1, &g_VAO);
        glGenBuffers(1, &g_VBO);

        glBindVertexArray(g_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
     
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void InitOpenGLDevice()
    {
		m_HDC = GetDC(m_Hwnd);

        PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            32,
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,
            8,
            0,
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

        int pixelFormat = ChoosePixelFormat(m_HDC, &pfd);
        SetPixelFormat(m_HDC, pixelFormat, &pfd);

        m_HGLRC = wglCreateContext(m_HDC);
        wglMakeCurrent(m_HDC, m_HGLRC);

        // Initialize glad after context is current!
        if (!gladLoadGL()) {
            MessageBoxA(0, "Failed to initialize GLAD", "Error", 0);
            exit(-1);
        }

        SetShaders();
    }

    HWND m_Hwnd{};
    HDC m_HDC = nullptr;
    HGLRC m_HGLRC = nullptr;

    unsigned int g_VBO, g_VAO;
    unsigned int g_VertexShader;
    unsigned int g_FragmentShader;
    unsigned int g_ShaderProgram;

    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n\0";
};
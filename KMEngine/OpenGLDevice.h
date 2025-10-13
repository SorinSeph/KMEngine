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

#define FOV 45.0f

class COpenGLDevice
{
public:
	COpenGLDevice() = default;

	void SetViewportHandle(HWND hwnd)
	{
		m_Hwnd = hwnd;
	}

	void SetViewportWidthAndHeight(int Width, int Height)
	{
		m_ViewportWidth = Width;
		m_ViewportHeight = Height;
	}

    void SetShader(const char* VertexPath, const char* FragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string VertexCodeString;
        std::string FragmentCodeString;
        std::ifstream VertexShaderFile;
        std::ifstream FragmentShaderFile;
        // ensure ifstream objects can throw exceptions:
        VertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        FragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            VertexShaderFile.open(VertexPath);
            FragmentShaderFile.open(FragmentPath);
            std::stringstream VertexShaderStream, FragmentShaderStream;
            // read file's buffer contents into streams
            VertexShaderStream << VertexShaderFile.rdbuf();
            FragmentShaderStream << FragmentShaderFile.rdbuf();
            // close file handlers
            VertexShaderFile.close();
            FragmentShaderFile.close();
            // convert stream into string
            VertexCodeString = VertexShaderStream.str();
            FragmentCodeString = FragmentShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* VertexShaderCode = VertexCodeString.c_str();
        const char* FragmentShaderCode = FragmentCodeString.c_str();
        // 2. compile shaders
        unsigned int VertexShader, FragmentShader;
        // vertex shader
        VertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(VertexShader, 1, &VertexShaderCode, NULL);
        glCompileShader(VertexShader);
        CheckCompileErrors(VertexShader, "VERTEX");
        // fragment Shader
        FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(FragmentShader, 1, &FragmentShaderCode, NULL);
        glCompileShader(FragmentShader);
        CheckCompileErrors(FragmentShader, "FRAGMENT");
        // shader Program
        m_ShaderProgram = glCreateProgram();
        glAttachShader(m_ShaderProgram, VertexShader);
        glAttachShader(m_ShaderProgram, FragmentShader);
        glLinkProgram(m_ShaderProgram);
        CheckCompileErrors(m_ShaderProgram, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(VertexShader);
        glDeleteShader(FragmentShader);
    }

    void InitOpenGLDevice()
    {
		m_HDC = GetDC(m_Hwnd);

        PIXELFORMATDESCRIPTOR PixelFormatDesc =
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

        int PixelFormat = ChoosePixelFormat(m_HDC, &PixelFormatDesc);
        SetPixelFormat(m_HDC, PixelFormat, &PixelFormatDesc);

        m_HGLRC = wglCreateContext(m_HDC);
        wglMakeCurrent(m_HDC, m_HGLRC);

        // Initialize glad after context is current!
        if (!gladLoadGL()) {
            MessageBoxA(0, "Failed to initialize GLAD", "Error", 0);
            exit(-1);
        }

        glEnable(GL_DEPTH_TEST);

        //SetBaseShaders();
        SetShader("Solid_Color_GLSL.vs", "Solid_Color_GLSL.fs");

        float Vertices[] = {
            // positions          // colors          
             0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   
             0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   
            -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   
            -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   
        };

        unsigned int Indices[] = 
        {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        // index attribute
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glUseProgram(m_ShaderProgram);
        glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(FOV), (float)m_ViewportWidth / (float)m_ViewportHeight, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, std::string{"projection"}.c_str()), 1, GL_FALSE, &ProjectionMatrix[0][0]);
    }

    void CheckCompileErrors(unsigned int Shader, std::string Type)
    {
        int Success;
        char InfoLog[1024];
        if (Type != "PROGRAM")
        {
            glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
            if (!Success)
            {
                glGetShaderInfoLog(Shader, 1024, NULL, InfoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << Type << "\n" << InfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(Shader, GL_LINK_STATUS, &Success);
            if (!Success)
            {
                glGetProgramInfoLog(Shader, 1024, NULL, InfoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << Type << "\n" << InfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    HWND m_Hwnd{};
    HDC m_HDC = nullptr;
    HGLRC m_HGLRC = nullptr;

    unsigned int m_ViewportWidth;
    unsigned int m_ViewportHeight;
    unsigned int m_VBO, m_VAO, m_EBO;
    unsigned int m_VertexShader;
    unsigned int m_FragmentShader;
    unsigned int m_ShaderProgram;
};
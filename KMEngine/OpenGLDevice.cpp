#include "OpenGLDevice.h"

void COpenGLDevice::SetViewportHandle(HWND hwnd)
{
	m_Hwnd = hwnd;
}

void COpenGLDevice::SetViewportWidthAndHeight(int Width, int Height)
{
    m_ViewportWidth = Width;
    m_ViewportHeight = Height;
}

void COpenGLDevice::SetShader(const char* VertexPath, const char* FragmentPath)
{

    std::string VertexCodeString;
    std::string FragmentCodeString;
    std::ifstream VertexShaderFile;
    std::ifstream FragmentShaderFile;

    VertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    FragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        VertexShaderFile.open(VertexPath);
        FragmentShaderFile.open(FragmentPath);
        std::stringstream VertexShaderStream, FragmentShaderStream;

        VertexShaderStream << VertexShaderFile.rdbuf();
        FragmentShaderStream << FragmentShaderFile.rdbuf();

        VertexShaderFile.close();
        FragmentShaderFile.close();

        VertexCodeString = VertexShaderStream.str();
        FragmentCodeString = FragmentShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char* VertexShaderCode = VertexCodeString.c_str();
    const char* FragmentShaderCode = FragmentCodeString.c_str();

    unsigned int VertexShader, FragmentShader;

    VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShader, 1, &VertexShaderCode, NULL);
    glCompileShader(VertexShader);
    CheckCompileErrors(VertexShader, "VERTEX");

    FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShader, 1, &FragmentShaderCode, NULL);
    glCompileShader(FragmentShader);
    CheckCompileErrors(FragmentShader, "FRAGMENT");

    m_ShaderProgram = glCreateProgram();
    glAttachShader(m_ShaderProgram, VertexShader);
    glAttachShader(m_ShaderProgram, FragmentShader);
    glLinkProgram(m_ShaderProgram);
    CheckCompileErrors(m_ShaderProgram, "PROGRAM");

    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);
}

void COpenGLDevice::InitOpenGLDevice()
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

    if (!gladLoadGL()) {
        MessageBoxA(0, "Failed to initialize GLAD", "Error", 0);
        exit(-1);
    }

    glEnable(GL_DEPTH_TEST);

    SetShader("Solid_Color_GLSL.vs", "Solid_Color_GLSL.fs");

    float Vertices[] = {
        // positions          // colors                 // texture coords
         0.5f,  0.5f, 0.0f,   /*1.0f, 0.0f, 0.0f,*/     1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   /*0.0f, 1.0f, 0.0f,*/     1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   /*0.0f, 0.0f, 1.0f,*/     0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   /*1.0f, 1.0f, 0.0f,*/     0.0f, 1.0f  // top left 
    };

    unsigned int Indices[] =
    {
        0, 1, 3, 
        1, 2, 3  
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;

    unsigned char* data = stbi_load("grey_grid.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glUseProgram(m_ShaderProgram);
    glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(FOV), (float)m_ViewportWidth / (float)m_ViewportHeight, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, std::string{ "projection" }.c_str()), 1, GL_FALSE, &ProjectionMatrix[0][0]);
}

void COpenGLDevice::CheckCompileErrors(unsigned int Shader, std::string Type)
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

void COpenGLDevice::CreateVertexBufferResource(COpenGLResource* Resource)
{
	Resource->m_ShaderProgram = m_ShaderProgram;
	Resource->m_VBO = m_VBO;
	Resource->m_VAO = m_VAO;
	Resource->m_EBO = m_EBO;
}
#include "World.h"
#include "Scene.h"
#include "TerrainGenerator.h"
#include <vector>

std::vector<float> g_Vertices;
std::vector<float> g_TexCoords;
std::vector<uint32_t> g_Indices;

void CWorld::Init()
{
	CTerrainGenerator TerrainGenerator{m_pOpenGLDevice};
	TerrainGenerator.GenerateTerrain();
    AddTestEntity();
}

void CWorld::LoadAttributes()
{
    //std::vector<CBufferViewBase*> BufferViews = m_ImporterGLTF.Import();
}

std::vector<float> LoadVerticesFromFile(const std::string& filePath)
{
    std::vector<float> values;
    std::ifstream file(filePath);
    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ','))
        {
            token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
            if (!token.empty())
                values.push_back(std::stof(token));
        }
    }
    return values;
}

std::vector<uint32_t> LoadIndicesFromFile(const std::string& filePath)
{
    std::vector<uint32_t> values;
    std::ifstream file(filePath);
    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ','))
        {
            token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
            if (!token.empty())
                values.push_back(std::stof(token));
        }
    }
    return values;
}

void InitAttributes()
{
    g_Vertices.reserve(104440);
    g_Indices.reserve(111408);
    std::string IndicesFilePath = "Indices.txt";
    std::string PosAndTexCoordFilePath = "Vertices.txt";
    g_Vertices = LoadVerticesFromFile(PosAndTexCoordFilePath);
    g_Indices = LoadIndicesFromFile(IndicesFilePath);
}

void CWorld::AddTestEntity()
{
	CGameEntity3D TestEntity;
	TestEntity.m_GameEntityTag = "Knight";
	CGameEntity3DComponent TestEntityComponent;
	TestEntityComponent.m_GameEntityTag = "KnightComponent";

    InitAttributes();
	uint32_t& ShaderProgram{ TestEntityComponent.m_OpenGLResource.m_ShaderProgram };
	uint32_t& VAO{ TestEntityComponent.m_OpenGLResource.m_VAO };
	uint32_t& VBO{ TestEntityComponent.m_OpenGLResource.m_VBO };
	uint32_t& EBO{ TestEntityComponent.m_OpenGLResource.m_EBO };

	CShaderGenerator ShaderGenerator;
	ShaderGenerator.GenerateBaseShaders(&TestEntityComponent.m_OpenGLResource);
    //float Vertices[] = 
    //{
    //    // positions            // colors               // texture coords
    //    0.25f,  0.25f, 3.0f,    /*1.0f, 0.0f, 0.0f,*/   1.0f, 1.0f, // top right
    //    0.25f, -0.25f, 3.0f,    /*0.0f, 1.0f, 0.0f,*/   1.0f, 0.0f, // bottom right
    //    -0.25f, -0.25f, 3.0f,   /*0.0f, 0.0f, 1.0f,*/   0.0f, 0.0f, // bottom left
    //    -0.25f,  0.25f, 3.0f,   /*1.0f, 1.0f, 0.0f,*/   0.0f, 1.0f  // top left 
    //};

    //uint32_t Indices[] =
    //{
    //    0, 1, 3, // first triangle
    //    1, 2, 3  // second triangle
    //};

    // notepad indices line start: 104457
    // indices line end: 215864

    //glGenVertexArrays(1, &VAO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, g_Vertices.size() * sizeof(float), g_Vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_Indices.size() * sizeof(float), g_Indices.data(), GL_STATIC_DRAW);

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

    glUseProgram(ShaderProgram);
    if (m_pOpenGLDevice)
    {
        glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(FOV), (float)m_pOpenGLDevice->m_ViewportWidth / (float)m_pOpenGLDevice->m_ViewportHeight, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, std::string{ "projection" }.c_str()), 1, GL_FALSE, &ProjectionMatrix[0][0]);
    }

	TestEntityComponent.m_OpenGLResource.m_Indices = g_Indices;

    CSceneGraphNode<CGameEntity3DComponent>* EntityComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
    EntityComponentNode->m_tType = TestEntityComponent;
    TestEntity.m_SceneGraph.m_pRootNode = EntityComponentNode;

    m_Scene.AddEntityToScene(TestEntity);
}

void CWorld::SetOpenGLDevice(COpenGLDevice* pOpenGLDevice)
{
	m_pOpenGLDevice = pOpenGLDevice;
}
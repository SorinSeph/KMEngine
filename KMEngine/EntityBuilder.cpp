#include "EntityBuilder.h"
#include "Modules/GraphicsModule.h"
#include "ImporterGLTF.h"
#include "ShaderGenerator.h"
#include "Scene.h"
#include <glad/glad.h>
#include <filesystem>

/***
Temporary helper variables and functions
***/
std::vector<float> g_EntityVertices;
std::vector<float> g_EntityTexCoords;
std::vector<uint32_t> g_EntityIndices;

std::vector<float> LoadEntityVerticesFromFile(const std::string& filePath)
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

std::vector<uint32_t> LoadEntityIndicesFromFile(const std::string& filePath)
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

void InitEntityAttributes()
{
	g_EntityVertices.reserve(104440);
	g_EntityIndices.reserve(111408);
	std::string IndicesFilePath = "C:\\Users\\sefce\\source\\KMEngine\\KMEngine\\Indices.txt";
	std::string PosAndTexCoordFilePath = "C:\\Users\\sefce\\source\\KMEngine\\KMEngine\\Vertices.txt";
	g_EntityVertices = LoadEntityVerticesFromFile(PosAndTexCoordFilePath);
	g_EntityIndices = LoadEntityIndicesFromFile(IndicesFilePath);
}

void CEntityBuilder::AddLinetrace()
{
	CScene& Scene = CScene::GetScene();
	CGameEntity3D LinetraceEntity{};
	LinetraceEntity.m_GameEntityTag = "LinetraceEntity";
	CGameEntity3DComponent LinetraceComponent;
	LinetraceComponent.m_GameEntityTag = "LinetraceComponent";

	uint32_t& ShaderProgram{ LinetraceComponent.m_OpenGLResource.m_ShaderProgram };
	uint32_t& VAO{ LinetraceComponent.m_OpenGLResource.m_VAO };
	uint32_t& VBO{ LinetraceComponent.m_OpenGLResource.m_VBO };
	uint32_t& EBO{ LinetraceComponent.m_OpenGLResource.m_EBO };

	LinetraceComponent.m_OpenGLResource.m_Indices = { 0, 1 };

	CShaderGenerator ShaderGenerator;
	ShaderGenerator.GenerateBaseSolidShaders(&LinetraceComponent.m_OpenGLResource);
	glUseProgram(ShaderProgram);

	float Vertices[] = {
		// positions          // colors			
		 0.f,  0.f, .0f,	1.0f, 0.0f, 0.0f,		
		 0.f, 0.f, -15.0f,  1.0f, 0.0f, 0.0f,	
	};

	uint32_t Indices[] =
	{
		0, 1,
	};

	LinetraceComponent.SetLocationF(1.0f, 0.2f, 0.f);

	
	//glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// index attribute
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glUseProgram(ShaderProgram);

	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(FOV), (float)COpenGLDevice::m_ViewportWidth / COpenGLDevice::m_ViewportHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, std::string{ "projection" }.c_str()), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	
	LinetraceComponent.m_OpenGLResource.m_DrawMode = GL_LINES;

	CSceneGraphNode<CGameEntity3DComponent>* LinetraceComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
	LinetraceComponentNode->m_tType = LinetraceComponent;
	LinetraceEntity.m_SceneGraph.m_pRootNode = LinetraceComponentNode;
	std::vector<uint32_t> TempIndices{ 0, 1, 3, 1, 2, 3 };
	//LinetraceComponent.m_OpenGLResource.m_Indices.insert(LinetraceComponent.m_OpenGLResource.m_Indices.end(), TempIndices.begin(), TempIndices.end());

	Scene.AddEntityToScene(LinetraceEntity);
}

void CEntityBuilder::AddLinetrace(glm::vec3 StartLocation, glm::vec3 EndLocation)
{
	CScene& Scene = CScene::GetScene();
	CGameEntity3D LinetraceEntity{};
	LinetraceEntity.m_GameEntityTag = "LinetraceEntity";
	CGameEntity3DComponent LinetraceComponent;
	LinetraceComponent.m_GameEntityTag = "LinetraceComponent";

	uint32_t& ShaderProgram{ LinetraceComponent.m_OpenGLResource.m_ShaderProgram };
	uint32_t& VAO{ LinetraceComponent.m_OpenGLResource.m_VAO };
	uint32_t& VBO{ LinetraceComponent.m_OpenGLResource.m_VBO };
	uint32_t& EBO{ LinetraceComponent.m_OpenGLResource.m_EBO };

	LinetraceComponent.m_OpenGLResource.m_Indices = { 0, 1 };

	CShaderGenerator ShaderGenerator;
	ShaderGenerator.GenerateBaseSolidShaders(&LinetraceComponent.m_OpenGLResource);
	glUseProgram(ShaderProgram);

	float Vertices[] = {
		// positions          // colors			
		 StartLocation.x, StartLocation.y, StartLocation.z,		1.0f, 0.0f, 0.0f,
		 EndLocation.x, EndLocation.y, EndLocation.z,			1.0f, 0.0f, 0.0f,
	};

	uint32_t Indices[] =
	{
		0, 1,
	};

	LinetraceComponent.SetLocationF(1.0f, 0.2f, 0.f);


	//glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// index attribute
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glUseProgram(ShaderProgram);

	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(FOV), (float)COpenGLDevice::m_ViewportWidth / COpenGLDevice::m_ViewportHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, std::string{ "projection" }.c_str()), 1, GL_FALSE, &ProjectionMatrix[0][0]);

	LinetraceComponent.m_OpenGLResource.m_DrawMode = GL_LINES;

	CSceneGraphNode<CGameEntity3DComponent>* LinetraceComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
	LinetraceComponentNode->m_tType = LinetraceComponent;
	LinetraceEntity.m_SceneGraph.m_pRootNode = LinetraceComponentNode;
	std::vector<uint32_t> TempIndices{ 0, 1, 3, 1, 2, 3 };
	//LinetraceComponent.m_OpenGLResource.m_Indices.insert(LinetraceComponent.m_OpenGLResource.m_Indices.end(), TempIndices.begin(), TempIndices.end());

	Scene.AddEntityToScene(LinetraceEntity);
}

void CEntityBuilder::AddTestEntity()
{
	CLogger& Logger = CLogger::GetLogger();
	//Logger.Log("EntityBuilder.cpp, AddTestEntity(): Adding entity");
	//Logger.Log("EntityBuilder.cpp, AddTestEntity(): Location: ", std::filesystem::current_path());

	CGameEntity3D TestEntity;
	TestEntity.m_GameEntityTag = "Knight";
	CGameEntity3DComponent TestEntityComponent;
	TestEntityComponent.m_GameEntityTag = "KnightComponent";

	InitEntityAttributes();
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
	glBufferData(GL_ARRAY_BUFFER, g_EntityVertices.size() * sizeof(float), g_EntityVertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_EntityIndices.size() * sizeof(float), g_EntityIndices.data(), GL_STATIC_DRAW);

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
	unsigned char* data = stbi_load("C:/Users/sefce/source/KMEngine/KMEngine/grey_grid.jpg", &width, &height, &nrChannels, 0);
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

	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(FOV), (float)COpenGLDevice::m_ViewportWidth / COpenGLDevice::m_ViewportHeight, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, std::string{ "projection" }.c_str()), 1, GL_FALSE, &ProjectionMatrix[0][0]);

	TestEntityComponent.m_OpenGLResource.m_Indices = g_EntityIndices;

	//auto TempOpenGLDevice = this->m_pGraphicsModule->m_Renderer.m_pRendererOpenGL->m_OpenGLDevice;
	Logger.Log("EntityBuilder.cpp, AddTestEntity(): Checking order of logs");

	CSceneGraphNode<CGameEntity3DComponent>* EntityComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
	EntityComponentNode->m_tType = TestEntityComponent;
	TestEntity.m_SceneGraph.m_pRootNode = EntityComponentNode;

	CScene& Scene = CScene::GetScene();
	Scene.AddEntityToScene(TestEntity);
}
void CEntityBuilder::ImportGLTF(std::string& FilePath, const std::string& FileContent)
{
	AddTestEntity();

	/*std::vector<CBufferViewBase*> BufferViewVector = m_ImporterGLTF.Import(FilePath, FileContent);
	CBufferView<float>* VerticesBuffer{ nullptr };
	CBufferView<float>* TexCoordsBuffer{ nullptr };
	CBufferView<uint32_t>* IndicesBuffer{ nullptr };

	std::vector<float> Vertices;
	std::vector<uint32_t> Indices;

	for (auto& BufferViewIt : BufferViewVector)
	{
		switch (BufferViewIt->m_BufferViewType)
		{
			case EAttributeType::Position:
			{
				VerticesBuffer = static_cast<CBufferView<float>*>(BufferViewIt);
				break;
			}

			case EAttributeType::TexCoords:
			{
				TexCoordsBuffer = static_cast<CBufferView<float>*>(BufferViewIt);
				break;
			}

			case EAttributeType::Indices:
			{
				IndicesBuffer = static_cast<CBufferView<uint32_t>*>(BufferViewIt);
				Indices = IndicesBuffer->m_Data;
				break;
			}
		}
	}

	int i{ 0 }, j{ 0 };

	if (VerticesBuffer != nullptr && TexCoordsBuffer != nullptr)
	{
		while (i < VerticesBuffer->m_Data.size())
		{
			Vertices.push_back(VerticesBuffer->m_Data[i]);
			Vertices.push_back(VerticesBuffer->m_Data[i + 1]);
			Vertices.push_back(VerticesBuffer->m_Data[i + 2]);

			while (j < TexCoordsBuffer->m_Data.size())
			{
				Vertices.push_back(TexCoordsBuffer->m_Data[j]);
				Vertices.push_back(TexCoordsBuffer->m_Data[j + 1]);
				break;
			}

			i += 3;
			j += 2;
		}
	}

	CreateModel(Vertices, Indices);

	auto Breakpoint = 1;*/
}

void CEntityBuilder::CreateModel(const std::vector<float>& Vertices, const std::vector<uint32_t>& Indices)
{
	CGameEntity3D TestEntity;
	TestEntity.m_GameEntityTag = "Knight";
	CGameEntity3DComponent TestEntityComponent;
	TestEntityComponent.m_GameEntityTag = "KnightComponent";

	uint32_t& ShaderProgram{ TestEntityComponent.m_OpenGLResource.m_ShaderProgram };
	uint32_t& VAO{ TestEntityComponent.m_OpenGLResource.m_VAO };
	uint32_t& VBO{ TestEntityComponent.m_OpenGLResource.m_VBO };
	uint32_t& EBO{ TestEntityComponent.m_OpenGLResource.m_EBO };

	CShaderGenerator ShaderGenerator;
	ShaderGenerator.GenerateBaseShaders(&TestEntityComponent.m_OpenGLResource);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), Vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// FIX: use sizeof(uint16_t) for index buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(float), Indices.data(), GL_STATIC_DRAW);

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

	if (this->m_pGraphicsModule)
	{
		if (this->m_pOpenGLDevice)
		{
			glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(FOV), (float)m_pOpenGLDevice->m_ViewportWidth / (float)m_pOpenGLDevice->m_ViewportHeight, 0.1f, 100.0f);
			glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, std::string{ "projection" }.c_str()), 1, GL_FALSE, &ProjectionMatrix[0][0]);
		}
	}


	TestEntityComponent.m_OpenGLResource.m_Indices = Indices;

	CSceneGraphNode<CGameEntity3DComponent>* EntityComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
	EntityComponentNode->m_tType = TestEntityComponent;
	TestEntity.m_SceneGraph.m_pRootNode = EntityComponentNode;

	CScene& Scene = CScene::GetScene();
	Scene.AddEntityToScene(TestEntity);
}

void CEntityBuilder::SetGraphicsModule(CGraphicsModule* GraphicsModule)
{
	m_pGraphicsModule = GraphicsModule;
}

void CEntityBuilder::SetOpenGLDevice(COpenGLDevice* pOpenGLDevice)
{
	m_pOpenGLDevice = pOpenGLDevice;
}


#pragma warning(disable: 4305)

#include "EntityBuilder.h"
#include "Modules/GraphicsModule.h"
#include "GLTFImporter.h"
#include "ShaderGenerator.h"
#include "Scene.h"
#include <glad/glad.h>
#include <filesystem>
#include "Core/CoreClock.h"

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
	//g_EntityVertices.reserve(104440);
	//g_EntityIndices.reserve(111408);
	std::string IndicesFilePath = "C:\\Users\\sefce\\source\\KMEngine\\KMEngine\\Indices.txt";
	std::string PosAndTexCoordFilePath = "C:\\Users\\sefce\\source\\KMEngine\\KMEngine\\Vertices.txt";
	g_EntityVertices = LoadEntityVerticesFromFile(PosAndTexCoordFilePath);
	g_EntityIndices = LoadEntityIndicesFromFile(IndicesFilePath);
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

	LinetraceComponent.SetLocationF(0.0f, 0.0f, 0.f);


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

void CEntityBuilder::InterpMoveTestEntity()
{
	CLogger& Logger = CLogger::GetLogger();
	CTimerManager& TimerManager = CTimerManager::GetTimerManager();
	CScene& Scene = CScene::GetScene();

	for (auto& SceneEntityIt : Scene.GetSceneList())
	{
		if (SceneEntityIt.m_GameEntityTag == "Knight")
		{
			m_pEntity = &SceneEntityIt;
			break;
		}
	}

	m_InterpMoveLoc = FInterpConstantTo(m_InterpMoveLoc, -10.0f, TimerManager.m_pCoreClock->GetFDeltaTime(), 1.0f);
	m_pEntity->SetLocationF(m_pEntity->GetLocationX() + m_InterpMoveLoc, m_pEntity->GetLocationY(), m_pEntity->GetLocationZ());
	CGameEntity3DComponent& EntityComponent = m_pEntity->m_SceneGraph.m_pRootNode->m_tType;
	//m_pEntity->m_SceneGraph.m_pRootNode->m_tType.SetLocationF(EntityComponent.GetLocationX() + m_InterpMoveLoc, EntityComponent.GetLocationY(), EntityComponent.GetLocationZ());
	m_pEntity->m_SceneGraph.m_pRootNode->m_tType.m_LocationX = m_InterpMoveLoc;
	//EntityComponent.SetLocationF(EntityComponent.GetLocationX() + m_InterpMoveLoc, EntityComponent.GetLocationY(), EntityComponent.GetLocationZ());

	Logger.Log("EntityBuilder.cpp, void InterpMoveEntity(): InterpMoveLoc = ", m_InterpMoveLoc);
	Logger.Log("EntityBuilder.cpp, void InterpMoveEntity(): X = ", m_pEntity->m_SceneGraph.m_pRootNode->m_tType.GetLocationX());
}

void CEntityBuilder::ImportGLTF(std::string& FilePath, const std::string& FileContent)
{

	m_FileContent = FileContent;
	m_FilePath = FilePath;
	//AddTestEntity();
		
	//InitEntityAttributes();

	std::vector<SSkeletalVertex> Vertices;
	std::vector<uint32_t> Indices;

	std::vector<CBufferViewBase*> BufferViewVector = m_GLTFImporter.Import(FilePath, FileContent);

	CBufferView<float>* VerticesBuffer = static_cast<CBufferView<float>*>(m_GLTFImporter.GetBufferViewByType(EAttributeType::Position));
	CBufferView<float>* TexCoordsBuffer = static_cast<CBufferView<float>*>(m_GLTFImporter.GetBufferViewByType(EAttributeType::TexCoords));
	CBufferView<uint8_t>* JointsBuffer = static_cast<CBufferView<uint8_t>*>(m_GLTFImporter.GetBufferViewByType(EAttributeType::Joints));
	CBufferView<float>* WeightsBuffer = static_cast<CBufferView<float>*>(m_GLTFImporter.GetBufferViewByType(EAttributeType::Weights));
	CBufferView<uint32_t>* IndicesBuffer = static_cast<CBufferView<uint32_t>*>(m_GLTFImporter.GetBufferViewByType(EAttributeType::Indices));

	uint64_t VerticesBufferSize = VerticesBuffer->m_Data.size();
	uint64_t TexCoordsBufferSize = TexCoordsBuffer->m_Data.size();
	uint64_t JointsBufferSize = JointsBuffer->m_Data.size();
	uint64_t WeightsBufferSize = WeightsBuffer->m_Data.size();
	uint64_t IndicesBufferSize = IndicesBuffer->m_Data.size();

	uint32_t i{ 0 }, j{ 0 };

	if (VerticesBuffer != nullptr && TexCoordsBuffer != nullptr)
	{
		Vertices.resize(VerticesBufferSize / 3);
		for (int i = 0; i < VerticesBufferSize / 3; ++i)
		{
			Vertices.at(i).m_Position = glm::vec3(VerticesBuffer->m_Data.at(i * 3),
				VerticesBuffer->m_Data.at(i * 3 + 1),
				VerticesBuffer->m_Data.at(i * 3 + 2));
		}

		Vertices.resize(VerticesBufferSize / 3);
		for (int i = 0; i < VerticesBufferSize / 3; ++i)
		{
			Vertices.at(i).m_Position = glm::vec3(VerticesBuffer->m_Data.at(i * 3),
				VerticesBuffer->m_Data.at(i * 3 + 1),
				VerticesBuffer->m_Data.at(i * 3 + 2));
		}

		for (int i = 0; i < JointsBufferSize / 4; ++i)
		{
			Vertices.at(i).m_Joint = glm::vec4(JointsBuffer->m_Data.at(i * 4),
				JointsBuffer->m_Data.at(i * 4 + 1),
				JointsBuffer->m_Data.at(i * 4 + 2),
				JointsBuffer->m_Data.at(i * 4 + 3));
		}

		for (int i = 0; i < WeightsBufferSize / 4; ++i)
		{
			Vertices.at(i).m_Weight = glm::vec4(WeightsBuffer->m_Data.at(i * 4),
				WeightsBuffer->m_Data.at(i * 4 + 1),
				WeightsBuffer->m_Data.at(i * 4 + 2),
				WeightsBuffer->m_Data.at(i * 4 + 3));
		}

		for (int i = 0; i < VerticesBufferSize / 3; ++i)
		{
			Vertices.at(i).m_TexCoords = glm::vec2(TexCoordsBuffer->m_Data.at(i * 2),
				TexCoordsBuffer->m_Data.at(i * 2 + 1));
		}

		for (int i = 0; i < IndicesBufferSize; ++i)
		{
			Indices = IndicesBuffer->m_Data;
		}
	}

	CreateModel(Vertices, Indices);

	auto Breakpoint = 1;
}

void CEntityBuilder::ImportGLTFAnimation()
{
	CGLTFAnimation Animation = m_GLTFImporter.ImportAnimation(m_FilePath, m_FileContent);

	auto breakpoint = 1;
}

CGLTFAnimation CEntityBuilder::GetGLTFAnimation()
{
	CGLTFAnimation Animation = m_GLTFImporter.ImportAnimation(m_FilePath, m_FileContent);

	return Animation;
}

void CEntityBuilder::CreateModel(const std::vector<SSkeletalVertex>& Vertices, const std::vector<uint32_t>& Indices)
{
	CScene& Scene = CScene::GetScene();
	CGameEntity3D Entity{};
	Entity.m_GameEntityTag = "Knight";
	CGameEntity3DComponent EntityComponent;
	EntityComponent.m_GameEntityTag = "KnightComponent";

	uint32_t& ShaderProgram{ EntityComponent.m_OpenGLResource.m_ShaderProgram };
	uint32_t& VAO{ EntityComponent.m_OpenGLResource.m_VAO };
	uint32_t& VBO{ EntityComponent.m_OpenGLResource.m_VBO };
	uint32_t& EBO{ EntityComponent.m_OpenGLResource.m_EBO };
	uint32_t& Texture{ EntityComponent.m_OpenGLResource.m_Texture };

	CShaderGenerator ShaderGenerator;
	ShaderGenerator.GenerateSkeletalMeshShaders(&EntityComponent.m_OpenGLResource);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	EntityComponent.SetLocationF(0.f, 0.f, -5.5f);
	EntityComponent.m_CollisionComponent.m_Center = glm::vec3{ 0.f, 0.f, -10.5f };
	EntityComponent.m_CollisionComponent.m_Extents = glm::vec3{ 0.5f, 0.5f, 0.5f };

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(SSkeletalVertex), Vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(uint32_t), Indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SSkeletalVertex), (void*)offsetof(SSkeletalVertex, m_Position));
	glEnableVertexAttribArray(0);

	glVertexAttribIPointer(1, 4, GL_INT, sizeof(SSkeletalVertex), (void*)offsetof(SSkeletalVertex, m_Joint));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(SSkeletalVertex), (void*)offsetof(SSkeletalVertex, m_Weight));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(SSkeletalVertex), (void*)offsetof(SSkeletalVertex, m_TexCoords));
	glEnableVertexAttribArray(3);

	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

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

	// Activate the shader program BEFORE setting any uniforms
	glUseProgram(ShaderProgram);

	glm::mat4 IdentityMatrix{ 1.f };

	glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, "bones[0]"), 1, GL_FALSE, &IdentityMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, "bones[1]"), 1, GL_FALSE, &IdentityMatrix[0][0]);

	glUniform1i(glGetUniformLocation(ShaderProgram, "material.diffuse"), 0);

	if (m_pOpenGLDevice)
	{
		glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, std::string{ "projection" }.c_str()), 1, GL_FALSE, &COpenGLDevice::g_ProjectionMatrix[0][0]);
	}

	EntityComponent.m_OpenGLResource.m_DrawMode = GL_TRIANGLES;

	CSceneGraphNode<CGameEntity3DComponent>* TerrainComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
	TerrainComponentNode->m_tType = EntityComponent;
	Entity.m_SceneGraph.m_pRootNode = TerrainComponentNode;

	Scene.AddEntityToScene(Entity);
}

void CEntityBuilder::SetGraphicsModule(CGraphicsModule* GraphicsModule)
{
	m_pGraphicsModule = GraphicsModule;
}

void CEntityBuilder::SetOpenGLDevice(COpenGLDevice* pOpenGLDevice)
{
	m_pOpenGLDevice = pOpenGLDevice;
}

void CEntityBuilder::CreateLight()
{
	CScene& Scene = CScene::GetScene();
	CGameEntity3D Terrain{};
	Terrain.m_GameEntityTag = "Terrain";
	CGameEntity3DComponent TerrainComponent;
	TerrainComponent.m_GameEntityTag = "TerrainComponent";

	uint32_t& ShaderProgram{ TerrainComponent.m_OpenGLResource.m_ShaderProgram };
	uint32_t& VAO{ TerrainComponent.m_OpenGLResource.m_VAO };
	uint32_t& VBO{ TerrainComponent.m_OpenGLResource.m_VBO };
	uint32_t& EBO{ TerrainComponent.m_OpenGLResource.m_EBO };

	CShaderGenerator ShaderGenerator;
	ShaderGenerator.GenerateBaseShaders(&TerrainComponent.m_OpenGLResource);

	float Vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   /*1.0f, 0.0f, 0.0f,*/   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   /*0.0f, 1.0f, 0.0f,*/   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   /*0.0f, 0.0f, 1.0f,*/   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   /*1.0f, 1.0f, 0.0f,*/   0.0f, 1.0f  // top left 
	};

	uint32_t Indices[] =
	{
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	TerrainComponent.SetLocationF(0.f, 0.f, -4.5f);

	//glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// index attribute
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
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
		glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, std::string{ "projection" }.c_str()), 1, GL_FALSE, &COpenGLDevice::g_ProjectionMatrix[0][0]);
	}

	TerrainComponent.m_OpenGLResource.m_DrawMode = GL_TRIANGLES;

	CSceneGraphNode<CGameEntity3DComponent>* TerrainComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
	TerrainComponentNode->m_tType = TerrainComponent;
	Terrain.m_SceneGraph.m_pRootNode = TerrainComponentNode;
	std::vector<uint32_t> TempIndices{ 0, 1, 3, 1, 2, 3 };
	TerrainComponent.m_OpenGLResource.m_Indices.insert(TerrainComponent.m_OpenGLResource.m_Indices.end(), TempIndices.begin(), TempIndices.end());

	Scene.AddEntityToScene(Terrain);
}
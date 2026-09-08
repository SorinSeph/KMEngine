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
			uint8_t x = JointsBuffer->m_Data.at(i * 4);
			uint8_t y = JointsBuffer->m_Data.at(i * 4 + 1);
			uint8_t z = JointsBuffer->m_Data.at(i * 4 + 2);
			uint8_t w = JointsBuffer->m_Data.at(i * 4 + 3);

			Vertices.at(i).m_Joint = glm::vec4{x, y, z, w};

			//Vertices.at(i).m_Joint = glm::vec4((float)m_GLTFImporter.m_JointArrayMap.find(x)->second,
			//	(float)m_GLTFImporter.m_JointArrayMap.find(y)->second,
			//	(float)m_GLTFImporter.m_JointArrayMap.find(z)->second,
			//	(float)m_GLTFImporter.m_JointArrayMap.find(w)->second);
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

		//__debugbreak();
	}

	CreateModel(Vertices, Indices);
}

CGLTFAnimation CEntityBuilder::GetGLTFAnimation()
{
	CGLTFAnimation Animation = m_GLTFImporter.ImportAnimation(m_FilePath, m_FileContent);

	return Animation;
}

CGLTFAnimation* CEntityBuilder::GetGLTFpAnimation()
{
	CGLTFAnimation* pAnimation = m_GLTFImporter.GetAnimation();

	return pAnimation;
}

void CEntityBuilder::CreateModel(const std::vector<SSkeletalVertex>& Vertices, const std::vector<uint32_t>& Indices)
{
	CScene& Scene = CScene::GetScene();
	CLogger& Logger = CLogger::GetLogger();
	CGameEntity3D Entity{};
	Entity.m_GameEntityTag = "Player";
	CGameEntity3DComponent EntityComponent;
	EntityComponent.m_GameEntityTag = "PlayerComponent";

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

	//glVertexAttribIPointer(1, 4, GL_INT, sizeof(SSkeletalVertex), (void*)offsetof(SSkeletalVertex, m_Joint));
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SSkeletalVertex), (void*)offsetof(SSkeletalVertex, m_Joint));
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

	std::string TexturePath{ "C:\\Users\\sefce\\source\\KMEngine\\KMEngine\\grey_grid.jpg" };
	int width, height, nrChannels;
	unsigned char* data = stbi_load(TexturePath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		Logger.Log("Sucessfully loaded texture located at ", std::filesystem::absolute(TexturePath));
	}
	else
	{
		Logger.Log("Failed to load texture in CreateModel function");
	}
	stbi_image_free(data);

	glUseProgram(ShaderProgram);

	glm::mat4 IdentityMatrix{ 1.f };

	for (int i = 0; i < 63; i++)
	{
		std::string BoneName{ "bones[" + std::to_string(i) + "]"};
		glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, BoneName.c_str()), 1, GL_FALSE, &IdentityMatrix[0][0]);
	}

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

void CEntityBuilder::CreateGizmoOpenGL(glm::vec3 Location)
{
	CPrimitiveGeometryFactory PrimitiveGeometryFactory;
	std::vector<glm::vec3> ArrowVertices = PrimitiveGeometryFactory.GetArrowVertices(); // 50 elements
	std::vector<uint32_t> ArrowIndices = PrimitiveGeometryFactory.GetArrowIndices();

	CScene& Scene = CScene::GetScene();
	CGameEntity3D Gizmo{};
	Gizmo.m_GameEntityTag = "Gizmo";
	CGameEntity3DComponent GizmoComponentX;
	CGameEntity3DComponent GizmoComponentY;
	CGameEntity3DComponent GizmoComponentZ;

	GizmoComponentX.m_GameEntityTag = "GizmoComponentX";
	GizmoComponentY.m_GameEntityTag = "GizmoComponentY";
	GizmoComponentZ.m_GameEntityTag = "GizmoComponentZ";

	uint32_t& ShaderProgram{ GizmoComponentX.m_OpenGLResource.m_ShaderProgram };
	uint32_t& VAO_X{ GizmoComponentX.m_OpenGLResource.m_VAO };
	uint32_t& VBO_X{ GizmoComponentX.m_OpenGLResource.m_VBO };
	uint32_t& EBO_X{ GizmoComponentX.m_OpenGLResource.m_EBO };

	uint32_t& ShaderProgramY{ GizmoComponentY.m_OpenGLResource.m_ShaderProgram };
	uint32_t& VAO_Y{ GizmoComponentY.m_OpenGLResource.m_VAO };
	uint32_t& VBO_Y{ GizmoComponentY.m_OpenGLResource.m_VBO };
	uint32_t& EBO_Y{ GizmoComponentY.m_OpenGLResource.m_EBO };

	uint32_t& ShaderProgramZ{ GizmoComponentZ.m_OpenGLResource.m_ShaderProgram };
	uint32_t& VAO_Z{ GizmoComponentZ.m_OpenGLResource.m_VAO };
	uint32_t& VBO_Z{ GizmoComponentZ.m_OpenGLResource.m_VBO };
	uint32_t& EBO_Z{ GizmoComponentZ.m_OpenGLResource.m_EBO };

	CShaderGenerator ShaderGenerator;
	ShaderGenerator.GenerateHoverableSolidColorShaders(&GizmoComponentX.m_OpenGLResource);
	ShaderGenerator.GenerateHoverableSolidColorShaders(&GizmoComponentY.m_OpenGLResource);
	ShaderGenerator.GenerateHoverableSolidColorShaders(&GizmoComponentZ.m_OpenGLResource);

	float HalfWidth = 0.5f;
	float HalfHeight = 0.5f;
	float HalfLength = 0.2f;
	glm::vec3 LocalOriginX{ Location.x + 0.005f, Location.y, Location.z };
	glm::vec3 LocalOriginY{ Location.x, Location.y + 0.005f, Location.z };
	glm::vec3 LocalOriginZ{ Location.x, Location.y, Location.z - 0.005f };

	// Build interleaved vertex data: position (vec3) + color (vec3)
	std::vector<float> Vertices;
	Vertices.reserve(ArrowVertices.size() * 6);
	glm::vec3 ColorVector{1.0f, 0.0f, 0.0f};
	auto& ColorVectorRef = ColorVector;

	for (size_t i = 0; i < ArrowVertices.size(); i++)
	{
		Vertices.push_back(ArrowVertices[i].x);
		Vertices.push_back(ArrowVertices[i].y);
		Vertices.push_back(ArrowVertices[i].z);

		Vertices.push_back(ColorVectorRef.x);
		Vertices.push_back(ColorVectorRef.y);
		Vertices.push_back(ColorVectorRef.z);
	}

	GizmoComponentX.m_CollisionComponent.m_Center = LocalOriginX;
	GizmoComponentX.m_CollisionComponent.m_Extents = glm::vec3{ HalfWidth, HalfHeight, HalfLength };

	GizmoComponentX.SetLocationF(LocalOriginX.x, LocalOriginX.y, LocalOriginX.z);
	GizmoComponentY.SetLocationF(LocalOriginY.x, LocalOriginY.y, LocalOriginY.z);
	GizmoComponentZ.SetLocationF(LocalOriginZ.x, LocalOriginZ.y, LocalOriginZ.z);
	GizmoComponentX.SetScale(0.005f, 0.005f, 0.005f);

	//GizmoComponentX.SetRotation(GizmoComponentX.GetRotationX(), GizmoComponentX.GetRotationY(), 90);
	GizmoComponentX.SetRotation(0, 0, 90);
	GizmoComponentY.SetRotation(0, 0, 180);
	GizmoComponentZ.SetRotation(0, -90, 0);

	glGenVertexArrays(1, &VAO_X);
	glGenBuffers(1, &VBO_X);
	glGenBuffers(1, &EBO_X);
	glBindVertexArray(VAO_X);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_X);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), Vertices.data(), GL_STATIC_DRAW);

	// position attribute (location 0, vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute (location 1, vec3)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_X);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ArrowIndices.size() * sizeof(uint32_t), ArrowIndices.data(), GL_STATIC_DRAW);

	glUseProgram(ShaderProgram);
	if (m_pOpenGLDevice)
	{
		glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, std::string{ "projection" }.c_str()), 1, GL_FALSE, &COpenGLDevice::g_ProjectionMatrix[0][0]);
	}

	// Y arrow

	for (int It = 3; It + 1 < Vertices.size(); It += 6)
	{
		Vertices.at(It) = 0.0f;
		Vertices.at(It + 1) = 1.0f;
	}

	glGenVertexArrays(1, &VAO_Y);
	glGenBuffers(1, &VBO_Y);
	glGenBuffers(1, &EBO_Y);
	glBindVertexArray(VAO_Y);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Y);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), Vertices.data(), GL_STATIC_DRAW);

	// position attribute (location 0, vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute (location 1, vec3)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Y);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ArrowIndices.size() * sizeof(uint32_t), ArrowIndices.data(), GL_STATIC_DRAW);

	glUseProgram(ShaderProgramY);
	if (m_pOpenGLDevice)
	{
		glUniformMatrix4fv(glGetUniformLocation(ShaderProgramY, std::string{ "projection" }.c_str()), 1, GL_FALSE, &COpenGLDevice::g_ProjectionMatrix[0][0]);
	}

	// Z Arrow buffers

	for (int It = 4; It + 1 < Vertices.size(); It += 6)
	{
		Vertices.at(It) = 0.0f;
		Vertices.at(It + 1) = 1.0f;
	}

	glGenVertexArrays(1, &VAO_Z);
	glGenBuffers(1, &VBO_Z);
	glGenBuffers(1, &EBO_Z);
	glBindVertexArray(VAO_Z);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Z);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), Vertices.data(), GL_STATIC_DRAW);

	// position attribute (location 0, vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute (location 1, vec3)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Z);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ArrowIndices.size() * sizeof(uint32_t), ArrowIndices.data(), GL_STATIC_DRAW);

	glUseProgram(ShaderProgramZ);
	if (m_pOpenGLDevice)
	{
		glUniformMatrix4fv(glGetUniformLocation(ShaderProgramY, std::string{ "projection" }.c_str()), 1, GL_FALSE, &COpenGLDevice::g_ProjectionMatrix[0][0]);
	}

	GizmoComponentX.m_OpenGLResource.m_DrawMode = GL_TRIANGLES;
	GizmoComponentY.m_OpenGLResource.m_DrawMode = GL_TRIANGLES;
	GizmoComponentZ.m_OpenGLResource.m_DrawMode = GL_TRIANGLES;

	GizmoComponentX.m_OpenGLResource.m_Indices = ArrowIndices;
	GizmoComponentY.m_OpenGLResource.m_Indices = ArrowIndices;
	GizmoComponentZ.m_OpenGLResource.m_Indices = ArrowIndices;

	CSceneGraphNode<CGameEntity3DComponent>* GizmoComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
	CSceneGraphNode<CGameEntity3DComponent>* GizmoComponentNodeY = new CSceneGraphNode<CGameEntity3DComponent>();
	CSceneGraphNode<CGameEntity3DComponent>* GizmoComponentNodeZ = new CSceneGraphNode<CGameEntity3DComponent>();

	GizmoComponentNode->m_tType = GizmoComponentX;
	GizmoComponentNodeY->m_tType = GizmoComponentY;
	GizmoComponentNodeZ->m_tType = GizmoComponentZ;
	Gizmo.m_SceneGraph.m_pRootNode = GizmoComponentNode;
	Gizmo.m_SceneGraph.AddChild(Gizmo.m_SceneGraph.m_pRootNode, GizmoComponentNodeY);
	Gizmo.m_SceneGraph.AddChild(Gizmo.m_SceneGraph.m_pRootNode, GizmoComponentNodeZ);

	Scene.AddEntityToScene(Gizmo);
}

void CEntityBuilder::CreateLight()
{
	CScene& Scene = CScene::GetScene();
	CGameEntity3D Light{};
	Light.m_GameEntityTag = "Light";
	CGameEntity3DComponent LightComponent;
	LightComponent.m_GameEntityTag = "LightComponent";

	uint32_t& ShaderProgram{ LightComponent.m_OpenGLResource.m_ShaderProgram };
	uint32_t& VAO{ LightComponent.m_OpenGLResource.m_VAO };
	uint32_t& VBO{ LightComponent.m_OpenGLResource.m_VBO };
	uint32_t& Texture{ LightComponent.m_OpenGLResource.m_Texture };

	CShaderGenerator ShaderGenerator;
	ShaderGenerator.GenerateBaseShaders(&LightComponent.m_OpenGLResource);

	float HalfWidth = 0.5f;
	float HalfHeight = 0.5f;
	float HalfLength = 0.2f;
	glm::vec3 LocalOrigin{ 1.f, 0.f, -4.5f };

	float Vertices[] = {
		// positions						// texture coords
		-HalfHeight, -HalfHeight,  0.f,		0.0f,  0.0f,
		 HalfHeight, -HalfHeight,  0.f,		1.0f,  0.0f,
		 HalfHeight,  HalfHeight,  0.f,		1.0f,  1.0f,
		 HalfHeight,  HalfHeight,  0.f,		1.0f,  1.0f,
		-HalfHeight,  HalfHeight,  0.f,		0.0f,  1.0f,
		-HalfHeight, -HalfHeight,  0.f,		0.0f,  0.0f,
	};

	LightComponent.m_CollisionComponent.m_Center = LocalOrigin;
	LightComponent.m_CollisionComponent.m_Extents = glm::vec3{ HalfWidth, HalfHeight, HalfLength};

	LightComponent.SetLocationF(LocalOrigin.x, LocalOrigin.y, LocalOrigin.z);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("Resources/Assets/Icons/Bulb4.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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

	LightComponent.m_OpenGLResource.m_DrawMode = GL_TRIANGLES;

	CSceneGraphNode<CGameEntity3DComponent>* TerrainComponentNode = new CSceneGraphNode<CGameEntity3DComponent>();
	TerrainComponentNode->m_tType = LightComponent;
	Light.m_SceneGraph.m_pRootNode = TerrainComponentNode;

	Scene.AddEntityToScene(Light);
}
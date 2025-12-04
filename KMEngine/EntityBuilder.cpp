#include "EntityBuilder.h"
#include "Modules/GraphicsModule.h"
#include "ImporterGLTF.h"
#include "ShaderGenerator.h"
#include <glad/glad.h>

void CEntityBuilder::ImportGLTF(std::string& FilePath, const std::string& FileContent)
{
	std::vector<CBufferViewBase*> BufferViewVector = m_ImporterGLTF.Import(FilePath, FileContent);
	CBufferView<float>* VerticesBuffer{ nullptr };
	CBufferView<float>* TexCoordsBuffer{ nullptr };
	CBufferView<uint16_t>* IndicesBuffer{ nullptr };

	std::vector<float> Vertices;
	std::vector<uint16_t> Indices;

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
				IndicesBuffer = static_cast<CBufferView<uint16_t>*>(BufferViewIt);
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

	auto Breakpoint = 1;
}

void CEntityBuilder::CreateModel(const std::vector<float>& Vertices, const std::vector<uint16_t>& Indices)
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(uint16_t), Indices.data(), GL_STATIC_DRAW);

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

	//if (this->m_GraphicsModule)
	//{
	//	if (this->m_GraphicsModule->m_Renderer.m_pRendererOpenGL)
	//	{
	//		auto OpenGLDevice = this->m_GraphicsModule->m_Renderer.m_pRendererOpenGL->m_OpenGLDevice;
	//		(void)OpenGLDevice;
	//	}
	//}
}

void CEntityBuilder::SetGraphicsModule(CGraphicsModule* GraphicsModule)
{
	m_GraphicsModule = GraphicsModule;
}
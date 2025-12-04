#pragma once
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include "Logger.h"

enum class EAttributeType
{
	Position,
	Normal,
	TexCoords,
	Color,
	Joints,
	Weights,
	Indices
};

enum class EValueType
{
	Byte,
	UnsignedByte,
	Short,
	UnsginedShort,
};

class CBufferViewBase
{
public:
	EAttributeType m_BufferViewType{};
	std::string m_BufferIndex{};
	size_t m_Count{};
	size_t m_ByteOffset{};
	size_t m_ByteLength{};
};

template <typename T>
class CBufferView : public CBufferViewBase
{
public:
	std::vector<T> m_Data{};
};

class CImporterGLTF
{
public:
	std::vector<CBufferViewBase*> Import(std::string& FilePath, const std::string& FileContent)
	{
		for (auto AttributeTypeIt : m_AttributeType)
		{
			std::string Index = GetBufferViewStringIndex(FileContent, AttributeTypeIt);
			if (!Index.empty())
			{
				if (AttributeTypeIt == "\"POSITION\":")
				{
					CBufferView<float>* pBufferView = new CBufferView<float>();
					pBufferView->m_BufferIndex = Index;
					pBufferView->m_BufferViewType = EAttributeType::Position;
					m_pBufferViews.push_back(pBufferView);
				}
				else if (AttributeTypeIt == "\"NORMAL\":")
				{
					CBufferView<float>* pBufferView = new CBufferView<float>();
					pBufferView->m_BufferIndex = Index;
					pBufferView->m_BufferViewType = EAttributeType::Normal;
					m_pBufferViews.push_back(pBufferView);
				}
				else if (AttributeTypeIt == "\"TEXCOORD_0\":")
				{
					CBufferView<float>* pBufferView = new CBufferView<float>();
					pBufferView->m_BufferIndex = Index;
					pBufferView->m_BufferViewType = EAttributeType::TexCoords;
					m_pBufferViews.push_back(pBufferView);
				}
				else if (AttributeTypeIt == "\"COLOR_0\":")
				{
					CBufferView<uint8_t>* pBufferView = new CBufferView<uint8_t>();
					pBufferView->m_BufferIndex = Index;
					pBufferView->m_BufferViewType = EAttributeType::Color;
					m_pBufferViews.push_back(pBufferView);
				}
				else if (AttributeTypeIt == "\"JOINTS_0\":")
				{
					CBufferView<uint8_t>* pBufferView = new CBufferView<uint8_t>();
					pBufferView->m_BufferIndex = Index;
					pBufferView->m_BufferViewType = EAttributeType::Joints; 
					m_pBufferViews.push_back(pBufferView);
				}
				else if (AttributeTypeIt == "\"WEIGHTS_0\":")
				{
					CBufferView<float>* pBufferView = new CBufferView<float>();
					pBufferView->m_BufferIndex = Index;
					pBufferView->m_BufferViewType = EAttributeType::Weights;
					m_pBufferViews.push_back(pBufferView);
				}
				else if (AttributeTypeIt == "\"indices\":")
				{
					CBufferView<uint16_t>* pBufferView = new CBufferView<uint16_t>();
					pBufferView->m_BufferIndex = Index;
					pBufferView->m_BufferViewType = EAttributeType::Indices;
					m_pBufferViews.push_back(pBufferView);
				}
			}
		}

		std::string BufferViewsData;
		std::string BufferViewSection = "\"accessors\":[";
		size_t BufferViewSectionIndex = FileContent.find(BufferViewSection);
		if (BufferViewSectionIndex != std::string::npos)
		{
			BufferViewSectionIndex += BufferViewSection.length();
			BufferViewsData.append(FileContent.substr(BufferViewSectionIndex + 1));
		}

		for (auto& BufferViewIt : m_pBufferViews)
		{
			switch (BufferViewIt->m_BufferViewType)
			{
				case EAttributeType::Position:
				{
					BufferViewIt->m_Count = GetBufferViewCount(BufferViewIt->m_BufferIndex, BufferViewsData);
					// Bytelength = count * 4 * 3 (count * 4 bytes from 32 bit float * VEC3 
					BufferViewIt->m_ByteLength = BufferViewIt->m_Count * 4 * 3;
					BufferViewIt->m_ByteOffset = GetByteOffset(BufferViewIt->m_BufferIndex, BufferViewsData);
					break;
				}

				case EAttributeType::Indices:
				{
					BufferViewIt->m_Count = GetBufferViewCount(BufferViewIt->m_BufferIndex, BufferViewsData);
					// Bytelength = count * 2 (count * 2 bytes from 16 bit unsigned short)
					BufferViewIt->m_ByteLength = BufferViewIt->m_Count * 2;
					BufferViewIt->m_ByteOffset = GetByteOffset(BufferViewIt->m_BufferIndex, BufferViewsData);
					break;
				}

				case EAttributeType::TexCoords:
				{
					BufferViewIt->m_Count = GetBufferViewCount(BufferViewIt->m_BufferIndex, BufferViewsData);
					// Bytelength = count * 4 * 2 (count * 4 bytes from 32 bit float * VEC2)
					BufferViewIt->m_ByteLength = BufferViewIt->m_Count * 4 * 2;
					BufferViewIt->m_ByteOffset = GetByteOffset(BufferViewIt->m_BufferIndex, BufferViewsData);
					break;
				}

				case EAttributeType::Joints:
				{
					BufferViewIt->m_Count = GetBufferViewCount(BufferViewIt->m_BufferIndex, BufferViewsData);
					// Bytelength = count * 4 * 2 (count * VEC4)
					BufferViewIt->m_ByteLength = BufferViewIt->m_Count * 4;
					BufferViewIt->m_ByteOffset = GetByteOffset(BufferViewIt->m_BufferIndex, BufferViewsData);
					break;
				}

				case EAttributeType::Weights:
				{
					BufferViewIt->m_Count = GetBufferViewCount(BufferViewIt->m_BufferIndex, BufferViewsData);
					// Bytelength = count * 4 * 2 (count * 4 bytes from 32 bit float * VEC4)
					BufferViewIt->m_ByteLength = BufferViewIt->m_Count * 4 * 4;
					BufferViewIt->m_ByteOffset = GetByteOffset(BufferViewIt->m_BufferIndex, BufferViewsData);
					break;
				}
			}
		}

		ReadBIN(FilePath);

		return m_pBufferViews;
	}

	size_t GetBufferViewCount(const std::string& BufferIndex, const std::string& BufferViewsData)
	{
		size_t BufferViewCount{ 0 };
		std::string SearchString = "\"bufferView\":" + BufferIndex;
		int AttributeIndex = BufferViewsData.find(SearchString);
		if (AttributeIndex != std::string::npos)
		{
			while (AttributeIndex < BufferViewsData.size())
			{
				if (BufferViewsData[AttributeIndex] == 'c')
				{
					if (BufferViewsData[AttributeIndex + 1] == 'o'
						&& BufferViewsData[AttributeIndex + 2] == 'u'
						&& BufferViewsData[AttributeIndex + 3] == 'n'
						&& BufferViewsData[AttributeIndex + 4] == 't')
					{
						AttributeIndex += 7;
						break;
					}
					else
					{
						AttributeIndex++;
					}
				}
				else
				{
					AttributeIndex++;
				}
			}

			std::string BufferViewCountString{};
			while (std::isdigit(BufferViewsData[AttributeIndex]))
			{
				BufferViewCountString += BufferViewsData[AttributeIndex];
				AttributeIndex++;
			}

			BufferViewCount = size_t(std::atoi(BufferViewCountString.c_str()));
		}

		return BufferViewCount;
	}

	size_t GetByteOffset(const std::string& BufferIndexString, const std::string& BufferViewsData)
	{
		size_t ByteOffset{ 0 };
		std::string SearchString = "\"bufferViews\":[";
		size_t BufferViewsPosition = BufferViewsData.find(SearchString);
		size_t OpenBracketCount = -1;
		size_t BufferIndex = size_t(std::atoi(BufferIndexString.c_str()));

		while (BufferViewsPosition < BufferViewsData.size())
		{
			if (BufferViewsData[BufferViewsPosition] == '{')
			{
				OpenBracketCount++;

				if (OpenBracketCount == BufferIndex)
				{
					break;
				}
			}

			BufferViewsPosition++;
		}

		while (BufferViewsPosition < BufferViewsData.size())
		{
			if (BufferViewsData[BufferViewsPosition] == 'b'
				&& BufferViewsData[BufferViewsPosition + 1] == 'y'
				&& BufferViewsData[BufferViewsPosition + 2] == 't'
				&& BufferViewsData[BufferViewsPosition + 3] == 'e'
				&& BufferViewsData[BufferViewsPosition + 4] == 'O'
				&& BufferViewsData[BufferViewsPosition + 5] == 'f'
				&& BufferViewsData[BufferViewsPosition + 6] == 'f'
				&& BufferViewsData[BufferViewsPosition + 7] == 's'
				&& BufferViewsData[BufferViewsPosition + 8] == 'e'
				&& BufferViewsData[BufferViewsPosition + 9] == 't'
				&& BufferIndex)
			{
				BufferViewsPosition += 12;
				break;
			}
			else
			{
				BufferViewsPosition++;
			}
		}		
		
		std::string ByteOffsetString{};
		while (std::isdigit(BufferViewsData[BufferViewsPosition]))
		{
			ByteOffsetString += BufferViewsData[BufferViewsPosition];
			BufferViewsPosition++;
		}

		ByteOffset = size_t(std::atoi(ByteOffsetString.c_str()));

		return ByteOffset;
	}

	std::string GetBufferViewStringIndex(const std::string& FileContent, const std::string& AttributeString)
	{
		std::string AttributeBufferIndex{};

		int AttributeIndex = FileContent.find(AttributeString);
		if (AttributeIndex != std::string::npos)
		{
			AttributeIndex += AttributeString.length();

			while (std::isdigit(FileContent[AttributeIndex]))
			{
				AttributeBufferIndex += FileContent[AttributeIndex];
				AttributeIndex++;
			}
		}

		return AttributeBufferIndex;
	}

	void ReadBIN(std::string& FilePath)
	{
		FilePath.erase(FilePath.end() - 4, FilePath.end());
		FilePath.append("bin");

		std::ifstream File(FilePath, std::ios::in | std::ios::binary);
		std::vector<uint8_t> Buffer((std::istreambuf_iterator<char>(File)), std::istreambuf_iterator<char>());

		for (auto& BufferViewIt : m_pBufferViews)
		{
			switch (BufferViewIt->m_BufferViewType)
			{
				case EAttributeType::Position:
				{
					CBufferView<float>* BufferView = static_cast<CBufferView<float>*>(BufferViewIt);
					BufferView->m_Data.reserve(BufferView->m_Count * 3);

					for (size_t i = BufferView->m_ByteOffset; i + 3 < BufferView->m_ByteLength + BufferView->m_ByteOffset; i += 4)
					{
						float Value;
						std::memcpy(&Value, &Buffer[i], sizeof(float));
						BufferView->m_Data.push_back(Value);
					}

					int breakpoint = 1;

					break;
				}

				case EAttributeType::Indices:
				{
					CBufferView<uint16_t>* BufferView = static_cast<CBufferView<uint16_t>*>(BufferViewIt);
					BufferView->m_Data.reserve(BufferView->m_Count);

					for (size_t i = BufferView->m_ByteOffset; i + 1 < BufferView->m_ByteLength + BufferView->m_ByteOffset; i += 2)
					{
						uint16_t Value;
						std::memcpy(&Value, &Buffer[i], sizeof(uint16_t));
						BufferView->m_Data.push_back(Value);
					}

					int breakpoint = 1;

					break;
				}

				case EAttributeType::TexCoords:
				{		
					CBufferView<float>* BufferView = static_cast<CBufferView<float>*>(BufferViewIt);				
					BufferView->m_Data.reserve(BufferView->m_Count * 2);

					for (size_t i = BufferView->m_ByteOffset; i + 3 < BufferView->m_ByteLength + BufferView->m_ByteOffset; i += 4)
					{
						float Value;
						std::memcpy(&Value, &Buffer[i], sizeof(float));				
						BufferView->m_Data.push_back(Value);
					}

					int breakpoint = 1;

					break;
				}

				case EAttributeType::Joints:
				{
					break;
				}

				case EAttributeType::Weights:
				{
					break;
				}
			}
		}
	}
	

	inline void ImportGLTF()
	{
		CLogger& Logger = CLogger::GetLogger();

		std::ifstream infile("C:/dev/GLTF/Lowpoly_sword/Lowpoly_sword.bin", std::ios::in | std::ios::binary);

		if (!infile) 
		{
			std::cerr << "Cannot open\n";
			return;
		}

		std::vector<uint8_t> buf
		{
			std::istreambuf_iterator<char>(infile),
			std::istreambuf_iterator<char>()   // note the ()
		};

		for (int i = 0; i < buf.size() - 3; i += 4)
		{
			float Value;
			std::memcpy(&Value, &buf[i], sizeof(float));
			Logger.Log("Value: " + std::to_string(Value));
		}
	}

	std::vector<CBufferViewBase*> m_pBufferViews;
	std::vector<std::string> m_AttributeType
	{
		"\"POSITION\":",
		"\"NORMAL\":",
		"\"TEXCOORD_0\":",
		"\"COLOR_0\":",
		"\"JOINTS_0\":",
		"\"WEIGHTS_0\":",
		"\"indices\":"
	};
};
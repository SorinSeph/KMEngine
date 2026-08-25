#include "GLTFImporter.h"
#include "Tree.h"
#include <iostream>

CGLTFImporter::CGLTFImporter() = default;

std::vector<CBufferViewBase*> CGLTFImporter::Import(std::string& FilePath, const std::string& FileContent)
{
    // Creates buffer views containing the index and buffer view tyype for each attribute
    ImportAttributes(FileContent);

    // Get buffer view data (count, byte length and byte offset)
    ImportAccesorsData(FileContent);

    // Read buffer data from BIN file
    ReadBIN(FilePath);

    GetJointsArray(FileContent);

    return m_BufferViews;
}

CGLTFAnimation CGLTFImporter::ImportAnimation(std::string& FilePath, const std::string& FileContent)
{
    // Previously working code, animation was a local variable
    m_Animation = ImportSamplers(FileContent);

    ImportSamplerBufferViews(FileContent, m_Animation);

    // Import nodes and animation data - better move to master ImportAnimation function
    ImportBoneData(FileContent);

    ReadAnimationBIN(FilePath);

    GetInverseBindMatrix(FileContent, FilePath);

    m_Animation.m_AnimKeyframes = GetAnimationRuntime();

    //CreateBoneHierarchy();
    CreateAnimBoneHierarchy<CGLTFNode>(m_Animation.m_AnimHierarchyTree);

    return m_Animation;
}

CGLTFAnimation* CGLTFImporter::GetAnimation()
{
    return &m_Animation;
}

void CGLTFImporter::ImportAttributes(const std::string& FileContent)
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
                m_BufferViews.push_back(pBufferView);
            }
            else if (AttributeTypeIt == "\"NORMAL\":")
            {
                CBufferView<float>* pBufferView = new CBufferView<float>();
                pBufferView->m_BufferIndex = Index;
                pBufferView->m_BufferViewType = EAttributeType::Normal;
                m_BufferViews.push_back(pBufferView);
            }
            else if (AttributeTypeIt == "\"TEXCOORD_0\":")
            {
                CBufferView<float>* pBufferView = new CBufferView<float>();
                pBufferView->m_BufferIndex = Index;
                pBufferView->m_BufferViewType = EAttributeType::TexCoords;
                m_BufferViews.push_back(pBufferView);
            }
            else if (AttributeTypeIt == "\"COLOR_0\":")
            {
                CBufferView<uint8_t>* pBufferView = new CBufferView<uint8_t>();
                pBufferView->m_BufferIndex = Index;
                pBufferView->m_BufferViewType = EAttributeType::Color;
                m_BufferViews.push_back(pBufferView);
            }
            else if (AttributeTypeIt == "\"JOINTS_0\":")
            {
                CBufferView<uint8_t>* pBufferView = new CBufferView<uint8_t>();
                pBufferView->m_BufferIndex = Index;
                pBufferView->m_BufferViewType = EAttributeType::Joints;
                m_BufferViews.push_back(pBufferView);
            }
            else if (AttributeTypeIt == "\"WEIGHTS_0\":")
            {
                CBufferView<float>* pBufferView = new CBufferView<float>();
                pBufferView->m_BufferIndex = Index;
                pBufferView->m_BufferViewType = EAttributeType::Weights;
                m_BufferViews.push_back(pBufferView);
            }
            else if (AttributeTypeIt == "\"indices\":")
            {
                CBufferView<uint32_t>* pBufferView = new CBufferView<uint32_t>();
                pBufferView->m_BufferIndex = Index;
                pBufferView->m_BufferViewType = EAttributeType::Indices;
                m_BufferViews.push_back(pBufferView);
            }
        }
    }
}

void CGLTFImporter::ImportAccesorsData(const std::string& FileContent)
{
    std::string BufferViewsData;
    std::string BufferViewSection = "\"accessors\":[";
    uint64_t BufferViewSectionIndex = FileContent.find(BufferViewSection);
    if (BufferViewSectionIndex != std::string::npos)
    {
        BufferViewSectionIndex += BufferViewSection.length();
        BufferViewsData.append(FileContent.substr(BufferViewSectionIndex + 1));
    }

    for (auto& BufferViewIt : m_BufferViews)
    {
        switch (BufferViewIt->m_BufferViewType)
        {
            case EAttributeType::Position:
            {
                BufferViewIt->m_Count = GetBufferViewByteCount(BufferViewIt->m_BufferIndex, BufferViewsData);
                // Bytelength = count * 4 * 3 (count * 4 bytes from 32 bit float * VEC3) 
                BufferViewIt->m_ByteLength = BufferViewIt->m_Count * 4 * 3;
                BufferViewIt->m_ByteOffset = GetByteOffset(BufferViewIt->m_BufferIndex, BufferViewsData);
                break;
            }

            case EAttributeType::Normal:
            {
                BufferViewIt->m_Count = GetBufferViewByteCount(BufferViewIt->m_BufferIndex, BufferViewsData);
                // Bytelength = count * 4 * 3 (count * 4 bytes from 32 bit float * VEC3) 
                BufferViewIt->m_ByteLength = BufferViewIt->m_Count * 4 * 3;
                BufferViewIt->m_ByteOffset = GetByteOffset(BufferViewIt->m_BufferIndex, BufferViewsData);
                break;
            }

            case EAttributeType::Indices:
            {
                BufferViewIt->m_Count = GetBufferViewByteCount(BufferViewIt->m_BufferIndex, BufferViewsData);
                // Bytelength = count * 2 (count * 2 bytes from 16 bit unsigned short)
                BufferViewIt->m_ByteLength = BufferViewIt->m_Count * 2;
                BufferViewIt->m_ByteOffset = GetByteOffset(BufferViewIt->m_BufferIndex, BufferViewsData);
                break;
            }

            case EAttributeType::TexCoords:
            {
                BufferViewIt->m_Count = GetBufferViewByteCount(BufferViewIt->m_BufferIndex, BufferViewsData);
                // Bytelength = count * 4 * 2 (count * 4 bytes from 32 bit float * VEC2)
                BufferViewIt->m_ByteLength = BufferViewIt->m_Count * 4 * 2;
                BufferViewIt->m_ByteOffset = GetByteOffset(BufferViewIt->m_BufferIndex, BufferViewsData);
                break;
            }

            case EAttributeType::Joints:
            {
                BufferViewIt->m_Count = GetBufferViewByteCount(BufferViewIt->m_BufferIndex, BufferViewsData);
                // Bytelength = count * 4 (count * VEC4)
                BufferViewIt->m_ByteLength = BufferViewIt->m_Count * 4;
                BufferViewIt->m_ByteOffset = GetByteOffset(BufferViewIt->m_BufferIndex, BufferViewsData);
                break;
            }

            case EAttributeType::Weights:
            {
                BufferViewIt->m_Count = GetBufferViewByteCount(BufferViewIt->m_BufferIndex, BufferViewsData);
                // Bytelength = count * 4 * 2 (count * 4 bytes from 32 bit float * VEC4)
                BufferViewIt->m_ByteLength = BufferViewIt->m_Count * 4 * 4;
                BufferViewIt->m_ByteOffset = GetByteOffset(BufferViewIt->m_BufferIndex, BufferViewsData);
                break;
            }
        }
    }
}

uint64_t CGLTFImporter::GetBufferViewByteCount(const std::string& BufferIndex, const std::string& BufferViewsData)
{
    uint64_t BufferViewCount{ 0 };
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

                    std::string BufferViewCountString{};
                    while (std::isdigit(BufferViewsData[AttributeIndex]))
                    {
                        BufferViewCountString += BufferViewsData[AttributeIndex];
                        AttributeIndex++;
                    }

                    BufferViewCount = uint64_t(std::atoi(BufferViewCountString.c_str()));
                    break;
                }
            }

            AttributeIndex++;
        }
    }

    return BufferViewCount;
}

std::string CGLTFImporter::GetBufferViewStringIndex(const std::string& FileContent, const std::string& AttributeString)
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

void CGLTFImporter::GetJointsArray(const std::string& FileContent)
{
    std::string SearchJointArrayString = "\"joints\":";
    uint64_t JointArrayPosition = FileContent.find(SearchJointArrayString);
    std::string JointArrayString;

    for (; JointArrayPosition < FileContent.size(); JointArrayPosition++)
    {
        if (FileContent.at(JointArrayPosition) == ']'
            && FileContent.at(JointArrayPosition + 1) == ',')
        {
            break;
        }
        else if (std::isdigit(FileContent[JointArrayPosition]))
        {
            JointArrayString += FileContent[JointArrayPosition];
        }
        else if (!std::isdigit(FileContent[JointArrayPosition]) && JointArrayString.size() > 0)
        {
            uint16_t Joint = uint16_t(std::atoi(JointArrayString.c_str()));
            m_JointArray.push_back(Joint);
            JointArrayString.clear();
        }
    }

    for (int i = 0; i < m_JointArray.size(); i++)
    {
        m_JointArrayMap.insert({i, m_JointArray.at(i) });
    }
}

uint64_t CGLTFImporter::GetByteOffset(const std::string& BufferIndexString, const std::string& BufferViewsData)
{
    uint64_t ByteOffset{ 0 };
    std::string SearchString = "\"bufferViews\":[";
    uint64_t BufferViewsPosition = BufferViewsData.find(SearchString);
    uint64_t OpenBracketCount = -1;
    uint64_t BufferIndex = uint64_t(std::atoi(BufferIndexString.c_str()));

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

    ByteOffset = uint64_t(std::atoi(ByteOffsetString.c_str()));

    return ByteOffset;
}

void CGLTFImporter::ReadBIN(std::string FilePath)
{
    FilePath.erase(FilePath.end() - 4, FilePath.end());
    FilePath.append("bin");

    std::ifstream File(FilePath, std::ios::in | std::ios::binary);
    std::vector<uint8_t> Buffer((std::istreambuf_iterator<char>(File)), std::istreambuf_iterator<char>());

    for (auto& BufferViewIt : m_BufferViews)
    {
        switch (BufferViewIt->m_BufferViewType)
        {
            case EAttributeType::Position:
            {
                CBufferView<float>* BufferView = static_cast<CBufferView<float>*>(BufferViewIt);
                BufferView->m_Data.reserve(BufferView->m_Count * 3);

                for (uint64_t i = BufferView->m_ByteOffset; i + 3 < BufferView->m_ByteLength + BufferView->m_ByteOffset; i += 4)
                {
                    float Value;
                    std::memcpy(&Value, &Buffer[i], sizeof(float));
                    BufferView->m_Data.push_back(Value);
                }

                break;
            }

            case EAttributeType::Indices:
            {
                CBufferView<uint32_t>* BufferView = static_cast<CBufferView<uint32_t>*>(BufferViewIt);
                BufferView->m_Data.reserve(BufferView->m_Count);

                for (uint64_t i = BufferView->m_ByteOffset; i + 1 < BufferView->m_ByteLength + BufferView->m_ByteOffset; i += 2)
                {
                    uint16_t Value;
                    std::memcpy(&Value, &Buffer[i], sizeof(uint16_t));
                    BufferView->m_Data.push_back(static_cast<uint32_t>(Value));
                }

                break;
            }

            case EAttributeType::TexCoords:
            {
                CBufferView<float>* BufferView = static_cast<CBufferView<float>*>(BufferViewIt);
                BufferView->m_Data.reserve(BufferView->m_Count * 2);

                for (uint64_t i = BufferView->m_ByteOffset; i + 3 < BufferView->m_ByteLength + BufferView->m_ByteOffset; i += 4)
                {
                    float Value;
                    std::memcpy(&Value, &Buffer[i], sizeof(float));
                    BufferView->m_Data.push_back(Value);
                }

                break;
            }

            case EAttributeType::Joints:
            {
                CBufferView<uint8_t>* BufferView = static_cast<CBufferView<uint8_t>*>(BufferViewIt);
                BufferView->m_Data.reserve(BufferView->m_Count * 4);

                for (uint64_t i = BufferView->m_ByteOffset; i < BufferView->m_ByteLength + BufferView->m_ByteOffset; ++i)
                {
                    uint8_t Value;
                    std::memcpy(&Value, &Buffer[i], sizeof(uint8_t));
                    BufferView->m_Data.push_back(Value);
                }

                break;
            }

            case EAttributeType::Weights:
            {
                CBufferView<float>* BufferView = static_cast<CBufferView<float>*>(BufferViewIt);
                BufferView->m_Data.reserve(BufferView->m_Count * 4);

                for (uint64_t i = BufferView->m_ByteOffset; i + 3 < BufferView->m_ByteLength + BufferView->m_ByteOffset; i += 4)
                {
                    float Value;
                    std::memcpy(&Value, &Buffer[i], sizeof(float));
                    BufferView->m_Data.push_back(Value);
                }

                break;
            }

            case EAttributeType::SamplerKeyframesInput:
            {
                CBufferView<float>* BufferView = static_cast<CBufferView<float>*>(BufferViewIt);
                BufferView->m_Data.reserve(BufferView->m_Count);

                for (uint64_t i = BufferView->m_ByteOffset; i + 3 < BufferView->m_ByteLength + BufferView->m_ByteOffset; i += 4)
                {
                    float Value;
                    std::memcpy(&Value, &Buffer[i], sizeof(float));
                    BufferView->m_Data.push_back(Value);
                }

                break;
            }
        }
    }
}

void CGLTFImporter::ImportBoneData(const std::string& FileContent)
{
    // Output log testing to see the exact contents of the file
    CLogger& Logger = CLogger::GetLogger();
    int FileContentIt = 0;
    Logger.Log("GLTFImporter.cpp, ImportBoneData(): \n");
    Logger.Log(FileContent);

    uint64_t ByteOffset{ 0 };
    std::string SearchString = "\n\t\"nodes\":[";
    uint64_t NodesPosition = FileContent.find(SearchString) + 14;

    // Blender adds a mesh (which might not be the penultimate) and master node (last element). 
    // We keep track of the mesh node to remove it and ther master node at the end of reading the node section data
    uint16_t MeshNodeIndex{};
    int64_t OpenBracketCount{ -1 };
    bool bNewNode{ false };
    CGLTFNode Node;

    for (; NodesPosition < FileContent.size(); NodesPosition++)
    {
        if (FileContent[NodesPosition] == '\n'
            && FileContent[NodesPosition + 1] == '\t'
            && FileContent[NodesPosition + 2] == ']'
            && FileContent[NodesPosition + 3] == ',')
        {
            break;
        }
        else if (FileContent[NodesPosition] == '{')
        {
            OpenBracketCount++;
            NodesPosition++;

            Node.m_ChildrenIndices.clear();
            Node.m_Name = "";
            //Node.m_PoseTranslation = glm::vec3(1.0f);
            //Node.m_PoseRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            //Node.m_PoseScale = glm::vec3(1.0f);

            m_Animation.m_Nodes.push_back(Node);

            continue;
        }
        else if (FileContent[NodesPosition] == 'm'
            && FileContent[NodesPosition + 1] == 'e'
            && FileContent[NodesPosition + 2] == 's'
            && FileContent[NodesPosition + 3] == 'h')
        {
            MeshNodeIndex = m_Animation.m_Nodes.size() - 1;
            continue;
        }
        else if (FileContent[NodesPosition] == 'n'
            && FileContent[NodesPosition + 1] == 'a'
            && FileContent[NodesPosition + 2] == 'm'
            && FileContent[NodesPosition + 3] == 'e')
        {
            NodesPosition += 7;

            while (FileContent[NodesPosition] != '\"')
            {
                m_Animation.m_Nodes.at(OpenBracketCount).m_Name.push_back(FileContent[NodesPosition]);
                NodesPosition++;
            }
        }
        else if (FileContent[NodesPosition] == 'c'
            && FileContent[NodesPosition + 1] == 'h'
            && FileContent[NodesPosition + 2] == 'i'
            && FileContent[NodesPosition + 3] == 'l'
            && FileContent[NodesPosition + 4] == 'd'
            && FileContent[NodesPosition + 5] == 'r'
            && FileContent[NodesPosition + 6] == 'e'
            && FileContent[NodesPosition + 7] == 'n')
        {
            NodesPosition += 11;

            std::string ChildIndexString;

            for (; NodesPosition < FileContent.size(); NodesPosition++)
            {
                if (std::isdigit(FileContent[NodesPosition]))
                {
                    ChildIndexString.push_back(FileContent[NodesPosition]);
                }
                else if (FileContent[NodesPosition] == ',')
                {
                    uint16_t ChildIndex = static_cast<uint16_t>(std::atoi(ChildIndexString.c_str()));
                    ChildIndexString = "";
                    m_Animation.m_Nodes.at(OpenBracketCount).m_ChildrenIndices.push_back(ChildIndex);
                }
                else if (FileContent[NodesPosition] == ']')
                {
                    uint16_t ChildIndex = static_cast<uint16_t>(std::atoi(ChildIndexString.c_str()));
                    ChildIndexString = "";
                    m_Animation.m_Nodes.at(OpenBracketCount).m_ChildrenIndices.push_back(ChildIndex);
                    break;
                }
            }
        }
        else if (FileContent[NodesPosition] == 't'
            && FileContent[NodesPosition + 1] == 'r'
            && FileContent[NodesPosition + 2] == 'a'
            && FileContent[NodesPosition + 3] == 'n'
            && FileContent[NodesPosition + 4] == 's'
            && FileContent[NodesPosition + 5] == 'l'
            && FileContent[NodesPosition + 6] == 'a'
            && FileContent[NodesPosition + 7] == 't'
            && FileContent[NodesPosition + 8] == 'i'
            && FileContent[NodesPosition + 9] == 'o'
            && FileContent[NodesPosition + 10] == 'n')
        {
            NodesPosition += 15;

            std::string TranslationString;
            std::vector<float> TranslationValuesVector;
            TranslationValuesVector.reserve(3);

            for (; NodesPosition < FileContent.size(); NodesPosition++)
            {
                if (std::isdigit(FileContent[NodesPosition])
                    || FileContent[NodesPosition] == 'e'
                    || FileContent[NodesPosition] == 'E'
                    || FileContent[NodesPosition] == '+'
                    || FileContent[NodesPosition] == '-'
                    || FileContent[NodesPosition] == '.')
                {
                    TranslationString.push_back(FileContent[NodesPosition]);
                }
                else if (FileContent[NodesPosition] == ',')
                {
                    float TranslationValue = std::stof(TranslationString.c_str());
                    TranslationString.clear();
                    TranslationValuesVector.push_back(TranslationValue);
                }
                else if (FileContent[NodesPosition] == ']')
                {
                    float TranslationValue = std::stof(TranslationString.c_str());
                    TranslationValuesVector.push_back(TranslationValue);
                    TranslationString.clear();

                    if (TranslationValuesVector.size() == 3)
                    {
                        glm::vec3 TranslationVec3{ TranslationValuesVector.at(0), TranslationValuesVector.at(1), TranslationValuesVector.at(2) };
                        m_Animation.m_Nodes.at(OpenBracketCount).m_PoseTranslation = TranslationVec3;
                        m_Animation.m_Nodes.at(OpenBracketCount).m_TransformTypeFlags = m_Animation.m_Nodes.at(OpenBracketCount).m_TransformTypeFlags & static_cast<uint16_t>(EChannelTransformType::Translation);
                    }
                    break;
                }
            }
        }
        else if (FileContent[NodesPosition] == 'r'
            && FileContent[NodesPosition + 1] == 'o'
            && FileContent[NodesPosition + 2] == 't'
            && FileContent[NodesPosition + 3] == 'a'
            && FileContent[NodesPosition + 4] == 't'
            && FileContent[NodesPosition + 5] == 'i'
            && FileContent[NodesPosition + 6] == 'o'
            && FileContent[NodesPosition + 7] == 'n')
        {
            NodesPosition += 15;

            std::string RotationString;
            std::vector<float> RotationValuesVector;
            RotationValuesVector.reserve(4);

            for (; NodesPosition < FileContent.size(); NodesPosition++)
            {
                if (std::isdigit(FileContent[NodesPosition])
                    || FileContent[NodesPosition] == 'e'
                    || FileContent[NodesPosition] == 'E'
                    || FileContent[NodesPosition] == '+'
                    || FileContent[NodesPosition] == '-'
                    || FileContent[NodesPosition] == '.')
                {
                    RotationString.push_back(FileContent[NodesPosition]);
                }
                else if (FileContent[NodesPosition] == ',')
                {
                    float RotationValue = std::stof(RotationString.c_str());
                    RotationString.clear();
                    RotationValuesVector.push_back(RotationValue);
                }
                else if (FileContent[NodesPosition] == ']')
                {
                    float RotationValue = std::stof(RotationString.c_str());
                    RotationValuesVector.push_back(RotationValue);
                    RotationString.clear();

                    if (RotationValuesVector.size() == 4)
                    {
                        glm::quat RotationQuat{ RotationValuesVector.at(3), RotationValuesVector.at(0), RotationValuesVector.at(1), RotationValuesVector.at(2) };
                        m_Animation.m_Nodes.at(OpenBracketCount).m_PoseRotation = RotationQuat;
                        m_Animation.m_Nodes.at(OpenBracketCount).m_TransformTypeFlags = m_Animation.m_Nodes.at(OpenBracketCount).m_TransformTypeFlags & static_cast<uint16_t>(EChannelTransformType::Rotation);
                    }
                    break;
                }
            }
        }
    }

    m_Animation.m_Nodes.erase(m_Animation.m_Nodes.begin() + MeshNodeIndex);
    m_Animation.m_Nodes.erase(m_Animation.m_Nodes.end() - 1);

    std::string SearchSkinString = "\"inverseBindMatrices\":";
    uint64_t SkinDataPosition = FileContent.find(SearchSkinString);
    SkinDataPosition += SearchSkinString.size();
    std::string SkinBufferViewString{ };
    while (std::isdigit(FileContent.at(SkinDataPosition)))
    {
        SkinBufferViewString += FileContent.at(SkinDataPosition);
        SkinDataPosition++;
    }

    std::string InverseBindMatrixNodeString{};
    std::vector<int32_t> InverseBindMatrixNode;

    for (; SkinDataPosition < FileContent.size(); SkinDataPosition++)
    {
        uint32_t InverseMatrixIndexCounter{ 0 };
        if (FileContent.at(SkinDataPosition) == ']'
            && FileContent.at(SkinDataPosition + 1) == ',')
        {
            break;
        }
        else if (std::isdigit(FileContent[SkinDataPosition]))
        {
            //InverseBindMatrixNodeString.at(InverseMatrixIndexCounter) += std::atoi(FileContent.at(SkinDataPosition));
            while (std::isdigit(FileContent[SkinDataPosition]))
            {
                InverseBindMatrixNodeString +=FileContent[SkinDataPosition];
                SkinDataPosition++;
            }

            uint32_t Node = uint32_t(std::atoi(InverseBindMatrixNodeString.c_str()));
            InverseBindMatrixNodeString.clear();
            InverseBindMatrixNode.push_back(Node);
            InverseMatrixIndexCounter++;
        }
        else
        {
            continue;
        }
    }
}

void CGLTFImporter::CreateBoneHierarchy()
{
    uint64_t NodeVectorSize{ m_Animation.m_Nodes.size() };

    // We need to add -1 for node with no children
    for (auto& NodeIt : m_Animation.m_Nodes)
    {
        if (NodeIt.m_ChildrenIndices.empty())
        {
            NodeIt.m_ChildrenIndices.push_back(-1);
        }
    }

    SNode<CGLTFNode> RootNode;
    RootNode.m_TData = m_Animation.m_Nodes.at(NodeVectorSize - 1);
    m_HierarchyTree.SetRootNode(RootNode);


    for (int NodeIt = m_Animation.m_Nodes.size() - 1; NodeIt >= 0; --NodeIt)
    {
        SNode<CGLTFNode> NewNode;
        NewNode.m_NodeName = m_Animation.m_Nodes.at(NodeIt).m_Name;
        NewNode.m_TData = m_Animation.m_Nodes.at(NodeIt);

        if (NodeIt == m_Animation.m_Nodes.size() - 1)
        {
            m_HierarchyTree.SetRootNode(NewNode);
        }

        if (NewNode.m_TData.m_ChildrenIndices.at(0) != -1)
        {
            for (auto ChildIt : NewNode.m_TData.m_ChildrenIndices)
            {
                SNode<CGLTFNode> ChildNode;
                ChildNode.m_NodeName = m_Animation.m_Nodes.at(ChildIt).m_Name;
                ChildNode.m_TData = m_Animation.m_Nodes.at(ChildIt);
                auto ParentNode = m_HierarchyTree.FindNodeByName(m_HierarchyTree.m_RootNode, NewNode.m_NodeName);
                m_HierarchyTree.Insert(ChildNode, *ParentNode);
            }
        }
    }

    //std::vector<SNode<CGLTFNode>*> NodeList;
    //m_HierarchyTree.PreOrderTraversal(m_HierarchyTree.m_RootNode, NodeList);
    //auto breakpoint = 1;
}

CGLTFAnimation CGLTFImporter::ImportSamplers(const std::string& FileContent)
{
    CGLTFAnimation Animation;
    uint16_t AnimationSamplerCount{ 0 };
    std::string NodeIndexString;
    bool bDataFound{ false };
    uint64_t AnimationPosition{ 0 };
    uint64_t LastAnimationPosition{ 0 };

    std::string SearchString = "\"sampler\":" + std::to_string(AnimationSamplerCount);
    AnimationPosition = FileContent.find(SearchString);
    if (AnimationPosition != std::string::npos)
    {
        CGLTFSampler Sampler;

        for (; ; ++AnimationPosition)
        {
            if (FileContent[AnimationPosition] == 's'
                && FileContent[AnimationPosition + 1] == 'a'
                && FileContent[AnimationPosition + 2] == 'm'
                && FileContent[AnimationPosition + 3] == 'p'
                && FileContent[AnimationPosition + 4] == 'l'
                && FileContent[AnimationPosition + 5] == 'e'
                && FileContent[AnimationPosition + 6] == 'r'
                && FileContent[AnimationPosition + 7] == '\"'
                && FileContent[AnimationPosition + 8] == ':')
            {
                Sampler.m_SamplerIndex = AnimationSamplerCount;
                AnimationSamplerCount++;
                AnimationPosition += 8;
            }
            else if (FileContent[AnimationPosition] == 'n'
                && FileContent[AnimationPosition + 1] == 'o'
                && FileContent[AnimationPosition + 2] == 'd'
                && FileContent[AnimationPosition + 3] == 'e')
            {
                AnimationPosition += 6;

                while (true)
                {
                    if (std::isdigit(FileContent[AnimationPosition]))
                    {
                        NodeIndexString.push_back(FileContent[AnimationPosition]);
                        AnimationPosition++;
                    }
                    else
                    {
                        Sampler.m_NodeIndex = static_cast<uint16_t>(std::atoi(NodeIndexString.c_str()));
                        NodeIndexString.clear();
                        break;
                    }
                }
            }
            else if (FileContent[AnimationPosition] == 't'
                && FileContent[AnimationPosition + 1] == 'r'
                && FileContent[AnimationPosition + 2] == 'a'
                && FileContent[AnimationPosition + 3] == 'n'
                && FileContent[AnimationPosition + 4] == 's'
                && FileContent[AnimationPosition + 5] == 'l'
                && FileContent[AnimationPosition + 6] == 'a'
                && FileContent[AnimationPosition + 7] == 't'
                && FileContent[AnimationPosition + 8] == 'i'
                && FileContent[AnimationPosition + 9] == 'o'
                && FileContent[AnimationPosition + 10] == 'n')
            {
                Sampler.m_TransformType = EChannelTransformType::Translation;
            }
            else if (FileContent[AnimationPosition] == 'r'
                && FileContent[AnimationPosition + 1] == 'o'
                && FileContent[AnimationPosition + 2] == 't'
                && FileContent[AnimationPosition + 3] == 'a'
                && FileContent[AnimationPosition + 4] == 't'
                && FileContent[AnimationPosition + 5] == 'i'
                && FileContent[AnimationPosition + 6] == 'o'
                && FileContent[AnimationPosition + 7] == 'n')
            {
                Sampler.m_TransformType = EChannelTransformType::Rotation;
            }
            else if (FileContent[AnimationPosition] == 's'
                && FileContent[AnimationPosition + 1] == 'c'
                && FileContent[AnimationPosition + 2] == 'a'
                && FileContent[AnimationPosition + 3] == 'l'
                && FileContent[AnimationPosition + 4] == 'e')
            {
                Sampler.m_TransformType = EChannelTransformType::Scale;
            }
            else if (FileContent[AnimationPosition] == '\t'
                && FileContent[AnimationPosition + 2] == '\t'
                && FileContent[AnimationPosition + 3] == '\t'
                && FileContent[AnimationPosition + 4] == '\t'
                && FileContent[AnimationPosition + 5] == '}')
            {
                Animation.m_Samplers.push_back(Sampler);
            }
            else if (FileContent[AnimationPosition] == ']'
                && FileContent[AnimationPosition + 1] == ',')
            {
                AnimationPosition += 3;
                break;
            }
        }
    }

    // Loop for retrieving animation name, input keyframes and output transform data
    std::string InputOutputString{};
    std::string InterpolationTypeString{};
    int SamplerIndex{ 0 };

    for (; ; ++AnimationPosition)
    {
        if (FileContent[AnimationPosition] == 'n'
            && FileContent[AnimationPosition + 1] == 'a'
            && FileContent[AnimationPosition + 2] == 'm'
            && FileContent[AnimationPosition + 3] == 'e')
        {
            AnimationPosition += 7;

            while (FileContent[AnimationPosition] != '\"')
            {
                Animation.m_AnimationName.push_back(FileContent[AnimationPosition]);
                AnimationPosition++;
            }
        }
        if (FileContent[AnimationPosition] == 'i'
            && FileContent[AnimationPosition + 1] == 'n'
            && FileContent[AnimationPosition + 2] == 'p'
            && FileContent[AnimationPosition + 3] == 'u'
            && FileContent[AnimationPosition + 4] == 't')
        {
            AnimationPosition += 7;
            while (std::isdigit(FileContent[AnimationPosition]))
            {
                InputOutputString.push_back(FileContent[AnimationPosition]);
                AnimationPosition++;
            }
            Animation.m_Samplers.at(SamplerIndex).m_InputIndex = static_cast<uint16_t>(std::atoi(InputOutputString.c_str()));
            InputOutputString.clear();
        }
        if (FileContent[AnimationPosition] == 'i'
            && FileContent[AnimationPosition + 1] == 'n'
            && FileContent[AnimationPosition + 2] == 't'
            && FileContent[AnimationPosition + 3] == 'e'
            && FileContent[AnimationPosition + 4] == 'r'
            && FileContent[AnimationPosition + 5] == 'p'
            && FileContent[AnimationPosition + 6] == 'o'
            && FileContent[AnimationPosition + 7] == 'l'
            && FileContent[AnimationPosition + 8] == 'a'
            && FileContent[AnimationPosition + 9] == 't'
            && FileContent[AnimationPosition + 10] == 'i'
            && FileContent[AnimationPosition + 11] == 'o'
            && FileContent[AnimationPosition + 12] == 'n')
        {
            AnimationPosition += 16;

            while (FileContent[AnimationPosition] != '"')
            {
                InterpolationTypeString.push_back(FileContent[AnimationPosition]);
                AnimationPosition++;
            }

            if (InterpolationTypeString == "LINEAR")
            {
                Animation.m_Samplers.at(SamplerIndex).m_InterpolationType = EChannelInterpolationType::Linear;
            }
            else if (InterpolationTypeString == "STEP")
            {
                Animation.m_Samplers.at(SamplerIndex).m_InterpolationType = EChannelInterpolationType::Step;
            }
            else if (InterpolationTypeString == "CUBICSPLINE")
            {
                Animation.m_Samplers.at(SamplerIndex).m_InterpolationType = EChannelInterpolationType::CubicSpline;
            }
            InterpolationTypeString.clear();
        }
        if (FileContent[AnimationPosition] == 'o'
            && FileContent[AnimationPosition + 1] == 'u'
            && FileContent[AnimationPosition + 2] == 't'
            && FileContent[AnimationPosition + 3] == 'p'
            && FileContent[AnimationPosition + 4] == 'u'
            && FileContent[AnimationPosition + 5] == 't')
        {
            AnimationPosition += 8;
            while (std::isdigit(FileContent[AnimationPosition]))
            {
                InputOutputString.push_back(FileContent[AnimationPosition]);
                AnimationPosition++;
            }
            Animation.m_Samplers.at(SamplerIndex).m_OutputIndex = static_cast<uint16_t>(std::atoi(InputOutputString.c_str()));
            InputOutputString.clear();
            SamplerIndex++;
        }
        if (FileContent[AnimationPosition] == ']'
            && FileContent[AnimationPosition + 1] == ',')
        {
            break;
        }
    }

    return Animation;
}

std::vector<CBufferViewBase*> CGLTFImporter::ImportSamplerBufferViews(const std::string& FileContent, const CGLTFAnimation& Animation)
{
    std::string BufferViewString{};
    std::string BufferViewData{};

    // Create every input buffer view and retrieve the count, byte length and byte offset
    for (int SamplerIt = 0; SamplerIt < Animation.m_Samplers.size(); ++SamplerIt)
    {
        CBufferView<float>* InputBuffer = new CBufferView<float>();
        std::string InputIndexString = std::to_string(Animation.m_Samplers.at(SamplerIt).m_InputIndex);
        InputBuffer->m_Count = GetBufferViewByteCount(InputIndexString, FileContent);
        InputBuffer->m_ByteOffset = GetByteOffset(InputIndexString, FileContent);
        InputBuffer->m_ByteLength = InputBuffer->m_Count * 4;
        InputBuffer->m_BufferViewType = EAttributeType::SamplerKeyframesInput;
        m_InputOutputBufferViews.push_back(InputBuffer);
    }

    // Same for every buffer view: retrieve the count, byte length and byte offset
    for (int SamplerIt = 0; SamplerIt < Animation.m_Samplers.size(); ++SamplerIt)
    {
        CBufferView<float>* OutputBuffer = new CBufferView<float>();
        std::string OutputIndexString = std::to_string(Animation.m_Samplers.at(SamplerIt).m_OutputIndex);
        OutputBuffer->m_Count = GetBufferViewByteCount(OutputIndexString, FileContent);
        OutputBuffer->m_ByteOffset = GetByteOffset(OutputIndexString, FileContent);

        if (Animation.m_Samplers.at(SamplerIt).m_TransformType == EChannelTransformType::Translation)
        {
            // Bytelength = count * 4 * 3 (count * 4 bytes from 32 bit float * VEC3) 
            OutputBuffer->m_ByteLength = OutputBuffer->m_Count * 4 * 3;
            OutputBuffer->m_BufferViewType = EAttributeType::SamplerVec3TransformOutput;
        }
        else if (Animation.m_Samplers.at(SamplerIt).m_TransformType == EChannelTransformType::Rotation)
        {
            // Bytelength = count * 4 * 4 (count * 4 bytes from 32 bit float * QUAT)
            OutputBuffer->m_ByteLength = OutputBuffer->m_Count * 4 * 4;
            OutputBuffer->m_BufferViewType = EAttributeType::SamplerQuatTransformOutput;
        }
        else if (Animation.m_Samplers.at(SamplerIt).m_TransformType == EChannelTransformType::Scale)
        {
            // Bytelength = count * 4 * 3 (count * 4 bytes from 32 bit float * VEC3)
            OutputBuffer->m_ByteLength = OutputBuffer->m_Count * 4 * 3;
            OutputBuffer->m_BufferViewType = EAttributeType::SamplerVec3TransformOutput;
        }
        m_InputOutputBufferViews.push_back(OutputBuffer);
    }

    return m_InputOutputBufferViews;
}

void CGLTFImporter::ReadAnimationBIN(std::string FilePath)
{
    FilePath.erase(FilePath.end() - 4, FilePath.end());
    FilePath.append("bin");

    std::ifstream FileStream(FilePath, std::ios::in | std::ios::binary);
    std::vector<uint8_t> FileBuffer((std::istreambuf_iterator<char>(FileStream)), std::istreambuf_iterator<char>());
    uint64_t SamplerItOffset = m_InputOutputBufferViews.size() / 2;

    // Read the transform data for the sampler's output buffer view first, we have the transform values ready to insert in the keyframe map

    for (uint64_t SamplerIt = SamplerItOffset; SamplerIt < m_InputOutputBufferViews.size(); ++SamplerIt)
    {
        uint64_t Offset = m_InputOutputBufferViews.at(SamplerIt)->m_ByteOffset;
        uint64_t Length = m_InputOutputBufferViews.at(SamplerIt)->m_ByteLength + Offset;

        if (m_Animation.m_Samplers.at(SamplerIt - SamplerItOffset).m_TransformType == EChannelTransformType::Translation)
        {
            float X, Y, Z;

            for (uint64_t i = Offset; i + 3 * 3 < Length; i += 4 * 3)
            {
                std::memcpy(&X, &FileBuffer[i], sizeof(float));
                std::memcpy(&Y, &FileBuffer[i + 4], sizeof(float));
                std::memcpy(&Z, &FileBuffer[i + 8], sizeof(float));
                m_Animation.m_Samplers.at(SamplerIt - SamplerItOffset).m_Translation.push_back(glm::vec3(X, Y, Z));
            }
        }
        else if (m_Animation.m_Samplers.at(SamplerIt - SamplerItOffset).m_TransformType == EChannelTransformType::Rotation)
        {
            float X, Y, Z, W;
            if (m_Animation.m_Samplers.at(SamplerIt - SamplerItOffset).m_Rotation.at(0) == glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
            {
                m_Animation.m_Samplers.at(SamplerIt - SamplerItOffset).m_Rotation.clear();
            }
            for (uint64_t i = Offset; i + 3 * 4 < Length; i += 4 * 4)
            {
                std::memcpy(&X, &FileBuffer[i], sizeof(float));
                std::memcpy(&Y, &FileBuffer[i + 4], sizeof(float));
                std::memcpy(&Z, &FileBuffer[i + 8], sizeof(float));
                std::memcpy(&W, &FileBuffer[i + 12], sizeof(float));
                m_Animation.m_Samplers.at(SamplerIt - SamplerItOffset).m_Rotation.push_back(glm::quat(W, X, Y, Z));

                uint16_t NodeIndex = m_Animation.m_Samplers.at(SamplerIt - SamplerItOffset).m_NodeIndex;
                //m_Animation.m_Nodes.at(NodeIndex).
            }
        }
        else if (m_Animation.m_Samplers.at(SamplerIt - SamplerItOffset).m_TransformType == EChannelTransformType::Scale)
        {
            float X, Y, Z;

            for (uint64_t i = Offset; i + 3 * 3 < Length; i += 4 * 3)
            {
                std::memcpy(&X, &FileBuffer[i], sizeof(float));
                std::memcpy(&Y, &FileBuffer[i + 4], sizeof(float));
                std::memcpy(&Z, &FileBuffer[i + 8], sizeof(float));
                m_Animation.m_Samplers.at(SamplerIt - SamplerItOffset).m_Scale.push_back(glm::vec3(X, Y, Z));
                auto offset = SamplerIt - SamplerItOffset;
                auto breakpoint = 1;
            }
        }
    }

    // Add the actual keyframes from the input buffer

    for (uint64_t SamplerIt = 0; SamplerIt < SamplerItOffset; ++SamplerIt)
    {
        uint64_t Offset = m_InputOutputBufferViews.at(SamplerIt)->m_ByteOffset;
        uint64_t Length = m_InputOutputBufferViews.at(SamplerIt)->m_ByteLength + Offset;

        for (uint64_t i = Offset; i + 3 < Length; i += 4)
        {
            float Value;
            std::memcpy(&Value, &FileBuffer[i], sizeof(float));
            m_Animation.m_Samplers.at(SamplerIt).m_Keyframes.push_back(Value);

            // Get the keyframe position in the buffer in order to add it to the node's keyframe map

            CKeyframe Keyframe;

            uint16_t NodeIndex = m_Animation.m_Samplers.at(SamplerIt).m_NodeIndex;
            uint16_t TransformTypeFlag = static_cast<uint16_t>(m_Animation.m_Samplers.at(SamplerIt).m_TransformType);
            uint16_t SamplerTransformIt = (i - Offset) / 4;

            if (m_Animation.m_Samplers.at(SamplerIt).m_TransformType == EChannelTransformType::Translation)
            {
                Keyframe.m_TransformTypeFlags = (uint16_t)EChannelTransformType::Translation;
                Keyframe.m_Translation = m_Animation.m_Samplers.at(SamplerIt).m_Translation.at(SamplerTransformIt);
                m_Animation.m_Nodes.at(NodeIndex).m_KeyframeMap.insert({ Value, Keyframe });
            }
            else if (m_Animation.m_Samplers.at(SamplerIt).m_TransformType == EChannelTransformType::Rotation)
            {
                Keyframe.m_TransformTypeFlags = (uint16_t)EChannelTransformType::Rotation;
                Keyframe.m_Rotation = m_Animation.m_Samplers.at(SamplerIt).m_Rotation.at(SamplerTransformIt);
                m_Animation.m_Nodes.at(NodeIndex).m_KeyframeMap.insert({ Value, Keyframe });
            }
            else if (m_Animation.m_Samplers.at(SamplerIt).m_TransformType == EChannelTransformType::Scale)
            {
                Keyframe.m_TransformTypeFlags = (uint16_t)EChannelTransformType::Scale;
                Keyframe.m_Scale = m_Animation.m_Samplers.at(SamplerIt).m_Scale.at(SamplerTransformIt);
                m_Animation.m_Nodes.at(NodeIndex).m_KeyframeMap.insert({ Value, Keyframe });
            }
        }
    }
}

void CGLTFImporter::GetInverseBindMatrix(const std::string& FileContent, std::string BinFilePath)
{
    // Get the buffer view index of the inverse bind matrices buffer

    for (auto AttributeTypeIt : m_AttributeType)
    {
        if (AttributeTypeIt == "\"inverseBindMatrices\":")
        {
            std::string Index = GetBufferViewStringIndex(FileContent, AttributeTypeIt);
            if (!Index.empty())
            {
                CBufferView<glm::mat4>* pBufferView = new CBufferView<glm::mat4>();
                pBufferView->m_BufferIndex = Index;
                pBufferView->m_BufferViewType = EAttributeType::InverseBindMatrix;
                m_BufferViews.push_back(pBufferView);
            }
        }
    }

    // @Temporary: to be refactored
    // Get the count, byte offset and byte length of the inverse matrix buffer

    std::string BufferViewsData;
    std::string BufferViewSection = "\"accessors\":[";
    uint64_t BufferViewSectionIndex = FileContent.find(BufferViewSection);
    if (BufferViewSectionIndex != std::string::npos)
    {
        BufferViewSectionIndex += BufferViewSection.length();
        BufferViewsData.append(FileContent.substr(BufferViewSectionIndex + 1));
    }

    for (auto& BufferViewIt : m_BufferViews)
    {
        if (BufferViewIt->m_BufferViewType == EAttributeType::InverseBindMatrix)
        {
            BufferViewIt->m_Count = GetBufferViewByteCount(BufferViewIt->m_BufferIndex, BufferViewsData);
            // Bytelength = count * 4 * 16 (count * 4 bytes from 32 bit float * 16 numbers in 4x4 matrix)
            BufferViewIt->m_ByteLength = BufferViewIt->m_Count * 4 * 16;
            BufferViewIt->m_ByteOffset = GetByteOffset(BufferViewIt->m_BufferIndex, BufferViewsData);
            break;
        }
    }

    // Retrieving the corresponding joint indices

    std::string InverseBindMatricesString = "\"joints\":[";
    uint64_t MatricesSectionIterator = FileContent.find(InverseBindMatricesString);

    std::string InverseBindMatrixNodeString{};
    std::vector<int32_t> InverseBindMatrixNode;

    for (; MatricesSectionIterator < FileContent.size(); MatricesSectionIterator++)
    {
        uint32_t InverseMatrixIndexCounter{ 0 };
        if (FileContent.at(MatricesSectionIterator) == ']'
            && FileContent.at(MatricesSectionIterator + 1) == ',')
        {
            break;
        }
        else if (std::isdigit(FileContent[MatricesSectionIterator]))
        {
            //InverseBindMatrixNodeString.at(InverseMatrixIndexCounter) += std::atoi(FileContent.at(SkinDataPosition));
            while (std::isdigit(FileContent[MatricesSectionIterator]))
            {
                InverseBindMatrixNodeString += FileContent[MatricesSectionIterator];
                MatricesSectionIterator++;
            }

            uint32_t Node = uint32_t(std::atoi(InverseBindMatrixNodeString.c_str()));
            InverseBindMatrixNodeString.clear();
            InverseBindMatrixNode.push_back(Node);
            InverseMatrixIndexCounter++;
        }
        else
        {
            continue;
        }
    }



    //std::vector<uint16_t> InverseMatrixJointsIndex{};
    //std::string InverseBindMatricesString = "\"joints\":[";
    //uint64_t MatricesSectionIndex = FileContent.find(InverseBindMatricesString);

    //if (MatricesSectionIndex != std::string::npos)
    //{
    //    std::string JointsIndexString{};
    //    BufferViewSectionIndex += InverseBindMatricesString.length();
    //    for (int i = BufferViewSectionIndex; i < FileContent.size(); i++)
    //    {
    //        if (std::isdigit(FileContent[i]))
    //        {
    //            JointsIndexString.push_back(FileContent[i]);
    //        }
    //        else if (FileContent[i] == ',')
    //        {
    //            uint16_t Index = static_cast<uint16_t>(std::atoi(JointsIndexString.c_str()));
    //            JointsIndexString.clear();
    //            InverseMatrixJointsIndex.push_back(Index);
    //        }
    //        else if (FileContent[i] == ']'
    //            && FileContent[i + 1] == ',')
    //        {
    //            break;
    //        }
    //    }
    //}

    BinFilePath.erase(BinFilePath.end() - 4, BinFilePath.end());
    BinFilePath.append("bin");
    std::ifstream File(BinFilePath, std::ios::in | std::ios::binary);
    std::vector<uint8_t> Buffer((std::istreambuf_iterator<char>(File)), std::istreambuf_iterator<char>());

    CBufferView<glm::mat4>* BufferView{ nullptr };
    std::vector<float> ValueVector;
    std::vector<glm::mat4> InverseMatricesVector;

    for (auto& BufferViewIt : m_BufferViews)
    {
        if (BufferViewIt->m_BufferViewType == EAttributeType::InverseBindMatrix)
        {
            BufferView = static_cast<CBufferView<glm::mat4>*>(BufferViewIt);
            for (uint64_t i = BufferView->m_ByteOffset; i + (sizeof(float) - 1) < BufferView->m_ByteOffset + BufferView->m_ByteLength; i += sizeof(float) * 16)
            {
                float Value;
                uint16_t ValueIt{ 0 };

                for (ValueIt = 0; ValueIt < 63; ValueIt += 4)
                {
                    std::memcpy(&Value, &Buffer[i + ValueIt], sizeof(float));
                    ValueVector.push_back(Value);
                }

                InverseMatricesVector.push_back(glm::mat4{ ValueVector.at(0), ValueVector.at(1), ValueVector.at(2), ValueVector.at(3),
                                                    ValueVector.at(4), ValueVector.at(5), ValueVector.at(6), ValueVector.at(7),
                                                    ValueVector.at(8), ValueVector.at(9), ValueVector.at(10), ValueVector.at(11),
                                                    ValueVector.at(12), ValueVector.at(13), ValueVector.at(14), ValueVector.at(15) });
                ValueVector.clear();
            }
        }
    }

    CLogger& Logger = CLogger::GetLogger();
    Logger.Log("GLTFImporter.cpp, GetInverseBindMatrix:");

    int LogCounter{ 0 };

    m_Animation.m_InverseBindMatrices.reserve(62);

    for (int i = 0; i < InverseMatricesVector.size(); i++)
    {   
        m_Animation.m_InverseBindMatrixMap.insert({ InverseBindMatrixNode.at(i), InverseMatricesVector.at(i) });
        m_Animation.m_JointsArrayMap.insert({ InverseBindMatrixNode.at(i), i });
    }
}

std::vector<float> CGLTFImporter::GetAnimationRuntime()
{   
    uint64_t Size{ 0 };
    uint64_t LongestRuntimeIndex{ 0 };

    for (int i = 0; i < m_Animation.m_Samplers.size(); ++i)
    {
        if (m_Animation.m_Samplers.at(i).m_Keyframes.size() > Size)
        {
            Size = m_Animation.m_Samplers.at(i).m_Keyframes.size();
            LongestRuntimeIndex = i;
        }
    }

    return m_Animation.m_Samplers.at(LongestRuntimeIndex).m_Keyframes;
}

void CGLTFImporter::CalculateLocalTransforms()
{
    for (uint64_t NodeIt = 0; NodeIt < m_Animation.m_Nodes.size() - 1; NodeIt++)
    {
        auto breakpoint = 1;
    }
}

CBufferViewBase* CGLTFImporter::GetBufferViewByType(EAttributeType Type)
{
    for (auto BufferViewIt : m_BufferViews)
    {
        if (BufferViewIt->m_BufferViewType == Type)
        {
            return BufferViewIt;
        }
    }
}
#pragma once
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Logger.h"
#include <unordered_map>

#include "Tree.h"

enum class EAttributeType
{
	Position,
	Normal,
	TexCoords,
	Color,
	Joints,
	Weights,
	Indices,
	InverseBindMatrix,
	SamplerKeyframesInput,
	SamplerVec3TransformOutput,
	SamplerQuatTransformOutput
};

enum class EValueType
{
	Byte,
	UnsignedByte,
	Short,
	UnsginedShort,
};

enum class EChannelTransformType
{
	Translation = 1 << 0,
	Rotation = 1 << 1,
	Scale = 1 << 2
};

enum class EChannelInterpolationType
{
	Linear,
	Step,
	CubicSpline
};

class CBufferViewBase
{
public:
	CBufferViewBase() = default;

	EAttributeType m_BufferViewType{};
	std::string m_BufferIndex{};
	uint64_t m_Count{};
	uint64_t m_ByteOffset{};
	uint64_t m_ByteLength{};
};

template <typename T>
class CBufferView : public CBufferViewBase
{
public:
	std::vector<T> m_Data{};
};

class CKeyframe
{
public:
	CKeyframe() = default;

	uint16_t m_TransformTypeFlags{};
	glm::vec3 m_Translation{ 1.0f };
	glm::quat m_Rotation{1.0f, 0.0f, 0.0f, 0.0f};
	glm::vec3 m_Scale{ 1.0f };
};

class CGLTFNode
{
public:
	CGLTFNode()
	{
		m_PoseTranslation = glm::vec3{ 0.f };
		m_PoseRotation = glm::quat{ 1.0f, 0.0f, 0.0f, 0.0f };
		m_PoseScale = glm::vec3{ 1.f };
	}

	std::string m_Name{};
	std::vector<int16_t> m_ChildrenIndices{};
	uint16_t m_TransformTypeFlags{};
	glm::vec3 m_PoseTranslation{};
	glm::quat m_PoseRotation{};
	glm::vec3 m_PoseScale{};
	std::unordered_map<float, CKeyframe> m_KeyframeMap;
};

class CGLTFSampler
{
public:
	CGLTFSampler() = default;

	uint16_t m_SamplerIndex{};
	uint16_t m_NodeIndex{};
	uint16_t m_InputIndex{};
	uint16_t m_OutputIndex{};
	std::vector<float> m_Keyframes{};
	std::vector<glm::vec3> m_Translation{ glm::vec3{0.0f} };
	std::vector<glm::quat> m_Rotation{ glm::quat{1.0f, 0.0f, 0.0f, 0.0f} };
	std::vector<glm::vec3> m_Scale{ glm::vec3{1.0f } };
	EChannelTransformType m_TransformType{};
	EChannelInterpolationType m_InterpolationType{};
};

class CGLTFAnimation
{
public:
	CGLTFAnimation() = default;

	std::string m_AnimationName{};
	std::vector<float> m_AnimKeyframes{};
	std::vector<CGLTFSampler> m_Samplers{};
	std::vector<CGLTFNode> m_Nodes{};
	std::vector<glm::mat4> m_InverseBindMatrices;
	std::unordered_map<uint32_t, glm::mat4> m_InverseBindMatrixMap;
	std::unordered_map<uint32_t, uint32_t> m_JointsArrayMap;
	CTemplatedTree<CGLTFNode> m_AnimHierarchyTree{};
	int m_TestInt{ 0 };
};

class CGLTFImporter
{
public:
	CGLTFImporter();

	std::vector<CBufferViewBase*> Import(std::string& FilePath, const std::string& FileContent);

	CGLTFAnimation ImportAnimation(std::string& FilePath, const std::string& FileContent);

	void ImportAttributes(const std::string& FileContent);

	void ImportAccesorsData(const std::string& FileContent);

	void ImportBoneData(const std::string& FileContent);

	void CreateBoneHierarchy();

	template <typename T>
	void CreateAnimBoneHierarchy(CTemplatedTree<T>& HierarchyTree)
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

		SNode<T> RootNode;
		RootNode.m_TData = m_Animation.m_Nodes.at(NodeVectorSize - 1);
		HierarchyTree.SetRootNode(RootNode);


		for (int NodeIt = m_Animation.m_Nodes.size() - 1; NodeIt >= 0; --NodeIt)
		{
			SNode<T> NewNode;
			NewNode.m_NodeName = m_Animation.m_Nodes.at(NodeIt).m_Name;
			NewNode.m_TData = m_Animation.m_Nodes.at(NodeIt);

			if (NodeIt == m_Animation.m_Nodes.size() - 1)
			{
				HierarchyTree.SetRootNode(NewNode);
			}

			if (NewNode.m_TData.m_ChildrenIndices.at(0) != -1)
			{
				for (auto ChildIt : NewNode.m_TData.m_ChildrenIndices)
				{
					SNode<T> ChildNode;
					ChildNode.m_NodeName = m_Animation.m_Nodes.at(ChildIt).m_Name;
					ChildNode.m_TData = m_Animation.m_Nodes.at(ChildIt);
					auto ParentNode = HierarchyTree.FindNodeByName(HierarchyTree.m_RootNode, NewNode.m_NodeName);
					HierarchyTree.Insert(ChildNode, *ParentNode);
				}
			}
		}
	}

	CGLTFAnimation ImportSamplers(const std::string& FileContent);

	std::vector<CBufferViewBase*> ImportSamplerBufferViews(const std::string& FileContent, const CGLTFAnimation& CGLTFAnimation);

	void GetInverseBindMatrix(const std::string& FileContent, std::string BinFilePath);

	uint64_t GetBufferViewByteCount(const std::string& BufferIndex, const std::string& BufferViewsData);

	std::string GetBufferViewStringIndex(const std::string& FileContent, const std::string& AttributeString);

	uint64_t GetByteOffset(const std::string& BufferIndexString, const std::string& BufferViewsData);

	void ReadBIN(std::string FilePath);

	void ReadAnimationBIN(std::string FilePath);

	void GetJointsArray(const std::string& FileContent);

	CGLTFAnimation* GetAnimation();

	std::vector<float> GetAnimationRuntime();

	std::vector<CBufferViewBase*> m_BufferViews;

	std::vector<std::string> m_AttributeType
	{
		"\"POSITION\":",
		"\"NORMAL\":",
		"\"TEXCOORD_0\":",
		"\"COLOR_0\":",
		"\"JOINTS_0\":",
		"\"WEIGHTS_0\":",
		"\"indices\":",
		"\"inverseBindMatrices\":"
	};

	CBufferViewBase* GetBufferViewByType(EAttributeType Type);

	void CalculateLocalTransforms();

	CGLTFAnimation m_Animation{};

	std::vector<CBufferViewBase*> m_InputOutputBufferViews{};

	CTemplatedTree<CGLTFNode> m_HierarchyTree{};

	std::vector<uint16_t> m_JointArray;

	std::unordered_map<uint16_t, uint16_t> m_JointArrayMap;
};
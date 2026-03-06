#pragma once
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Logger.h"

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

class CGLTFNode
{
public:
	CGLTFNode() = default;

	std::string m_Name{};
	std::vector<int16_t> m_ChildrenIndices{};
	uint16_t m_TransformTypeFlags{};
	glm::vec3 m_PoseTranslation{};
	glm::quat m_PoseRotation{};
	glm::vec3 m_PoseScale{};
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

	CGLTFAnimation ImportSamplers(const std::string& FileContent);

	std::vector<CBufferViewBase*> ImportSamplerBufferViews(const std::string& FileContent, const CGLTFAnimation& CGLTFAnimation);

	uint64_t GetBufferViewByteCount(const std::string& BufferIndex, const std::string& BufferViewsData);

	std::string GetBufferViewStringIndex(const std::string& FileContent, const std::string& AttributeString);

	uint64_t GetByteOffset(const std::string& BufferIndexString, const std::string& BufferViewsData);

	void ReadBIN(std::string FilePath);

	void ReadAnimationBIN(std::string FilePath);

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
		"\"indices\":"
	};

	CBufferViewBase* GetBufferViewByType(EAttributeType Type);

	void CalculateLocalTransforms();

	CGLTFAnimation m_Animation{};

	std::vector<CBufferViewBase*> m_InputOutputBufferViews{};

	CTemplatedTree<CGLTFNode> m_HierarchyTree{};
};
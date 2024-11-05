#pragma once

#include <vector>

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
};

struct SimpleColorVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Col;
};

class TEntityPhysicalMesh
{
public:

	~TEntityPhysicalMesh()
	{

	}

	void SetSimpleVerticesList(std::vector<SimpleVertex> VerticesList)
	{
		m_VerticesList = VerticesList;
	}

	std::vector<SimpleVertex> GetSimpleVerticesList()
	{
		return m_VerticesList;
	}

	void SetSimpleColorVerticesList(std::vector<SimpleColorVertex> VerticesList)
	{
		m_ColorVerticesList = VerticesList;
	}

	std::vector<SimpleColorVertex> GetSimpleColorVerticesList()
	{
		return m_ColorVerticesList;
	}

	void SetIndicesList(std::vector<WORD> IndicesList)
	{
		m_IndicesList = IndicesList;
	}

	WORD* GetIndicesListArray()
	{
		return m_IndicesList.data();
	}

	std::vector<WORD> GetIndicesList()
	{
		return m_IndicesList;
	}

	void SetStride(UINT InStride)
	{
		m_Stride = InStride;
	}

	UINT GetStride()
	{
		return m_Stride;
	}

private:
	std::vector<SimpleVertex> m_VerticesList;
	
	std::vector<SimpleColorVertex> m_ColorVerticesList;

	std::vector<WORD> m_IndicesList;

	UINT m_Stride;
};
#pragma once

#include <vector>

struct SSimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
};

struct SSimpleColorVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Col;
};

class TEntityPhysicalMesh
{
public:
	void SetSimpleVerticesList(std::vector<SSimpleVertex> VerticesList)
	{
		m_VerticesList = VerticesList;
	}

	std::vector<SSimpleVertex> GetSimpleVerticesList()
	{
		return m_VerticesList;
	}

	void SetSimpleColorVerticesList(std::vector<SSimpleColorVertex> VerticesList)
	{
		m_ColorVerticesList = VerticesList;
	}

	std::vector<SSimpleColorVertex> GetSimpleColorVerticesList()
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
	std::vector<SSimpleVertex> m_VerticesList;
	
	std::vector<SSimpleColorVertex> m_ColorVerticesList;

	std::vector<WORD> m_IndicesList;

	UINT m_Stride;
};
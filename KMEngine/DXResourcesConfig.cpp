#include "DXResourcesConfig.h"

struct InputLayoutHolder
{
	InputLayoutHolder(
		LPCSTR InSemanticName,
		UINT InSemanticIndex,
		DXGI_FORMAT InFormat,
		UINT InInputSlot,
		UINT InAlignedByteOffset,
		D3D11_INPUT_CLASSIFICATION InInputSlotClass,
		UINT InInstanceDataStepRate
	)
		: SemanticName{ InSemanticName }
		, SemanticIndex{ InSemanticIndex }
		, Format{ InFormat }
		, InputSlot{ InInputSlot }
		, AlignedByteOffset{ InAlignedByteOffset }
		, InputSlotClass{ InInputSlotClass }
		, InstanceDataStepRate{ InInstanceDataStepRate }
	{}

	LPCSTR                     SemanticName;
	UINT                       SemanticIndex;
	DXGI_FORMAT                Format;
	UINT                       InputSlot;
	UINT                       AlignedByteOffset;
	D3D11_INPUT_CLASSIFICATION InputSlotClass;
	UINT                       InstanceDataStepRate;
};

int DXResourcesConfig::UIDCounter = 1;

DXResourcesConfig::DXResourcesConfig() {}

void DXResourcesConfig::CreateUID()
{
	if (DXResourcesConfig::UIDCounter <= 10)
	{
		std::string uid = "dxr00000";
		std::string counter = std::to_string(DXResourcesConfig::UIDCounter);

		m_UID = uid + counter;
	}
	else if (DXResourcesConfig::UIDCounter / 10 != 0)
	{
		std::string uid = "dxr0000";
		std::string counter = std::to_string(DXResourcesConfig::UIDCounter);

		m_UID = uid + counter;
	}
	else if (DXResourcesConfig::UIDCounter / 100 != 0)
	{
		std::string uid = "dxr000";
		std::string counter = std::to_string(DXResourcesConfig::UIDCounter);

		m_UID = uid + counter;
	}
	else if (DXResourcesConfig::UIDCounter / 1000 != 0)
	{
		std::string uid = "dxr00";
		std::string counter = std::to_string(DXResourcesConfig::UIDCounter);

		m_UID = uid + counter;
	}
	else if (DXResourcesConfig::UIDCounter / 10000 != 0)
	{
		std::string uid = "dxr0";
		std::string counter = std::to_string(DXResourcesConfig::UIDCounter);

		m_UID = uid + counter;
	}
	else if (DXResourcesConfig::UIDCounter / 100000 != 0)
	{
		std::string uid = "dxr";
		std::string counter = std::to_string(DXResourcesConfig::UIDCounter);

		m_UID = uid + counter;
	}
	DXResourcesConfig::UIDCounter++;
}

void CreateInputLayout(
	UINT LayoutsNumber,
	LPCSTR InSemanticName,
	UINT InSemanticIndex,
	DXGI_FORMAT InFormat,
	UINT InInputSlot,
	UINT InAlignedByteOffset,
	D3D11_INPUT_CLASSIFICATION InInputSlotClass,
	UINT InInstanceDataStepRate
)
{
	std::vector<InputLayoutHolder> InputVector;
	for (int i = 0; i <= LayoutsNumber; i++)
	{
		InputLayoutHolder Input(InSemanticName, InSemanticIndex, InFormat, InInputSlot, InAlignedByteOffset, InInputSlotClass, InInstanceDataStepRate);
	}
}

void DXResourcesConfig::SetVertexBuffer(ID3D11Buffer* VertexBuffer)
{
	m_VertexBuffer = VertexBuffer;
}

void DXResourcesConfig::SetIndexBuffer(ID3D11Buffer* IndexBuffer, DXGI_FORMAT Format, int Offset)
{
	m_IndexBuffer = IndexBuffer;
	m_IndexBufferFormat = Format;
	m_IndexBufferOffset = Offset;
}

void DXResourcesConfig::SetInputLayout(ID3D11InputLayout* InputLayout)
{
	m_InputLayout = InputLayout;
}

void DXResourcesConfig::SetVertexShader(ID3D11VertexShader* VS)
{
	m_VertexShader = VS;
}

void DXResourcesConfig::SetPixelShader(ID3D11PixelShader* PS)
{
	m_PixelShader = PS;
}

void DXResourcesConfig::SetConstantBuffer(ID3D11Buffer* ConstantBuffer)
{
	m_ConstantBuffer = ConstantBuffer;
}

//void DXResourcesConfig::CreateInputLayout(int Size, ID3D11InputLayout Input[])
//{
//	m_InputLayout = Input;
//}
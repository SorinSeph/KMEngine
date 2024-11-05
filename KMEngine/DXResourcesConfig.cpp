#include "DXResourcesConfig.h"
#include "DX11Device.h"

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

int CDXResourcesConfig::UIDCounter = 1;

CDXResourcesConfig::CDXResourcesConfig() {}

void CDXResourcesConfig::SetContextResourcePtr(std::function<void()> InFn)
{
	m_pContextResourcePtr.push_back(InFn);
}

void CDXResourcesConfig::Execute()
{
	for (auto It : m_pContextResourcePtr)
	{
		It();
	}
}

void CDXResourcesConfig::CreateUID()
{
	if (CDXResourcesConfig::UIDCounter <= 10)
	{
		std::string uid = "dxr00000";
		std::string counter = std::to_string(CDXResourcesConfig::UIDCounter);

		m_UID = uid + counter;
	}
	else if (CDXResourcesConfig::UIDCounter / 10 != 0)
	{
		std::string uid = "dxr0000";
		std::string counter = std::to_string(CDXResourcesConfig::UIDCounter);

		m_UID = uid + counter;
	}
	else if (CDXResourcesConfig::UIDCounter / 100 != 0)
	{
		std::string uid = "dxr000";
		std::string counter = std::to_string(CDXResourcesConfig::UIDCounter);

		m_UID = uid + counter;
	}
	else if (CDXResourcesConfig::UIDCounter / 1000 != 0)
	{
		std::string uid = "dxr00";
		std::string counter = std::to_string(CDXResourcesConfig::UIDCounter);

		m_UID = uid + counter;
	}
	else if (CDXResourcesConfig::UIDCounter / 10000 != 0)
	{
		std::string uid = "dxr0";
		std::string counter = std::to_string(CDXResourcesConfig::UIDCounter);

		m_UID = uid + counter;
	}
	else if (CDXResourcesConfig::UIDCounter / 100000 != 0)
	{
		std::string uid = "dxr";
		std::string counter = std::to_string(CDXResourcesConfig::UIDCounter);

		m_UID = uid + counter;
	}
	CDXResourcesConfig::UIDCounter++;
}

//void DXResourcesConfig::SetDX11Device(DX11_Device* Device)
//{
//	m_pDX11Device = Device;
//}

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

void CDXResourcesConfig::SetVertexBuffer(ID3D11Buffer* VertexBuffer)
{
	m_VertexBuffer = VertexBuffer;
}

void CDXResourcesConfig::SetIndexBuffer(ID3D11Buffer* IndexBuffer, DXGI_FORMAT Format, int Offset)
{
	m_IndexBuffer = IndexBuffer;
	m_IndexBufferFormat = Format;
	m_IndexBufferOffset = Offset;
}

void CDXResourcesConfig::SetInputLayout(ID3D11InputLayout* InputLayout)
{
	m_InputLayout = InputLayout;
}

void CDXResourcesConfig::SetVertexShader(ID3D11VertexShader* VS)
{
	m_VertexShader = VS;
}

void CDXResourcesConfig::SetPixelShader(ID3D11PixelShader* PS)
{
	m_PixelShader = PS;
}

void CDXResourcesConfig::SetConstantBuffer(ID3D11Buffer* ConstantBuffer)
{
	m_ConstantBuffer = ConstantBuffer;
}

void CDXResourcesConfig::SetArrowConstantBuffer(ID3D11Buffer* ConstantBuffer)
{
	m_ConstantBuffer = ConstantBuffer;
}

void CDXResourcesConfig::SetDepthStencilState(ID3D11DepthStencilState* DepthStencilState)
{
	m_pDepthStencilState = DepthStencilState;
}

void CDXResourcesConfig::SetTextureResourceView(ID3D11ShaderResourceView* TextureRV)
{
	m_TextureRV = TextureRV;
}



//void DXResourcesConfig::CreateInputLayout(int Size, ID3D11InputLayout Input[])
//{
//	m_InputLayout = Input;
//}
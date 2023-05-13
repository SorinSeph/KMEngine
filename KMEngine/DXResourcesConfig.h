#pragma once
#include <string>
#include <vector>
#include <d3d11_1.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxcolors.h>

class DXResourcesConfig
{
public:
	DXResourcesConfig();

	void CreateUID();

	std::string m_UID;

	static int UIDCounter;

	ID3D11Buffer* m_ConstantBuffer{ nullptr };
	ID3D11InputLayout* m_InputLayout{ nullptr };
	ID3D11Buffer* m_IndexBuffer{ nullptr };
	DXGI_FORMAT m_IndexBufferFormat{ };
	UINT m_IndexBufferOffset{ };
	ID3D11Buffer* m_VertexBuffer{ nullptr };
	ID3D11VertexShader* m_VertexShader{ nullptr };
	ID3D11PixelShader* m_PixelShader{ nullptr };
	ID3D11ShaderResourceView* m_TextureRV{ nullptr };
	ID3D11SamplerState* m_SamplerLinear{ nullptr };
	ID3D11RasterizerState* m_RasterizerState{ nullptr };

	void SetConstantBuffer(ID3D11Buffer* ConstantBuffer);

	void SetVertexBuffer(ID3D11Buffer* VertexBuffer);

	void SetIndexBuffer(ID3D11Buffer* IndexBuffer, DXGI_FORMAT Format, int Offset);

	void SetInputLayout(ID3D11InputLayout* InputLayout);

	void SetVertexShader(ID3D11VertexShader* VS);

	void SetPixelShader(ID3D11PixelShader* PS);

	ID3D11Buffer* GetVertexBuffer() { return m_VertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return m_IndexBuffer; }
	ID3D11InputLayout* GetInputLayout() { return m_InputLayout; }
};



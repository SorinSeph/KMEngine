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

public:
	std::string					m_UID;
	static int					UIDCounter;
	ID3D11Buffer*				m_ConstantBuffer{ nullptr };
	ID3D11InputLayout*			m_InputLayout{ nullptr };
	ID3D11Buffer*				m_IndexBuffer{ nullptr };
	DXGI_FORMAT					m_IndexBufferFormat{ };
	UINT						m_IndexBufferOffset{ };
	ID3D11Buffer*				m_VertexBuffer{ nullptr };
	ID3D11VertexShader*			m_VertexShader{ nullptr };
	ID3D11PixelShader*			m_PixelShader{ nullptr };
	ID3D11ShaderResourceView*	m_TextureRV{ nullptr };
	ID3D11SamplerState*			m_SamplerLinear{ nullptr };
	ID3D11RasterizerState*		m_RasterizerState{ nullptr };
	ID3D11DepthStencilState*	m_DepthStencilState{ nullptr };

	// Setters

	void SetConstantBuffer(ID3D11Buffer* ConstantBuffer);

	void SetArrowConstantBuffer(ID3D11Buffer* ConstantBuffer);

	void SetVertexBuffer(ID3D11Buffer* VertexBuffer);

	void SetIndexBuffer(ID3D11Buffer* IndexBuffer, DXGI_FORMAT Format, int Offset);

	void SetInputLayout(ID3D11InputLayout* InputLayout);

	void SetVertexShader(ID3D11VertexShader* VS);

	void SetPixelShader(ID3D11PixelShader* PS);

	void SetDepthStencilState(ID3D11DepthStencilState* DepthStencilState);

	// Getters

	ID3D11Buffer* GetConstantBuffer() { return m_ConstantBuffer; }

	ID3D11Buffer* GetVertexBuffer() { return m_VertexBuffer; }

	ID3D11Buffer* GetIndexBuffer() { return m_IndexBuffer; }

	DXGI_FORMAT GetIndexBufferFormat() { return m_IndexBufferFormat; }

	UINT GetIndexBufferOffset() { return m_IndexBufferOffset; }

	ID3D11InputLayout* GetInputLayout() { return m_InputLayout; }

	ID3D11VertexShader* GetVertexShader() { return m_VertexShader; }

	ID3D11PixelShader* GetPixelShader() { return m_PixelShader; }

	std::string GetUID() { return m_UID; }

	ID3D11DepthStencilState* GetDepthStencilState() { return m_DepthStencilState; }
};



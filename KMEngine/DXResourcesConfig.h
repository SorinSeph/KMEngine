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

	ID3D11InputLayout* m_InputLayout{ nullptr };
	ID3D11Buffer* m_IndexBuffer{ nullptr };
	ID3D11Buffer* m_ConfigVertexBuffer{ nullptr };
	ID3D11VertexShader* m_VertexShader{ nullptr };
	ID3D11PixelShader* m_PixelShader{ nullptr };
	ID3D11ShaderResourceView* m_TextureRV{ nullptr };
	ID3D11SamplerState* m_SamplerLinear{ nullptr };
	ID3D11RasterizerState* m_RasterizerState{ nullptr };

	/*void CreateInputLayout(int Size, ID3D11InputLayout Input[]);*/

	void SetVertexBuffer(ID3D11Buffer* VertexBuffer);

	void SetIndexBuffer(ID3D11Buffer* IndexBuffer, DXGI_FORMAT Format, int Offset);

	ID3D11Buffer* GetVertexBuffer() { return m_ConfigVertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return m_IndexBuffer; }
};


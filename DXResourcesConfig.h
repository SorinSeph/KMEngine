#pragma once

#ifndef DXRESOURCECONFIG_H
#define DXRESOURCECONFIG_H

#include <string>
#include <vector>
#include <functional>
#include <d3d11_1.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxcolors.h>

class CDX11Device;

class DXResource
{
public:

};

class CDXResourcesConfig
{
public:
	CDXResourcesConfig();

	void CreateUID();

public:
	std::string					m_UID;
	static int					UIDCounter;
	CDX11Device* m_pDX11Device{ nullptr };
	ID3D11Buffer* m_ConstantBuffer{ nullptr };
	ID3D11InputLayout* m_InputLayout{ nullptr };
	ID3D11Buffer* m_IndexBuffer{ nullptr };
	DXGI_FORMAT					m_IndexBufferFormat{ };
	UINT						m_IndexBufferOffset{ };
	ID3D11Buffer* m_VertexBuffer{ nullptr };
	ID3D11VertexShader* m_VertexShader{ nullptr };
	ID3D11PixelShader* m_PixelShader{ nullptr };
	ID3D11ShaderResourceView* m_TextureRV{ nullptr };
	ID3D11SamplerState* m_SamplerLinear{ nullptr };
	ID3D11RasterizerState* m_RasterizerState{ nullptr };
	ID3D11DepthStencilState* m_pDepthStencilState{ nullptr };

	/**
	* vector of type std::function that holds a lambda to set the respective DirectX resource (shader, buffer, raster state, texture etc.)
	*/

	std::vector<std::function<void()>> m_pContextResourcePtr;

	void Execute();

	/**
	* DirectX11 Resources Setters
	*/

	void SetConstantBuffer(ID3D11Buffer* ConstantBuffer);

	void SetArrowConstantBuffer(ID3D11Buffer* ConstantBuffer);

	void SetVertexBuffer(ID3D11Buffer* VertexBuffer);

	void SetIndexBuffer(ID3D11Buffer* IndexBuffer, DXGI_FORMAT Format, int Offset);

	void SetInputLayout(ID3D11InputLayout* InputLayout);

	void SetVertexShader(ID3D11VertexShader* VS);

	void SetPixelShader(ID3D11PixelShader* PS);

	void SetDepthStencilState(ID3D11DepthStencilState* DepthStencilState);

	void SetTextureResourceView(ID3D11ShaderResourceView* TextureRV);

	//void SetDX11Device(DX11_Device* Device);


	/**
	* DirectX11 Resources Getters
	*/

	void SetContextResourcePtr(std::function<void()> InFn);

	void SetContextResourcePtr2(std::function<void(CDX11Device* Device)> InFn);

	ID3D11Buffer* GetConstantBuffer() { return m_ConstantBuffer; }

	ID3D11Buffer* GetVertexBuffer() { return m_VertexBuffer; }

	ID3D11Buffer* GetIndexBuffer() { return m_IndexBuffer; }

	DXGI_FORMAT GetIndexBufferFormat() { return m_IndexBufferFormat; }

	UINT GetIndexBufferOffset() { return m_IndexBufferOffset; }

	ID3D11InputLayout* GetInputLayout() { return m_InputLayout; }

	ID3D11VertexShader* GetVertexShader() { return m_VertexShader; }

	ID3D11PixelShader* GetPixelShader() { return m_PixelShader; }

	std::string GetUID() { return m_UID; }

	ID3D11DepthStencilState* GetDepthStencilState() { return m_pDepthStencilState; }

	ID3D11ShaderResourceView* GetTextureResourceView() { return m_TextureRV; }

	//DX11_Device* GetDevice() { return m_pDX11Device; }
};

#endif


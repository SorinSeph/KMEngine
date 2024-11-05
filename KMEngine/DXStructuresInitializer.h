#pragma once
#include <d3d11.h>
#include <d3d11_1.h>
#include <directxmath.h>
#include "PrimitiveGeometryFactory.h"

using namespace DirectX;

class CDXStructuresInitializer
{
public:
	CDXStructuresInitializer(HRESULT hr) :
		m_HR{ hr },
		m_DriverType{ D3D_DRIVER_TYPE_NULL },
		m_FeatureLevel{ D3D_FEATURE_LEVEL_11_0 },
		m_D3DDevice{ nullptr },
		m_pImmediateContext{ nullptr },
		m_SwapChainDesc1{},
		m_SwapChainDesc{},
		m_VerticesList{},
		m_BufferDesc{},
		m_SubresourceData{},
		m_SamplerDesc{},
		m_DepthStencilDesc{},
		m_DepthStencilViewDesc{},
		m_Texture2DDesc{},
		m_ViewPort{}
	{}

	void CreateDriverType();

	void SetSwapChainDesc1(int Width, int Height);
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc1();

	void SetSwapChainDesc(int Width, int Height, HWND OutputHWnd);
	DXGI_SWAP_CHAIN_DESC GetSwapChainDesc();

	void SetBufferDesc();
	D3D11_BUFFER_DESC GetBufferDesc();

	void SetSubresourceData(std::vector<CGameEntity3D> GameEntityList);
	D3D11_SUBRESOURCE_DATA GetSubresourceData();

	void SetSamplerDesc();
	D3D11_SAMPLER_DESC GetSamplerDesc();

	void SetDepthStencilDesc();
	D3D11_DEPTH_STENCIL_DESC GetDepthStencilDesc();

	void SetTexture2DDesc(int Width, int Height);
	D3D11_TEXTURE2D_DESC GetTexture2DDesc();

	void SetDepthStencilViewDesc(D3D11_TEXTURE2D_DESC Texture2DDesc);
	D3D11_DEPTH_STENCIL_VIEW_DESC GetDepthStencilViewDesc();

	void SetViewPort(int Width, int Height);
	D3D11_VIEWPORT GetViewPort();

private:
	HRESULT m_HR;

	std::vector<CGameEntity3D> m_GameEntityList;
	SSimpleVertex m_VerticesList;

	D3D_DRIVER_TYPE m_DriverType;
	D3D_FEATURE_LEVEL m_FeatureLevel;
	ID3D11Device* m_D3DDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	DXGI_SWAP_CHAIN_DESC1 m_SwapChainDesc1;
	DXGI_SWAP_CHAIN_DESC m_SwapChainDesc;
	D3D11_BUFFER_DESC m_BufferDesc;
	D3D11_SUBRESOURCE_DATA m_SubresourceData;
	D3D11_SAMPLER_DESC m_SamplerDesc;
	D3D11_DEPTH_STENCIL_DESC m_DepthStencilDesc;
	D3D11_TEXTURE2D_DESC m_Texture2DDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC m_DepthStencilViewDesc;
	D3D11_VIEWPORT m_ViewPort;
};

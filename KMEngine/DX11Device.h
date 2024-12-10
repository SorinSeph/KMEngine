#pragma once

#ifndef DX11DEVICE_H
#define DX11DEVICE_H

#include <windows.h>
#include <d3d11_1.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <cmath>
#include <string>
#include <vector>

#include "PrimitiveGeometryFactory.h"
#include "DDSTextureLoader.h"
#include "Cube.h"
#include "Scene.h"
#include "Logger.h"
#include "Math.h"

static BOOL g_DoesFrustumContain;

static WORD* GetIndices(std::vector<WORD> InVector)
{
	WORD* Vector = InVector.data();
	return Vector;
}

class CDX11Device
{
public:
	CDX11Device()
	{
		CLogger& Logger = CLogger::GetLogger();
		Logger.Log("CDX11Device constructor");// from ", initFile);
	}

	HRESULT CompileShaderFromFile(const wchar_t* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
	{
		HRESULT hr = S_OK;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG

		dwShaderFlags |= D3DCOMPILE_DEBUG;

		dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ID3DBlob* pErrorBlob = nullptr;
		hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
			dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
		if (FAILED(hr))
		{
			if (pErrorBlob)
			{
				OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
				pErrorBlob->Release();
			}
			return hr;
		}
		if (pErrorBlob) pErrorBlob->Release();

		return S_OK;
	}

public:
	/**
	* Calls all of the init functions below
	*/
	HRESULT InitDX11Device();

	void SetViewport(HWND InViewport);
	//void SetViewportSize(UINT width, UINT height);
	void InitDriveTypeAndFeatureLevelFinal();
	IDXGIFactory1* InitDXGIFactoryFinal();
	void InitSwapChainFinal(IDXGIFactory1* dxgiFactory);
	void InitRenderTargetViewFinal();
	D3D11_TEXTURE2D_DESC InitTexture2D();
	void InitDepthStencilView(D3D11_TEXTURE2D_DESC descDepth);
	void InitDefaultDepthStencil3();
	void InitDisabledDepthStencil();
	//void InitOutlineDepthStencil();
	void InitViewportFinal();
	void CleanupDX11Device();

	void InitRaycast(float OriginX, float OriginY, float OriginZ, float DestinationX, float DestinationY, float DestinationZ);

	/**
	* Functional but WIP functions for initializing various game primitives
	*/

	HRESULT InitTexturedCube();

	HRESULT InitTexturedCube2();

	HRESULT InitSolidColorCube();

	HRESULT InitFrustum();

	//HRESULT InitQuatCube();
	/**
	* Outline tests
	*/

	void InitCubeOutline();

	void InitCubeOutline2();

	void InitSingleCubeOutline();

	/**
	* WIP methods of DirectXCollision, to be implemented into their own file later
	*/

	void XM_CALLCONV CreateFrustumFromMatrix(_Out_ CFrustumComponent& Out, _In_ FXMMATRIX Projection, bool rhcoords = false) noexcept;

	void GetFrustumCorners(XMFLOAT3* Corners, CFrustumComponent& Out);

	/**
	* Recreation of the Intersects() (Ray + AAB) function in DirectXCollision.inl
	*/
	bool DoesIntersect(FXMVECTOR Origin, FXMVECTOR Direction, XMFLOAT3 Center, XMFLOAT3 Extents, float& Dist);

	/**
	* Check the intersection of the raycast with the first cube
	*/
	void CheckCollision(float OriginX, float OriginY, float OriginZ, float DestX, float DestY, float DestZ);

	void AddGizmo();

	UINT GetViewportWidth();

	UINT GetViewportHeight();

	/**
	* Temporary helper section of functions / variables
	*/

	void InterpMoveEntity();

	CGameEntity3D CubeEntity;

	CGameEntity3DComponent CubeEntityComponent{ };

	CGameEntity3D* InterpMoveCubeRef{ nullptr };

	float InterpMoveLoc{ 10.0f };

	ID3D11ShaderResourceView* m_TextureRV2{ nullptr };

public:
	HWND m_Viewport{ };
	HRESULT m_HR{ };
	D3D_DRIVER_TYPE m_DriverType{ D3D_DRIVER_TYPE_NULL };
	D3D_FEATURE_LEVEL m_FreatureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device* m_pD3D11Device{ nullptr };
	ID3D11Device1* m_pD3D11Device1{ nullptr };
	ID3D11DeviceContext* m_pImmediateContext{ nullptr };
	ID3D11DeviceContext1* m_pImmediateContext1{ nullptr };
	ID3D11RenderTargetView* m_pRenderTargetView{ nullptr };
	ID3D11Texture2D* m_pDepthStencil{ nullptr };
	D3D11_DEPTH_STENCIL_DESC m_DepthStencilDesc{ };
	D3D11_DEPTH_STENCIL_DESC m_DepthStencilDescOutlineMask{ };
	D3D11_DEPTH_STENCIL_DESC m_DepthStencilDescOutline{ };
	D3D11_DEPTH_STENCIL_DESC m_DepthStencilDisabledDesc{ };
	ID3D11DepthStencilState* m_pDepthStencilState{ };
	ID3D11DepthStencilState* m_pDepthStencilStateDisabled{ };
	ID3D11DepthStencilState* m_pDepthStencilStateOutlineMask{ };
	ID3D11DepthStencilState* m_pDepthStencilStateOutline{ };
	ID3D11DepthStencilView* m_DepthStencilView{ nullptr };
	ID3D11VertexShader* m_VertexShader{ nullptr };
	ID3D11PixelShader* m_PixelShader{ nullptr };
	ID3D11InputLayout* m_VertexLayout{ nullptr };
	ID3D11Buffer* m_VertexBuffer{ nullptr };
	ID3D11Buffer* m_IndexBuffer{ nullptr };
	ID3D11Buffer* m_ConstantBuffer{ nullptr };
	ID3D11ShaderResourceView* m_TextureRV{ nullptr };
	ID3D11SamplerState* m_SamplerLinear{ nullptr };
	ID3D11RasterizerState* m_RasterizerState{ nullptr };

	ID3D11ShaderResourceView* m_TextureColorGridRV{ nullptr };



	//ID3D11ShaderResourceView* m_TextureRV5{ nullptr };
	ID3D11DepthStencilState* m_NullDepthStencilState{ nullptr };


	/*
	* Default Depth Stencil
	*/

	ID3D11Texture2D* pDefDepthStencil3{ nullptr };
	D3D11_TEXTURE2D_DESC defDescDepth3{ };
	ID3D11DepthStencilState* pDefDepthStencilState3{ nullptr };
	ID3D11DepthStencilView* pDefDepthStencilView3{ nullptr };
	D3D11_DEPTH_STENCIL_VIEW_DESC defDescDepthStencilViewDesc3{ };
	D3D11_DEPTH_STENCIL_DESC defDepthStencilDesc3{ };


	ID3D11DepthStencilState* pDepthStencilStateOutline{ };
	D3D11_DEPTH_STENCIL_DESC OutlineDepthStencilDesc{ };

	D3D11_DEPTH_STENCIL_VIEW_DESC defDescDepthStencilViewDesc{ };
	ID3D11DepthStencilView* pDefDepthStencilView{ nullptr };

	/**
	* For InitShaders2()
	*/
	ID3D11InputLayout* m_VertexLayout2{ nullptr };
	ID3D11Buffer* m_VertexBuffer2{ nullptr };
	ID3D11Buffer* m_IndexBuffer2{ nullptr };
	ID3D11Buffer* m_ConstantBuffer2{ nullptr };
	//ID3D11ShaderResourceView* m_TextureRV2{ nullptr };

	ID3D11InputLayout* m_LinetraceVertexLayout{ nullptr };
	ID3D11Buffer* m_LinetraceVertexBuffer{ nullptr };
	ID3D11Buffer* m_LinetraceConstantBuffer{ nullptr };
	ID3D11VertexShader* m_LinetraceVertexShader{ nullptr };
	ID3D11PixelShader* m_LinetracePixelShader{ nullptr };

	std::vector<CGameEntity3D>		m_UnrenderedList{ };

	/**
	* Base DX resources for simple colored cube
	*/

	ID3D11VertexShader* BaseVertexShader;
	ID3D11InputLayout* BaseVertexLayout;
	ID3D11PixelShader* BasePixelShader;
	ID3D11Buffer* BaseVertexBuffer;
	ID3D11Buffer* BaseIndexBuffer;
	DXGI_FORMAT	BaseFormat;
	int	BaseOffset;
	ID3D11Buffer* BaseConstantBuffer;

public:
	static XMMATRIX m_WorldMatrix;
	static XMMATRIX m_ViewMatrix;
	static XMMATRIX m_ProjectionMatrix;

	IDXGISwapChain* m_SwapChain{ nullptr };
	IDXGISwapChain1* m_SwapChain1{ nullptr };

private:
	ID3D11Buffer* m_IndexBufferArray[3]{ };

	CGameEntity3D m_CubeEntity = { };
	CGameEntity3D m_CubeEntity2 = { };
	//CGameEntity3D m_CubeOutlineEntity{ };
	CGameEntity3D m_Linetrace{ };
	CGameEntity3D m_CubeOutline{ };

	UINT m_ViewportWidth{ };
	UINT m_ViewportHeight{ };

	float m_RotX{ };
	float m_RotY{ };
	float m_EyeX{ };
	float m_EyeY{ };
	float m_EyeZ{ };
};

#endif
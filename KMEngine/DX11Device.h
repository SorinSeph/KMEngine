#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <cmath>

#include "PrimitiveGeometryFactory.h"
#include "DDSTextureLoader.h"
#include "Cube.h"
#include "Scene.h"
#include "Logger.h"

static WORD* GetIndices(std::vector<WORD> InVector)
{
	WORD* Vector = InVector.data();
	return Vector;
}

class DX11Device
{
public:
	DX11Device() {}

	DX11Device(HWND Viewport)
		: m_Viewport{ Viewport }
	{
		RECT rc;
		GetClientRect(m_Viewport, &rc);
		m_ViewportWidth = rc.right - rc.left;
		m_ViewportHeight = rc.bottom - rc.top;
	}

	DX11Device(UINT ViewportWidth, UINT ViewportHeight)
		: m_ViewportWidth{ ViewportWidth }
		, m_ViewportHeight{ ViewportHeight }
	{}

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
	HRESULT InitDX11Device();

	void SetViewport(HWND InViewport);
	//void SetViewportSize(UINT width, UINT height);
	void InitDriveTypeAndFeatureLevel();
	IDXGIFactory1* InitDXGIFactory();
	void InitSwapChain(IDXGIFactory1* dxgiFactory);
	void InitRenderTargetView();
	D3D11_TEXTURE2D_DESC InitTexture2D();
	void InitDepthStencilView(D3D11_TEXTURE2D_DESC descDepth);
	void InitDefaultDepthStencil();
	void InitDisabledDepthStencil();
	//void InitOutlineDepthStencil();
	void InitViewport();

	/**
	* Initializes a plain, textured cube
	*/
	void InitCube();

	void InitCubeComponents();

	void InitSingleCubeComponent();

	/**
	* Initializes a yellow cube, with outline stencil state
	*/
	void InitCubeOutline();

	void InitCubeOutline2();

	void InitSingleCubeOutline();

	/**
	* Initializes a red arrow object, with fixed value scaling
	*/
	void InitArrow();

	void AddLine(float OriginX, float OriginY, float OriginZ, float DestinationX, float DestinationY, float DestinationZ);
	void InitLine(float OriginX, float OriginY, float OriginZ, float DestinationX, float DestinationY, float DestinationZ);
	//void CleanupDX11Device();

	/**
	* Recreation of the Intersects() (Ray + AAB) function in DirectXCollision.inl
	*/
	bool DoesIntersect(FXMVECTOR Origin, FXMVECTOR Direction, XMFLOAT3 Center, XMFLOAT3 Extents, float& Dist);

	/**
	* Check the intersection of the raycast with the first cube
	*/
	void CheckCollision(float OriginX, float OriginY, float OriginZ, float DestX, float DestY, float DestZ);

	void AddOutline();

	void AddGizmo();

	// Raytracing that is missing the cube at (3, 0, 0)
	void AddTestLine();

	// Testing vector normalizing, because the DirectXCollision assert for IsUnitVector fails
	void TestVector();

	void Render(float RotX, float RotY, float EyeX, float EyeY, float EyeZ);
	void CleanupDX11Device();
	//void Render2();

	// Testing a timer function for removing a linetrace
	void RemoveLineTraceTimer();

	void RemoveLastElementFromScene();

	UINT GetViewportWidth();

	UINT GetViewportHeight();

public:
	HWND							m_Viewport{ };
	HRESULT							m_hr{ };
	D3D_DRIVER_TYPE                 m_DriverType{ D3D_DRIVER_TYPE_NULL };
	D3D_FEATURE_LEVEL               m_FreatureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*					m_D3D11Device{ nullptr };
	ID3D11Device1*					m_D3D11Device1{ nullptr };
	ID3D11DeviceContext*			m_ImmediateContext{ nullptr };
	ID3D11DeviceContext1*			m_ImmediateContext1{ nullptr };
	ID3D11RenderTargetView*			m_RenderTargetView{ nullptr };
	ID3D11Texture2D*				m_DepthStencil{ nullptr };
	D3D11_DEPTH_STENCIL_DESC        m_DepthStencilDesc{ };
	D3D11_DEPTH_STENCIL_DESC        m_DepthStencilDescOutlineMask{ };
	D3D11_DEPTH_STENCIL_DESC        m_DepthStencilDescOutline{ };
	D3D11_DEPTH_STENCIL_DESC        m_DepthStencilDisabledDesc{ };
	ID3D11DepthStencilState*		m_DepthStencilState{ };
	ID3D11DepthStencilState*		m_DepthStencilStateDisabled{ };
	ID3D11DepthStencilState*		m_DepthStencilStateOutlineMask{ };
	ID3D11DepthStencilState*		m_DepthStencilStateOutline{ };
	ID3D11DepthStencilView*			m_DepthStencilView{ nullptr };
	ID3D11VertexShader*				m_VertexShader{ nullptr };
	ID3D11PixelShader*				m_PixelShader{ nullptr };
	ID3D11InputLayout*				m_VertexLayout{ nullptr };
	ID3D11Buffer*					m_VertexBuffer{ nullptr };
	ID3D11Buffer*					m_IndexBuffer{ nullptr };
	ID3D11Buffer*					m_ConstantBuffer{ nullptr };
	ID3D11ShaderResourceView*		m_TextureRV{ nullptr };
	ID3D11SamplerState*				m_SamplerLinear{ nullptr };
	ID3D11RasterizerState*			m_RasterizerState{ nullptr };

	/**
	* Empty Resource
	*/

	ID3D11ShaderResourceView*		m_TextureRV5{ nullptr };
	ID3D11DepthStencilState*		m_NullDepthStencilState{ nullptr };


	/*
	* Default Depth Stencil
	* Contains "def" in variable name
	*/

	ID3D11Texture2D*				pDefDepthStencil{ nullptr };
	D3D11_TEXTURE2D_DESC			defDescDepth{ };
	D3D11_DEPTH_STENCIL_DESC		defDepthStencilDesc{ };
	ID3D11DepthStencilState*		pDefDepthStencilState{ nullptr };
	ID3D11DepthStencilState*		pDepthStencilStateOutline{ };
	D3D11_DEPTH_STENCIL_VIEW_DESC	defDescDepthStencilViewDesc{ };
	ID3D11DepthStencilView*			pDefDepthStencilView{ nullptr };

	/**
	* For InitShaders2()
	*/
	ID3D11VertexShader*				m_VertexShader2{ nullptr };
	ID3D11PixelShader*				m_PixelShader2{ nullptr };
	ID3D11InputLayout*				m_VertexLayout2{ nullptr };
	ID3D11Buffer*					m_VertexBuffer2{ nullptr };
	ID3D11Buffer*					m_IndexBuffer2{ nullptr };
	ID3D11Buffer*					m_ConstantBuffer2{ nullptr };
	ID3D11ShaderResourceView*		m_TextureRV2{ nullptr };

	/**
	* For InitShaders3()
	*/
	ID3D11VertexShader*				m_VertexShader3{ nullptr };
	ID3D11PixelShader*				m_PixelShader3{ nullptr };
	ID3D11InputLayout*				m_VertexLayout3{ nullptr };
	ID3D11Buffer*					m_VertexBuffer3{ nullptr };
	ID3D11Buffer*					m_IndexBuffer3{ nullptr };
	ID3D11Buffer*					m_ConstantBuffer3{ nullptr };
	ID3D11ShaderResourceView*		m_TextureRV3{ nullptr };

	// Used for line
	ID3D11Buffer*					m_VertexBuffer4{ nullptr };
	ID3D11Buffer*					m_ConstantBuffer4{ nullptr };

	std::vector<GameEntity3D>		m_UnrenderedList{ };

public:
	static XMMATRIX                 m_WorldMatrix;
	static XMMATRIX                 m_ViewMatrix;
	static XMMATRIX                 m_ProjectionMatrix;

	IDXGISwapChain*					m_SwapChain{ nullptr };
	IDXGISwapChain1*				m_SwapChain1{ nullptr };

private:
	ID3D11Buffer*					m_IndexBufferArray[3]{ };

	GameEntity3D					m_CubeEntity = { };
	GameEntity3D					m_CubeEntity2 = { };
	GameEntity3D					m_CubeOutlineEntity{ };
	GameEntity3D					m_Linetrace{ };
	GameEntity3D					m_CubeOutline{ };

	UINT							m_ViewportWidth{ };
	UINT							m_ViewportHeight{ };

	float							m_RotX{ };
	float							m_RotY{ };
	float							m_EyeX{ };
	float							m_EyeY{ };
	float							m_EyeZ{ };
};
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
#include <DirectXCollision.h>
#include <DirectXTex.h>


// Temporary headers

#include "ImporterGLTF.h"

static BOOL g_DoesFrustumContain = false;

static float g_Near;
static float g_Far;
static float g_RightSlope;
static float g_LeftSlope;
static float g_TopSlope;
static float g_BottomSlope;

class CRenderer;

class CCamera
{
public:
	CCamera() = default;

	static XMMATRIX m_WorldMatrix;
	static XMMATRIX m_ViewMatrix;
	static XMMATRIX m_ProjectionMatrix;
	static XMMATRIX m_MVPMatrix;
};

class CDX11Device
{
public:
	CDX11Device()
	{
		CLogger& Logger = CLogger::GetLogger();
		Logger.Log("CDX11Device constructor");// from ", initFile);
	}

	HRESULT CompileShaderFromFile(const wchar_t* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

public:
	/**
	* Initialization functions
	*/

	HRESULT InitDX11Device();

	void SetViewport(HWND InViewport);

	void SetViewportSize(int Width, int Height);

	void InitDriveTypeAndFeatureLevelFinal();

	IDXGIFactory1* InitDXGIFactoryFinal();

	void InitSwapChainFinal(IDXGIFactory1* dxgiFactory);

	void InitRenderTargetViewFinal();

	D3D11_TEXTURE2D_DESC InitTexture2D();

	void InitDepthStencilView(D3D11_TEXTURE2D_DESC descDepth);

	void InitDefaultDepthStencil();

	void InitDisabledDepthStencil();

	void InitOutlineDepthStencil();

	// Create a temporary plane for gizmo planar picking translation
	HRESULT InitPlane();

	void InitRasterizerState();

	void InitViewport();

	void CleanupDX11Device();

	void OnPreRender();

	void OnPostRender();

	/**
	* DirectX11 resources
	*/
	D3D_DRIVER_TYPE m_DriverType{ D3D_DRIVER_TYPE_NULL };

	D3D_FEATURE_LEVEL m_FreatureLevel = D3D_FEATURE_LEVEL_11_0;

	ID3D11Device* m_pD3D11Device{ nullptr };

	ID3D11Device1* m_pD3D11Device1{ nullptr };

	ID3D11DeviceContext* m_pImmediateContext{ nullptr };

	ID3D11DeviceContext1* m_pImmediateContext1{ nullptr };

	ID3D11RenderTargetView* m_pRenderTargetView{ nullptr };

	ContainmentType CollisionCheck(CGameEntity3DComponent* Frustum, CGameEntity3DComponent* Cube);

	HRESULT InitTexturedCube();

	HRESULT InitSolidColorCube();

	HRESULT InitFrustum();

	void InitSingleCubeOutline();

	void RaycastLine(float OriginX, float OriginY, float OriginZ, float DestinationX, float DestinationY, float DestinationZ);

	void XM_CALLCONV CreateFrustumFromMatrix(_Out_ CFrustumComponent& Out, _In_ FXMMATRIX Projection, bool rhcoords = false) noexcept;

	void GetFrustumCorners(XMFLOAT3* Corners, CFrustumComponent& Out);

	void CopyEntity(CGameEntity3D Entity);

	HRESULT SpawnGizmo(const CGameEntity3D& SelectedEntity);

	HRESULT AddGizmo();

	UINT GetViewportWidth();

	UINT GetViewportHeight();

	CRenderer* m_pRenderer{ nullptr };

	void SetRenderer(CRenderer* Renderer);

	void InterpMoveEntity();

	void SetGizmoTimer();

	/**
	* Temporary variables and functions, needs refactoring
	*/

	// Temporary GLTF import
	void TempImportGLTF();

	CGameEntity3D* InterpMoveCubeRef{ nullptr };

	ID3D11Buffer* m_pArrowConstantBuffer{ nullptr };
	SArrowConstantBuffer m_ArrowConstantBuffer{ };

	int m_bGizmoHovered{ false };

	float InterpMoveLoc{ 10.0f };

	ID3D11ShaderResourceView* m_TextureRV2{ nullptr };

	void (CDX11Device::*m_PreRenderPtr[5])() { nullptr };
	void (CDX11Device::*m_PostRenderPtr[5])() { nullptr };

public:
	HWND m_Viewport{ };
	HRESULT m_HR{ };

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
	//ID3D11VertexShader* m_VertexShader{ nullptr };
	//ID3D11PixelShader* m_PixelShader{ nullptr };
	//ID3D11InputLayout* m_VertexLayout{ nullptr };
	//ID3D11Buffer* m_VertexBuffer{ nullptr };
	//ID3D11Buffer* m_IndexBuffer{ nullptr };
	//ID3D11Buffer* m_ConstantBuffer{ nullptr };
	ID3D11ShaderResourceView* m_TextureRV{ nullptr };
	ID3D11SamplerState* m_SamplerLinear{ nullptr };
	ID3D11RasterizerState* m_RasterizerState{ nullptr };

	ID3D11ShaderResourceView* m_TextureColorGridRV{ nullptr };



	//ID3D11ShaderResourceView* m_TextureRV5{ nullptr };
	ID3D11DepthStencilState* m_NullDepthStencilState{ nullptr };


	/**
	* Default Depth Stencil
	*/
	D3D11_TEXTURE2D_DESC DefDepthDesc{ };
	D3D11_DEPTH_STENCIL_VIEW_DESC DefDepthStencilViewDesc{ };
	ID3D11DepthStencilView* pDefDepthStencilView{ nullptr };
	D3D11_DEPTH_STENCIL_DESC DefDepthStencilDesc{ };
	ID3D11DepthStencilState* pDefDepthStencilState{ nullptr };

	/**
	* Disabled Depth Stencil
	*/

	D3D11_DEPTH_STENCIL_DESC DisabledDepthDesc{ };
	ID3D11DepthStencilState* pDisabledDepthStencilState{ nullptr };
	ID3D11DepthStencilView* pDisabledDepthStencilView{ nullptr };

	/**
	* Outline Depth Stencil
	*/

	ID3D11Texture2D* pDefDepthStencil3{ nullptr };
	ID3D11DepthStencilState* pOutlineDepthStencilState{ };
	D3D11_DEPTH_STENCIL_DESC OutlineDepthStencilDesc{ };
	ID3D11DepthStencilView* pOutlineDepthStencilView{ nullptr };




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
	IDXGISwapChain* m_SwapChain{ nullptr };
	IDXGISwapChain1* m_SwapChain1{ nullptr };

	int m_YKeyPressed{ 0 };	

private:

	/**
	* These are incorrect values, need to see why
	*/

	UINT m_ViewportWidth{ };
	UINT m_ViewportHeight{ };

	float m_RotX{ };
	float m_RotY{ };
	float m_EyeX{ };
	float m_EyeY{ };
	float m_EyeZ{ };
};

#endif
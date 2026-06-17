#pragma once  

#include "Renderer.h"  

class CRendererDirectX11 
{  
public:
	CRendererDirectX11() = default;

	void SetViewport(HWND InViewport);  

	void SetViewportSize(int Width, int Height);  

	void InitRenderer();  

	void SetCameraParams(float RotX, float RotY, float EyeX, float EyeY, float EyeZ);

	CDX11Device* GetDX11Device();  

	void Render();  

	void AddOutline();  

	void AddGizmo();  

	void CleanupRenderer();  

	void TestGraphicsModuleLog();  

	float m_RotX{ 0 };
	float m_RotY{ 0 };
	float m_EyeX{ 0 };
	float m_EyeY{ 0 };
	float m_EyeZ{ 0 };
	
	CGraphicsModule* m_pGraphicsModule{ nullptr };  

	CDX11Device m_DX11Device;  

	HWND m_Viewport{ };  
	UINT m_ViewportWidth{ };  
	UINT m_ViewportHeight{ };  
};

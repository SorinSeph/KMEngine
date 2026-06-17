#pragma once

#include "GameEntity3D.h"
#include "DX11Device.h"
#include "OpenGLDevice.h"
#include "Logger.h"

class CTerrain : public CGameEntity3D
{
public:
	void SetDX11Device(CDX11Device* InDevice)
	{
		m_pDX11Device = InDevice;
	}

	CDX11Device* m_pDX11Device{ nullptr };
};

class CTerrainGenerator
{
public:
	CTerrainGenerator(CDX11Device* InDX11Device)
	{
		m_pDX11Device = InDX11Device;
	}

	CTerrainGenerator(COpenGLDevice* OpenGLDevice)
	{
		m_pOpenGLDevice = OpenGLDevice;
	}

	~CTerrainGenerator()
	{
		CLogger& Logger = CLogger::GetLogger();
		Logger.Log("Terrain generator destroyed");
	}

	// Currently DX11 only, unused and to be refactored
	HRESULT GenerateTestTerrain();

	// OpenGL only
	void GenerateTerrain();

	// Temporary shader util function, to be refactored
	void CheckCompileErrors(unsigned int Shader, std::string Type)
	{
		int Success;
		char InfoLog[1024];
		if (Type != "PROGRAM")
		{
			glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
			if (!Success)
			{
				glGetShaderInfoLog(Shader, 1024, NULL, InfoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << Type << "\n" << InfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(Shader, GL_LINK_STATUS, &Success);
			if (!Success)
			{
				glGetProgramInfoLog(Shader, 1024, NULL, InfoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << Type << "\n" << InfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}

	CDX11Device* GetDX11Device();

private:
	CDX11Device* m_pDX11Device{ nullptr };
	COpenGLDevice* m_pOpenGLDevice{ nullptr };
};
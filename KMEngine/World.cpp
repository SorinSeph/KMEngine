#include "World.h"
#include "Scene.h"
#include "TerrainGenerator.h"
#include "Core/CoreClock.h"
#include <vector>
#include <filesystem>

std::vector<float> g_Vertices;
std::vector<float> g_TexCoords;
std::vector<uint32_t> g_Indices;

void CWorld::Init()
{
	CTerrainGenerator TerrainGenerator{m_pOpenGLDevice};
	TerrainGenerator.GenerateTerrain();
	CLogger& Logger = CLogger::GetLogger();
    Logger.Log("World.cpp, Init(): Location: ", std::filesystem::current_path());

    //CTimerManager& TimerManager = CTimerManager::GetTimerManager();
    //TimerManager.SetSingleTimer3<CWorld, void, &CWorld::AddTestEntity>(this, 10.0f);
    //AddTestEntity();
}

void CWorld::SetOpenGLDevice(COpenGLDevice* pOpenGLDevice)
{
	m_pOpenGLDevice = pOpenGLDevice;
}
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

	CTimerManager& TimerManager = CTimerManager::GetTimerManager();
	TimerManager.SetSingleTimer3<CWorld, void, &CWorld::AddPlayer>(this, 0.5f);
}

void CWorld::AddPlayer()
{
	CTimerManager& TimerManager = CTimerManager::GetTimerManager();
	CLogger& Logger = CLogger::GetLogger();

	if (m_pGraphicsModule)
	{
		Logger.Log("World.cpp, Init(): Importing model");
		std::string FilePath{ "E:/Work/2/Human_Basemesh_Simplified/Human_Basemesh_Simplified_Anim_Wave2.gltf" };
		std::ifstream file(FilePath, std::ios::binary);
		std::string FileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		m_pGraphicsModule->m_EntityBuilder.ImportGLTF(FilePath, FileContent);
	}
	else
	{
		__debugbreak();
	}

	CScene& Scene = CScene::GetScene();

	for (auto& SceneObjectIt : Scene.GetSceneList())
	{
		if (SceneObjectIt.m_GameEntityTag == "Player")
		{
			m_pPlayer = &SceneObjectIt;
			Logger.Log("World.cpp, AddPlayer(): player found");
		}
	}
}

void CWorld::MovePlayerStartTimer()
{
	CTimerManager& TimerManager = CTimerManager::GetTimerManager();
	TimerManager.SetTimer3<CWorld, void, &CWorld::MovePlayer>(this, 1.5f, 4.5f);
}

void CWorld::MovePlayer()
{
	if (m_pPlayer)
	{
		auto LocX = m_pPlayer->m_SceneGraph.m_pRootNode->m_tType.GetLocationX();
		auto LocY = m_pPlayer->m_SceneGraph.m_pRootNode->m_tType.GetLocationY();
		auto LocZ = m_pPlayer->m_SceneGraph.m_pRootNode->m_tType.GetLocationZ();

		m_pPlayer->m_SceneGraph.m_pRootNode->m_tType.SetLocationF(LocX, LocY, LocZ + 0.1f);
	}
}

void CWorld::SetOpenGLDevice(COpenGLDevice* pOpenGLDevice)
{
	m_pOpenGLDevice = pOpenGLDevice;
}

void CWorld::SetGraphicsModule(CGraphicsModule* pGraphicsModule)
{
	m_pGraphicsModule = pGraphicsModule;
}
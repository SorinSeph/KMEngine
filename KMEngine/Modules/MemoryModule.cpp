#define _HAS_STD_BYTE 0 // Needed to solve conflict of multiple definitions of byte

#include "MemoryModule.h"
#include "GraphicsModule.h"

uint64_t ObjectUID = 1;

void CMemoryModule::Init()
{
    m_MapFileStream.open("Resources/Assets/Map/Map.kmemap");
}

void CMemoryModule::SerializeGameEntity(CCoreObject& InObject)
{
    CGameEntity3D* Object = static_cast<CGameEntity3D*>(&InObject);
    //if (Object)
    //{
    //    m_MapFileStream << "POSITION:\n";

    //    for (int Vertex = 0; Vertex < Object->m_Vertices.size(); Vertex++)
    //    {
    //        m_MapFileStream << Object->m_Vertices.at(Vertex).m_Position.x << " "
    //            << Object->m_Vertices.at(Vertex).m_Position.y << " "
    //            << Object->m_Vertices.at(Vertex).m_Position.z << "\n";
    //    }
    //}

    m_MapFileStream.close();
}

void CMemoryModule::LoadPlayer()
{
    std::string FilePath{ "E:/Work/2/Human_Basemesh_Simplified/Human_Basemesh_Simplified_Anim_Wave2.gltf" };
    std::ifstream File(FilePath, std::ios::in | std::ios::binary);
    const std::string Buffer((std::istreambuf_iterator<char>(File)), std::istreambuf_iterator<char>());
    //m_EntityBuilder.ImportGLTF(FilePath, Buffer);
    CGraphicsModule* pGraphicsModule = static_cast<CGraphicsModule*>(this->m_pMediator->m_ModuleArray[3]);
    pGraphicsModule->m_EntityBuilder.ImportGLTF(FilePath, Buffer);
}

void CMemoryModule::LoadPlayer(std::string FilePath, const std::string FileContent)
{
    m_EntityBuilder.ImportGLTF(FilePath, FileContent);
}

std::string CMemoryModule::GenerateUID()
{
    std::string UID{ };

    if (ObjectUID <= 10)
    {
        std::string uid = "e3d00000";
        std::string counter = std::to_string(ObjectUID);

        UID = uid + counter;
        return UID;
    }
    else if (ObjectUID / 10 != 0)
    {
        std::string uid = "ed30000";
        std::string counter = std::to_string(ObjectUID);

        UID = uid + counter;
        return UID;
    }
    else if (ObjectUID / 100 != 0)
    {
        std::string uid = "e3d000";
        std::string counter = std::to_string(ObjectUID);

        UID = uid + counter;
        return UID;
    }
    else if (ObjectUID / 1000 != 0)
    {
        std::string uid = "e3d00";
        std::string counter = std::to_string(ObjectUID);

        UID = uid + counter;
        return UID;
    }
    else if (ObjectUID / 10000 != 0)
    {
        std::string uid = "e3d0";
        std::string counter = std::to_string(ObjectUID);

        UID = uid + counter;
        return UID;
    }
    else if (ObjectUID / 100000 != 0)
    {
        std::string uid = "e3d";
        std::string counter = std::to_string(ObjectUID);

        UID = uid + counter;
        return UID;
    }
    ObjectUID++;
    return {};
}

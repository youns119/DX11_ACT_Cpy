#include "Physics_Chunk.h"

#include "Physics.h"

void CPhysics_Chunk::Add_Physics(CPhysics* pPhysics)
{
    m_Physics.push_back(pPhysics);
    Safe_AddRef(pPhysics);
}

void CPhysics_Chunk::Add_Count()
{
    for (auto& pPhysics : m_Physics)
    {
        pPhysics->Add_Count();
    }
}

void CPhysics_Chunk::Sub_Count()
{
    for (auto& pPhysics : m_Physics)
    {
        pPhysics->Sub_Count();
    }
}

CPhysics_Chunk* CPhysics_Chunk::Create()
{
    return new CPhysics_Chunk();
}

void CPhysics_Chunk::Free()
{
    super::Free();

    for (auto& pPhysics : m_Physics)
        Safe_Release(pPhysics);
}

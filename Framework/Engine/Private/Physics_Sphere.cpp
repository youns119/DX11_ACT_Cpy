#include "Physics_Sphere.h"

#include "GameInstance.h"

CPhysics_Sphere::CPhysics_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
{
}

CPhysics_Sphere::CPhysics_Sphere(const CPhysics_Sphere& Prototype)
    : super(Prototype)
{

}

HRESULT CPhysics_Sphere::Initialize_Prototype()
{
    if (FAILED(super::Initialize_Prototype()))
        return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_Sphere::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

#ifdef _DEBUG
    m_fRadius = pDesc->fRadius;
    m_vCenter = pDesc->vCenter;
#endif // _DEBUG
    PxPhysics& Physics = PxGetPhysics();

	m_pActor = Physics.createRigidDynamic(PxTransform(PxVec3(
        pDesc->vPosition.x, 
        pDesc->vPosition.y,
        pDesc->vPosition.z)));

    PxMaterial* pMaterial = Physics.createMaterial(pDesc->PhysicsDesc.fStaticFriction, pDesc->PhysicsDesc.fDynamicFriction, pDesc->PhysicsDesc.fRestitution);

    PxShape* pShape = PxRigidActorExt::createExclusiveShape(*m_pActor, PxSphereGeometry(pDesc->fRadius), *pMaterial);

    PxRigidDynamic* pActor = static_cast<PxRigidDynamic*>(m_pActor);
    PxRigidBodyExt::updateMassAndInertia(*pActor, 10.f);
    //m_pActor->setMass(1.f);
    pShape->setLocalPose(PxTransform(PxVec3(pDesc->vCenter.x, pDesc->vCenter.y, pDesc->vCenter.z)));

    PxScene* pScene { nullptr };
    Physics.getScenes(&pScene, 1);

    pActor->setRigidDynamicLockFlags(static_cast<PxRigidDynamicLockFlag::Enum>(pDesc->PhysicsDesc.eLockFlog));

    pScene->addActor(*m_pActor);

    m_pFinalMatrix = pDesc->PhysicsDesc.pFinalMatrix;

    return S_OK;
}

HRESULT CPhysics_Sphere::Render()
{
#ifdef _DEBUG
    PxVec3 pxPos = m_pActor->getGlobalPose().p;
    BoundingSphere sphere;
    sphere.Center.x = pxPos.x + m_vCenter.x;
    sphere.Center.y = pxPos.y + m_vCenter.y;
    sphere.Center.z = pxPos.z + m_vCenter.z;
    sphere.Radius = m_fRadius;
    CGameInstance::GetInstance()->Add_DebugRender_Sphere(sphere, { 1.f, 0.f, 0.f, 1.f });
#endif // _DEBUG

    return S_OK;
}

CPhysics_Sphere* CPhysics_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPhysics_Sphere* pInstance = new CPhysics_Sphere(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CPhysics_Sphere");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CPhysics_Sphere::Clone(void* pArg)
{
    CPhysics_Sphere* pInstance = new CPhysics_Sphere(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CPhysics_Sphere");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPhysics_Sphere::Free()
{
    super::Free();

}

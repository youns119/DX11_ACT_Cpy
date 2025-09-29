#include "Physics_Capsule.h"

#include "GameInstance.h"
#include "Physics_Function.h"

CPhysics_Capsule::CPhysics_Capsule(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CPhysics_Capsule::CPhysics_Capsule(const CPhysics_Capsule& Prototype)
	: super(Prototype)
{
}

HRESULT CPhysics_Capsule::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_Capsule::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

    PxPhysics& Physics = PxGetPhysics();
    CPhysics::DESC PhysicsDesc = pDesc->PhysicsDesc;

    m_pActor = Physics.createRigidDynamic(PxTransform(Convert(pDesc->vPosition)));

    PxMaterial* pMaterial = Physics.createMaterial(PhysicsDesc.fStaticFriction, PhysicsDesc.fDynamicFriction, PhysicsDesc.fRestitution);

    PxShape* pShape = PxRigidActorExt::createExclusiveShape(*m_pActor, PxCapsuleGeometry(pDesc->fRadius, pDesc->fHeight), *pMaterial);
    PxTransform relativePose(PxVec3(0.f, pDesc->fHeight, 0.f), PxQuat(PxHalfPi, PxVec3(0, 0, 1.f)));
    pShape->setLocalPose(relativePose);

    PxRigidDynamic* pActor = static_cast<PxRigidDynamic*>(m_pActor);
    PxRigidBodyExt::updateMassAndInertia(*pActor, pDesc->PhysicsDesc.fMass);

    PxScene* pScene { nullptr };
    Physics.getScenes(&pScene, 1);


    pActor->setRigidDynamicLockFlags(static_cast<PxRigidDynamicLockFlag::Enum>(pDesc->PhysicsDesc.eLockFlog));

    pScene->addActor(*m_pActor);

    m_pFinalMatrix = pDesc->PhysicsDesc.pFinalMatrix;
    return S_OK;
}

HRESULT CPhysics_Capsule::Render()
{
    return S_OK;
}
//
//void CPhysics_Capsule::Manipulate_Physics(const _mat& Matrix)
//{
//    //super::Manipulate_Physics();
//}

CPhysics_Capsule* CPhysics_Capsule::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPhysics_Capsule* pInstance = new CPhysics_Capsule(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CPhysics_Capsule");
        Safe_Release(pInstance);
    }

	return pInstance;
}

CCloneable* CPhysics_Capsule::Clone(void* pArg)
{
    CPhysics_Capsule* pInstance = new CPhysics_Capsule(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CPhysics_Capsule");
        Safe_Release(pInstance);
    }

	return pInstance;
}

void CPhysics_Capsule::Free()
{
    super::Free();
}

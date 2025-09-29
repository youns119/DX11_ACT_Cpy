#include "Physics.h"

#include "GameInstance.h"
#include "Physics_Function.h"

CPhysics::CPhysics(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super(pDevice, pContext)
    , m_Physics { PxGetPhysics() }
{
}

CPhysics::CPhysics(const CPhysics& Prototype)
    : super(Prototype)
    , m_Physics { PxGetPhysics() }
{
    m_Physics.getScenes(&m_pScene, 1);
}

void CPhysics::Manipulate_Physics(const _mat& Matrix)
{
    _vector vScale, vQuaterinion, vTranslation;
    XMMatrixDecompose(&vScale, &vQuaterinion, &vTranslation, Matrix._matrix());

    _vec3 vTemp = _vec3(vTranslation);
    PxVec3 pxTranslation = PxVec3(vTemp.x, vTemp.y, vTemp.z);
    _vec4 vTempQuat = _vec4(vQuaterinion);
    PxQuat pxQuaternion = PxQuat(vTempQuat.x, vTempQuat.y, vTempQuat.z, vTempQuat.w);

    m_pActor->setGlobalPose(PxTransform(pxTranslation, PxQuat(pxQuaternion)));
}

void CPhysics::Manipulate_Object()
{
    if (nullptr == m_pFinalMatrix)
        return;

    PxTransform pxTransform = m_pActor->getGlobalPose();
    PxVec3 pxPosition = pxTransform.p;
    PxQuat pxQuat = pxTransform.q;

    _vec4 vTempQuat = _vec4(pxQuat.x, pxQuat.y, pxQuat.z, pxQuat.w);
    _vec3 vTempPos = _vec3(pxPosition.x, pxPosition.y, pxPosition.z);

    _mat ObjectFinalMatrix = *m_pFinalMatrix;
    _vec3 vScale = _vec3(ObjectFinalMatrix.right.Length(), ObjectFinalMatrix.up.Length(), ObjectFinalMatrix.look.Length());

	_matrix FinalMatrix = 
        XMMatrixScaling(vScale.x, vScale.y, vScale.z) * 
        XMMatrixRotationQuaternion(vTempQuat._vector()) * 
        XMMatrixTranslation(vTempPos.x, vTempPos.y, vTempPos.z);

    XMStoreFloat4x4(m_pFinalMatrix, FinalMatrix);


    if (m_pActor->getConcreteType() == PxConcreteType::eRIGID_DYNAMIC)
    {
		m_pGameInstance->Update_ChunkIndex_On_PhysicsMove(vTempPos, m_iChunkIndexX, m_iChunkIndexZ);
        
        PxRigidDynamic* pRigidDynamic = static_cast<PxRigidDynamic*>(m_pActor);
        if (m_fKinematicThreshold > pRigidDynamic->getLinearVelocity().magnitudeSquared())
        {
            Set_Kinematic(true);
        }
    }
}

void CPhysics::Add_Force(_vec3 vForce)
{
    //m_pActor->is<PxRigidBody>();
    PxType eType = m_pActor->getConcreteType();
    if (eType == PxConcreteType::eRIGID_DYNAMIC || eType == PxConcreteType::eARTICULATION_LINK)
    {
        PxRigidBody* pRigidBody = static_cast<PxRigidBody*>(m_pActor);
        if (!pRigidBody->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
        {
            pRigidBody->addForce(Convert(vForce));
        }
    }
}

void CPhysics::Add_Torque(_vec3 vTorque)
{
    PxType eType = m_pActor->getConcreteType();
    if (eType == PxConcreteType::eRIGID_DYNAMIC)
    {
        PxRigidDynamic* pRigidDynamic = static_cast<PxRigidDynamic*>(m_pActor);
        if (!pRigidDynamic->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
            pRigidDynamic->addTorque(Convert(vTorque));
    }
}

void CPhysics::Set_Velocity(_vec3 vVelocity)
{
    PxType eType = m_pActor->getConcreteType();
    if (eType == PxConcreteType::eRIGID_DYNAMIC)
    {
        PxRigidDynamic* pRigidDynamic = static_cast<PxRigidDynamic*>(m_pActor);
        pRigidDynamic->setLinearVelocity(Convert(vVelocity));
    }
}

void CPhysics::Sleep()
{
    PxType eType = m_pActor->getConcreteType();
    if (eType == PxConcreteType::eRIGID_DYNAMIC)
    {
        PxRigidDynamic* pRigidDynamic = static_cast<PxRigidDynamic*>(m_pActor);
        pRigidDynamic->putToSleep();
    }
}

void CPhysics::Wake()
{
    PxType eType = m_pActor->getConcreteType();
    if (eType == PxConcreteType::eRIGID_DYNAMIC)
    {
        PxRigidDynamic* pRigidDynamic = static_cast<PxRigidDynamic*>(m_pActor);
        pRigidDynamic->putToSleep();
    }
}

void CPhysics::Add_Count()
{
    /* Add_Count 먼저 호출하기 */
    if (0 == m_iCount)
    {
        ////m_pScene->addActor(*m_pActor);
        //_uint iNumShape = m_pActor->getNbShapes();
        //vector<PxShape*> Shapes(iNumShape, nullptr);
        //m_pActor->getShapes(Shapes.data(), iNumShape);
        
        //for (auto& pShape : Shapes)
        //{
        //    if (nullptr == pShape)
        //        _CrtDbgBreak();
        
        //    pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        //}
    }
    ++m_iCount;
}

void CPhysics::Sub_Count()
{
    /* Add_Count() 먼저 호출하기 */
    --m_iCount;
    if (0 == m_iCount)
    {
        //m_pScene->removeActor(*m_pActor);
        _uint iNumShape = m_pActor->getNbShapes();
        vector<PxShape*> Shapes(iNumShape, nullptr);
        m_pActor->getShapes(Shapes.data(), iNumShape);

        for (auto& pShape : Shapes)
        {
            if (nullptr == pShape)
                _CrtDbgBreak();

            pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
            //pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);

        }
    }
}

void CPhysics::Resist_To_Chunk(_vec3 vMin, _vec3 vMax)
{
    m_pGameInstance->Resist_Physics_To_Chunk(vMin, vMax, this);
}

void CPhysics::Remove_From_Scene()
{
    m_pScene->removeActor(*m_pActor);
}

void CPhysics::Add_To_Scene()
{
    m_pScene->addActor(*m_pActor);
}

void CPhysics::Set_Kinematic(_bool isKinematic)
{
    PxType eType = m_pActor->getConcreteType();
    if (eType == PxConcreteType::eRIGID_DYNAMIC || eType == PxConcreteType::eARTICULATION_LINK)
    {
        auto pRigidBody = static_cast<PxRigidBody*>(m_pActor);

        _bool isDuplicated = _bool(pRigidBody->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC) == isKinematic;
        pRigidBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, isKinematic);

        if (false == isDuplicated)
        {
            if (isKinematic)
                m_pGameInstance->Remove_Controller_From_Chunk(m_iChunkIndexX, m_iChunkIndexZ);
            else
                m_pGameInstance->Add_MovablePhysics_To_Chunk( Convert(m_pActor->getGlobalPose().p), m_iChunkIndexX, m_iChunkIndexZ);
        }
    }
}

#ifdef _DEBUG
_bool CPhysics::Is_Simulated() const
{
    PxShape* pShape { nullptr };
    m_pActor->getShapes(&pShape, 1);
    _bool isSimulated = pShape->getFlags() & PxShapeFlag::eSIMULATION_SHAPE;

    return isSimulated;
}
#endif // DEBUG

HRESULT CPhysics::Setup_Mass(const DESC& Desc, const _vec3& vCenter)
{
    if (false == Desc.isStatic)
    {
        static_cast<PxRigidDynamic*>(m_pActor)->setCMassLocalPose(PxTransform(Convert(vCenter)));

        PxVec3 pxCenter = Convert(vCenter);
        PxRigidBody* pRigidBody = static_cast<PxRigidBody*>(m_pActor);
        if (false == PxRigidBodyExt::setMassAndUpdateInertia(*pRigidBody, Desc.fMass, &pxCenter))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CPhysics::Setup_InitForce(const DESC& Desc)
{
    if (false == Desc.isStatic)
        static_cast<PxRigidDynamic*>(m_pActor)->addForce(Convert(Desc.InitialForce));

    return S_OK;
}

void CPhysics::Free()
{
    super::Free();

    m_pGameInstance->Erase_DynamicPhysicsHash(m_pActor);

    PX_RELEASE(m_pActor);
}

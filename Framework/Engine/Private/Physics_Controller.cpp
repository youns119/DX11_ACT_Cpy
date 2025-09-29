#include "Physics_Controller.h"

#include "GameInstance.h"
#include "Physics_Function.h"

CPhysics_Controller::CPhysics_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
	, m_Physics { PxGetPhysics() }
{
}

CPhysics_Controller::CPhysics_Controller(const CPhysics_Controller& Prototype)
	: super(Prototype)
	, m_Physics { PxGetPhysics() }
{
	m_Physics.getScenes(&m_pScene, 1);
}

HRESULT CPhysics_Controller::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPhysics_Controller::Initialize(void* pArg)
{
    DESC* pDesc = static_cast<DESC*>(pArg);
    if (nullptr == pDesc)
        return E_FAIL;

	m_fRadius = pDesc->fRadius;
	m_fHeight = pDesc->fHeight;
	m_pFinalMatrix = pDesc->pFinalMatrix;
	m_UserData = pDesc->UserData;

    m_pController = m_pGameInstance->Create_PhysicsCharacterController(*pDesc, &m_UserData, m_Filters, m_iChunkIndexX, m_iChunkIndexZ);
    if (nullptr == m_pController)
        return E_FAIL;


	PxShape* pShape { nullptr };
	m_pController->getActor()->getShapes(&pShape, 1);

	PxFilterData filter;
	filter.word0 = pDesc->iFilter;
	pShape->setQueryFilterData(filter);


	return S_OK;
}

HRESULT CPhysics_Controller::Render()
{
#ifdef _DEBUG
	if (nullptr != m_pContext)
	{
		_vec3 vPos = ConvertExtended(m_pController->getFootPosition());
		_vec3 vDir = _vec3(0.f, 1.f, 0.f);
		vPos += vDir * (m_fRadius + m_fHeight * 0.5f);

		m_pGameInstance->Add_DebugRender_Capsule(vPos, vDir, m_fRadius, m_fHeight, _float4{ 1.0f, 0.5f, 0.f, 1.f });
	}
#endif // _DEBUG

	return S_OK;
}

COLLISION_DESC CPhysics_Controller::Move(_vec3 vDisp, _float fMinDist, _float fDeltaTime)
{
	if (nullptr == m_pController)
		return COLLISION_DESC();

	//PxControllerFilters filter;
	//filter.mCCTFilterCallback = :;

	return m_pController->move(
		{ vDisp.x, vDisp.y, vDisp.z },
		fMinDist,
		fDeltaTime,
		m_Filters);
}

void CPhysics_Controller::Manipulate_Object()
{
	if (nullptr == m_pFinalMatrix)
	{
		_CrtDbgBreak();
		return;
	}

	if (nullptr == m_pController)
		return;

	_vec3 vFootPos = ConvertExtended(m_pController->getFootPosition());

	_mat ObjectFinalMatrix = *m_pFinalMatrix;

	_vec3 vScale;
	_vec4 vQuat;
	_vec3 vPos;
	ObjectFinalMatrix.Decompose(vScale, vQuat, vPos);

	_mat FinalMatrix =
		_mat::ScalingMatrix(vScale) *
		_mat::RotationMatrix(vQuat) *
		_mat::TranslationMatrix(vFootPos);

	*m_pFinalMatrix = FinalMatrix._float4x4();

	m_pGameInstance->Update_ChunkIndex_On_PhysicsMove(vFootPos, m_iChunkIndexX, m_iChunkIndexZ);
}

void CPhysics_Controller::Set_Position(_vec3 vPosistion)
{
	m_pController->setPosition({ vPosistion.x, vPosistion.y, vPosistion.z});
}

void CPhysics_Controller::Die()
{
	PX_RELEASE(m_pController);

	m_pGameInstance->Remove_Controller_From_Chunk(m_iChunkIndexX, m_iChunkIndexZ);
}

CPhysics_Controller* CPhysics_Controller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPhysics_Controller* pInstance = new CPhysics_Controller(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CPhysics_Controller");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCloneable* CPhysics_Controller::Clone(void* pArg)
{
	CPhysics_Controller* pInstance = new CPhysics_Controller(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Clone : CPhysics_Controller");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPhysics_Controller::Free()
{
    super::Free();

	if (true == m_isCloned)
		Die();
	else
		PX_RELEASE(m_pController);
}

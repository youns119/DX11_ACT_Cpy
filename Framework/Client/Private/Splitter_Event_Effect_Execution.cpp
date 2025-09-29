#include "pch.h"
#include "Splitter_Event_Effect_Execution.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"


CSplitter_Event_Effect_Execution::CSplitter_Event_Effect_Execution(CGameObject* _pGameObject)
{
	m_pModel = static_cast<CModel*>(_pGameObject->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
}


HRESULT CSplitter_Event_Effect_Execution::Execute()
{
	_matrix BoneMatrix = XMLoadFloat4x4(&m_pModel->Get_BoneCombinedMatrix_By_Name("clawUpper_l.28_end"));
	_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
	_vector vPos = (BoneMatrix * WorldMatrix).r[(_uint)CTransform::STATE::POSITION];
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	// _vector vUp = m_pTransform->Get_State(CTransform::STATE::UP) * 0.25f;
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_EXECUTE, vPos, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_EXECUTE_AFTER, vPos, vLook, false, nullptr);

	return S_OK;
}

CSplitter_Event_Effect_Execution* CSplitter_Event_Effect_Execution::Create(CGameObject* _pGameObject)
{
	CSplitter_Event_Effect_Execution* pInstance = new CSplitter_Event_Effect_Execution(_pGameObject);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CSplitter_Event_Effect_Execution");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_Event_Effect_Execution::Free()
{
	__super::Free();
}
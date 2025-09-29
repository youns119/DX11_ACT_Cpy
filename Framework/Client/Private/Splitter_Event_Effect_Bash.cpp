#include "pch.h"
#include "Splitter_Event_Effect_Bash.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"


CSplitter_Event_Effect_Bash::CSplitter_Event_Effect_Bash(CGameObject* _pGameObject)
{
	m_pModel = static_cast<CModel*>(_pGameObject->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
}


HRESULT CSplitter_Event_Effect_Bash::Execute()
{
	_matrix BoneMatrix = XMLoadFloat4x4(&m_pModel->Get_BoneCombinedMatrix_By_Name("clawUpper_l.28"));
	_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
	_vector vPos = (BoneMatrix * WorldMatrix).r[(_uint)CTransform::STATE::POSITION];
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	// _vector vUp = m_pTransform->Get_State(CTransform::STATE::UP) * 0.25f;
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_BASH, vPos + vLook * 1.2f, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_BASH_AFTER, vPos + vLook * 1.2f, vLook, true, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_SPRT_HIT_END, vPos + vLook * 1.2f, vLook, true, nullptr);


	BoneMatrix = XMLoadFloat4x4(&m_pModel->Get_BoneCombinedMatrix_By_Name("clawUpper_l.28_end"));
	vPos = (BoneMatrix * WorldMatrix).r[(_uint)CTransform::STATE::POSITION];
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_MESH_BASH, vPos + vLook * 1.2f, -vLook, false, nullptr);

	return S_OK;
}

CSplitter_Event_Effect_Bash* CSplitter_Event_Effect_Bash::Create(CGameObject* _pGameObject)
{
	CSplitter_Event_Effect_Bash* pInstance = new CSplitter_Event_Effect_Bash(_pGameObject);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CSplitter_Event_Effect_Bash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_Event_Effect_Bash::Free()
{
	__super::Free();
}
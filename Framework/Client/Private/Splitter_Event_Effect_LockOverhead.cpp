#include "pch.h"
#include "Splitter_Event_Effect_LockOverhead.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"


CSplitter_Event_Effect_LockOverhead::CSplitter_Event_Effect_LockOverhead(CGameObject* _pGameObject)
{
	m_pModel = static_cast<CModel*>(_pGameObject->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
}


HRESULT CSplitter_Event_Effect_LockOverhead::Execute()
{
	// Áß¾Ó : FX_SandBurst_Beeg.106
	// ¸éµµ³¯ : FXP_Excution_OverheadSlam.114
	_matrix MidBoneMatrix = XMLoadFloat4x4(&m_pModel->Get_BoneCombinedMatrix_By_Name("Blade.64"));
	_matrix UpperBoneMatrix = XMLoadFloat4x4(&m_pModel->Get_BoneCombinedMatrix_By_Name("Blade.64_end"));
	_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();

	_vector vMidPos = (MidBoneMatrix * WorldMatrix).r[(_uint)CTransform::STATE::POSITION];
	_vector vUpperPos = (UpperBoneMatrix * WorldMatrix).r[(_uint)CTransform::STATE::POSITION];
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK) * 0.25f;
	_vector vUp = m_pTransform->Get_State(CTransform::STATE::UP) * 0.25f;
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_SANDBURST, vMidPos - vUp, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_SANDBURST, vUpperPos - vUp, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_SLAMSHOCK, vMidPos - vUp, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_SLAMSHOCK, vUpperPos - vUp, vLook, false, nullptr);

	return S_OK;
}

CSplitter_Event_Effect_LockOverhead* CSplitter_Event_Effect_LockOverhead::Create(CGameObject* _pGameObject)
{
	CSplitter_Event_Effect_LockOverhead* pInstance = new CSplitter_Event_Effect_LockOverhead(_pGameObject);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CSplitter_Event_Effect_LockOverhead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_Event_Effect_LockOverhead::Free()
{
	__super::Free();
}
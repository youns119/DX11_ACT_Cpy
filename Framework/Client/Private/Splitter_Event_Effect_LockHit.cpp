#include "pch.h"
#include "Splitter_Event_Effect_LockHit.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"


CSplitter_Event_Effect_LockHit::CSplitter_Event_Effect_LockHit(CGameObject* _pGameObject)
{
	m_pModel = static_cast<CModel*>(_pGameObject->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
}


HRESULT CSplitter_Event_Effect_LockHit::Execute()
{
	_matrix BoneMatrix = XMLoadFloat4x4(&m_pModel->Get_BoneCombinedMatrix_By_Name("Collider_Guillotine.69"));
	_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
	_vector vPos = (BoneMatrix * WorldMatrix).r[(_uint)CTransform::STATE::POSITION];
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK) * 0.25f;
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_PTCL_HIT, vPos + vLook, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_SPRT_HIT_AFTER, vPos + vLook, vLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_SPRT_HIT_END, vPos + vLook, vLook, false, nullptr);

	CGameInstance::GetInstance()->Play_Sound(L"Executioner Slam 3.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK, 0.5f);

	return S_OK;
}

CSplitter_Event_Effect_LockHit* CSplitter_Event_Effect_LockHit::Create(CGameObject* _pGameObject)
{
	CSplitter_Event_Effect_LockHit* pInstance = new CSplitter_Event_Effect_LockHit(_pGameObject);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CSplitter_Event_Effect_LockHit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_Event_Effect_LockHit::Free()
{
	__super::Free();
}
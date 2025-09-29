#include "pch.h"
#include "Event_Item_HeartKelp.h"
#include "GameInstance.h"
#include "Player.h"

CEvent_Item_HeartKelp::CEvent_Item_HeartKelp()
{
}

HRESULT CEvent_Item_HeartKelp::Init(CGameObject* _pGameObject)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

HRESULT CEvent_Item_HeartKelp::Execute()
{
	FPlayerStat* pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_pPlayer->Heal(pPlayerStat->fHeartKelpHeal);

	pPlayerStat->bUseItem = false;

	if(!pPlayerStat->bPreserveItem)
		pPlayerStat->iHeartKelpCount--;

	Call_HeartKelpEffect();

    return S_OK;
}

void CEvent_Item_HeartKelp::Call_HeartKelpEffect()
{
	CGameInstance* pGameInstance = { CGameInstance::GetInstance() };
	CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	_vector vUp = pTransform->Get_State(CTransform::STATE::UP);
	_vector vPos = pTransform->Get_State(CTransform::STATE::POSITION) + vUp;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HEAL_DIAMOND, vPos, vLook, false, nullptr);
	pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HEAL_CIRCLE, vPos, vLook, false, nullptr);
	pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_HEAL_SPREAD, vPos, vLook, false, nullptr);
	pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_SPRT_HEAL_SPREAD, vPos, vLook, false, nullptr);
}

CEvent_Item_HeartKelp* CEvent_Item_HeartKelp::Create(CGameObject* _pGameObject)
{
	CEvent_Item_HeartKelp* pInstance = new CEvent_Item_HeartKelp;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_Item_HeartKelp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Item_HeartKelp::Free()
{
	super::Free();
}
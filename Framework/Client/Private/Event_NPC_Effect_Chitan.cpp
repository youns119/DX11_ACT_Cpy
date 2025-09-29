#include "pch.h"
#include "Event_NPC_Effect_Chitan.h"
#include "GameInstance.h"
#include "EffectObject.h"

CEvent_NPC_Effect_Chitan::CEvent_NPC_Effect_Chitan(CGameObject* _pGameObject, _uint _iEventType)
{
	m_iEventType = _iEventType;
}


HRESULT CEvent_NPC_Effect_Chitan::Execute()
{
	CGameInstance::GetInstance()->Play_Sound(L"Base_Impact_Sword1.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK1, 0.5f);

	if (nullptr == m_pLeftSocketMatrix)
	{
		CModel* pModel = static_cast<CModel*>(m_pGameObject->Find_Component(L"Com_Model"));
		m_pLeftSocketMatrix = &pModel->Get_BoneCombinedMatrix_By_Name("Sword.L");
	}

	if (nullptr == m_pRightSocketMatrix)
	{
		CModel* pModel = static_cast<CModel*>(m_pGameObject->Find_Component(L"Com_Model"));
		m_pRightSocketMatrix = &pModel->Get_BoneCombinedMatrix_By_Name("Sword.R");
	}


	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	_vector vEffectLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) }, vLeftPos{}, vRightPos{}, vRight{}, vLook{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vCamPos{}, vCamLook{};
	vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
	vRight = m_pTransform->Get_State(CTransform::STATE::RIGHT);
	vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	vLeftPos = (XMLoadFloat4x4(m_pLeftSocketMatrix) * XMLoadFloat4x4(&m_pTransform->Get_WorldMatrix_Ref())).r[(_uint)CTransform::STATE::POSITION] - vRight * 0.5f - vLook * 0.5f;
	vRightPos = (XMLoadFloat4x4(m_pRightSocketMatrix) * XMLoadFloat4x4(&m_pTransform->Get_WorldMatrix_Ref())).r[(_uint)CTransform::STATE::POSITION] - vRight * 0.5f - vLook * 0.5f;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_bool bIsFlip{ false };
	_float fRotDegree{ 0.f };

	switch (m_iEventType)
	{
	case NPCCHI_EFFECT_TYPE::SANDBURST:

		break;
	case NPCCHI_EFFECT_TYPE::IMPACT_DOUBLE:
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT00_PTCL_DIAMOND, vLeftPos, vEffectLook, false, nullptr);
		// pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_REDBIGONE, vLeftPos, vEffectLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPBUBBLE, vLeftPos, vEffectLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTSCENE_PTCL_CHITAN_FRACTIONSPARK, vLeftPos, vEffectLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTSCENE_PTCL_CHITAN_EXPLSLOWDOWNSPARK, vLeftPos, vEffectLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTSCENE_PTCL_CHITAN_DOWNLOOP, vLeftPos, vEffectLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTSCENE_PTCL_CHITAN_UPLOOP, vLeftPos, vEffectLook, false, nullptr);

		vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vLeftPos), 0.f);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vLeftPos + vCamLook, vCamLook, false, nullptr);

		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT00_PTCL_DIAMOND, vRightPos, vEffectLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_REDBIGONE, vRightPos, vEffectLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPBUBBLE, vRightPos, vEffectLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTSCENE_PTCL_CHITAN_FRACTIONSPARK, vRightPos, vEffectLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTSCENE_PTCL_CHITAN_EXPLSLOWDOWNSPARK, vRightPos, vEffectLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTSCENE_PTCL_CHITAN_DOWNLOOP, vRightPos, vEffectLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTSCENE_PTCL_CHITAN_UPLOOP, vRightPos, vEffectLook, false, nullptr);

		vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vRightPos), 0.f);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vRightPos + vCamLook, vCamLook, false, nullptr);
		break;
	default:
		break;
	}

	return S_OK;
}

CEvent_NPC_Effect_Chitan* CEvent_NPC_Effect_Chitan::Create(CGameObject* _pGameObject, _uint _iEventType)
{
	CEvent_NPC_Effect_Chitan* pInstance = new CEvent_NPC_Effect_Chitan(_pGameObject, _iEventType);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_NPC_Effect_Chitan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_NPC_Effect_Chitan::Free()
{
	__super::Free();
}

#include "pch.h"
#include "Event_NPC_Effect_Inkerton.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "EffectObject.h"

CEvent_NPC_Effect_Inkerton::CEvent_NPC_Effect_Inkerton(CGameObject* _pGameObject, _uint _iEventType)
{
	m_iEventType = _iEventType;
}


HRESULT CEvent_NPC_Effect_Inkerton::Execute()
{
	if (nullptr == m_pSocketMatrix)
	{
		CModel* pModel = static_cast<CModel*>(m_pGameObject->Find_Component(L"Com_Model"));
		m_pSocketMatrix = &pModel->Get_BoneCombinedMatrix_By_Name("Nutcracker.L");
	}

	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	_vector vEffectLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) }, vPos{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vCamPos{}, vCamLook{};
	vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
	vPos = (XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(&m_pTransform->Get_WorldMatrix_Ref())).r[(_uint)CTransform::STATE::POSITION];
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_bool bIsFlip{ false };
	_float fRotDegree{ 0.f };

	switch (m_iEventType)
	{
	case NPCINK_EFFECT_TYPE::FLY:
		// pGameInstance->Call_Effect((_uint)EFFECT_TYPE::ENEMY_SPRITE_SQUID, vPos, vEffectLook, false, nullptr);
		break;
	case NPCINK_EFFECT_TYPE::CHARGE:
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE00_PTCL_CHARGING, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE00_PTCL_CHARGE, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE01_PTCL_CIRCLE, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE02_PTCL_FLOWER, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE03_PTCL_DIAMOND, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE04_PTCL_CIRCLE, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE05_PTCL_INNERCIRCLE, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE06_PTCL_SHIRINKDIAMOND, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::INKERTON_CHARGE07_PTCL_FIXEDDIAMOND, vZeroPos, vEffectLook, true, m_pSocketMatrix, fRotDegree, bIsFlip, &m_pTransform->Get_WorldMatrix_Ref());
		break;
	case NPCINK_EFFECT_TYPE::IMPACT:
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::INKERTON_GUNSHOT00_PTCL_DIAMOND, vPos, vEffectLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_PARRY_REDBIGONE, vPos, vEffectLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPBUBBLE, vPos, vEffectLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTSCENE_PTCL_CHITAN_FRACTIONSPARK, vPos, vEffectLook, false, nullptr);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::CUTSCENE_PTCL_CHITAN_EXPLSLOWDOWNSPARK, vPos, vEffectLook, false, nullptr);

		vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
		pGameInstance->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook, vCamLook, false, nullptr);
		break;
	default:
		break;
	}
	return S_OK;
}

CEvent_NPC_Effect_Inkerton* CEvent_NPC_Effect_Inkerton::Create(CGameObject* _pGameObject, _uint _iEventType)
{
	CEvent_NPC_Effect_Inkerton* pInstance = new CEvent_NPC_Effect_Inkerton(_pGameObject, _iEventType);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_NPC_Effect_Inkerton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_NPC_Effect_Inkerton::Free()
{
	__super::Free();
}

#include "pch.h"
#include "Bowman_Event_Effect.h"
#include "GameInstance.h"
#include "GameObject.h"

CBowman_Event_Effect::CBowman_Event_Effect(CGameObject* _pGameObject, _uint _iEffectType)
{
	CModel* pModelCom = static_cast<CModel*>(_pGameObject->Find_Component(L"Com_Model"));
	m_pGameObject = _pGameObject;
	m_iEffectType = _iEffectType;
	m_pParentWorldMatrix = &static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag))->Get_WorldMatrix_Ref();
	m_pClawArm = &pModelCom->Get_BoneCombinedMatrix_By_Name("Arrow.75_end_end");
	m_pRubberBand = &pModelCom->Get_BoneCombinedMatrix_By_Name("RubberBand1_big_001.38");
}

HRESULT CBowman_Event_Effect::Execute()
{
	// BOWMAN_PTCL_BASH
	// BOWMAN_PTCL_CLAW
	// BOWMAN_PTCL_SANDBURST

	_vector vPos{}, vUp{}, vLook{}, vCamPos{}, vCamLook{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	CTransform* pTransformCom = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	vPos = pTransformCom->Get_State(CTransform::STATE::POSITION);
	vUp = pTransformCom->Get_State(CTransform::STATE::UP);
	
	
	switch (m_iEffectType)
	{
	case JUMP: // 걍 트랜스폼
		// 샌드 버스트 & 디스토션
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::BOWMAN_PTCL_SANDBURST, vPos, vZeroLook, false, nullptr);
		vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
		vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook, vCamLook, false, nullptr);
		break;
	case DISTORTION: // 러버 밴드
		// 디스토션
		vPos = (XMLoadFloat4x4(m_pRubberBand) * XMLoadFloat4x4(m_pParentWorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPBUBBLE, vPos, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::BOWMAN_PTCL_CLAW, vPos - vUp * 0.5f, vZeroLook, false, nullptr);
		vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
		vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook, vCamLook, false, nullptr);
		break;
	case BASH: // 배쉬 암
		// 배쉬
		// vPos = (XMLoadFloat4x4(m_pBashArm) * XMLoadFloat4x4(m_pParentWorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::BOWMAN_PTCL_BASH, vZeroPos, vZeroLook, true, m_pRubberBand, 0.f, false, m_pParentWorldMatrix);
		break;
	case CLAW: // 클로 암
		// 클로 & 샌드 버스트
		vPos = (XMLoadFloat4x4(m_pClawArm) * XMLoadFloat4x4(m_pParentWorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::BOWMAN_PTCL_CLAW, vPos - vUp * 0.5f, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::BOWMAN_PTCL_SANDBURST, vPos - vUp * 0.5f, vZeroLook, false, nullptr);
		break;
	default:
		break;
	}
	return S_OK;
}

CBowman_Event_Effect* CBowman_Event_Effect::Create(CGameObject* _pGameObject, _uint _iEffectType)
{
	CBowman_Event_Effect* pInstance = new CBowman_Event_Effect(_pGameObject, _iEffectType);

	return pInstance;
}

void CBowman_Event_Effect::Free()
{
	__super::Free();
}

#include "pch.h"
#include "Cuttlefish_Event_Effect.h"
#include "GameInstance.h"
#include "EffectObject.h"

CCuttlefish_Event_Effect::CCuttlefish_Event_Effect(CGameObject* _pGameObject, _uint _iEffectType)
{
	CModel* pModelCom = static_cast<CModel*>(_pGameObject->Find_Component(L"Com_Model"));
	m_pGameObject = _pGameObject;
	m_iEffectType = _iEffectType;
	m_pParentWorldMatrix = &static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag))->Get_WorldMatrix_Ref();
	m_pLeftArm = &pModelCom->Get_BoneCombinedMatrix_By_Name("tentacle3_03_L.47_end_end_end");
	m_pRightArm = &pModelCom->Get_BoneCombinedMatrix_By_Name("tentacle3_03_R.61_end_end_end");
	m_pPunchMatrix = &pModelCom->Get_BoneCombinedMatrix_By_Name("HB_Punch.001");
}

HRESULT CCuttlefish_Event_Effect::Execute()
{
	_vector vPos{}, vUp{}, vLook{}, vCamPos{}, vCamLook{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	CTransform* pTransformCom = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	vPos = pTransformCom->Get_State(CTransform::STATE::POSITION);
	vUp = pTransformCom->Get_State(CTransform::STATE::UP);
	vLook = pTransformCom->Get_State(CTransform::STATE::LOOK);


	switch (m_iEffectType)
	{
	case INK:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::CUTTLEFISH_PTCL_INK, vPos, vZeroLook, false, nullptr);
		break;
	case LEFT_CHARGE:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::CUTTLEFISH_PTCL_PROJ_CHARGE, vZeroPos, vZeroLook, true, m_pLeftArm, 0.f, false, m_pParentWorldMatrix);
		break;
	case RIGHT_CHARGE:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::CUTTLEFISH_PTCL_PROJ_CHARGE, vZeroPos, vZeroLook, true, m_pRightArm, 0.f, false, m_pParentWorldMatrix);
		break;
	case PUNCH_READY:
		break;
	case PUNCH:
		break;
	case PUNCH_IMPACT:
		vPos = (XMLoadFloat4x4(m_pPunchMatrix) * XMLoadFloat4x4(m_pParentWorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPBUBBLE, vPos, vZeroLook, false, nullptr);
		vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
		vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook, vCamLook, false, nullptr);
		break;
	case GRAB:
		break;
	default:
		break;
	}
	return S_OK;
}

CCuttlefish_Event_Effect* CCuttlefish_Event_Effect::Create(CGameObject* _pGameObject, _uint _iEffectType)
{
	CCuttlefish_Event_Effect* pInstance = new CCuttlefish_Event_Effect(_pGameObject, _iEffectType);

	return pInstance;
}

void CCuttlefish_Event_Effect::Free()
{
	__super::Free();
}

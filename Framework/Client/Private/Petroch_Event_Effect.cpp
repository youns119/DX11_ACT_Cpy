#include "pch.h"
#include "Petroch_Event_Effect.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"

CPetroch_Event_Effect::CPetroch_Event_Effect(CGameObject* _pGameObject, _uint _iEffectType)
{
	CModel* pModelCom = static_cast<CModel*>(_pGameObject->Find_Component(L"Com_Model"));
	m_pGameObject = _pGameObject;
	m_iEffectType = _iEffectType;
	m_pParentWorldMatrix = &static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag))->Get_WorldMatrix_Ref();
	m_pMidLegFootL = &pModelCom->Get_BoneCombinedMatrix_By_Name("leg1_foot_l");
	m_pMidLegFootR = &pModelCom->Get_BoneCombinedMatrix_By_Name("leg1_foot_r");
	m_pBackLegFootL = &pModelCom->Get_BoneCombinedMatrix_By_Name("leg2_foot_l");
	m_pBackLegFootR = &pModelCom->Get_BoneCombinedMatrix_By_Name("leg2_foot_r");
	m_pArmL = &pModelCom->Get_BoneCombinedMatrix_By_Name("arm_l_002");
	m_pArmR = &pModelCom->Get_BoneCombinedMatrix_By_Name("arm_r_002");
}

HRESULT CPetroch_Event_Effect::Execute()
{
	_vector vPos{}, vUp{}, vLook{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	CTransform* pTransformCom = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	vPos = pTransformCom->Get_State(CTransform::STATE::POSITION);
	vUp = pTransformCom->Get_State(CTransform::STATE::UP);
	// CHARGE, JUMP, GROUNDSLAM, POUNCE
	switch (m_iEffectType)
	{
	case CHARGE:// µÚ µÎ¹ß
		break;
	case JUMP:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_SANDBURST, vPos, vZeroLook, false, nullptr);
		break;
	case GROUNDSLAM: // µÞ¹ß Â÷±â
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_SANDBURST, vPos, vZeroLook, false, nullptr);

		vPos = (XMLoadFloat4x4(m_pBackLegFootL) * XMLoadFloat4x4(m_pParentWorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_POUNCE, vPos, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CRYSTAL_INSALL_SMOKE, vPos, vZeroLook, false, nullptr);

		vPos = (XMLoadFloat4x4(m_pBackLegFootR) * XMLoadFloat4x4(m_pParentWorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_POUNCE, vPos, vZeroLook, false, nullptr);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CRYSTAL_INSALL_SMOKE, vPos, vZeroLook, false, nullptr);
		break;
	case POUNCE: // ¾Õ µÎ¹ß, ÁÂ µÞ¹ß, ¿ì Áß¹ß
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_POUNCE, vUp * -1.f, vZeroLook, true, m_pArmL, 0.f, false, m_pParentWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_POUNCE, vUp * -1.f, vZeroLook, true, m_pArmR, 0.f, false, m_pParentWorldMatrix);
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_SANDBURST, vPos, vZeroLook, false, nullptr);
		break;
	default:
		break;
	}
	return S_OK;
}

CPetroch_Event_Effect* CPetroch_Event_Effect::Create(CGameObject* _pGameObject, _uint _iEffectType)
{
	CPetroch_Event_Effect* pInstance = new CPetroch_Event_Effect(_pGameObject, _iEffectType);

	return pInstance;
}

void CPetroch_Event_Effect::Free()
{
	__super::Free();
}

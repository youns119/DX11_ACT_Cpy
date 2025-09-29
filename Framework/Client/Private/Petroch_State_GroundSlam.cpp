#include "pch.h"
#include "Petroch_State_GroundSlam.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"
#include "Animation.h"

#include "EffectObject.h"

CPetroch_State_GoundSlam::CPetroch_State_GoundSlam()
{
}

HRESULT CPetroch_State_GoundSlam::Initialize(void* pObj)
{
	m_strStateName = "GoundSlam";

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 3.f;

	m_pPetroch = static_cast<CBoss_Petroch*>(pObj);
	m_pModel = static_cast<CModel*>(m_pPetroch->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(m_pPetroch->Find_Component(TEXT("Com_Transform")));

	//m_pMovement = static_cast<CMovement*>(m_pPetroch->Find_Component(TEXT("Com_Movement")));

	m_pParentWorldMatrix = &m_pTransform->Get_WorldMatrix_Ref();
	m_pMidLegFootL = &m_pModel->Get_BoneCombinedMatrix_By_Name("leg1_foot_l");
	m_pMidLegFootR = &m_pModel->Get_BoneCombinedMatrix_By_Name("leg1_foot_r");
	m_pBackLegFootL = &m_pModel->Get_BoneCombinedMatrix_By_Name("leg2_foot_l");
	m_pBackLegFootR = &m_pModel->Get_BoneCombinedMatrix_By_Name("leg2_foot_r");
	return S_OK;
}

void CPetroch_State_GoundSlam::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::GROUNDSLAM, false, 0.2f);
	m_iAnimIndex = (_uint)PETROCH_ANIM_INDEX::GROUNDSLAM;

	Call_Effects();
	CGameInstance::GetInstance()->Play_Sound(L"ChargeUp.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK1, 0.7f);

	super::Enter(pObj);
}

void CPetroch_State_GoundSlam::Update(_float fTimeDelta, void* pObj)
{
	if (m_iAnimIndex == (_uint)PETROCH_ANIM_INDEX::GROUNDSLAM && !m_pModel->Get_IsPlaying((_uint)PETROCH_ANIM_INDEX::GROUNDSLAM)) {
		static_cast<CBoss_Petroch*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}


void CPetroch_State_GoundSlam::Exit(void* pObj)
{	
	Disable_Effects();
	super::Exit(pObj);
}

void CPetroch_State_GoundSlam::Call_Effects()
{
	CEffectObject* pEffect = nullptr;
	_vector vPos{}, vUp{}, vLook{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	if (false == m_EffectObjects.empty())
		Disable_Effects();

	pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CHARGE, vZeroPos, vZeroLook, true, m_pBackLegFootL, 0.f, false, m_pParentWorldMatrix);
	m_EffectObjects.push_back(pEffect);
	pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PETROCH_SPRT_CHARGE, vZeroPos, vZeroLook, true, m_pBackLegFootL, 0.f, false, m_pParentWorldMatrix);
	m_EffectObjects.push_back(pEffect);

	pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_CHARGE, vZeroPos, vZeroLook, true, m_pBackLegFootR, 0.f, false, m_pParentWorldMatrix);
	m_EffectObjects.push_back(pEffect);
	pEffect = CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PETROCH_SPRT_CHARGE, vZeroPos, vZeroLook, true, m_pBackLegFootR, 0.f, false, m_pParentWorldMatrix);
	m_EffectObjects.push_back(pEffect);
}

void CPetroch_State_GoundSlam::Disable_Effects()
{
	if (true == m_EffectObjects.empty())
		return;

	for (auto& Effect : m_EffectObjects)
	{
		Effect->Clear_Info(0.f);
		Effect = nullptr;
	}
	m_EffectObjects.clear();
}

CPetroch_State_GoundSlam* CPetroch_State_GoundSlam::Create(void* pArg)
{
	CPetroch_State_GoundSlam* pInstance = new CPetroch_State_GoundSlam();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CPetroch_State_GoundSlam");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CPetroch_State_GoundSlam::Free()
{
	__super::Free();
}

#include "pch.h"
#include "Petroch_State_Roll.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Transform.h"

#include "Player.h"
#include "Boss_Petroch.h"


CPetroch_State_Roll::CPetroch_State_Roll()
{
}

HRESULT CPetroch_State_Roll::Initialize(void* pObj)
{
	m_strStateName = "Roll";
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 20.f;

	m_pPetroch = static_cast<CBoss_Petroch*>(pObj);
	m_pModel = static_cast<CModel*>(m_pPetroch->Find_Component(TEXT("Com_Model")));	
	m_pTransform = static_cast<CTransform*>(m_pPetroch->Find_Component(TEXT("Com_Transform")));
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pMovement = static_cast<CMovement*>(m_pPetroch->Find_Component(TEXT("Com_Movement")));
	return S_OK;
}

void CPetroch_State_Roll::Enter(void* pObj)
{				
	m_pPetroch->Set_Roll(true);
	m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::ROLL_ENTER, false, 0.2f);
	m_iCurAnimIndex = (_uint)PETROCH_ANIM_INDEX::ROLL_ENTER;
	m_fCurPreRollTime = 0.f;
	
	m_pMovement->Set_RotationScale(0.25f);		
	m_pMovement->Set_RotateToDirection(false);
	m_fMaxPreRollTime = CUtils::Compute_Random(5.f, 10.f);

	super::Enter(pObj);
}

void CPetroch_State_Roll::Update(_float fTimeDelta, void* pObj)
{
	if (m_iCurAnimIndex == (_uint)PETROCH_ANIM_INDEX::ROLL_ENTER && !m_pModel->Get_IsPlaying((_uint)PETROCH_ANIM_INDEX::ROLL_ENTER)) {
		m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::ROLL, true, 0.f);				
		m_iCurAnimIndex = (_uint)PETROCH_ANIM_INDEX::ROLL;
		m_pModel->Get_CurrAnimation()->Set_BaseAnimSpeed(1.5f);	
		m_pPetroch->Call_RollingEffects();
		
		CGameInstance::GetInstance()->Play_Sound_Loop(L"Petroch_Roll.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK1, 1.f);
	}	
	else if (m_iCurAnimIndex == (_uint)PETROCH_ANIM_INDEX::ROLL && Roll(fTimeDelta)) {
		m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::ROLL_EXIT, false, 0.2f);
		m_iCurAnimIndex = (_uint)PETROCH_ANIM_INDEX::ROLL_EXIT;
		m_pPetroch->Set_Roll(false);
		m_pMovement->Reset_Velocity();
		m_pPetroch->Disable_RollingEffects();

		CGameInstance::GetInstance()->Stop_Sound((_uint)SOUND_CHANNEL::ENEMY_ATTACK1);
		//CGameInstance::GetInstance()->Play_Sound(L"Petroch_RollThump1.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK1, 1.f);
	}
	else if (m_iCurAnimIndex == (_uint)PETROCH_ANIM_INDEX::ROLL_EXIT && !m_pModel->Get_IsPlaying((_uint)PETROCH_ANIM_INDEX::ROLL_EXIT)) {
		m_pPetroch->Select_Action();
	}
	
	if(m_iCurAnimIndex == (_uint)PETROCH_ANIM_INDEX::ROLL)
	{
		m_fEffectTimeAcc += fTimeDelta;
		if (m_fEffectTimeAcc >= m_fEffectTime)
		{
			m_fEffectTimeAcc = 0.f;
			m_pPetroch->Call_NonLoopRollingEffects();
		}
		m_pPetroch->Control_RollingEffects();
	}
	super::Update(fTimeDelta, pObj);
}

void CPetroch_State_Roll::Exit(void* pObj)
{	
	m_pMovement->Set_RotationScale(1.f);
	m_pModel->Get_CurrAnimation()->Set_BaseAnimSpeed(1.f);

	super::Exit(pObj);
}

_bool CPetroch_State_Roll::Roll(_float fTimeDelta)
{
	if (m_fCurPreRollTime >= m_fMaxPreRollTime)
		return true;
	else
		m_fCurPreRollTime += fTimeDelta;

	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);	
	_vector vTargetLook = XMVector4Normalize(XMVectorSubtract(vPlayerPos, vPos));
	_vector vLook = XMVector4Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

	_float fDistance = XMVectorGetX(XMVector4Length(vPlayerPos - vPos));
	vTargetLook = XMVectorSetY(vTargetLook, XMVectorGetY(vPos));
	vLook = XMVectorSetY(vLook, 0.f);

	_float fAngle = abs(CUtils::GetBetweenAngle(vTargetLook, vLook));
	
	if (fAngle < 5.f) {
		m_pTransform->Set_State(CTransform::STATE::LOOK, vTargetLook);
		m_pMovement->Set_Accel(60.f);
		m_pMovement->Set_MaxSpeed(60.f);
	}
	else if (fDistance < 3.f) {
		m_pMovement->Set_MaxSpeed(30.f);
		m_pMovement->Set_RotationScale(0.f);
	}
	else if (fAngle < 180.f && fAngle > 151.f ) {
		m_pMovement->Set_MaxSpeed(30.f);
		m_pMovement->Set_RotationScale(0.05f);
	}
	else if (fAngle < 150.f && fAngle >= 20.f) {
		m_pMovement->Set_MaxSpeed(30.f);
		m_pMovement->Set_RotationScale(0.7f);
	}
	else {
		m_pMovement->Set_MaxSpeed(10.f);
		m_pMovement->Set_RotationScale(.55f);
	}
			
	m_pMovement->TurnToDirection(vPlayerPos, fTimeDelta);
	m_pMovement->Add_Velocity(vLook, fTimeDelta);

	return false;
}

CPetroch_State_Roll* CPetroch_State_Roll::Create(void* pArg)
{
	CPetroch_State_Roll* pInstance = new CPetroch_State_Roll();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CPetroch_State_Roll");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CPetroch_State_Roll::Free()
{
	__super::Free();
}
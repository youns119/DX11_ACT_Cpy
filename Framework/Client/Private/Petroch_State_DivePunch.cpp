#include "pch.h"
#include "Petroch_State_DivePunch.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"
#include "Animation.h"


CPetroch_State_DivePunch::CPetroch_State_DivePunch()
{
}

HRESULT CPetroch_State_DivePunch::Initialize(void* pObj)
{
	m_strStateName = "DivePunch";

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 3.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));

	m_pPetroch = static_cast<CBoss_Petroch*>(pObj);
	m_pModel = static_cast<CModel*>(m_pPetroch->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(m_pPetroch->Find_Component(TEXT("Com_Transform")));
	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	//m_pMovement = static_cast<CMovement*>(m_pPetroch->Find_Component(TEXT("Com_Movement")));

	return S_OK;
}

void CPetroch_State_DivePunch::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::DIVE_PUNCH, false, 0.2f);
	m_iAnimIndex = (_uint)PETROCH_ANIM_INDEX::DIVE_PUNCH;
	m_fInterporateTime = 0.f;	
	m_fDive_Speed = 0.f;
	m_bIsDive = false;

	CGameInstance::GetInstance()->Play_Sound(L"Heikea Jump Impact.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE2, 0.7f);	
	super::Enter(pObj);
}

void CPetroch_State_DivePunch::Update(_float fTimeDelta, void* pObj)
{
	if (!m_bIsDive && m_iAnimIndex == (_uint)PETROCH_ANIM_INDEX::DIVE_PUNCH && m_pModel->Get_CurrAnimation()->Get_Blendable()) {
		m_pModel->Get_CurrAnimation()->Set_BaseAnimSpeed(0.5f);
		_vector vCurPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vCurPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
		XMStoreFloat4(&m_vDivePos, vCurPlayerPos);		
		m_bIsDive = true;
	}

	if (m_iAnimIndex == (_uint)PETROCH_ANIM_INDEX::DIVE_PUNCH && !m_pModel->Get_IsPlaying((_uint)PETROCH_ANIM_INDEX::DIVE_PUNCH)) {		
		CGameInstance::GetInstance()->Play_Sound(L"PetrochVO_Grunt1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.7f);
		static_cast<CBoss_Petroch*>(pObj)->Select_Action();
	}

	if (m_iAnimIndex == (_uint)PETROCH_ANIM_INDEX::DIVE_PUNCH && m_bIsDive) {
		if (m_fInterporateTime > 1.0f) 
			return;		

		m_fInterporateTime += (m_fDive_Speed / 10) * fTimeDelta;
		if (m_fInterporateTime > 1.0f) {
			m_pModel->Get_CurrAnimation()->Set_BaseAnimSpeed(1.f);
			m_fInterporateTime = 1.0f;					
		}
		else {		
			Compute_DiveSpeed();
			Dive(fTimeDelta, m_fInterporateTime);
		}			
	}

	super::Update(fTimeDelta, pObj);
}

void CPetroch_State_DivePunch::Exit(void* pObj)
{	
	super::Exit(pObj);
}

void CPetroch_State_DivePunch::Compute_DiveSpeed()
{
	_vector vCurPos = m_pTransform->Get_State(CTransform::STATE::POSITION);	
	_float fDis = Compute_Distance(XMLoadFloat4(&m_vDivePos), vCurPos);

	_float fCurFrame = (_float)m_pModel->Get_CurrAnimation()->Get_TrackPosition();
	_float fCurAnimSpeed = m_pModel->Get_CurrAnimation()->Get_BaseAnimSpeed();
	_float fRemainAnimTime = m_pModel->Get_CurrAnimation()->Get_Duration();
	_float fTickPerSec = (_float)m_pModel->Get_CurrAnimation()->Get_TickPerSecond();
	_float fReminingTime = (((91.f - fCurFrame) / (fTickPerSec * fCurAnimSpeed)));	
	 
	m_fDive_Speed = (2 * fDis) / (fReminingTime * fReminingTime);	
}

void CPetroch_State_DivePunch::Dive(_float fTimeDelta, _float t)
{
	_vector vCurPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	XMStoreFloat4(&m_vDivePos, XMVectorSetY(XMLoadFloat4(&m_vDivePos), XMVectorGetY(vCurPos)));
	m_pTransform->Set_State(CTransform::STATE::POSITION, XMVectorLerp(vCurPos, XMLoadFloat4(&m_vDivePos), t));
}

_float CPetroch_State_DivePunch::Compute_Distance(_vector vA, _vector vB)
{
	_float dx = XMVectorGetX(vA) - XMVectorGetX(vB);
	_float dy = XMVectorGetY(vA) - XMVectorGetY(vB);
	_float dz = XMVectorGetZ(vA) - XMVectorGetZ(vB);

	return sqrt(dx * dx + dy * dy + dz * dz);
}

CPetroch_State_DivePunch* CPetroch_State_DivePunch::Create(void* pArg)
{
	CPetroch_State_DivePunch* pInstance = new CPetroch_State_DivePunch();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CPetroch_State_DivePunch");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CPetroch_State_DivePunch::Free()
{
	__super::Free();
}
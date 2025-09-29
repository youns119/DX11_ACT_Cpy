#include "pch.h"
#include "Pagurus_State_Chase.h"
#include "GameInstance.h"
#include "Boss_Pagurus.h"

#include "FSM.h"

CPagurus_State_Chase::CPagurus_State_Chase()
{
}

HRESULT CPagurus_State_Chase::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Chase";
#endif
	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 0.f;

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pPagurus = static_cast<CBoss_Pagurus*>(pObj);
	m_pModel = static_cast<CModel*>(m_pPagurus->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(m_pPagurus->Find_Component(TEXT("Com_Transform")));
	m_pWorldMatrix = &m_pTransform->Get_WorldMatrix_Ref();
	m_pFSM = static_cast<CFSM*>(m_pPagurus->Find_Component(TEXT("Com_StateMachine")));
	m_fSandBurstTime = 0.15f;
	return S_OK;;
}

void CPagurus_State_Chase::Enter(void* pObj)
{
	m_pMovement = static_cast<CMovement*>(m_pPagurus->Find_Component(TEXT("Com_Movement")));
	
	m_pModel->SetUp_Animation((_uint)PAGURUS_ANIM_INDEX::IDLE, true, 0.01f);

	__super::Enter(pObj);
}

void CPagurus_State_Chase::Update(_float fTimeDelta, void* pObj)
{
	_vector vLookTarget = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	m_pMovement->TurnToDirection(vLookTarget, fTimeDelta * 0.5f);
	m_pTransform->Go_Straight(fTimeDelta * 2.5f);

	_vector vDis = vLookTarget - m_pTransform->Get_State(CTransform::STATE::POSITION);
	_float fDis = XMVectorGetX(XMVector3Length(vDis));

	if (fDis < 4) {
		m_pPagurus->Select_Action();
	}
	else if (!m_pPagurus->Detect_Player()) {
		m_pPagurus->Select_Action();
	}
	Call_SandBurst(fTimeDelta);
	__super::Update(fTimeDelta, pObj);
}

void CPagurus_State_Chase::Exit(void* pObj)
{
	__super::Enter(pObj);
}

void CPagurus_State_Chase::Call_SandBurst(_float fTimeDelta)
{
	m_fSandBurstTimeAcc += fTimeDelta;
	if (m_fSandBurstTimeAcc >= m_fSandBurstTime)
	{
		m_fSandBurstTimeAcc = 0.f;
		// 샌드버스트 콜링
		// PAGURUS_PTCL_SANDBURST
		const _float4x4* pLeftFootUpper = nullptr; // L 1, 2
		const _float4x4* pLeftFootUnder = nullptr; // L 3, 4

		const _float4x4* pRightFootUpper = nullptr; // R 1, 2
		const _float4x4* pRightFootUnder = nullptr; // R 3, 4
		_vector vPos{}, vLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (true == m_bLeftTurn)
		{
			pLeftFootUpper = &m_pModel->Get_BoneCombinedMatrix_By_Name("Foot_L_1");
			vPos = (XMLoadFloat4x4(pLeftFootUpper) * XMLoadFloat4x4(m_pWorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
			pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PAGURUS_PTCL_SANDBURST, vPos, vLook, false, nullptr);
			pLeftFootUnder = &m_pModel->Get_BoneCombinedMatrix_By_Name("Foot_L_3");
			vPos = (XMLoadFloat4x4(pLeftFootUnder) * XMLoadFloat4x4(m_pWorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
			pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PAGURUS_PTCL_SANDBURST, vPos, vLook, false, nullptr);

			pRightFootUpper = &m_pModel->Get_BoneCombinedMatrix_By_Name("Foot_R_2");
			vPos = (XMLoadFloat4x4(pRightFootUpper) * XMLoadFloat4x4(m_pWorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
			pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PAGURUS_PTCL_SANDBURST, vPos, vLook, false, nullptr);

			pRightFootUnder = &m_pModel->Get_BoneCombinedMatrix_By_Name("Foot_R_4");
			vPos = (XMLoadFloat4x4(pRightFootUnder) * XMLoadFloat4x4(m_pWorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
			pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PAGURUS_PTCL_SANDBURST, vPos, vLook, false, nullptr);
		}
		else 
		{
			pLeftFootUpper = &m_pModel->Get_BoneCombinedMatrix_By_Name("Foot_L_2");
			vPos = (XMLoadFloat4x4(pLeftFootUpper) * XMLoadFloat4x4(m_pWorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
			pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PAGURUS_PTCL_SANDBURST, vPos, vLook, false, nullptr);
			pLeftFootUnder = &m_pModel->Get_BoneCombinedMatrix_By_Name("Foot_L_4");
			vPos = (XMLoadFloat4x4(pLeftFootUnder) * XMLoadFloat4x4(m_pWorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
			pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PAGURUS_PTCL_SANDBURST, vPos, vLook, false, nullptr);

			pRightFootUpper = &m_pModel->Get_BoneCombinedMatrix_By_Name("Foot_R_1");
			vPos = (XMLoadFloat4x4(pRightFootUpper) * XMLoadFloat4x4(m_pWorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
			pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PAGURUS_PTCL_SANDBURST, vPos, vLook, false, nullptr);

			pRightFootUnder = &m_pModel->Get_BoneCombinedMatrix_By_Name("Foot_R_3");
			vPos = (XMLoadFloat4x4(pRightFootUnder) * XMLoadFloat4x4(m_pWorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
			pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PAGURUS_PTCL_SANDBURST, vPos, vLook, false, nullptr);
		}
	}
}

CPagurus_State_Chase* CPagurus_State_Chase::Create(void* pArg)
{
	CPagurus_State_Chase* pInstance = new CPagurus_State_Chase();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CFSM");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPagurus_State_Chase::Free()
{
	__super::Free();
}

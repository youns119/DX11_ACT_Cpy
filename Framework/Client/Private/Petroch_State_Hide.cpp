#include "pch.h"
#include "Petroch_State_Hide.h"
#include "GameInstance.h"
#include "Boss_Petroch.h"
#include "Animation.h"

#include "Player.h"
#include "UI_Manager.h"


CPetroch_State_Hide::CPetroch_State_Hide()
{
}

HRESULT CPetroch_State_Hide::Initialize(void* pObj)
{
	m_strStateName = "Hide";

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 15.f;

	m_pPetroch = static_cast<CBoss_Petroch*>(pObj);
	m_pModel = static_cast<CModel*>(m_pPetroch->Find_Component(TEXT("Com_Model")));
	return S_OK;
}

void CPetroch_State_Hide::Enter(void* pObj)
{		
	m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::AGGRO_HIDING, false, 0.2f);
	m_iAnimIndex = (_uint)PETROCH_ANIM_INDEX::AGGRO_HIDING;
	m_pPetroch->Set_Hide(true);
		
	super::Enter(pObj);
}

void CPetroch_State_Hide::Update(_float fTimeDelta, void* pObj)
{
	if (m_pPetroch->Is_Hide() && CGameInstance::GetInstance()->Key_Down(KEY::E))
	{
		CUI_Manager* pUIManager = static_cast<CUI_Manager*>(CGameInstance::GetInstance()->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
		pUIManager->Hide_Interaction();

		static_cast<CCollider*>(m_pPetroch->Find_Component(L"Com_Collider"))->Set_Active(true);
		static_cast<CCollider*>(m_pPetroch->Find_Component(L"Com_ShellHouseCollider"))->Set_Active(false);

		m_pModel->SetUp_Animation((_uint)PETROCH_ANIM_INDEX::PARRY, true, 0.2f);
		m_iAnimIndex = (_uint)PETROCH_ANIM_INDEX::PARRY;
		CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
		static_cast<CPlayer*>(pPlayer)->KnockBack(45.f, IDamaged::DAMAGE::DAMAGE_SUPER, m_pPetroch);
		m_pPetroch->Set_Hide(false);
		Call_ParryEffect();

		CGameInstance::GetInstance()->Play_Sound(L"King2_Tantrum_Impact_1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.6f);
		CGameInstance::GetInstance()->FadeSwap_Sound(L"Petroch OGG 4x Ultra.wav", (_uint)SOUND_CHANNEL::BGM,0.1f, 0.5f);	
	}

	if (m_iAnimIndex == (_uint)PETROCH_ANIM_INDEX::PARRY && !m_pModel->Get_IsPlaying((_uint)PETROCH_ANIM_INDEX::PARRY)) {

		m_pPetroch->Set_Hide(false);
		m_pPetroch->Set_Aggroed(true);
		m_pPetroch->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CPetroch_State_Hide::Exit(void* pObj)
{	
	super::Exit(pObj);
}

void CPetroch_State_Hide::Call_ParryEffect()
{
	CTransform* pTransform = static_cast<CTransform*>(m_pPetroch->Find_Component(g_strTransformTag));
	const _float4x4* pParentWorldMatrix = &pTransform->Get_WorldMatrix_Ref();
	const _float4x4* pArmL = &m_pModel->Get_BoneCombinedMatrix_By_Name("arm_l_002");
	const _float4x4* pArmR = &m_pModel->Get_BoneCombinedMatrix_By_Name("arm_r_002");
	_vector vPos{}, vUp{}, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) }, vCamPos{ XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition()) }, vCamLook{};
	vUp = pTransform->Get_State(CTransform::STATE::UP);
	vPos = pTransform->Get_State(CTransform::STATE::POSITION);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_POUNCE, vUp * -1.f, vZeroLook, true, pArmL, 0.f, false, pParentWorldMatrix);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PETROCH_PTCL_POUNCE, vUp * -1.f, vZeroLook, true, pArmR, 0.f, false, pParentWorldMatrix);

	vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook * 2.f + vUp * 0.5f, vCamLook, false, nullptr);
}

CPetroch_State_Hide* CPetroch_State_Hide::Create(void* pArg)
{
	CPetroch_State_Hide* pInstance = new CPetroch_State_Hide();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CPetroch_State_Hide");
		Safe_Release(pInstance);
	}
	return pInstance; 
}

void CPetroch_State_Hide::Free()
{
	__super::Free();
}

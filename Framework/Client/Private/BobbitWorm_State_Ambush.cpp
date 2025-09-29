#include "pch.h"
#include "BobbitWorm_State_Ambush.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Enemy_BobbitWorm.h"
#include "Animation.h"
#include "CineCamera.h"

CBobbitWorm_State_Ambush::CBobbitWorm_State_Ambush()
{
}

HRESULT CBobbitWorm_State_Ambush::Initialize(void* pObj)
{
	m_strStateName = "Execution Attack";
	m_iPriorityLevel = 2;
	m_fMaxCoolTime = 10.f;

	m_pBobbitWorm = static_cast<CEnemy_BobbitWorm*>(pObj);
	m_pModel = static_cast<CModel*>(m_pBobbitWorm->Find_Component(TEXT("Com_Model")));	
	return S_OK;
}

void CBobbitWorm_State_Ambush::Enter(void* pObj)
{	
	m_pModel->SetUp_Animation((_uint)BOBBITWORM_ANIM_INDEX::EXECUTION, false, 0.3f);
	m_iAnimIndex = (_uint)BOBBITWORM_ANIM_INDEX::EXECUTION;

	m_pBobbitWorm->Set_IsGrab(true);
	m_pBobbitWorm->Set_Aggroed(true);
	
	CGameInstance::GetInstance()->Play_Sound(L"Bobbit_Scream.wav", (_uint)SOUND_CHANNEL::ENEMY_ATTACK1, 0.3f);

	Call_Effects();
	__super::Enter(pObj);
}

void CBobbitWorm_State_Ambush::Update(_float fTimeDelta, void* pObj)
{
	if ((m_iAnimIndex == (_uint)BOBBITWORM_ANIM_INDEX::EXECUTION && m_pModel->Get_CurrAnimation()->Get_Blendable())) {
		m_pBobbitWorm->Set_IsGrab(false);
		if ((m_iAnimIndex == (_uint)BOBBITWORM_ANIM_INDEX::EXECUTION && !m_pModel->Get_IsPlaying((_uint)BOBBITWORM_ANIM_INDEX::EXECUTION)) && m_pBobbitWorm->Is_Execution()) {			
			CCineCamera* pCineCamera = static_cast<CCineCamera*>(m_pBobbitWorm->Find_PartObject({ L"Part_CineCamera" }));
			pCineCamera->PositionShake(0.f, 0.f, 0.f, false);
			m_pBobbitWorm->Select_Action();
		}
		else if (!m_pBobbitWorm->Is_Execution()) {
			m_pModel->SetUp_Animation((_uint)BOBBITWORM_ANIM_INDEX::ATTACK_MISS, false, 0.1f);			
			m_iAnimIndex = (_uint)BOBBITWORM_ANIM_INDEX::ATTACK_MISS;
		}		
	}
	else if (m_iAnimIndex == (_uint)BOBBITWORM_ANIM_INDEX::ATTACK_MISS && !m_pModel->Get_IsPlaying((_uint)BOBBITWORM_ANIM_INDEX::ATTACK_MISS)) {
		m_pBobbitWorm->Select_Action();
	}
}

void CBobbitWorm_State_Ambush::Exit(void* pObj)
{
	m_pBobbitWorm->Set_IsGrab(false);
	m_pBobbitWorm->Set_Execution(false);
	m_iAnimIndex = 0;
	__super::Exit(pObj);
}

void CBobbitWorm_State_Ambush::Call_Effects()
{
	_vector vPos{}, vUp{}, vLook{}, vCamPos{}, vCamLook{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	CTransform* pTransformCom = static_cast<CTransform*>(m_pBobbitWorm->Find_Component(g_strTransformTag));
	vPos = pTransformCom->Get_State(CTransform::STATE::POSITION);
	vUp = pTransformCom->Get_State(CTransform::STATE::UP);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::WORM_PTCL_SANDBURST_EXPL, vPos, vZeroLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::WORM_PTCL_SANDBURST_EXPLSLOWDOWN, vPos, vZeroLook, false, nullptr);
}

CBobbitWorm_State_Ambush* CBobbitWorm_State_Ambush::Create(void* pObj)
{
	CBobbitWorm_State_Ambush* pInstance = new CBobbitWorm_State_Ambush();
	if (FAILED(pInstance->Initialize(pObj)))
	{
		MSG_BOX("Failed To Create : CBobbitWorm_State_Ambush");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBobbitWorm_State_Ambush::Free()
{
	__super::Free();
}
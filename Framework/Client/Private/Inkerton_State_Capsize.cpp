#include "pch.h"
#include "Inkerton_State_Capsize.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "Inkerton_Nutcracker.h"
#include "Inkerton_StretchedNutcracker.h"
#include "Player.h"

CInkerton_State_Capsize::CInkerton_State_Capsize()
{
}

HRESULT CInkerton_State_Capsize::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Capsize";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 1.f;

	m_pModel = static_cast<CModel*>(static_cast<CGameObject*>(pObj)->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CInkerton_State_Capsize::Enter(void* pObj)
{		
	m_pInkerton = static_cast<CBoss_Inkerton*>(pObj);
	CInkerton_Nutcracker* pNutcracker = static_cast<CInkerton_Nutcracker*>(m_pInkerton->Find_PartObject({ TEXT("Part_Nutcracker") }));
	CInkerton_StretchedNutcracker* pStretchedNutcracker = static_cast<CInkerton_StretchedNutcracker*>(m_pInkerton->Find_PartObject({ TEXT("Part_StretchedNutcracker") }));
	static_cast<CCollider*>(pNutcracker->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(pNutcracker->Find_Component(TEXT("Com_HandleCollider")))->Set_Active(false);
	static_cast<CCollider*>(pStretchedNutcracker->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	
	_vector vLook = static_cast<CTransform*>(m_pInkerton->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::LOOK);
	CPlayer* pPlayer = static_cast<CPlayer*>(pNutcracker->Get_GrabObject());
	//if (pPlayer) pPlayer->UnGrab(vLook);
	pPlayer = static_cast<CPlayer*>(pStretchedNutcracker->Get_GrabObject());
	//if (pPlayer) pPlayer->UnGrab(vLook);

	m_pInkerton->Set_IsChase(false);
	m_pInkerton->Set_IsSideMove(false);
		
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::CAPSIZE, false, 0.2f);
	m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::CAPSIZE;

	CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Effort_Prone_1.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	CGameInstance::GetInstance()->Play_Sound(L"Inkerton_Prone.wav", (_uint)SOUND_CHANNEL::ENEMY_AMBIENCE2, 0.5f);
	super::Enter(pObj);
}

void CInkerton_State_Capsize::Update(_float fTimeDelta, void* pObj)
{					
	if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::CAPSIZE && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::CAPSIZE)) {
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::CAPSIZE_IDLE, true, 0.2f);
		m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::CAPSIZE_IDLE;
	}
	else if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::CAPSIZE_IDLE && Decrease_CapsizedDuration(fTimeDelta) && !m_pInkerton->IsFinisher()) {
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::CAPSIZE_EXIT, false, 0.2f);
		m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::CAPSIZE_EXIT;		
	}
	else if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::CAPSIZE_EXIT && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::CAPSIZE_EXIT)) {		
		m_pInkerton->Select_Action();
		m_pInkerton->Set_Capsized(false);
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_Capsize::Exit(void* pObj)
{	
	m_pInkerton->Set_Finished(false);
	m_fCapsizeTime = 0;
	super::Exit(pObj);
}

_bool CInkerton_State_Capsize::Decrease_CapsizedDuration(_float fTimeDelta)
{
	if (m_fCapsizeTime > m_fCapsizeDuration)
		return true;
	else
		m_fCapsizeTime += fTimeDelta;

	return false;
}


CInkerton_State_Capsize* CInkerton_State_Capsize::Create(void* pArg)
{
	CInkerton_State_Capsize* pInstance = new CInkerton_State_Capsize();	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_Capsize");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_Capsize::Free()
{
	__super::Free();
}
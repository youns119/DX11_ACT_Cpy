#include "pch.h"
#include "Cuttlefish_State_Hooked.h"
#include "GameInstance.h"
#include "Enemy_Cuttlefish.h"


CCuttlefish_State_Hooked::CCuttlefish_State_Hooked()
{
}

HRESULT CCuttlefish_State_Hooked::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Hooked";
#endif

	m_iPriorityLevel = 1;
	m_fMaxCoolTime = 10.f;

	m_pCuttlefish = static_cast<CEnemy_Cuttlefish*>(pObj);		

	CGameObject* pPlayer = CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back();
	m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
	m_pTransform = static_cast<CTransform*>(m_pCuttlefish->Find_Component(TEXT("Com_Transform")));

	m_pModel = static_cast<CModel*>(m_pCuttlefish->Find_Component(TEXT("Com_Model")));

	return S_OK;
}

void CCuttlefish_State_Hooked::Enter(void* pObj)
{	
	Reset_RadialblurParams();
	static_cast<CCollider*>(m_pCuttlefish->Find_PartObject({ TEXT("Part_Fist") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(m_pCuttlefish->Find_PartObject({ TEXT("Part_Tail") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);
	static_cast<CCollider*>(m_pCuttlefish->Find_PartObject({ TEXT("Part_Grab") })->Find_Component(TEXT("Com_Collider")))->Set_Active(false);

	m_pCuttlefish->Set_IsChase(false);
	m_pCuttlefish->Set_IsSideMove(false);

	m_pCuttlefish->Set_Hooked(true);
	m_pModel->SetUp_Animation((_uint)CUTTLEFISH_ANIM_INDEX::HOOKED, true);

	_vector temp = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector sour = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	sour = XMVectorSetY(sour, XMVectorGetY(temp));
	m_pTransform->LookAt(sour);

	CGameInstance::GetInstance()->Play_Sound(L"Cuttle_Hit3.wav", (_uint)SOUND_CHANNEL::ENEMY_VOICE1, 0.5f);
	__super::Enter(pObj);
}

void CCuttlefish_State_Hooked::Update(_float fTimeDelta, void* pObj)
{			
	if (!m_pCuttlefish->IsHooked())
		m_pCuttlefish->Select_Action();

	__super::Update(fTimeDelta, pObj);
}

void CCuttlefish_State_Hooked::Exit(void* pObj)
{	
	__super::Exit(pObj);
}

void CCuttlefish_State_Hooked::Reset_RadialblurParams()
{
	RADIALBLUR_PARAMS tRadialParam{};
	tRadialParam.fDensity = 2;
	tRadialParam.fIntensity = 0;
	CGameInstance::GetInstance()->Set_RadialBlurParams(tRadialParam);
}

CCuttlefish_State_Hooked* CCuttlefish_State_Hooked::Create(void* pArg)
{
	CCuttlefish_State_Hooked* pInstance = new CCuttlefish_State_Hooked();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCuttlefish_State_Hooked");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCuttlefish_State_Hooked::Free()
{
	__super::Free();
}

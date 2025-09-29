#include "pch.h"
#include "Inkerton_State_InkAmbushAttack.h"
#include "GameInstance.h"

#include "SpringArm_Player.h"

//Engine
#include "Animation.h"
#include "Transform.h"

CInkerton_State_InkAmbushAttack::CInkerton_State_InkAmbushAttack()
{
}

HRESULT CInkerton_State_InkAmbushAttack::Initialize(void* pObj)
{
#ifdef _DEBUG
	m_strStateName = "Ink Ambush Attack";
#endif

	m_iPriorityLevel = 1;	
	m_fMaxCoolTime = 40.f;

	m_pPlayer = static_cast<CPlayer*>(CGameInstance::GetInstance()->Get_GameObjects_In_Layer(CGameInstance::GetInstance()->Get_CurrentLevelIndex(), TEXT("Layer_Player")).back());
	m_pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pInkerton = static_cast<CBoss_Inkerton*>(pObj);
	m_pModel = static_cast<CModel*>(m_pInkerton->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(m_pInkerton->Find_Component(TEXT("Com_Transform")));

	static_cast<CBoss_Inkerton*>(pObj)->Set_IsChase(false);
	static_cast<CBoss_Inkerton*>(pObj)->Set_IsSideMove(false);
	return S_OK;
}

void CInkerton_State_InkAmbushAttack::Enter(void* pObj)
{	
	m_DefaultScale = static_cast<CTransform*>(m_pInkerton->Find_Component(L"Com_Transform"))->Get_Scale();
	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::PEACE_OUT, false, 0.2f);
	m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::PEACE_OUT;
	//m_pPlayer->Get_PlayerSpringArm()->UnLock_LockOn();
	static_cast<CBoss_Inkerton*>(pObj)->Play_BaterQuote();
	m_fCurRetreatTime = 0;
	super::Enter(pObj);
}

void CInkerton_State_InkAmbushAttack::Update(_float fTimeDelta, void* pObj)
{	
	if (!m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::PEACE_OUT) || (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::PEACE_OUT && m_pModel->Get_CurrAnimation()->Get_Blendable())) {		
		Decrease_Scale(fTimeDelta);
	}
	if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::SWIM_FORWRD_AMBUSH && !m_pModel->Get_IsPlaying((_uint)INKERTON_ANIM_INDEX::SWIM_FORWRD_AMBUSH) || (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::SWIM_FORWRD_AMBUSH && m_pModel->Get_CurrAnimation()->Get_Blendable())) {
		static_cast<CBoss_Inkerton*>(pObj)->Select_Action();
	}

	super::Update(fTimeDelta, pObj);
}

void CInkerton_State_InkAmbushAttack::Exit(void* pObj)
{
	super::Exit(pObj);
}

void CInkerton_State_InkAmbushAttack::Teleport_Behind()
{
	_vector vPosition = m_pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_vector vLook = m_pPlayerTransform->Get_State(CTransform::STATE::LOOK);

	vPosition -= XMVector3Normalize(vLook) * 20.f;

	m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
}

void CInkerton_State_InkAmbushAttack::Reset_Scale(_float fTimeDelta)
{	
	m_pTransform->Set_Scale(m_DefaultScale);	
}

void CInkerton_State_InkAmbushAttack::Decrease_Scale(_float fTimeDelta)
{
	if (m_iAnimIndex == (_uint)INKERTON_ANIM_INDEX::SWIM_FORWRD_AMBUSH)
		return;

	m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::IDLE, true, 0.2f);
	if (m_fCurRetreatTime > m_fMaxRetreatTime) {
		m_pModel->SetUp_Animation((_uint)INKERTON_ANIM_INDEX::SWIM_FORWRD_AMBUSH, false, 0.f);
		m_iAnimIndex = (_uint)INKERTON_ANIM_INDEX::SWIM_FORWRD_AMBUSH;
		Reset_Scale(fTimeDelta);
		Teleport_Behind();
		return;
	}
	else
		m_fCurRetreatTime += fTimeDelta;

	//m_fSacle = m_pTransform->Get_Scale();
	//m_fSacle = _float3(m_fSacle.x - m_fScaleDecreaseAmount, m_fSacle.y - m_fScaleDecreaseAmount, m_fSacle.z - m_fScaleDecreaseAmount);
	//m_pTransform->Set_Scale(m_fSacle);
	//m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 1.f), fTimeDelta * 2.5f);
}

CInkerton_State_InkAmbushAttack* CInkerton_State_InkAmbushAttack::Create(void* pArg)
{
	CInkerton_State_InkAmbushAttack* pInstance = new CInkerton_State_InkAmbushAttack();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CInkerton_State_InkAmbushAttack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInkerton_State_InkAmbushAttack::Free()
{
	__super::Free();
}
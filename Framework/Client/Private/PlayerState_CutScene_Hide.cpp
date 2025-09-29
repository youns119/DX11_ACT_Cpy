#include "pch.h"
#include "PlayerState_CutScene_Hide.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Movement.h"
#include "Animation.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_CutScene_Hide::CPlayerState_CutScene_Hide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_CutScene_Hide::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pPlayerState = pPlayerState;

	m_pPlayer = static_cast<CPlayer*>(Get_Character());

	m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
	m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

	m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("cutscene_Shellidle");

	return S_OK;
}

void CPlayerState_CutScene_Hide::Enter()
{
	m_pPlayerState->set(PLAYERSTATE_CUTSCENE_HIDE);

	//m_pPlayerStat->bInputEnable = false;

	m_pModel->SetUp_Animation(m_iAnimIndex, true, 0.1f);
}

void CPlayerState_CutScene_Hide::Update(_float fTimeDelta)
{
}

void CPlayerState_CutScene_Hide::Exit()
{
	m_pPlayerState->reset(PLAYERSTATE_CUTSCENE_HIDE);
	CGameInstance::GetInstance()->Play_Sound(L"Beach_Kril_01.wav", (_uint)SOUND_CHANNEL::CONVERSATION1, 1.f);
	//m_pPlayerStat->bInputEnable = true;
}

CPlayerState_CutScene_Hide* CPlayerState_CutScene_Hide::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_CutScene_Hide* pInstance = new CPlayerState_CutScene_Hide(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_CutScene_Hide");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_CutScene_Hide::Free()
{
	super::Free();
}
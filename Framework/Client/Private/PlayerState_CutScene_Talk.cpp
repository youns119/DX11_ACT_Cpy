#include "pch.h"
#include "PlayerState_CutScene_Talk.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Movement.h"
#include "Animation.h"
#include "StateMachine.h"

#include "Player.h"

CPlayerState_CutScene_Talk::CPlayerState_CutScene_Talk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : super{ pDevice, pContext }
{
}

HRESULT CPlayerState_CutScene_Talk::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
    if (FAILED(super::Initialize(pOwner)))
        return E_FAIL;

    m_pPlayerState = pPlayerState;

    m_pPlayer = static_cast<CPlayer*>(Get_Character());

    m_pModel = static_cast<CModel*>(m_pPlayer->Find_Component(L"Com_Model"));
    m_pMovement = static_cast<CMovement*>(m_pPlayer->Find_Component(L"Com_Movement"));

    m_pPlayerStat = m_pPlayer->Get_PlayerStat();

	m_iAnimIndex = m_pModel->Get_AnimationIndex("Cutscene-talk.Fork");

    return S_OK;
}

void CPlayerState_CutScene_Talk::Enter()
{
    m_pPlayerState->set(PLAYERSTATE_CUTSCENE_TALK);

    m_pPlayer->Reset_PlayerStat();

    m_pPlayerStat->bInputEnable = false;

    m_pModel->SetUp_Animation(m_iAnimIndex, false, 0.1f);
}

void CPlayerState_CutScene_Talk::Update(_float fTimeDelta)
{
    if (m_pModel->Get_CurrAnimation()->Get_Blendable())
        m_pOwner->ChangeState(L"Idle");
}

void CPlayerState_CutScene_Talk::Exit()
{
    m_pPlayerState->reset(PLAYERSTATE_CUTSCENE_TALK);

    m_pPlayerStat->bInputEnable = true;
}

CPlayerState_CutScene_Talk* CPlayerState_CutScene_Talk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
    CPlayerState_CutScene_Talk* pInstance = new CPlayerState_CutScene_Talk(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
    {
        MSG_BOX("Failed To Create : CPlayerState_CutScene_Talk");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerState_CutScene_Talk::Free()
{
    super::Free();
}
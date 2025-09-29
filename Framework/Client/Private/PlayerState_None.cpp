#include "pch.h"
#include "PlayerState_None.h"

#include "Model.h"

#include "Player.h"

CPlayerState_None::CPlayerState_None(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

HRESULT CPlayerState_None::Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	if (FAILED(super::Initialize(pOwner)))
		return E_FAIL;

	m_pModel = static_cast<CModel*>(Get_Character()->Find_Component(L"Com_Model"));

	m_pPlayerState = pPlayerState;

	return S_OK;
}

void CPlayerState_None::Enter()
{
	m_pModel->Activate_SlotAnim(ANIMSLOT::SLOT_UPPER, false);
}

void CPlayerState_None::Update(_float fTimeDelta)
{
}

void CPlayerState_None::Exit()
{
	m_pModel->Activate_SlotAnim(ANIMSLOT::SLOT_UPPER, true);
}

CPlayerState_None* CPlayerState_None::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState)
{
	CPlayerState_None* pInstance = new CPlayerState_None(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pOwner, pPlayerState)))
	{
		MSG_BOX("Failed To Create : CPlayerState_None");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerState_None::Free()
{
	super::Free();
}
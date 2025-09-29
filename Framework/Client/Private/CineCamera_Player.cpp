#include "pch.h"
#include "CineCamera_Player.h"

#include "GameInstance.h"

#include "Player.h"
#include "SpringArm_Player.h"

#include "IGrapple.h"

CCineCamera_Player::CCineCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CCineCamera_Player::CCineCamera_Player(const CCineCamera_Player& Prototype)
	: super(Prototype)
{
}

HRESULT CCineCamera_Player::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCineCamera_Player::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCineCamera_Player::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CCineCamera_Player::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CCineCamera_Player::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

void CCineCamera_Player::Final_Update(_float fTimeDelta)
{
	super::Final_Update(fTimeDelta);
}

HRESULT CCineCamera_Player::Render()
{
	if (FAILED(super::Render()))
		return E_FAIL;

	return E_NOTIMPL;
}

void CCineCamera_Player::Play_Cinematic(const _wstring& strCinematic)
{
	m_strCurr = strCinematic;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Find_Component(g_strTransformTag));
	CMovement* pMovement = static_cast<CMovement*>(m_pOwner->Find_Component(L"Com_Movement"));

	if (m_strCurr == L"Shelleport")
	{
		m_bInputEnable = true;
		pPlayer->Get_PlayerSpringArm()->Toggle_Input(false);

		SetUp_CurrCinematic(L"Shelleport", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEINOUT, 1.f);
		m_pGameInstance->Camera_Transition(L"CineCamera_Player", (_uint)CCameraManager::LERP::LERP_EASEINOUT, 1.f);
	}
	else if (m_strCurr == L"GrappleAttack")
	{
		m_bTimeLag = false;
		m_bInputEnable = false;

		pPlayer->Get_PlayerSpringArm()->Sync_Rotation();
		pPlayer->Get_PlayerSpringArm()->Toggle_Input(false);

		SetUp_CurrCinematic(L"GrappleAttack", L"Camera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 1.f);
		m_pGameInstance->Camera_Transition(L"CineCamera_Player", (_uint)CCameraManager::LERP::LERP_EASEOUT, 0.6f);
	}
}

void CCineCamera_Player::Stop_Cinematic()
{
	Reset();
	super::Stop();
}

void CCineCamera_Player::Reset()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	m_bTimeLag = true;
	m_bInputEnable = true;

	static_cast<CPlayer*>(m_pOwner)->Get_PlayerSpringArm()->Toggle_Input(true);

	if (m_strCurr == L"ParryAttack" ||
		m_strCurr == L"BackStepAttack" ||
		m_strCurr == L"GrappleAttack" ||
		m_strCurr == L"FinishAttack" ||
		m_strCurr == L"MantisPunch2")
	{
		pPlayer->Get_PlayerSpringArm()->Sync_Rotation();
	}
}

CCineCamera_Player* CCineCamera_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCineCamera_Player* pInstance = new CCineCamera_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CCineCamera_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCineCamera_Player::Clone(void* pArg)
{
	CCineCamera_Player* pInstance = new CCineCamera_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CCineCamera_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCineCamera_Player::Free()
{
	super::Free();
}
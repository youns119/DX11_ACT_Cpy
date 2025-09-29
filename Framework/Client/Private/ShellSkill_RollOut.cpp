#include "pch.h"
#include "ShellSkill_RollOut.h"

#include "GameInstance.h"
#include "Movement.h"
#include "StateMachine.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Shell_Part.h"

#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"

CShellSkill_RollOut::CShellSkill_RollOut(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CShellSkill_RollOut::CShellSkill_RollOut(const CShellSkill_RollOut& Prototype)
	: super(Prototype)
{
}

HRESULT CShellSkill_RollOut::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	m_fSkillDamage = 70.f;

	return S_OK;
}

HRESULT CShellSkill_RollOut::Initialize(void* pArg)
{
	CShellSkill::DESC* pDesc = static_cast<CShellSkill::DESC*>(pArg);
	if (pDesc == nullptr)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(Get_ShellOwner());

	m_pTransform = static_cast<CTransform*>(m_pOwner->Find_Component(g_strTransformTag));

	return S_OK;
}

void CShellSkill_RollOut::Priority_Update(_float fTimeDelta)
{
	if (m_bRoll)
	{
		m_fSkillTimeAcc += fTimeDelta;
		m_fEffectTimeAcc += fTimeDelta;
		if (m_fSkillTimeAcc >= m_fSkillDuration)
		{
			Cancel_Skill();

			m_pPlayer->Get_StateMachine()->ChangeState(L"ShellParry");
			m_pPlayer->Reset_Rolling();

			CGameInstance::GetInstance()->Stop_Sound((_uint)SOUND_CHANNEL::PLAYER_LOOP);
			m_pPlayer->Play_PlayerSound(L"Parry");
		}
		else if (m_fEffectTimeAcc >= m_fEffectDuration)
			Call_NonLoopEffects();
	}

	super::Priority_Update(fTimeDelta);
}

void CShellSkill_RollOut::Update(_float fTimeDelta)
{
	if (m_bRoll)
	{
		m_pTransform->Turn(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_fRotationSpeed * fTimeDelta * -1.f);
		Control_RollingEffects();
	}

	super::Update(fTimeDelta);
}

void CShellSkill_RollOut::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

void CShellSkill_RollOut::Final_Update(_float fTimeDelta)
{
	super::Final_Update(fTimeDelta);
}

void CShellSkill_RollOut::Use_Skill()
{
	super::Use_Skill();

	m_bRoll = true;
	m_fSkillTimeAcc = 0.f;
	m_fEffectTimeAcc = 0.f;

	m_pPlayer->Hide_PlayerOnly(true);
	m_pOwner->Swap_Model(true);

	Call_RollingEffects();
}

void CShellSkill_RollOut::Cancel_Skill()
{
	m_bRoll = false;
	m_fSkillTimeAcc = 0.f;
	m_fEffectTimeAcc = 0.f;

	m_pPlayer->Hide_PlayerOnly(false);
	m_pOwner->Swap_Model(false);

	Disable_RollingEffects();
}

void CShellSkill_RollOut::Call_RollingEffects()
{
	if (false == m_EffectObjects.empty() || nullptr != m_pSparkEffect)
		Disable_RollingEffects();

	CEffectObject* pEffect = nullptr;
	_vector vPos{}, vUp{}, vLook{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) }, vCamLook{};
	CTransform* pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	const _float4x4* pWorldMatrix = &pPlayerTransform->Get_WorldMatrix_Ref();
	vUp = pPlayerTransform->Get_State(CTransform::STATE::UP) * 0.3f;
	vPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION) + vUp;
	vLook = pPlayerTransform->Get_State(CTransform::STATE::LOOK);
	vCamLook = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::VIEW).r[(_uint)CTransform::STATE::LOOK];
	pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_ROLL_MAINROLL, vUp, vLook, true, pWorldMatrix);
	m_EffectObjects.push_back(pEffect);
	pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_ROLL_SUBROLL, vUp, vLook, true, pWorldMatrix);
	m_EffectObjects.push_back(pEffect);

	// 스파크
	m_pSparkEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_ROLL_SPARK, vZeroPos, vZeroLook, true, pWorldMatrix);
	if (nullptr != m_pSparkEffect)
		static_cast<CVIBuffer_InstancingParticle*>(m_pSparkEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(vLook * 0.5f - vUp * 0.2f);

	m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_ROLL_WINDROLL, vUp, vLook, true, pWorldMatrix);
	m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook, -vCamLook, false, nullptr);
}

void CShellSkill_RollOut::Call_NonLoopEffects()
{
	m_fEffectTimeAcc = 0.f;

	CEffectObject* pEffect = nullptr;
	_vector vPos{}, vUp{}, vLook{}, vZeroPos{}, vZeroLook{XMVectorSet(0.f, 0.f, 1.f, 0.f)}, vCamLook{};
	CTransform* pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	const _float4x4* pWorldMatrix = &pPlayerTransform->Get_WorldMatrix_Ref();
	vUp = pPlayerTransform->Get_State(CTransform::STATE::UP) * 0.3f;
	vPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION) + vUp;
	vLook = pPlayerTransform->Get_State(CTransform::STATE::LOOK);
	vCamLook = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::VIEW).r[(_uint)CTransform::STATE::LOOK];

	// 모래
	pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_ROLL_SANDBURST, vPos , vZeroLook, false, nullptr);
	if (nullptr != pEffect)
		static_cast<CVIBuffer_InstancingParticle*>(pEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(vLook * 0.2f - vUp * 0.01f);

	

	// 윈드 롤링 이펙트
	m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_ROLL_WINDROLL, vUp, vLook, true, pWorldMatrix);
}

void CShellSkill_RollOut::Control_RollingEffects()
{
	if (true == m_EffectObjects.empty() && nullptr == m_pSparkEffect)
		return;
		
	if (false == m_EffectObjects.empty())
	{
		for (auto& Effect : m_EffectObjects)
		{
			CTransform* pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
			const _float4x4* pWorldMatrix = &pPlayerTransform->Get_WorldMatrix_Ref();
			_vector vLook = pPlayerTransform->Get_State(CTransform::STATE::LOOK);
			static_cast<CTransform*>(Effect->Find_Component(g_strTransformTag))->LookAt(vLook);
		}
	}
	if (nullptr != m_pSparkEffect)
	{
		_vector vPos{}, vUp{}, vLook{};
		CTransform* pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
		vUp = pPlayerTransform->Get_State(CTransform::STATE::UP);
		vPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);
		vLook = pPlayerTransform->Get_State(CTransform::STATE::LOOK);
		static_cast<CVIBuffer_InstancingParticle*>(m_pSparkEffect->Find_Component(L"Com_VIBuffer"))->Set_Pivot(vLook * 0.5f - vUp * 0.2f);
	}
}

void CShellSkill_RollOut::Disable_RollingEffects()
{
	if (true == m_EffectObjects.empty() && nullptr == m_pSparkEffect)
		return;

	if (false == m_EffectObjects.empty())
	{
		for (auto& Effect : m_EffectObjects)
			Effect->Clear_Info(0.f);
		m_EffectObjects.clear();
	}

	if (nullptr != m_pSparkEffect)
	{
		m_pSparkEffect->Clear_Info(0.f);
		m_pSparkEffect = nullptr;
	}
}

CShellSkill_RollOut* CShellSkill_RollOut::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShellSkill_RollOut* pInstance = new CShellSkill_RollOut(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CShellSkill_RollOut");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CShellSkill_RollOut::Clone(void* pArg)
{
	CShellSkill_RollOut* pInstance = new CShellSkill_RollOut(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CShellSkill_RollOut");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShellSkill_RollOut::Free()
{
	super::Free();
}
#include "pch.h"
#include "Projectile_Urchin.h"

#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"
#include "Collider.h"
#include "Movement.h"

#include "Player.h"
#include "SpringArm_Player.h"
#include "Camera_Player.h"
#include "CineCamera_Player.h"

#include "EffectObject.h"
#include "VIBuffer_InstancingParticle.h"

CProjectile_Urchin::CProjectile_Urchin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CProjectile_Urchin::CProjectile_Urchin(const CProjectile_Urchin& Prototype)
	: super(Prototype)
{
}

HRESULT CProjectile_Urchin::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile_Urchin::Initialize(void* pArg)
{
	CProjectile::DESC tDesc{};

	if (FAILED(super::Initialize(&tDesc)) ||
		FAILED(Ready_Components()) ||
		FAILED(Ready_PartObject()))
		return E_FAIL;

	Deactivate();

	return S_OK;
}

void CProjectile_Urchin::Priority_Update(_float fTimeDelta)
{
	if (!m_bRelease)
	{
		if (m_pGameInstance->Key_Press(KEY::Q))
		{
			if (m_iLevel < 5 && m_pPlayer->Get_PlayerStat()->fCurrSkillPoint >= 1.f)
			{
				m_fChargeAcc += fTimeDelta;
				if (m_fChargeAcc >= 0.5f)
				{
					m_iLevel++;
					m_pPlayer->Get_PlayerStat()->fCurrSkillPoint -= 1.f;

					m_fPrevScale = static_cast<_float>(m_iLevel - 1);
					m_fScale = static_cast<_float>(m_iLevel);
					m_fChargeAcc = 0.f;

					m_bLerp = true;

					m_pPlayer->Player_CameraShake_Position(0.01f + 0.01f * (m_iLevel - 1), 25.f - 1.5f * (m_iLevel - 1), 0.f, true);

					if (m_iLevel == 3)
					{
						m_bTimeLag = false;
						m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength() * 2.f);

						m_pPlayer->Play_PlayerSound(L"Urchin_Charge_Voice");
						m_pPlayer->Play_PlayerSound_Loop(L"Urchin_Base");
					}
					else if (m_iLevel == 4)
					{
						m_bTimeLag = false;
						m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength() * 4.5f);
						m_pPlayer->Play_PlayerSound(L"Urchin_Whirl");
					}

					Call_ThrowEffects();
					Call_RemainEffects();

					m_pPlayer->Play_PlayerSound(L"Urchin_Charge");
				}


			}

			m_pPlayer->Get_PlayerStat()->bUrchinThrow = false;
			m_bRelease = false;
		}
		else
		{
			m_pPlayer->Get_PlayerStat()->bUrchinThrow = true;
			m_bRelease = true;
			Disable_RemainEffects();
		}
	}
	else
	{
		if (!m_bThrown && m_pPlayer->Get_PlayerStat()->bUrchinLaunch)
		{
			_vec3 vDir = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::LOOK);
			vDir.y += 0.5f;

			m_pMovementCom->Activate(true);
			m_pMovementCom->Set_GravityScale(7.f);
			m_pMovementCom->Add_Force(vDir._vector(), m_tProjectileStat.fForce);

			m_pPlayer->Get_PlayerStat()->bUrchinLaunch = false;
			m_bThrown = true;

			m_pPlayer->Get_PlayerSpringArm()->Get_PlayerCamera()->Stop_Shake();

			m_pPlayer->Play_PlayerSound(L"Urchin_Throw_Voice");

			if(m_iLevel >= 3)
				m_pPlayer->Get_PlayerSpringArm()->Zoom(m_pPlayer->Get_PlayerSpringArm()->Get_DefaultArmLength());
			if (m_iLevel >= 4)
			{
				m_pPlayer->Get_PlayerCineCamera()->Play_Cinematic(L"Urchin");
				CGameInstance::GetInstance()->Call_Lag(0.05f, 5.f);

				m_pPlayer->Play_PlayerSound(L"Urchin_Throw");
				CGameInstance::GetInstance()->Stop_Sound((_uint)SOUND_CHANNEL::PLAYER_LOOP);
			}
		}
	}

	super::Priority_Update(fTimeDelta);
}

void CProjectile_Urchin::Update(_float fTimeDelta)
{
	if (m_bThrown)
	{
		_vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);
		_vec3 vDir = _vec3(0.f, -1.f, 0.f);
		_float fDist{};

		if (m_pGameInstance->Ray_Cast_To_Physics(vPos, vDir, m_fScale * 0.7f, fDist, 1))
		{
			m_pMovementCom->Activate(false);

			m_bExplosion = true;
			m_bRender = false;

			m_pPlayer->Player_CameraShake_Position(0.05f + 0.015f * (m_iLevel - 1), 25.f - 1.5f * (m_iLevel - 1), 0.5f + 1.f * (m_iLevel - 1));
		}
	}
	else
	{
		_vec3 vPos = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);
		vPos.y += 1.f + m_fScale * 0.5f;

		m_pTransformCom->Locate(vPos._float3());
	}

	m_pMovementCom->Update(fTimeDelta);

	if (!m_bRelease)
	{
		if(m_bLerp)
			Lerp(fTimeDelta);
		else Vibrate(fTimeDelta);

		Call_NonLoopChargingEffects(fTimeDelta);
	}

	if (m_bExplosion)
		Explosion(fTimeDelta);

	if (!m_bExplosion)
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	else m_pColliderCom->Update(XMLoadFloat4x4(&m_matExplosion));
}

void CProjectile_Urchin::Late_Update(_float fTimeDelta)
{
	if(m_bRender)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

	super::Late_Update(fTimeDelta);
}

void CProjectile_Urchin::Final_Update(_float fTimeDelta)
{
	super::Final_Update(fTimeDelta);
}

HRESULT CProjectile_Urchin::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = static_cast<_uint>(m_pModelCom->Get_NumMeshes());

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iShaderPass = 0;
		if (m_pModelCom->Has_Texture_In_MeshMaterial(i, MATERIAL_TEXTURE::DIFFUSE))
		{
			iShaderPass = (_uint)SHADER_PASS_MESH::DEFAULT;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else
		{
			iShaderPass = (_uint)SHADER_PASS_MESH::BASE_COLOR;

			if (FAILED(m_pModelCom->Bind_BaseColor(m_pShaderCom, i, "g_vMtrlBaseColor")))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::NORMAL, "g_NormalTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::METALNESS, "g_MetalicTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::SHININESS, "g_SmoothnessTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, MATERIAL_TEXTURE::EMISSIVE, "g_EmissiveMaskTexture")))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_EmissionColor(m_pShaderCom, i, "g_vEmissionColor")))
			return E_FAIL;

		m_pShaderCom->Begin(iShaderPass);
		m_pModelCom->Render(i);
	}

	if (FAILED(super::Render()))
		return E_FAIL;

	return S_OK;
}

void CProjectile_Urchin::Activate(const FProjectileStat& tProjectileStat)
{
	super::Activate(tProjectileStat);

	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player"));
	
	CTransform* pTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag));
	_vec3 vOriginPos = pTransform->Get_State(CTransform::STATE::POSITION);
	m_pTransformCom->Locate(vOriginPos._float3());


	Call_ThrowEffects();
	Call_RemainEffects();
}

void CProjectile_Urchin::Deactivate()
{
	super::Deactivate();

	m_pPlayer = nullptr;
	m_bRelease = false;
	m_bThrown = false;
	m_bExplosion = false;

	m_bLerp = false;
	m_bRender = true;

	m_iLevel = 1;
	m_fChargeAcc = 0.f;
	m_fExplosionAcc = 0.f;
	m_fExplosionDuration = 1.f;

	XMStoreFloat4x4(&m_matExplosion, XMMatrixIdentity());

	m_fPrevScale = 1.f;
	m_fScale = 1.f;
	m_fLerp = 0.f;
	m_fVibrate = 0.f;

	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
	m_pMovementCom->Set_GravityScale(0.f);
	m_pMovementCom->Reset_Velocity();
	m_pMovementCom->Activate(false);

	Disable_ThrowEffects();
}

void CProjectile_Urchin::Call_ThrowEffects()
{
	_vector vPos{ m_pTransformCom->Get_State(CTransform::STATE::POSITION) }, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	CEffectObject* pEffect = nullptr;
	switch (m_iLevel)
	{
	case 1:
		if (false == m_ThrowEffects.empty())
			Disable_ThrowEffects();
		vZeroPos = XMVectorSet(0.f, -0.25f, 0.f, 1.f);
		pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_CHARGING_URCHIN, vZeroPos, vZeroLook, true, &m_CombinedWorldMatrix, 0.f, false, &m_pTransformCom->Get_WorldMatrix_Ref());
		if(pEffect != nullptr)
			m_ThrowEffects.push_back(pEffect);
		break;
	case 2:
		break;
	case 3:
		pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_CHARGING_WIND, vZeroPos, vZeroLook, true, &m_CombinedWorldMatrix, 0.f, false, &m_pTransformCom->Get_WorldMatrix_Ref());
		if (pEffect != nullptr)
			m_ThrowEffects.push_back(pEffect);
		break;
	case 4:
		break;
	case 5:
		break;
	default:
		break;
	}
}

void CProjectile_Urchin::Call_RemainEffects()
{
	_float fDistance{};
	_vector vPos{ m_pTransformCom->Get_State(CTransform::STATE::POSITION) }, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	CEffectObject* pEffect = nullptr;
	switch (m_iLevel)
	{
	case 1:
		if (false == m_RemainEffects.empty())
			Disable_RemainEffects();
		break;
	case 2:
		pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_PTCL_CHARGING_LIGHT_LV0, vZeroPos, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		if (pEffect != nullptr)
			m_ChargingPtclEffects.push_back(pEffect);
		break;
	case 3:
		break;
	case 4:
		if (false == m_ChargingPtclEffects.empty() && m_ChargingPtclEffects.front() != nullptr)
			static_cast<CVIBuffer_InstancingParticle*>(m_ChargingPtclEffects.front()->Find_Component(L"Com_VIBuffer"))->Loop_End();
		pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_PTCL_CHARGING_LIGHT_LV1, vZeroPos, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		if (pEffect != nullptr)
			m_ChargingPtclEffects.push_back(pEffect);
		m_pGameInstance->Ray_Cast_To_Physics(vPos, XMVectorSet(0.f, -1.f, 0.f, 0.f), 25.f, fDistance, PxQueryFlag::eSTATIC);
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) - fDistance);
		pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_CHARGING_TORNADO_ED, vPos, vZeroLook, false, nullptr);
		if (pEffect != nullptr)
			m_RemainEffects.push_back(pEffect);
		break;
	case 5:
		pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_CHARGING_RING, vZeroPos, vZeroLook, true, &m_CombinedWorldMatrix, 0.f, false, &m_pTransformCom->Get_WorldMatrix_Ref());
		if (pEffect != nullptr)
			m_RemainEffects.push_back(pEffect);
		m_pGameInstance->Ray_Cast_To_Physics(vPos, XMVectorSet(0.f, -1.f, 0.f, 0.f), 25.f, fDistance, PxQueryFlag::eSTATIC);
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) - fDistance);
		pEffect = m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_CHARGING_TORNADO_RED, vPos, vZeroLook, false, nullptr);
		if (pEffect != nullptr)
			m_RemainEffects.push_back(pEffect);
		break;
	default:
		break;
	}
}

void CProjectile_Urchin::Call_NonLoopChargingEffects(_float fTimeDelta)
{
	if (m_iLevel < 2)
		return;

	m_fChargingEffectAcc += fTimeDelta;
	if (m_fChargingEffectDuration <= m_fChargingEffectAcc)
	{
		m_fChargingEffectAcc = 0.f;
		_float fDistance{};
		_vector vPos{ m_pTransformCom->Get_State(CTransform::STATE::POSITION) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
		if(m_iLevel > 4)
			m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_CHARGING_DISTORTION, vPos, vZeroLook, false, nullptr);

		m_pGameInstance->Ray_Cast_To_Physics(vPos, XMVectorSet(0.f, -1.f, 0.f, 0.f), 25.f, fDistance, PxQueryFlag::eSTATIC);
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) - fDistance);
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_PTCL_CHARGING_SMOKE_LV1, vPos, vZeroLook, false, nullptr);
	}
}

void CProjectile_Urchin::Call_NonLoopThrowEffects(_float fTimeDelta)
{
	// _vector vPos{ m_pTransformCom->Get_State(CTransform::STATE::POSITION) }, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	
}

void CProjectile_Urchin::Disable_ThrowEffects()
{
	if (true == m_ThrowEffects.empty())
		return;

	for (auto& Effect : m_ThrowEffects)
	{
		if (Effect != nullptr)
		{
			Effect->Clear_Info(0.f);
			Effect = nullptr;
		}
	}

	m_ThrowEffects.clear();
}

void CProjectile_Urchin::Disable_RemainEffects()
{
	if (false == m_ChargingPtclEffects.empty())
	{
		for (auto& Effect : m_ChargingPtclEffects)
		{
			if (Effect != nullptr)
			{
				Effect->Clear_Info(0.f);
				Effect = nullptr;
			}
		}

		m_ChargingPtclEffects.clear();
	}

	if (false == m_RemainEffects.empty())
	{
		for (auto& Effect : m_RemainEffects)
		{
			if (Effect != nullptr)
			{
				Effect->Clear_Info(0.f);
				Effect = nullptr;
			}
		}

		m_RemainEffects.clear();
	}	
}

void CProjectile_Urchin::Call_ExplosionEffects()
{
	_float fDistance{};
	_vector vPos{ m_pTransformCom->Get_State(CTransform::STATE::POSITION) }, vUp{ m_pTransformCom->Get_State(CTransform::STATE::UP) },
		vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };

	m_pGameInstance->Ray_Cast_To_Physics(vPos, XMVectorSet(0.f, -1.f, 0.f, 0.f), 25.f, fDistance, PxQueryFlag::eSTATIC);
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) - fDistance);

	switch (m_iLevel)
	{
	case 1:
		m_pPlayer->Play_PlayerSound(L"Urchin_Charge_Explosion_Small");
	case 2:
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_PTCL_EXPLODE_SMOKE_LV0, vZeroPos, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_EXPLODE_BACKBLAST_LV0, vZeroPos, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		m_pPlayer->Play_PlayerSound(L"Urchin_Charge_Explosion_Medium");
		break;
	case 3:
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_CHARGING_DISTORTION, vZeroPos, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_PTCL_EXPLODE_SMOKE_LV1, vZeroPos, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_EXPLODE_BACKBLAST_LV1, vZeroPos - vUp * 6.f, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		m_pPlayer->Play_PlayerSound(L"Urchin_Charge_Explosion_Medium");
		break;
	case 4:
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_CHARGING_DISTORTION, vZeroPos, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_PTCL_EXPLODE_SMOKE_LV2, vZeroPos, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_EXPLODE_GROUNDBURST, vZeroPos + vUp * 3.f, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_EXPLODE_BACKBLAST_LV2, vZeroPos - vUp * 6.f, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		m_pPlayer->Play_PlayerSound(L"Urchin_Charge_Explosion");
		break;
	case 5:
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_EXPLODE_SPHERE, vZeroPos, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_CHARGING_DISTORTION, vZeroPos, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_PTCL_EXPLODE_SMOKE_LV2, vZeroPos, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_EXPLODE_GROUNDBURST, vZeroPos + vUp * 3.f, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		m_pGameInstance->Call_Effect((_uint)EFFECT_TYPE::URCHINBOMB_MESH_EXPLODE_BACKBLAST_LV2, vZeroPos - vUp * 6.f, vZeroLook, true, &m_pTransformCom->Get_WorldMatrix_Ref());
		m_pPlayer->Play_PlayerSound(L"Urchin_Charge_Explosion");
		break;
	default:
		break;
	}
}

void CProjectile_Urchin::Disable_ExplosionEffects()
{

}

void CProjectile_Urchin::Lerp(_float fTimeDelta)
{
	_vec3 vPos = static_cast<CTransform*>(m_pPlayer->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);

	m_fLerp += fTimeDelta;
	if (m_fLerp < 0.2f)
	{
		_float fScale = CUtils::Lerp_Float(m_fPrevScale, m_fScale, m_fLerp / 0.2f);
		_float fDist = CUtils::Lerp_Float(vPos.y + 1.f + m_fPrevScale * 0.5f, vPos.y + 1.f + m_fScale * 0.5f, m_fLerp / 0.2f);
		
		vPos.y = fDist;
		m_pTransformCom->Set_Scale(fScale, fScale, fScale);
		m_pTransformCom->Locate(vPos._float3());
	}
	else
	{
		m_fLerp = 0.f;
		m_bLerp = false;

		vPos.y += 1.f + m_fScale * 0.5f;
		m_pTransformCom->Set_Scale(m_fScale, m_fScale, m_fScale);
		m_pTransformCom->Locate(vPos._float3());

		m_fPrevScale = m_fScale;
	}
}

void CProjectile_Urchin::Explosion(_float fTimeDelta)
{
	if (m_fExplosionAcc == 0.f)
		Call_ExplosionEffects();

	m_fExplosionAcc += fTimeDelta;
	if (m_fExplosionAcc < m_fExplosionDuration * m_iLevel * 0.5f)
	{
		_float fLerp = m_fExplosionAcc / (m_fExplosionDuration * m_iLevel * 0.5f);
		_float fScale = CUtils::Lerp_Float(m_fScale, m_fScale * 3.f, fLerp);

		_mat matCollider = m_pTransformCom->Get_WorldMatrix();
		_mat matScale = XMMatrixIdentity() * XMMatrixScaling(fScale * 0.7f, fScale * 0.7f, fScale * 0.7f);
		_mat matCombined = matScale * matCollider;

		XMStoreFloat4x4(&m_matExplosion, matCombined._matrix());
	}
	else
	{
		m_fExplosionAcc = 0.f;
		Deactivate();
	}
}

void CProjectile_Urchin::Vibrate(_float fTimeDelta)
{
	m_fVibrate += fTimeDelta;
	if (m_fVibrate >= 0.1f)
		m_fVibrate = 0.f;

	_float fScale = m_fScale + m_fScale * 0.01f * sinf(XMConvertToRadians(360.f) * (m_fVibrate / 0.15f));
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);
}

void CProjectile_Urchin::On_CollisionEnter(CGameObject* pOther, _uint iGroup)
{
	switch (iGroup)
	{
	case (_uint)COLLISION_GROUP::MONSTER:
	{
		IDamaged* pDamaged = dynamic_cast<IDamaged*>(pOther);
		if (pDamaged == nullptr)
			return;

		pDamaged->Damaged(m_tProjectileStat.fDamage * m_iLevel, IDamaged::DAMAGE::DAMAGE_NORMAL, this);

		m_pMovementCom->Activate(false);

		m_bExplosion = true;
		m_bRender = false;

		m_pPlayer->Player_CameraShake_Position(0.05f + 0.015f * (m_iLevel - 1), 30.f - 1.f * (m_iLevel - 1), 0.5f + 0.15f * (m_iLevel - 1));
	}
	break;
	}
}

void CProjectile_Urchin::On_Collision(CGameObject* pOther, _uint iGroup)
{
}

void CProjectile_Urchin::On_CollisionExit(CGameObject* pOther, _uint iGroup)
{
}

HRESULT CProjectile_Urchin::Ready_Components()
{
	// Com_Model
	if (FAILED(Add_Component
	(
		LEVEL_STATIC,
		L"Prototype_Component_Model_Urchin_Skill",
		L"Com_Model",
		reinterpret_cast<CComponent**>(&m_pModelCom)
	)))
		return E_FAIL;

	// Com_Shader
	if (FAILED(super::Add_Component
	(
		LEVEL_STATIC,
		TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"),
		reinterpret_cast<CComponent**>(&m_pShaderCom)
	)))
		return E_FAIL;

	// Com_Collider
	{
		CCollider_Sphere::DESC Desc{};
		Desc.ColliderDesc.pOwner = this;
		Desc.ColliderDesc.iCollisionGroup = (_uint)COLLISION_GROUP::PLAYER_PROJECTILE;

		Desc.fRadius = 0.7f;
		Desc.vCenter = { 0.f, 0.f, 0.f };

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider_Sphere",
			L"Com_Collider", reinterpret_cast<CComponent**>(&m_pColliderCom), &Desc)))
			return E_FAIL;
	}

	// Com_Movement
	{
		CMovement::DESC Desc{};
		Desc.fMoveAccel = 40.f;
		Desc.fMoveDecel = 20.f;
		Desc.fMinSpeed = 1.f;
		Desc.fMaxSpeed = 10.f;
		Desc.fForceFriction = 5.f;
		Desc.fRotationScale = 1.f;
		Desc.fGravityScale = 0.f;
		Desc.fMaxFallingSpeed = 15.f;

		if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Movement",
			L"Com_Movement", reinterpret_cast<CComponent**>(&m_pMovementCom), &Desc)))
			return E_FAIL;

		m_pMovementCom->Set_UpdateComponent(m_pTransformCom);
		m_pMovementCom->Set_RotateToDirection_3D(true);
	}

	return S_OK;
}

HRESULT CProjectile_Urchin::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrix_Ref())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile_Urchin::Ready_PartObject()
{
	return S_OK;
}

CProjectile_Urchin* CProjectile_Urchin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProjectile_Urchin* pInstance = new CProjectile_Urchin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CProjectile_Urchin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CProjectile_Urchin::Clone(void* pArg)
{
	CProjectile_Urchin* pInstance = new CProjectile_Urchin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CProjectile_Urchin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile_Urchin::Free()
{
	super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
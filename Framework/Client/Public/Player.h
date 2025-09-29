#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "EPlayerState.h"
#include "FPlayerStat.h"
#include "FPlayerSound.h"
#include "ESkill.h"
#include "FShell.h"
#include "FWeapon.h"

BEGIN(Engine)

class CModel;
class CShader;
class CCollider;
class CMovement;
class CStateMachine;
class CPhysics_Controller;
class CInputBuffer;
class CInputGate;
class CEffectObject;

END

BEGIN(Client)

class CWeapon_Player;
class CHand_Player;
class CSpringArm_Player;
class CCamera_Player;
class CCineCamera_Player;
class CShell_Part;
class CItem;
class CActiveSkill;

class CPlayer final
	: public CGameObject
	, public IDamaged
{
	using super = CGameObject;

public:
	struct DESC
	{
		super::GAMEOBJECT_DESC GameObjectDesc{};
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	CWeapon_Player* Get_PlayerWeapon() { return m_pPlayerWeapon; }
	CHand_Player* Get_PlayerHand() { return m_pPlayerHand; }
	CSpringArm_Player* Get_PlayerSpringArm() { return m_pPlayerSpringArm; }
	CCamera_Player* Get_PlayerCamera();
	CCineCamera_Player* Get_PlayerCineCamera() { return m_pPlayerCineCamera; }
	CShell_Part* Get_PlayerShell() { return m_pPlayerShell; }

	CStateMachine* Get_StateMachine() { return m_pStateMachine; }
	CStateMachine* Get_UpperStateMachine() { return m_pUpperStateMachine; }
	FPlayerStat* Get_PlayerStat() { return &m_tPlayerStat; }
	const FPlayerStat* Get_PlayerStat_Const() const { return &m_tPlayerStat; }
	const FShellStat* Get_PlayerShellStat_Const() const;
	const FWeaponStat* Get_PlayerWeaponStat_Const() const;
	const FShellStat* Get_PlayerWeaponShellStat_Const() const;

	const FPlayerSound* Get_PlayerSound() const { return &m_tPlayerSound; }

	PlayerStateFlags Get_PlayerState() const { return m_PlayerState; }

	CTransform* Get_LockOnTransform() const;
	CGameObject* Get_AutoLockTarget() { return m_pAutoLockTarget; }

	const _float4x4* Get_SocketMatrix() { return m_pGrabSocketMatrix; }
	const _float4x4* Get_ParentMatrix() { return m_pParentWorldMatrix; }

	CGameObject* Get_GrappleTarget() { return m_pGrappleTarget; }

	CGameObject* Get_FinishTarget() { return m_pFinishTarget; }

	_bool Is_Render() const { return m_bRender; }

public :
	void Sibal();

public :
	void Move_Player(_fvector vPos);

public:
	void Set_AutoLock(_bool bAutoLock, _float fAutoDist = 10.f);
	void UnLock_LockOn();

public:
	virtual void Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager) override;
	void Grabbed(const _float4x4* pGrabSocketMatrix, const _float4x4* pParentMatrix);
	void UnGrab(_float fPower, DAMAGE eDamage, CGameObject* pDamager, _fvector vDir = XMVectorZero());

public:
	void Player_WeaponCollider(_bool bCollider);
	void Player_HandCollider(_bool bCollider);
	void Player_CheckLockOn();

public:
	HRESULT Equip_Shell(SHELL eShell, _float fHP);
	HRESULT Equip_Shell_NoSound(SHELL eShell, _float fHP);
	HRESULT UnEquip_Shell();
	HRESULT UnEquip_Shell_NoSound();

	HRESULT Equip_Weapon(_uint iLevel, _float fDamage, SHELL eShell, _float fHP);

public:
	void Grapple_Aim();
	void Grapple_Hit();
	void Grapple_Launch();
	void UnGrapple();

public:
	HRESULT Use_HeartKelp();
	void UnUse_Item();

	_bool Upgrade_Weapon();
	void MoonShell_Interact(_bool bEnter);

public :
	void Change_PrimaryActiveSkill(ACTIVESKILL eActiveSkill);
	void Change_SecondaryActiveSkill(ACTIVESKILL eActiveSkill);

	void Swap_ActiveSkill()
	{
		ACTIVESKILL ePrimary = m_tPlayerStat.ePrimaryActiveSkill;
		m_tPlayerStat.ePrimaryActiveSkill = m_tPlayerStat.eSecondaryActiveSkill;
		m_tPlayerStat.eSecondaryActiveSkill = ePrimary;
	}

	void UnLock_ActionSkill(ACTIONSKILL eActionSkill);
	void UnLock_ActiveSkill(ACTIVESKILL eActiveSkill);

	void Use_ActiveSkill();

public :
	void Change_Hammer();

public :
	void Reset_Rolling();
	void Reset_Finish();

public:
	void Heal(_float fHeal);
	void Heal_Umami(_float fDamage);
	void Calculate_Stat();
	void Rest();

public :
	void Feared(_float fGauge);

public :
	void Instant_Damage(_float fDamage);
	void Shell_Damage(_float fDamage);
	void KnockBack(_float fPower, DAMAGE eDamage, CGameObject* pDamager, _fvector vDir = XMVectorZero());

public:
	void Reset_PlayerStat();
	void Clear_Buffer();

public :
	void Player_CameraShake_Position(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop = false);
	void Player_CameraShake_Rotation(_float fAmplitude, _float fFrequency, _float fDuration, _bool bLoop = false);

public :
	void Apply_RadialBlur(_float fIntensity);

public :
	void Player_TimeSlow(_bool bSlow, _float fTimeRatio = 0.1f);

public :
	void Reset_XZRotation();

public :
	void Toggle_Render(_bool bRender) { m_bRender = bRender; }
	void Hide_PlayerOnly(_bool bHide) { m_bHide = bHide; }

public :
	void Play_PlayerSound(const _wstring& strSound);
	void Play_PlayerSound_Loop(const _wstring& strSound);

private:
	_float m_fBubbleGenTime = {};
	_float m_fBubbleGenTimeAcc = {};

private:
	CModel* m_pModelCom{};
	CShader* m_pShaderCom{};
	CMovement* m_pMovementCom{};
	CCollider* m_pColliderCom{};
	CPhysics_Controller* m_pPhysicsControllerCom{};

private:
	CWeapon_Player* m_pPlayerWeapon{};
	CHand_Player* m_pPlayerHand{};
	CSpringArm_Player* m_pPlayerSpringArm{};
	CCineCamera_Player* m_pPlayerCineCamera{};
	CShell_Part* m_pPlayerShell{};
	CItem* m_pItem{};

private:
	CStateMachine* m_pStateMachine{};
	CStateMachine* m_pUpperStateMachine{};
	FPlayerStat m_tPlayerStat{};
	FShellStat* m_pPlayerShellStat{};

	FPlayerSound m_tPlayerSound{};

private:
	CInputGate* m_pInputGate{};
	CInputBuffer* m_pInputBuffer{};

private:
	PlayerStateFlags m_PlayerState{};

private:
	_bool m_bAutoLock{};
	CGameObject* m_pAutoLockTarget{};
	CTransform* m_pAutoLockTransform{};

private:
	const _float4x4* m_pGrabSocketMatrix{};

private:
	CGameObject* m_pGrappleTarget{};
	map<_wstring, _uint> m_mapGrappleLayer;

private:
	CActiveSkill* m_ActiveSkills[static_cast<_uint>(ACTIVESKILL::ACTIVESKILL_END)]{};

private:
	_float m_fRespawnAcc{};

private:
	CGameObject* m_pBackStepTarget{};

private :
	CTransform* m_pMagnetTransform{};
	_float m_fMagnetLerp{ 0.1f };

private :
	CGameObject* m_pFinishTarget{};
	_float3 m_vFinishStart{};
	_float m_fFinishLerp{};

private :
	_float m_fDebuffLatency{};

private :
	_bool m_bRender{ true };
	_bool m_bHide{};

private :
	CGameObject* m_pRollingTarget{};

private:
	CEffectObject* m_pTearEffect{ nullptr };

private :
	map<_wstring, FPlayerSound> m_mapSound;

private :
	void Input_Control(_float fTimeDelta);	
	void Cheat_Control(_float fTimeDelta);
	
private:
	void Call_ParryEffect();
	void Call_BackStepEffect();
	void Call_BubbleEnvEffect(_float fTimeDelta);
	void Call_GrappleEffect(_fvector vPos);
	void Call_DamagedEffect(IDamaged::DAMAGE iDamageType);
	void Call_FearEffect();
	void Disable_FearEffect();
	void Call_GrappleHitEffect();

private :
	void AutoLock_Movement(_float fTimeDelta);
	void Lerp_MaxSpeed(_float fTimeDelta);
	void GrappleAim_Movement();
	void Finish_Movement(_float fTimeDelta);

private :
	void Delete_Shell();

private :
	void Grapple_Throw();
	void Grapple_Boost();
	void Check_GrappleTarget();

private :
	void Hammer_SlotAnim();

private :
	void Magnet_Pulled();
	void Magnet_UnPulled();

private :
	void Check_FinishTarget();

private :
	void Grappling_Collide();
	void Rolling_Collide();

private :
	void Attacked(_bool bShellGuard, _float fDamage, DAMAGE eDamage, CGameObject* pDamager);

	void Check_Debuff(_float fTimeDelta);

private :
	void Respawn(_float fTimeDelta);

private :
	void Decrease_RadialBlur(_float fTimeDelta);

private:
	void Calculate_CombinedMatrix();

// UI ฐüทร
private:
	void UI_Setting();

private :
	HRESULT Add_Sound(const _wstring strSoundName, const _wstring strSoundFile, SOUND_CHANNEL eChannel, _float fVolume);

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

private :
	HRESULT Ready_Component();
	HRESULT Ready_PartObject();
	HRESULT Ready_StateMachine();
	HRESULT Ready_Event();
	HRESULT Ready_Sound();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
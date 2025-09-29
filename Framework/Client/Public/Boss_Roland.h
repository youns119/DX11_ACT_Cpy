#pragma once

#include "Client_Defines.h"
#include "Enemy.h"

#include "EnemyStateEnum.h"

#include "IDamaged.h"
#include "IParried.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CMovement;
class CFSM;
class CPhysics_Controller;
class CTexture;
class CEffectObject;
END

BEGIN(Client)
class CBoss_Roland final : public CEnemy, public IDamaged, public IParried
{
	using super = CEnemy;

public:
	struct DESC
	{
		CEnemy::DESC EnemyDesc{};
		// Lock On Bone: key_X.111
	};

private:
	CBoss_Roland(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Roland(const CBoss_Roland& Prototype);
	virtual ~CBoss_Roland() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public:		
	_bool Is_Bounced() { return m_bIsBounced; }
	_bool Is_Unblockable() { return m_bIsUnblockable; }
	_vector Get_MinAngleTarget();

public:
	void Set_Roll(_bool _bIsRoll) { m_bIsRoll = _bIsRoll; }
	void Set_IsUnblockable(_bool _bIsUnblockable) { m_bIsUnblockable = _bIsUnblockable; }
	void Set_Bounced(_bool _bIsBounced) { m_bIsBounced = _bIsBounced; }
	void Set_ChaseSpeed(_float _fSpeed) { m_fChaseSpeed = _fSpeed; }
	void Set_IsChase(_bool _bIsChase, _float _fSpeed = 1.f)
	{
		m_bIsChase = _bIsChase; m_fChaseSpeed = _fSpeed;
	}
	void Set_IsSideMove(_bool _bIsSideMove, _float _fSpeed = 1.f)
	{
		m_bIsSideMove = _bIsSideMove; m_fSideMoveSpeed = _fSpeed;
	}	

public:
	HRESULT Ready_Components();
	HRESULT Ready_Physics();
	HRESULT Ready_PartObject();
	HRESULT Ready_Events();
	HRESULT Ready_Bounceable();

public:
	virtual void Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager) override;
	virtual void Parried() override;
	virtual void Active() override;
	virtual void Deactive() override;
	virtual void Respawn() override;
	virtual void Death() override;

public:
	virtual CGameObject* Get_Parried() override { return this; }

public:
	void Locate(_fvector _vPos);
	void Chase(_float fTimeDelta);
	void MoveSide(_float fTimeDelta);
	void Select_Action();	
	void Delete_Physics();
	void Disable_AllWeaponCollider();
	void Disable_AllWrenches();	
	void Enable_AllWrenches();

public:
	void Setup_Light();
	void Enable_RollEffects();
	void Call_NonLoopRollEffects();
	void Disable_RollEffects();
	void Rotate_RollEffects(_fvector vLookAt);

private:
	CFSM*					m_pFSMCom{ nullptr };
	CModel*					m_pModelCom{ nullptr };
	CShader*				m_pShaderCom{ nullptr };
	CCollider*				m_pColliderCom{ nullptr };
	CMovement*				m_pMovementCom{ nullptr };
	CTexture*				m_pNoiseTextureCom{ nullptr };
	CTransform*				m_pPlayerTransform{ nullptr };
	CPhysics_Controller*	m_pPhysicsControllerCom{ nullptr };

private:
	_bool					m_bIsUnblockable{ false };
	_bool					m_bIsRoll{ false };
	_bool					m_bIsBounced{ false };
	_int					m_iCurBouncingObjIndex{ -1 };
	_bool					m_bIsExecution{ false };	
	_bool					m_bIsChase{ false };
	_bool					m_bIsSideMove{ false };
	_float					m_fChaseSpeed{ 1.f };
	_float					m_fSideMoveSpeed{ 1.f };

private:
	vector<CGameObject*>	m_BounceObject{};
	vector<_vec3>			m_BounceObjectPos{};
	vector<CGameObject*>	m_Wrenches{};
	vector<CCollider*>		m_WeaponColliders{};
	vector<CEffectObject*>	m_RollingEffects{};

public:
	static CBoss_Roland* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
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
class CTexture;
class CPhysics_Controller;
END

BEGIN(Client)
class CPetroch_CrystalSpawner;
class CPetroch_SharpCrystal;

class CBoss_Petroch final : public CEnemy, public IDamaged, public IParried
{
	using super = CEnemy;

public:
	struct DESC
	{
		CEnemy::DESC EnemyDesc{};
	};

private:
	CBoss_Petroch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Petroch(const CBoss_Petroch& Prototype);
	virtual ~CBoss_Petroch() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public:
	void Set_Hide(_bool _bIsHide) { m_bIsHide = _bIsHide; }
	void Set_Block(_bool _bIsBlock) { m_bIsBlock = _bIsBlock; }
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
	HRESULT Ready_CrystalSpawner();

public:
	virtual void Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager) override;
	virtual void Parried() override;	
	virtual void Active() override;
	virtual void Deactive() override;
	virtual void Respawn() override;
	virtual void Death() override;

public:
	void Dessolve(_float fTimeDelta);

public:	
	_bool Is_Hide() { return m_bIsHide; }
	_bool Is_Block() { return m_bIsBlock; }
	_bool Is_Roll() { return m_bIsRoll; }
	virtual CGameObject* Get_Parried() override { return this; }

public:
	void Reset_BlockTimer() { m_fCurBlockTimer = 0; m_bIsBlock = true; }
	void Set_Roll(_bool _bIsRoll);	

public:	
	void Decrease_BlockTimer(_float fTimeDelta);
	void Chase(_float fTimeDelta);
	void Delete_Physics();
	void MoveSide(_float fTimeDelta);
	void Ground_Slam();
	void Dive_Punch();
	void Pounce();
	void Select_Action();
	void Setup_Light();

public:
	void Call_RollingEffects();
	void Call_NonLoopRollingEffects();
	void Control_RollingEffects();
	void Disable_RollingEffects();

private:
	CFSM*								m_pFSMCom{ nullptr };
	CModel*								m_pModelCom{ nullptr };
	CShader*							m_pShaderCom{ nullptr };
	CCollider*							m_pColliderCom{ nullptr };
	CCollider*							m_pShellHouseColliderCom{ nullptr };
	CMovement*							m_pMovementCom{ nullptr };
	CTexture*							m_pNoiseTextureCom{ nullptr };
	CTransform*							m_pPlayerTransform{ nullptr };
	CPhysics_Controller*				m_pPhysicsControllerCom{ nullptr };
	vector<CPetroch_CrystalSpawner*>	m_pCrystalSpwners{};
	vector<CPetroch_SharpCrystal*>		m_pCrystals{};
	vector<CEffectObject*>				m_RollingEffects{};

private:
	_bool								m_bIsHide{ false };	;
	_bool								m_bIsRoll{ false };
	_bool								m_bIsChase{ false };
	_bool								m_bIsSideMove{ false };	
	_int								m_iCrystalCount{ 3 };
	_int								m_iCurCrystalIndex{ 0 };
	_float								m_fChaseSpeed{ 1.f };
	_float								m_fSideMoveSpeed{ 1.f };

private:
	_bool								m_bIsBlock{ false };
	_float								m_fCurBlockTimer{ 0.f };
	_float								m_fMaxBlockTimer{ 0.1f };

public:
	static CBoss_Petroch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
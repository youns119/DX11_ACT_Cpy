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
class CTexture;
class CFSM;
class CMovement;
class CPhysics_Controller;
class CCineCamera;
END

BEGIN(Client)
class CCuttlefish_Missile;

class CEnemy_Cuttlefish final 
	: public CEnemy
	, public IDamaged
	, public IParried
{
	using super = CEnemy;

public:
	struct DESC
	{
		CEnemy::DESC EnemyDesc{};	

		// body_00_x.17
	};

private:
	CEnemy_Cuttlefish(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy_Cuttlefish(const CEnemy_Cuttlefish& Prototype);
	virtual ~CEnemy_Cuttlefish() = default;

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
	HRESULT Ready_Components();
	HRESULT Ready_Physics();	
	HRESULT Ready_PartObject();
	HRESULT Ready_Events();
	HRESULT Ready_Missile();


public:	
	void Set_IsChase(_bool _bIsChase, _float _fSpeed = 1.f)
	{ 
		m_bIsChase = _bIsChase; m_fChaseSpeed = _fSpeed; 
	}
	void Set_IsSideMove(_bool _bIsSideMove, _float _fSpeed = 1.f)
	{
		m_bIsSideMove = _bIsSideMove; m_fSideMoveSpeed = _fSpeed;
	}

public:
	void Set_IsGrab(_bool _bIsGrab) { m_bIsGrab = _bIsGrab; }
	void Set_Execution(_bool _bIsExecution) { m_bIsExecution = _bIsExecution; }

public:
	_bool Is_Grab() { return m_bIsGrab; }
	_bool Is_Execution() { return m_bIsExecution; }

public:		
	void Chase(_float fTimeDelta);
	void MoveSide(_float fTimeDelta);
	void Locate(_vec3 vTargetPos);
	void Throw_Missile_L();
	void Throw_Missile_R();

public:
	virtual void Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager) override;
	virtual void Select_Action() override;	
	virtual void Parried() override;
	virtual void Active() override;
	virtual void Deactive() override;
	virtual void Respawn() override;
	virtual void Death() override;

public:
	virtual CGameObject* Get_Parried() override { return this; }

private:
	CShader*						m_pShaderCom { nullptr };
	CModel*							m_pModelCom { nullptr };
	CMovement*						m_pMovementCom{ nullptr };
	CCollider*						m_pColliderCom { nullptr };	
	CTransform*						m_pPlayerTransform{ nullptr };
	CFSM*							m_pFSMCom{ nullptr };
	CTexture*						m_pNoiseTextureCom{ nullptr };
	CPhysics_Controller*			m_pPhysicsControllerCom{ nullptr };
	CCineCamera*					m_pCineCamera{ nullptr };

private:
	vector<CCuttlefish_Missile*>	m_Missiles{};

private:
	_int							m_iAnimIndex{ 0 };
	_bool							m_bIsChase = { false };
	_float							m_fChaseSpeed{ 1 };
	_bool							m_bIsSideMove{ false };;
	_float							m_fSideMoveSpeed{ 1.f };

private:
	_bool							m_bIsGrab{ false };
	_bool							m_bIsExecution{ false };

public:
	static CEnemy_Cuttlefish* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;	
};
END

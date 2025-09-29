#pragma once

#include "Client_Defines.h"
#include "Enemy.h"

#include "IDamaged.h"
#include "IParried.h"
#include "EnemyStateEnum.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CMovement;
class CFSM;
class CTexture;
class CCineCamera;
class CPhysics_Controller;
END

BEGIN(Client)
class CIK_Manager;
class CBoss_Pagurus final : public CEnemy, public IDamaged, public IParried
{
	using super = CEnemy;

public:
	struct Pagurus_DESC
	{
		CEnemy::DESC EnemyDesc{};	
	};

private:
	CBoss_Pagurus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Pagurus(const CBoss_Pagurus& Prototype);
	virtual ~CBoss_Pagurus() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public:		
	void Set_IsGrab(_bool _bIsGrab) { m_bIsGrab = _bIsGrab; }
	void Set_Execution(_bool _bIsExecution) { m_bIsExecution = _bIsExecution; }
	void Set_ChaseSpeed(_float _fSpeed) { m_fChaseSpeed = _fSpeed; }
	void Set_IsChase(_bool _bIsChase, _float _fSpeed = 1.f) {
		m_bIsChase = _bIsChase; m_fChaseSpeed = _fSpeed;
	}
	void Set_IsSideMove(_bool _bIsSideMove, _float _fSpeed = 1.f) {
		m_bIsSideMove = _bIsSideMove; m_fSideMoveSpeed = _fSpeed;
	}


public:
	_bool Is_Grab() { return m_bIsGrab; }
	_bool Is_Execution() { return m_bIsExecution; }

public:
	HRESULT Ready_Components();
	HRESULT Ready_Physics();
	HRESULT Ready_PartObject();
	HRESULT Ready_Events();
	HRESULT Ready_IK();

private:
	void Set_Segment(const _char* szBoneName, _float fBaseAngle, _float fDefaultDistance, _int _iGroupIndex);

private:
	void Delete_Physics();

public:
	virtual void Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager) override;
	virtual void Parried() override;
	virtual void Active() override;
	virtual void Deactive() override;
	virtual void Respawn() override;
	virtual void Death() override;
	
public :
	virtual CGameObject* Get_Parried() override { return this; }

public:	
	void Ambush();
	void Locate(_fvector _vPos);
	_bool Detect_Player();
	void Chase(_float fTimeDelta);
	void MoveSide(_float fTimeDelta);	
	void Select_Action();

private:	
	CFSM*					m_pFSMCom{ nullptr };
	CModel*					m_pModelCom{ nullptr };
	CShader*				m_pShaderCom{ nullptr };
	CCollider*				m_pColliderCom{ nullptr };
	CMovement*				m_pMovementCom{ nullptr };	
	CTexture*				m_pNoiseTextureCom{ nullptr };
	CTransform*				m_pPlayerTransform{ nullptr };
	CPhysics_Controller*	m_pPhysicsControllerCom{ nullptr };	
	CCineCamera*			m_pCineCamera{ nullptr };

private:
	CIK_Manager* m_pIKManager{ nullptr };

private:	
	_bool					m_bIsUnblockable{ false };
	_bool					m_bIsGrab{ false };	
	_bool					m_bIsExecution{ false };
	_bool					m_bIsChase{ false };
	_bool					m_bIsSideMove{ false };
	_float					m_fChaseSpeed{ 1.f };
	_float					m_fSideMoveSpeed{ 1.f };

		
public:
	static CBoss_Pagurus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
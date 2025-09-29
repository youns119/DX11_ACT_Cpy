#pragma once

#include "Client_Defines.h"
#include "Enemy.h"

#include "EnemyStateEnum.h"

#include "IDamaged.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CFSM;
class CMovement;
class CTexture;
class CPhysics_Controller;
class CCineCamera;
END

BEGIN(Client)
class CEnemy_BobbitWorm final
	: public CEnemy
	, public IDamaged
{
	using super = CEnemy;

public:
	struct DESC
	{
		CEnemy::DESC EnemyDesc{};
	};

private:
	CEnemy_BobbitWorm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy_BobbitWorm(const CEnemy_BobbitWorm& Prototype);
	virtual ~CEnemy_BobbitWorm() = default;

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

public:
	void Ambush();

public:
	void Set_IsGrab(_bool _bIsGrab) { m_bIsGrab = _bIsGrab; }
	void Set_Execution(_bool _bIsExecution) { m_bIsExecution = _bIsExecution; }

public:
	_bool Is_Grab() { return m_bIsGrab; }
	_bool Is_Execution() { return m_bIsExecution; }

public:
	virtual void Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager) override;
	virtual void Select_Action() override;	
	virtual void Active() override;
	virtual void Deactive() override;
	virtual void Respawn() override;
	virtual void Death() override;

private:
	CShader*				m_pShaderCom{ nullptr };
	CModel*					m_pModelCom{ nullptr };	
	CCollider*				m_pColliderCom{ nullptr };
	CTransform*				m_pPlayerTransform{ nullptr };
	CFSM*					m_pFSMCom{ nullptr };
	CMovement*				m_pMovementCom{ nullptr };
	CTexture*				m_pNoiseTextureCom{ nullptr };
	CPhysics_Controller*	m_pPhysicsControllerCom{ nullptr };
	CCineCamera*			m_pCineCamera{ nullptr };

private:
	_bool					m_bIsGrab{ false };
	_bool					m_bIsExecution{ false };

public:
	static CEnemy_BobbitWorm* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END

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
class CFSM;
class CMovement;
class CTexture;
class CPhysics_Controller;
END

BEGIN(Client)
class CEnemy_Sardine final 
	: public CEnemy
	, public IDamaged
	, public IParried
{
	using super = CEnemy;

public:
	struct DESC
	{
		CEnemy::DESC EnemyDesc{};

		//Lock On Bone: Tail.12
	};

private:
	CEnemy_Sardine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy_Sardine(const CEnemy_Sardine& Prototype);
	virtual ~CEnemy_Sardine() = default;

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
	void Set_IsChase(_bool _bIsChase, _float _fSpeed = 1.f)
	{ m_bIsChase = _bIsChase; m_fChaseSpeed = _fSpeed; }

public:	
	void Chase(_float fTimeDelta);	

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
	CShader*				m_pShaderCom { nullptr };
	CModel*					m_pModelCom { nullptr };
	CMovement*				m_pMovementCom{ nullptr };
	CCollider*				m_pColliderCom { nullptr };	
	CTransform*				m_pPlayerTransform{ nullptr };
	CFSM*					m_pFSMCom{ nullptr };
	CTexture*				m_pNoiseTextureCom{ nullptr };
	CPhysics_Controller*	m_pPhysicsControllerCom{ nullptr };

private:
	_bool					m_bIsChase = { false };		
	_float					m_fChaseSpeed{ 1 };	

public:
	static CEnemy_Sardine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;	
};
END

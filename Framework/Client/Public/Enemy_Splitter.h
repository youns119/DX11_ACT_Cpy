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
class CPhysics_Controller;
class CCineCamera;
END

BEGIN(Client)
class CEnemy_Splitter final : public CEnemy, public IDamaged, public IParried
{
	using super = CEnemy;

public:
	struct DESC
	{
		CEnemy::DESC EnemyDesc{};
		//_vector vOgriginPos{};

		// Lock On Bone: spine.12
	};

private:
	CEnemy_Splitter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy_Splitter(const CEnemy_Splitter& Prototype);
	virtual ~CEnemy_Splitter() = default;

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
	void Set_IsFear(_bool _bIsFear) { m_bIsFear= _bIsFear; }
	void Set_IsGrab(_bool _bIsGrab) { m_bIsGrab = _bIsGrab; }
	void Set_IsUnblockable(_bool _bIsUnblockable) { m_bIsUnblockable = _bIsUnblockable; }
	void Set_Execution(_bool _bIsExecution) { m_bIsExecution = _bIsExecution; }
	void Set_IsChase(_bool _bIsChase, _float _fSpeed = 1.f)
	{ m_bIsChase = _bIsChase; m_fChaseSpeed = _fSpeed; }
	void Set_IsGrayScaling(_bool _bIsGrayScaling) { m_bIsGrayScaling = _bIsGrayScaling; }

public:
	_bool Is_Grab() { return m_bIsGrab; }
	_bool Is_Fear() { return m_bIsFear; }
	_bool Is_Unblockable() { return m_bIsUnblockable; }
	_bool Is_Execution() { return m_bIsExecution; }
	_bool Is_GrayScaling() { return m_bIsGrayScaling; }

public:
	HRESULT Ready_Components();
	HRESULT Ready_Physics();
	HRESULT Ready_PartObject();
	HRESULT Ready_Events();

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
	void Select_Action();	

private:
	void Control_GrayScale(_float fTimeDelta, _float fSpeed);

private:	
	CFSM*					m_pFSMCom{ nullptr };
	CModel*					m_pModelCom{ nullptr };
	CShader*				m_pShaderCom{ nullptr };
	CMovement*				m_pMovementCom{ nullptr };
	CCollider*				m_pColliderCom{ nullptr };
	CCollider*				m_pFearColliderCom{ nullptr };		
	CTexture*				m_pNoiseTextureCom{ nullptr };
	CTransform*				m_pPlayerTransform{ nullptr };
	CPhysics_Controller*	m_pPhysicsControllerCom{ nullptr };
	CCineCamera*			m_pCineCamera{};
	
private:	
	_bool					m_bIsFear{ false };
	_bool					m_bIsUnblockable{ false };
	_bool					m_bIsExecution{ false };
	_bool					m_bIsGrab{ false };
	_bool					m_bIsChase{ false };
	_float					m_fChaseSpeed{ 1 };	

private:
	_bool					m_bIsGrayScaling{ false };

public:
	static CEnemy_Splitter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;		
};
END

#pragma once

#include "Client_Defines.h"
#include "Enemy.h"

#include "EnemyStateEnum.h"

#include "IK_Manager.h"
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

class CEnemy_Bowman final : public CEnemy, public IDamaged, public IParried
{
	using super = CEnemy;

public:
	struct DESC
	{
		CEnemy::DESC EnemyDesc{};

		//Lock On Target: Torso.7

		/*
	 		Torso.7

			Thigh_L_1.45, Knee_L_1.46, Foot_L_1.47
			Thigh_L_2.48, Knee_L_2.49, Foot_L_2.50
			Thigh_L_3.51, Knee_L_3.52, Foot_L_3.53

			Thigh_R_1.54, Knee_R_1.55, Foot_R_1.56
			Thigh_R_2.57, Knee_R_2.58, Foot_R_2.59
			Thigh_R_3.60, Knee_R_3.61, Foot_R_3.62
		*/		
	};

private:
	CEnemy_Bowman(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy_Bowman(const CEnemy_Bowman& Prototype);
	virtual ~CEnemy_Bowman() = default;

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
	virtual void Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager) override;
	virtual void Parried() override;
	virtual void Active() override;
	virtual void Deactive() override;
	virtual void Respawn() override;
	virtual void Death() override;

public:
	void Set_IKOperation(_bool _bOperation) { m_pIKManager->Set_Operation(_bOperation); }
	virtual CGameObject* Get_Parried() override { return this; }

public:
	void Set_IsChase(_bool _bIsChase, _float _fSpeed = 1.f)
	{
		m_bIsChase = _bIsChase; m_fChaseSpeed = _fSpeed;
	}

public:
	void Chase(_float fTimeDelta);

public:
	HRESULT Ready_Components();
	HRESULT Ready_Physics();
	HRESULT Ready_Events();
	HRESULT Ready_PartObject();
	HRESULT Ready_IK();

private:
	void Set_Segment(const _char* szBoneName, _float fBaseAngle, _float fDefaultDistance, _int _iGroupIndex);

public:		
	virtual void Select_Action() override;

private:
	CFSM*					m_pFSMCom = { nullptr };
	CModel*					m_pModelCom{ nullptr };
	CShader*				m_pShaderCom { nullptr };
	CCollider*				m_pColliderCom { nullptr };
	CTransform*				m_pPlayerTransform{ nullptr };
	CTexture*				m_pNoiseTextureCom{ nullptr };
	CMovement*				m_pMovementCom = { nullptr };	
	CPhysics_Controller*	m_pPhysicsControllerCom{ nullptr };		

private:
	CIK_Manager*			m_pIKManager{ nullptr };

private:
	_bool					m_bIsChase = { false };
	_float					m_fChaseSpeed{ 1 };

private:
	//_float m_fConstraintAngle = 3.5f;
	//_float m_fConstraintMinDistance = 0.5f;
	//_float m_fConstraintMaxDistance = 2.0f;
	//_float m_fDefaultDistance = 1.5f;
		
public:
	static CEnemy_Bowman* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;	
};
END	

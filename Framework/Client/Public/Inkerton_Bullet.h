#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CFSM;
class CMovement;
class CPhysics_Controller;
END

//Gun_end.6

BEGIN(Client)
class CInkerton_Bullet final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
	};

private:
	CInkerton_Bullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInkerton_Bullet(const CInkerton_Bullet& Prototype);
	virtual ~CInkerton_Bullet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;	

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public:		
	void DecreaseLifeTime(_float fTimeDelta);
	void Fire_Bullet(_float4x4 StartMat);

private:
	void Call_ImpactEffect(_float fTimeDelta);

public:
	HRESULT Ready_Components();

private:
	CTransform*				m_pPlayerTransform{ nullptr };
	CShader*				m_pShaderCom { nullptr };	
	CCollider*				m_pColliderCom { nullptr };		

private:
	_bool					m_bIsFired{ false };
	_float					m_fCurLifeTime{ 0.f };
	_float					m_fMaxLifeTime{ 3.5f };
	_float					m_fImpactInterval{ 0.25f };
	_float					m_fImpactIntervalAcc{0.f};

public:
	static CInkerton_Bullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END

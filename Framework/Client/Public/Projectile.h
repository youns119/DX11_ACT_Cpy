#pragma once

#include "Client_Defines.h"

#include "GameObject.h"

#include "FProjectile.h"

BEGIN(Engine)

class CCollider;
class CMovement;

END

BEGIN(Client)

class CProjectile abstract
	: public CGameObject
{
	using super = CGameObject;

public :
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
	};

protected :
	CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile(const CProjectile& Prototype);
	virtual ~CProjectile() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
public :
	_bool Get_Active() { return m_bActive; }

public :
	virtual void Activate(const FProjectileStat& tProjectileStat);
	virtual void Deactivate();

protected :
	CCollider* m_pColliderCom{};
	CMovement* m_pMovementCom{};

protected :
	_bool m_bActive{};

protected :
	FProjectileStat m_tProjectileStat{};

protected :
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public :
	virtual CCloneable* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END
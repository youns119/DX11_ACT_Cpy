#pragma once

#include "Client_Defines.h"

#include "GameObject.h"

BEGIN(Engine)

class CModel;
class CShader;
class CCollider;
class CMovement;

END

BEGIN(Client)

class CPlayer;

class CSkill_Topoda
	: public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		GAMEOBJECT_DESC tGameObjectDesc{};

		CPlayer* pOwner{};
	};

public :
	enum class TOPODA
	{
		TOPODA_PUNCH,
		TOPODA_RETREAT,

		TOPODA_END
	};

private:
	CSkill_Topoda(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Topoda(const CSkill_Topoda& Prototype);
	virtual ~CSkill_Topoda() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual void Final_Update(_float fTimeDelta);
	HRESULT Render() override;

public :
	CPlayer* Get_Owner() { return m_pOwner; }
	CGameObject* Get_AutoLockTarget() { return m_pAutoLockTarget; }

public:
	void Spawn();
	void Despawn();

public :
	void Toggle_Collider(_bool bToggle);
	void Dettach();

private:
	CModel* m_pModelCom{};
	CShader* m_pShaderCom{};
	CCollider* m_pColliderCom{};
	CMovement* m_pMovementCom{};

private:
	CPlayer* m_pOwner{};

private:
	_bool m_bActive{};
	_bool m_bAttach{ true };

	_float m_fLerpTime{ 0.5f };
	_float m_fLerpAcc{};

private :
	CGameObject* m_pAutoLockTarget{};

private :
	_float4x4 m_matSocket{};

private :
	TOPODA m_eTopoda{ TOPODA::TOPODA_END };
	_uint m_iAnimIndex[static_cast<_uint>(TOPODA::TOPODA_END)]{};

private :
	_float m_fRimPower{ 1.5f };
	_float4 m_vRimColor{ 1.f, 0.f, 1.f, 1.f };
	_float4 m_vCamPosition{};

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

private :
	HRESULT Ready_Component();
	HRESULT Ready_Event();

public:
	static CSkill_Topoda* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once

#include "Projectile.h"

BEGIN(Engine)

class CShader;
class CCollider;
class CMovement;
class CEffectObject;

END

BEGIN(Client)

class CEnemy;

class CProjectile_Fizzle final
	: public CProjectile
{
	using super = CProjectile;

private :
	CProjectile_Fizzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Fizzle(const CProjectile_Fizzle& Prototype);
	virtual ~CProjectile_Fizzle() = default;

public :
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Activate(const FProjectileStat& tProjectileStat) override;
	virtual void Deactivate() override;

private :
	list<CGameObject*> m_listMonster;

	CEnemy* m_pTarget{};
	CCollider* m_pTargetCollider{};
	CEffectObject* m_pEffect{};

private :
	_bool m_bHasTarget{};

	_float m_fDistance{ 30.f };
	_float m_fLaunchTime{};
	_float m_fLaunchAcc{};

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

private :
	HRESULT Ready_Components();

public :
	static CProjectile_Fizzle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
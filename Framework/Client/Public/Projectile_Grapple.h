#pragma once

#include "Projectile.h"

BEGIN(Engine)

class CModel;
class CShader;
class CCollider;
class CMovement;

END

BEGIN(Client)

class CPlayer;

class CProjectile_Grapple final
	: public CProjectile
{
	using super = CProjectile;

private:
	CProjectile_Grapple(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Grapple(const CProjectile_Grapple& Prototype);
	virtual ~CProjectile_Grapple() = default;

public:
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
	CModel* m_pModelCom{};
	CShader* m_pShaderCom{};

private:
	CPlayer* m_pPlayer{};
	CGameObject* m_pTarget{};

private:
	_bool m_bHasTarget{};

	_bool m_bBazier{};
	_float m_fBazierTime{};
	_float m_fDuration{};
	_float3 m_vSrc{};
	_float3 m_vDst{};
	_float3 m_vMid{};

	_float3 m_vPrevPos{};

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Ready_PartObject();

public:
	static CProjectile_Grapple* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once

#include "Projectile.h"

BEGIN(Engine)

class CModel;
class CShader;
class CCollider;
class CMovement;
class CEffectObject;

END

BEGIN(Client)

class CPlayer;

class CProjectile_Urchin final
	: public CProjectile
{
	using super = CProjectile;

private:
	CProjectile_Urchin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Urchin(const CProjectile_Urchin& Prototype);
	virtual ~CProjectile_Urchin() = default;

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

private:
	void Call_ThrowEffects();
	void Disable_ThrowEffects();

	void Call_NonLoopChargingEffects(_float fTimeDelta);
	void Call_NonLoopThrowEffects(_float fTimeDelta);

	void Call_RemainEffects();
	void Disable_RemainEffects();

	void Call_ExplosionEffects();
	void Disable_ExplosionEffects();

private:
	CModel* m_pModelCom{};
	CShader* m_pShaderCom{};

private:
	vector<CEffectObject*> m_ThrowEffects;
	vector<CEffectObject*> m_RemainEffects;
	vector<CEffectObject*> m_ChargingPtclEffects;
	CPlayer* m_pPlayer{};

private :
	_bool m_bRelease{};
	_bool m_bThrown{};
	_bool m_bExplosion{};

	_bool m_bLerp{};
	_bool m_bRender{};

private :
	_uint m_iLevel{ 1 };
	_float m_fChargeAcc{};
	_float m_fExplosionAcc{};
	_float m_fExplosionDuration{ 1.f };

private:
	_float m_fChargingEffectAcc{};
	_float m_fChargingEffectDuration{ 0.5f };

private :
	_float4x4 m_matExplosion{};

	_float m_fPrevScale{ 1.f };
	_float m_fScale{ 1.f };
	_float m_fLerp{};
	_float m_fVibrate{};

private:
	void Lerp(_float fTimeDelta);
	void Explosion(_float fTimeDelta);
	void Vibrate(_float fTimeDelta);

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Ready_PartObject();

public:
	static CProjectile_Urchin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
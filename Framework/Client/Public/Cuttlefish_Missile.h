#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CMovement;
class CEffectObject;
END

BEGIN(Client)


class CCuttlefish_Missile final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
		CGameObject* pOwner{ nullptr };
	};

private:
	CCuttlefish_Missile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCuttlefish_Missile(const CCuttlefish_Missile& Prototype);
	virtual ~CCuttlefish_Missile() = default;

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
	HRESULT Ready_Components();
	HRESULT Ready_Physics();	

private:
	_vec3 Compute_CardinalSpline(const _vec2& p0, const _vec2& p1, _float fFactor, _float fTension, _float fAmplitude);

public:
	void Set_ParentWorldMatrix(const _float4x4* ParentWorldMatrix) { m_pParentWorldMatrix = ParentWorldMatrix; }
	void Set_SpawnOption(_float4x4* vStartPos, _vector vTargetPos, _float fTension, _float fMoveSpeed, _bool bMoveLeft);

private:
	void Deactive();
	void Active();

private:
	void Enable_Effects();
	void Disable_Effects();
	void Call_TrailEffect(_fvector vPivot);
	void Call_ExplodeEffect();

private:
	CGameObject*					m_pOwner{ nullptr };
	CMovement*						m_pMovementCom{ nullptr };
	CTransform*						m_pPlayerTransform{ nullptr };
	CCollider*						m_pColliderCom{ nullptr };	

private:	
	_bool							m_bIsLeft{ false };	
	_float							m_fCurSplineFactor{ 0.f };
	_float							m_fCurSplineTension{ 0.f };

private:
	_float							m_fMoveSpeed{ 1.f };		

private:
	_float4							m_vDefaultLook{};
	_float4							m_fStartPos{};
	_float4							m_fTargetPos{};	

private:
	vector<CEffectObject*>			m_Effects;
	_float							m_fTrailCallTime{};
	_float							m_fTrailCallTimeAcc{};

public:
	static CCuttlefish_Missile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
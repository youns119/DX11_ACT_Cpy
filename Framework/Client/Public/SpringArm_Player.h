#pragma once

#include "Client_Defines.h"

#include "SpringArm.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Client)

class CCamera_Player;

class CSpringArm_Player
	: public CSpringArm
{
	using super = CSpringArm;

public:
	struct DESC
	{
		super::DESC SpringArmDesc{};

		CGameObject* pOwner{};

		_float fLerpSpeed{ 1.f };
		_float fMaxPitchDegree{ 90.f };
	};

private:
	CSpringArm_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpringArm_Player(const CSpringArm_Player& Prototype);
	virtual ~CSpringArm_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CCamera_Player* Get_PlayerCamera() { return m_pPlayerCamera; }

	_bool Get_IsLockOn() const { return m_bLockOn; }
	CGameObject* Get_LockOnTarget() const { return m_pLockOnTarget; }

	_float Get_DefaultArmLength() const { return m_fDefaultArmLength; }

	CGameObject* Get_AutoLockTarget(_float fAutoDist = 10.f) const;

public:
	void Zoom(_float fArmLength);

public:
	void UnLock_LockOn();

public:
	void Check_LockOnTarget();

public :
	void Sync_Rotation();
	void Reset_Rotation(_fvector vDir);
	void Reset_Position(_fvector vPos);
	void Toggle_Input(_bool bToggle) { m_bInputToggle = bToggle; }

private:
	CCollider* m_pColliderCom{};

private:
	_float m_fCurrYaw{};
	_float m_fTargetYaw{};
	_float m_fCurrPitch{};
	_float m_fTargetPitch{};

	_float m_fLerpSpeed{ 6.f };
	_float m_fMaxPitchDegree{ 90.f };

private:
	CCamera_Player* m_pPlayerCamera{};

private:
	list<CGameObject*> m_listLockOn;

private:
	CGameObject* m_pLockOnTarget{};
	CTransform* m_pLockOnTransform{};

private:
	_float m_fDefaultArmLength{};

private:
	_bool m_bLockOn{};
	_float m_fLockOnDist{ 30.f };
	_float m_fLockOnAngle{ 60.f };

private :
	_bool m_bInputToggle{ true };

private:
	CGameObject* m_pOwner{};

private:
	void Input_Control(_float fTimeDelta);

private:
	_bool Check_Physics();

private:
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

private:
	HRESULT Ready_Component();
	HRESULT Ready_PartObject();

public:
	static CSpringArm_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg);
	virtual void Free() override;
};

END
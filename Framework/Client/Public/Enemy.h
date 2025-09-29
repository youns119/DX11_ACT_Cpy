#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
 
#include "IGrapple.h"

BEGIN(Client)
class IDamaged;

struct EnemyStats {
	_bool	bIsHooked{ false };
	_bool	bIsFinisher{ false };
	_bool	bIsFinished{ false };
	_bool	bIsLockedOn{ false };
	_bool	bIsCapsized{ false };
	_bool	bIsBalanceBreakable{ false };
	_float	fCurHP{ 100.f };
	_float	fMaxHP{ 100.f };
	_float	fCurBP{ 0.f };
	_float	fMaxBP{ 100.f };
	_float	fCurBalanceBreakDuration{ 0.f };
	_float	fMaxBalanceBreakDuration{ 10.f };
};

class CEnemy 
	: public CGameObject
	, public IGrapple
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC	GameObjectDesc{};
		EnemyStats			enemyStats{};
		_float4x4			OriginalPos{};
		vector<_vec3>		PatrolPoints{};
		const _tchar*		szName;
	};

protected:
	CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy(const CEnemy& Prototype);
	virtual ~CEnemy() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:	
	virtual void On_CollisionEnter(CGameObject* pOther, _uint iGroup) override;
	virtual void On_Collision(CGameObject* pOther, _uint iGroup) override;
	virtual void On_CollisionExit(CGameObject* pOther, _uint iGroup) override;

public:
	virtual void Active();
	virtual void Deactive();
	virtual void Respawn();
	virtual void Death();

public:
	_bool IsAggroed() { return m_bIsAggroed; }	
	_bool IsFinisher() { return m_EnemyStats.bIsFinisher; }
	_bool IsFinished() { return m_EnemyStats.bIsFinished; }
	_bool IsHooked() { return m_EnemyStats.bIsHooked; }
	_bool IsLockedOn() { return m_EnemyStats.bIsLockedOn; }
	_bool IsCapsized() { return m_EnemyStats.bIsCapsized; }	
	_float Get_CurrentHP() { return m_EnemyStats.fCurHP; }
	_float Get_MaxHP() { return m_EnemyStats.fMaxHP; }
	_float Get_CurrentBP() { return m_EnemyStats.fCurBP; }
	_float Get_MaxBP() { return m_EnemyStats.fMaxBP; }
	_float Get_IsDead() { return m_bIsDead; }
	_float Get_FrustumRange() { return m_fFrustumRange; }

public:
	const _float4x4* Get_CenterMetrixPtr() { return m_CenterMatrixPtr; }
	_float Get_DistanceBtwTarget(_vec3 vTargetPos);
	vector<_vec3> Get_PatrolPoints() { return m_PatrolPoints; }
	_vector Get_OriginalPos() { return XMLoadFloat4x4(&m_OriginalPos).r[(_uint)CTransform::STATE::POSITION]; }

public:	
	void Set_Dissolve(_bool _bDissolve) { m_bIsDessolve = _bDissolve; }
	void Set_Aggroed(_bool _bIsAggroed) { m_bIsAggroed = _bIsAggroed; }
	void Set_Finisher(_bool _bIsFinisher) { m_EnemyStats.bIsFinisher = _bIsFinisher; }
	void Set_Finished(_bool _bIsFinished) { m_EnemyStats.bIsFinished = _bIsFinished; }
	void Set_Hooked(_bool _bIsHooked) { m_EnemyStats.bIsHooked = _bIsHooked; }
	void Set_Capsized(_bool _bIsCapsize) { m_EnemyStats.bIsCapsized = _bIsCapsize; }
	void Set_LockedOn(_bool _bIsLockedOn) { m_EnemyStats.bIsLockedOn = _bIsLockedOn; }

public:		
	void Restore_Health();
	void Reset_Balance();
	void Reset_RecoverDelay() { m_fCurRecoverDelay = 0; }

public:
	virtual void Select_Action();
	_bool Detect_Target(_float _fFovAngle, _float _fMaxDistance, _float _MinimumDistance, CTransform* _pTargetTransform, CTransform* _pBaseTransform);
	virtual void Drop_Loot();

public :
	void Hooked();

public :
	virtual _vec3 Get_GrapplePos() override;
	virtual _uint Get_Type() override;

protected:
	void Dessolve(_float fTimeDelta);
	void Flash_Effect(_float fTimeDelta);
	void Balance_BreakTimer(_float fTimeDelta);
	void Balance_RecoverTimer(_float fTimeDelta);

protected:
	const _tchar*			m_szName{};
	EnemyStats				m_EnemyStats{};
	_float4x4*				m_CenterMatrixPtr{ nullptr };

protected:
	_bool					m_bIsDead{ false };	
	_bool					m_bIsAggroed{ false };
	_float					m_fFrustumRange{ 10.f };
	_float					m_fCurRecoverDelay{ 0.f };
	_float					m_fMaxRecoverDelay{ 5.f };

protected:
	vector<_vec3>			m_PatrolPoints{};
	_float4x4				m_OriginalPos{};

protected:
	CGameObject*			m_pLoot{ nullptr };
	_uint					m_iDropChance{ 50 };

protected:
	_bool					m_bIsDessolve{ false };
	_float					m_fSwellAmount{ 0.f };
	_float					m_fDissolveRate{ 5.f };
	_float					m_fDissolveDarknessRate{ 0.5f };

protected:
	_bool					m_bIsDamaged{ false };
	_float					m_fCurFlashTime{ 0.f };
	_float					m_fFlashDuration{ .5f };
	_float					m_fFlashIntensity{ 1.f };

public:
	virtual CCloneable* Clone(void* pArg) = 0;
	virtual void Free() override;
};
END	
#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameInstance;
class CModel;
class CGameObject;

class ENGINE_DLL CState : public CBase
{
public:
	struct STATE_DESC {
		CGameObject* pOwner{ nullptr };
	};

protected:
	CState();
	virtual ~CState() = default;

public:
	virtual HRESULT Initialize(void* pObj);

public:
	virtual void Enter(void* pObj);
	virtual void Update(_float fTimeDelta, void* pObj);
	virtual void Exit(void* pObj);

public:
	_int Get_PriorLevel() { return m_iPriorityLevel; }
	_bool IsCooldown() { return m_fCurCoolTime == m_fMaxCoolTime ? true : false; }
	void Set_MaxCooldownTime(_float _fCoolTime) { m_fMaxCoolTime = _fCoolTime; }

public:
	const _char* Get_CurrentStateName() { return m_strStateName; }
	_float Get_CurCoolTime() { return m_fCurCoolTime; }
	_float Get_MaxCoolTime() { return m_fMaxCoolTime; }

public:
	void Refresh_CoolTime();
	void Cooldown(_float fTimeDelta);

protected:
	CGameObject* m_pOwner{ nullptr };
	CModel* m_pModel{ nullptr };

protected:	
	_int			m_iPriorityLevel{ 0 };
	_float			m_fCurCoolTime{ 0.f };
	_float			m_fMaxCoolTime{ 0.f };

	const _char* m_strStateName{};


public:
	virtual void Free();
};

END
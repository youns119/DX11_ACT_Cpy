#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CMovement;
class CTransform;

END

BEGIN(Client)

struct FPlayerStat;

class CEvent_JumpLaunchStart final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_JumpLaunchStart();
	virtual ~CEvent_JumpLaunchStart() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CMovement* m_pMovement{};
	CTransform* m_pTransform{};

private :
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_JumpLaunchStart* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END
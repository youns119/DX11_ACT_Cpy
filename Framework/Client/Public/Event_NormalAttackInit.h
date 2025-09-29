#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CModel;
class CMovement;

END

BEGIN(Client)

struct FPlayerStat;

class CEvent_NormalAttackInit final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_NormalAttackInit();
	virtual ~CEvent_NormalAttackInit() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CMovement* m_pMovement{};

private:
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_NormalAttackInit* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CTransform;
class CMovement;

END

BEGIN(Client)

class CPlayer;

class CEvent_GrappleAttack_BackFlip final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_GrappleAttack_BackFlip();
	virtual ~CEvent_GrappleAttack_BackFlip() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private :
	CTransform* m_pTransform{};
	CMovement* m_pMovement{};

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_GrappleAttack_BackFlip* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END
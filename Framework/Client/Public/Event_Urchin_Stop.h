#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CModel;
class CMovement;

END

BEGIN(Client)

class CPlayer;

class CEvent_Urchin_Stop final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_Urchin_Stop();
	virtual ~CEvent_Urchin_Stop() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private :
	CModel* m_pModel{};
	CMovement* m_pMovement{};

private :
	CPlayer* m_pPlayer{};

public:
	static CEvent_Urchin_Stop* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END
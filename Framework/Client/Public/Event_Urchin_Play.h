#pragma once

#include "Client_Defines.h"

#include "Event.h"

BEGIN(Engine)

class CModel;

END

BEGIN(Client)

class CPlayer;

class CEvent_Urchin_Play final
	: public CEvent
{
	using super = CEvent;

private:
	CEvent_Urchin_Play();
	virtual ~CEvent_Urchin_Play() = default;

public:
	HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CModel* m_pModel{};

private:
	CPlayer* m_pPlayer{};

public:
	static CEvent_Urchin_Play* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END
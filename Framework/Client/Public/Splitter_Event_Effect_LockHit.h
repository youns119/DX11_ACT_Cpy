#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CSplitter_Event_Effect_LockHit final : public CEvent
{
private:
	CSplitter_Event_Effect_LockHit(CGameObject* _pGameObject);
	virtual ~CSplitter_Event_Effect_LockHit() = default;

public:
	HRESULT Execute() override;

private:
	CTransform*			m_pTransform{ nullptr };
	CModel*				m_pModel{ nullptr };

public:
	static CSplitter_Event_Effect_LockHit* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END
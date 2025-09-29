#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CSplitter_Event_Fear final : public CEvent
{
	using super = CEvent;

private:
	CSplitter_Event_Fear();
	virtual ~CSplitter_Event_Fear() = default;

public:
	virtual HRESULT Init(CGameObject* _pGameObject);

public:
	virtual HRESULT Execute() override;

private:
	CTransform*		m_pTransform{ nullptr };
	CModel*			m_pModel{ nullptr };


public:
	static CSplitter_Event_Fear* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END
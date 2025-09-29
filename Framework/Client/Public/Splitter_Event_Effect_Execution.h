#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CSplitter_Event_Effect_Execution final : public CEvent
{
private:
	CSplitter_Event_Effect_Execution(CGameObject* _pGameObject);
	virtual ~CSplitter_Event_Effect_Execution() = default;

public:
	HRESULT Execute() override;

private:
	CTransform* m_pTransform{ nullptr };
	CModel* m_pModel{ nullptr };

public:
	static CSplitter_Event_Effect_Execution* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};
END
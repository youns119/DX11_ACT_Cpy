#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CEvent_BlackSmith final : public CEvent
{
private:
	CEvent_BlackSmith(CGameObject* _pGameObject);
	virtual ~CEvent_BlackSmith() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4* m_pSocketMatrix{ nullptr };

public:
	static CEvent_BlackSmith* Create(CGameObject* _pGameObject);
	virtual void Free() override;
};

END
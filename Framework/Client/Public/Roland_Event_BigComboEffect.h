#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CRoland_Event_BigComboEffect final : public CEvent
{
private:
	CRoland_Event_BigComboEffect(CGameObject* _pGameObject, const _float4x4* _pCombinedWorldMatrix, _uint _iComboCount);
	virtual ~CRoland_Event_BigComboEffect() = default;

public:
	HRESULT Execute() override;

private:
	const _float4x4* m_pCombinedWorldMatrix{};
	_uint m_iComboCount{};

public:
	static CRoland_Event_BigComboEffect* Create(CGameObject* _pGameObject, const _float4x4* _pCombinedWorldMatrix, _uint _iComboCount);
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CBowman_Event_Collider final : public CEvent
{
private:
	CBowman_Event_Collider(const _wstring& _strPartObject, const _wstring& _strComName, _bool _bIsActive);
	virtual ~CBowman_Event_Collider() = default;

public:
	HRESULT Execute() override;
	
private:
	const _wstring m_strPartObject;
	const _wstring m_strComName;
	_bool m_bIsActive;

public:
	static CBowman_Event_Collider* Create(CGameObject* _pGameObject, const _wstring& _strPartObject, const _wstring& _strComName, _bool _bIsActive);
	virtual void Free() override;
};
END	
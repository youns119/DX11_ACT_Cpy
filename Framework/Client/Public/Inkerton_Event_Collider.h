#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)

class CSplitter_Event_Collider final : public CEvent
{
private:
	CSplitter_Event_Collider(const _wstring& _strPartObject, const _wstring& _strComName, _bool _bIsActive);
	virtual ~CSplitter_Event_Collider() = default;

public:
	HRESULT Execute() override;
	
private:
	const _wstring m_strPartObject;
	const _wstring m_strComName;
	_bool m_bIsActive;

public:
	static CSplitter_Event_Collider* Create(CGameObject* _pGameObject, const _wstring& _strPartObject, const _wstring& _strComName, _bool _bIsActive);
	virtual void Free() override;
};
END	
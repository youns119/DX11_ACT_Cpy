#pragma once

#include "Client_Defines.h"
#include "Event.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CEnemy_Event_Sound final : public CEvent
{
private:
	CEnemy_Event_Sound(_wstring _strName, _float _fVolume, _int _iChannel, _wstring _strQuote, _float _fDuration);
	virtual ~CEnemy_Event_Sound() = default;

public:
	HRESULT Execute() override;
	
private:
	const _wstring	m_strName;
	_float			m_fVolume;
	_uint			m_iChannel;
	_wstring		m_strQuote;
	_float			m_fDuration;

public:
	static CEnemy_Event_Sound* Create(CGameObject* _pGameObject, const _wstring _strName, _float _fVolume, _uint _iChannel, _wstring _strQuote = L"", _float _fDuration = 0.f);
	virtual void Free() override;
};
END	
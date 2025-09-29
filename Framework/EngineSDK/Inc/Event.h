#pragma once

#include "Engine_Defines.h"

#include "Base.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CEvent abstract
	: public CBase
{
	using super = CBase;

protected:
	CEvent();
	virtual ~CEvent() = default;

public:
	virtual HRESULT Init(CGameObject* pGameObject);

public:
	virtual HRESULT Execute() PURE;

protected:
	CGameObject* m_pGameObject{};

public:
	virtual void Free() override;
};

END
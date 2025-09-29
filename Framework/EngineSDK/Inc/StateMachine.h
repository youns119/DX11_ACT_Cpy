#pragma once

#include "Engine_Defines.h"
#include "Cloneable.h"

BEGIN(Engine)

class CGameObject;
class CAnimState;

class ENGINE_DLL CStateMachine abstract
	: public CCloneable
{
	using super = CCloneable;

public:
	struct DESC
	{
		CGameObject* pOwner{};

		_wstring strState{};
	};

protected:
	CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStateMachine(const CStateMachine& Prototype);
	virtual ~CStateMachine() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_float fTimeDelta);

public:
	CGameObject* Get_Owner() { return m_pOwner; }

public:
	virtual HRESULT ChangeState(const _wstring& strState);

protected:
	HRESULT Add_AnimState(const _wstring& strAnimState, CAnimState* pAnimState);

protected:
	ID3D11Device* m_pDevice{};
	ID3D11DeviceContext* m_pContext{};

protected:
	CGameObject* m_pOwner{};

protected:
	map<const _wstring, CAnimState*> m_mapAnimState;

protected:
	CAnimState* m_pCurrAnimState{};

protected:
	CAnimState* Find_AnimState(const _wstring& strAnimState);

protected:
	virtual _bool Check_AnimGraph(const _wstring& strAnimState) PURE;

protected:
	virtual HRESULT Ready_AnimState() PURE;

public:
	virtual void Free() override;
};

END
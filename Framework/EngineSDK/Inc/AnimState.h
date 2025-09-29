#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CGameObject;
class CStateMachine;

class ENGINE_DLL CAnimState abstract
    : public CBase
{
    using super = CBase;

protected:
    CAnimState(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CAnimState() = default;

public:
    virtual HRESULT Initialize(CStateMachine* pOwner);

public:
    CGameObject* Get_Character();

public:
    virtual void Enter() PURE;
    virtual void Update(_float fTimeDelta) PURE;
    virtual void Exit() PURE;

protected:
    ID3D11Device* m_pDevice{};
    ID3D11DeviceContext* m_pContext{};

protected:
    CStateMachine* m_pOwner{};

public:
    virtual void Free() override;
};

END
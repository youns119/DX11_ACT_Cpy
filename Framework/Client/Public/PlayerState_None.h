#pragma once

#include "Client_Defines.h"
#include "AnimState.h"

#include "EPlayerState.h"

BEGIN(Engine)

class CModel;

END

BEGIN(Client)

class CPlayerState_None final
    : public CAnimState
{
    using super = CAnimState;

private:
    CPlayerState_None(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_None() = default;

public:
    virtual HRESULT Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState);

public:
    virtual void Enter() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Exit() override;

private :
    CModel* m_pModel{};

private :
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_None* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
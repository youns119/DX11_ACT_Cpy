#pragma once

#include "Client_Defines.h"
#include "AnimState.h"

#include "EPlayerState.h"

BEGIN(Engine)

class CModel;
class CMovement;

END

BEGIN(Client)

class CPlayer;

struct FPlayerStat;

class CPlayerState_Evade final
    : public CAnimState
{
    using super = CAnimState;

public:
    enum class EVADE
    {
        EVADE_NAKED,
        EVADE_MEDIUM,
        EVADE_HEAVY,

        EVADE_END
    };

private:
    CPlayerState_Evade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_Evade() = default;

public:
    virtual HRESULT Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState);

public:
    virtual void Enter() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Exit() override;

private:
    CPlayer* m_pPlayer{};

    CModel* m_pModel{};
    CMovement* m_pMovement{};

private:
    FPlayerStat* m_pPlayerStat{};

private:
    EVADE m_eEvade{};

private:
    _uint m_iAnimIndex[static_cast<_uint>(EVADE::EVADE_END)]{};

private:
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_Evade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
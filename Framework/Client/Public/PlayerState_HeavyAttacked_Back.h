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

class CPlayerState_HeavyAttacked_Back final
    : public CAnimState
{
    using super = CAnimState;

public:
    enum class HEAVYATTACKED_BACK
    {
        HEAVYATTACKED_BACK_START,
        HEAVYATTACKED_BACK_LOOP,

        HEAVYATTACKED_BACK_END
    };

private:
    CPlayerState_HeavyAttacked_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_HeavyAttacked_Back() = default;

public:
    virtual HRESULT Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState);

public:
    virtual void Enter() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Exit() override;

private:
    CPlayer* m_pPlayer{};
    CMovement* m_pMovement{};

    CModel* m_pModel{};

private:
    FPlayerStat* m_pPlayerStat{};

private:
    HEAVYATTACKED_BACK m_eHeavyAttacked_Back{ HEAVYATTACKED_BACK::HEAVYATTACKED_BACK_END };

private:
    HEAVYATTACKED_BACK m_eAttacked{ HEAVYATTACKED_BACK::HEAVYATTACKED_BACK_END };

private:
    _uint m_iAnimIndex[static_cast<_uint>(HEAVYATTACKED_BACK::HEAVYATTACKED_BACK_END)]{};

private:
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_HeavyAttacked_Back* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
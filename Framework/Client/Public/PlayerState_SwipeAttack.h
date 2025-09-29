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

class CPlayerState_SwipeAttack final
    : public CAnimState
{
    using super = CAnimState;

public :
    enum class SWIPEATTACK
    {
        SWIPEATTACK_1,

        HAMMERSWIPEATTACK_1,

        SWIPEATTACK_END
    };

private:
    CPlayerState_SwipeAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_SwipeAttack() = default;

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

private :
    SWIPEATTACK m_eSwipeAttack{ SWIPEATTACK::SWIPEATTACK_END };

private:
    _uint m_iAnimIndex[static_cast<_uint>(SWIPEATTACK::SWIPEATTACK_END)]{};

private :
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_SwipeAttack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
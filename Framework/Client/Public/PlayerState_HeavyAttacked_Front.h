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

class CPlayerState_HeavyAttacked_Front final
    : public CAnimState
{
    using super = CAnimState;

public :
    enum class HEAVYATTACKED_FRONT
    {
        HEAVYATTACKED_FRONT_START,
        HEAVYATTACKED_FRONT_LOOP,

        HEAVYATTACKED_FRONT_END
    };

private:
    CPlayerState_HeavyAttacked_Front(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_HeavyAttacked_Front() = default;

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

private :
    HEAVYATTACKED_FRONT m_eAttacked{ HEAVYATTACKED_FRONT::HEAVYATTACKED_FRONT_END };

private:
    _uint m_iAnimIndex[static_cast<_uint>(HEAVYATTACKED_FRONT::HEAVYATTACKED_FRONT_END)]{};

private:
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_HeavyAttacked_Front* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
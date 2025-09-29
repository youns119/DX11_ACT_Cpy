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

class CPlayerState_MantisPunch final
    : public CAnimState
{
    using super = CAnimState;

public :
    enum class MANTISPUNCH
    {
        MANTISPUNCH_WINDUP,
        MANTISPUNCH_TRAVEL,
        MANTISPUNCH_PUNCH,

        MANTISPUNCH_END
    };

private:
    CPlayerState_MantisPunch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_MantisPunch() = default;

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
    MANTISPUNCH m_eMantisPunch{ MANTISPUNCH::MANTISPUNCH_END };

private:
    _uint m_iAnimIndex[static_cast<_uint>(MANTISPUNCH::MANTISPUNCH_END)]{};

private:
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_MantisPunch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
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

class CPlayerState_ChargeAttack final
    : public CAnimState
{
    using super = CAnimState;

public :
    enum class CHARGEATTACK
    {
        CHARGEATTACK_1,

        HAMMERCHARGEATTACK_1,

        CHARGEATTACK_END,
    };

private:
    CPlayerState_ChargeAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_ChargeAttack() = default;

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

private :
    FPlayerStat* m_pPlayerStat{};

private :
    CHARGEATTACK m_eChargeAttack{ CHARGEATTACK::CHARGEATTACK_END };

private:
    _uint m_iAnimIndex[static_cast<_uint>(CHARGEATTACK::CHARGEATTACK_END)]{};

private :
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_ChargeAttack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
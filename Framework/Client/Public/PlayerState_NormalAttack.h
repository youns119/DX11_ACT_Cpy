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

class CPlayerState_NormalAttack final
    : public CAnimState
{
    using super = CAnimState;

public:
    enum class NORMALATTACK
    {
        NORMALATTACK_1,
        NORMALATTACK_2,
        NORMALATTACK_3,
        NORMALATTACK_4,

        HAMMERATTACK_1,
        HAMMERATTACK_2,

        NORMALATTACK_END,
    };

private:
    CPlayerState_NormalAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_NormalAttack() = default;

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

private:
    NORMALATTACK m_eNormalAttack{};

private:
    _uint m_iAnimIndex[static_cast<_uint>(NORMALATTACK::NORMALATTACK_END)]{};

private :
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_NormalAttack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
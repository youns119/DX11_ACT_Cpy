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

class CPlayerState_SuperUltraCheatMode final
    : public CAnimState
{
    using super = CAnimState;

public :
    enum class CHEAT
    {
        CHEAT1,
        CHEAT2,
        CHEAT3,
        CHEAT4,
        CHEAT5,
        CHEAT6,
        CHEAT7,
        CHEAT8,
        CHEAT9,

        CHEAT_END
    };

private:
    CPlayerState_SuperUltraCheatMode(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_SuperUltraCheatMode() = default;

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
    _uint m_iAnimIndex[static_cast<_uint>(CHEAT::CHEAT_END)]{};

private :
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_SuperUltraCheatMode* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
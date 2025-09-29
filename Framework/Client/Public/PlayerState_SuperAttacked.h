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

class CPlayerState_SuperAttacked final
    : public CAnimState
{
    using super = CAnimState;

public :
    enum class ATTACKED
    {
        ATTACKED_Y,
        ATTACKED_X,

        ATTACKED_END
    };

private:
    CPlayerState_SuperAttacked(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_SuperAttacked() = default;

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
    ATTACKED m_eAttacked{ ATTACKED::ATTACKED_END };

private:
    _uint m_iAnimIndex[static_cast<_uint>(ATTACKED::ATTACKED_END)]{};

    _float3 m_vInitialPos{};

private:
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_SuperAttacked* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
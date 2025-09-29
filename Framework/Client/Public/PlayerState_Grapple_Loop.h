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

class CPlayerState_Grapple_Loop final
    : public CAnimState
{
    using super = CAnimState;

public :
    enum class LOOP
    {
        LOOP_GRAPPLING,
        LOOP_GRAB,
        LOOP_FALLING,

        LOOP_END
    };

private:
    CPlayerState_Grapple_Loop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_Grapple_Loop() = default;

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
    LOOP m_eLoop{};

private:
    _uint m_iAnimIndex[static_cast<_uint>(LOOP::LOOP_END)]{};

private:
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_Grapple_Loop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
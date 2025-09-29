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

class CPlayerState_DashAttack final
    : public CAnimState
{
    using super = CAnimState;

public :
    enum class DASHATTACK
    {
        DASHATTACK_1,

        HAMMERDASHATTACK_1,

        DASHATTACK_END,
    };

private:
    CPlayerState_DashAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_DashAttack() = default;

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
    DASHATTACK m_eDashAttack{ DASHATTACK::DASHATTACK_END };

private:
    _uint m_iAnimIndex[static_cast<_uint>(DASHATTACK::DASHATTACK_END)]{};

private :
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_DashAttack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
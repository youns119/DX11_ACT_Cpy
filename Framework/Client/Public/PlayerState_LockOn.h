#pragma once

#include "Client_Defines.h"
#include "AnimState.h"

#include "EPlayerState.h"

BEGIN(Engine)

class CModel;
class CTransform;
class CMovement;

END

BEGIN(Client)

class CPlayer;

struct FPlayerStat;

class CPlayerState_LockOn final
    : public CAnimState
{
    using super = CAnimState;

public:
    enum class LOCKON
    {
        LOCKON_IDLE,
        LOCKON_WALK_F,
        LOCKON_WALK_B,
        LOCKON_WALK_L,
        LOCKON_WALK_R,

        LOCKON_END,
    };

private:
    CPlayerState_LockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_LockOn() = default;

public:
    virtual HRESULT Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState);

public:
    virtual void Enter() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Exit() override;

private:
    CPlayer* m_pPlayer{};

    CModel* m_pModel{};
    CTransform* m_pTransform{};
    CTransform* m_pLockOnTransform{};
    CMovement* m_pMovement{};

private:
    FPlayerStat* m_pPlayerStat{};

private:
    LOCKON m_eLockOn{};

private:
    _uint m_iAnimIndex[static_cast<_uint>(LOCKON::LOCKON_END)]{};
    _float m_fRunMaxSpeedThreshold{};

private :
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_LockOn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
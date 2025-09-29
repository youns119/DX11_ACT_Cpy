#pragma once

#include "Client_Defines.h"
#include "AnimState.h"

#include "EPlayerState.h"

BEGIN(Engine)

class CModel;
class CMovement;
class CTransform;

END

BEGIN(Client)

class CPlayer;

struct FPlayerStat;

class CPlayerState_BackStepAttack final
    : public CAnimState
{
    using super = CAnimState;

private:
    CPlayerState_BackStepAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_BackStepAttack() = default;

public:
    virtual HRESULT Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState);

public:
    virtual void Enter() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Exit() override;

private:
    void Call_BackStepAttackEffect();

private:
    CPlayer* m_pPlayer{};

    CTransform* m_pTransform{};
    CModel* m_pModel{};
    CMovement* m_pMovement{};

private:
    FPlayerStat* m_pPlayerStat{};

private:
    _uint m_iAnimIndex{};

private:
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_BackStepAttack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
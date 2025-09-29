#pragma once

#include "Client_Defines.h"
#include "AnimState.h"

#include "EPlayerState.h"

BEGIN(Engine)

class CModel;
class CMovement;
class CEffectObject;

END

BEGIN(Client)

class CPlayer;

struct FPlayerStat;

class CPlayerState_Debuff_Fear final
    : public CAnimState
{
    using super = CAnimState;

private:
    CPlayerState_Debuff_Fear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_Debuff_Fear() = default;

public:
    virtual HRESULT Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState);

public:
    virtual void Enter() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Exit() override;

private:
    void Call_FearEffect();
    void Disable_FearEffect();

private:
    CPlayer* m_pPlayer{};

    CModel* m_pModel{};
    CMovement* m_pMovement{};

private:
    FPlayerStat* m_pPlayerStat{};

private:
    _uint m_iAnimIndex{};

private:
    PlayerStateFlags* m_pPlayerState{};

private:
    CEffectObject* m_pTearEffect{nullptr};

public:
    static CPlayerState_Debuff_Fear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
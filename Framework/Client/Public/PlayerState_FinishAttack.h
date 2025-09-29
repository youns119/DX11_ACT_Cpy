#pragma once

#include "Client_Defines.h"
#include "AnimState.h"

#include "EPlayerState.h"

BEGIN(Engine)

class CTransform;
class CModel;
class CMovement;

END

BEGIN(Client)

class CPlayer;

struct FPlayerStat;

class CPlayerState_FinishAttack final
    : public CAnimState
{
    using super = CAnimState;

public :
    enum class FINISHATTACK
    {
        FINISHATTACK_ENTER,
        FINISHATTACK_STAB,
        FINISHATTACK_EXIT,

        FINISHATTACK_END
    };

private:
    CPlayerState_FinishAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_FinishAttack() = default;

public:
    virtual HRESULT Initialize(CStateMachine* pOwner, PlayerStateFlags* pPlayerState);

public:
    virtual void Enter() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Exit() override;

private:
    CPlayer* m_pPlayer{};

    CTransform* m_pTransform{};
    CModel* m_pModel{};
    CMovement* m_pMovement{};

private:
    FPlayerStat* m_pPlayerStat{};

private :
    FINISHATTACK m_eFinishAttack{ FINISHATTACK::FINISHATTACK_END };

private:
    _uint m_iAnimIndex[static_cast<_uint>(FINISHATTACK::FINISHATTACK_END)]{};

private:
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_FinishAttack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
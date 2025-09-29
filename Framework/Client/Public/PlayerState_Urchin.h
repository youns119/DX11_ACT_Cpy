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

class CPlayerState_Urchin final
    : public CAnimState
{
    using super = CAnimState;

public :
    enum class URCHIN
    {
        URCHIN_BACKFLIP,
        URCHIN_THROW,
        URCHIN_DODGE,

        URCHIN_END
    };

private:
    CPlayerState_Urchin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CPlayerState_Urchin() = default;

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
    URCHIN m_eUrchin{ URCHIN::URCHIN_END };

private:
    _uint m_iAnimIndex[static_cast<_uint>(URCHIN::URCHIN_END)]{};

private:
    PlayerStateFlags* m_pPlayerState{};

public:
    static CPlayerState_Urchin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, PlayerStateFlags* pPlayerState);
    virtual void Free() override;
};

END
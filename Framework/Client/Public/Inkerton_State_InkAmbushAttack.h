#pragma once

#include "Client_Defines.h"
#include "State.h"
#include "Boss_Inkerton.h"
#include "Player.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CInkerton_State_InkAmbushAttack final : public CState
{
	using super = CState;

private:
	CInkerton_State_InkAmbushAttack();
	virtual ~CInkerton_State_InkAmbushAttack() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	void Teleport_Behind();
	void Reset_Scale(_float fTimeDelta);
	void Decrease_Scale(_float fTimeDelta);

private:
	CBoss_Inkerton*		m_pInkerton{ nullptr };
	CPlayer*			m_pPlayer{ nullptr };
	CTransform*			m_pTransform{ nullptr };
	CTransform*			m_pPlayerTransform{ nullptr };

private:
	_int				m_iAnimIndex{ 0 };
	_float3				m_fSacle{};
	_float				m_fScaleDecreaseAmount{ 0.06f };
	_float				m_fCurRetreatTime{ 0.f };
	_float				m_fMaxRetreatTime{ 0.3f };
	_float3				m_DefaultScale{};
	
public:
	static CInkerton_State_InkAmbushAttack* Create(void* pArg);
	virtual void Free() override;
};
END	
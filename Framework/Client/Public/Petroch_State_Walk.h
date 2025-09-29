#pragma once

#include "Client_Defines.h"
#include "State.h"

#include "EnemyStateEnum.h"

BEGIN(Client)
class CBoss_Petroch;

class CPetroch_State_Walk final : public CState
{
	using super = CState;
private:
	enum BLOCK_ACTION
	{		
		WALK_FRONT = (_uint)PETROCH_ANIM_INDEX::WALK_FRONT,
		WALK_BACK = (_uint)PETROCH_ANIM_INDEX::WALK_BACK,
		WALK_LEFT = (_uint)PETROCH_ANIM_INDEX::WALK_LEFT,
		WALK_RIGHT = (_uint)PETROCH_ANIM_INDEX::WALK_RIGHT,
		WALK_END = 4
	};

private:
	CPetroch_State_Walk();
	virtual ~CPetroch_State_Walk() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	void Decrease_Duration(_float fTimeDelta);
	void Select_Action();

private:
	CBoss_Petroch* m_pPetroch{ nullptr };
	CTransform* m_pTransform{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr };

private:
	_int			m_iAnimIndex{ 0 };
	_float			m_fCurActDuration{ 0.f };
	_float			m_fCurWalkDuration{ 0.f };
	_float			m_fActDuration{ 0.f };
	_float			m_fWalkDuration{ 5.f };

public:
	static CPetroch_State_Walk* Create(void* pArg);
	virtual void Free() override;
};
END	
#pragma once

#include "Client_Defines.h"
#include "State.h"
#include "EnemyStateEnum.h"

BEGIN(Engine)
class CTransform;
class CMovement;
END

BEGIN(Client)
class CBoss_Petroch;

class CPetroch_State_Block final : public CState
{
	using super = CState;

private:
	enum BLOCK_ACTION
	{
		BLOCK_ENTER = (_uint)PETROCH_ANIM_INDEX::BLOCK_ENTER,
		BLOCK_EXIT = (_uint)PETROCH_ANIM_INDEX::BLOCK_EXIT,
		BLOCK_IDLE = (_uint)PETROCH_ANIM_INDEX::BLOCK_IDLE,
		BLOCK_FORWARD = (_uint)PETROCH_ANIM_INDEX::BLOCK_WALK_FRONT,		
		BLOCK_LEFT = (_uint)PETROCH_ANIM_INDEX::BLOCK_WALK_LEFT,
		BLOCK_RIGHT = (_uint)PETROCH_ANIM_INDEX::BLOCK_WALK_RIGHT,
		BLOCK_END = 5	
	};

private:
	CPetroch_State_Block();
	virtual ~CPetroch_State_Block() = default;

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
	CBoss_Petroch*	m_pPetroch{ nullptr };

private:
	CMovement* m_pMovement = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };

private:	
	_int			m_iAnimIndex{ 0 };	
	_float			m_fCurActDuration{ 0.f };
	_float			m_fCurBlockDuration{ 0.f };
	_float			m_fActDuration{ 0.f };
	_float			m_fBlockDuration{ 10.f };

public:
	static CPetroch_State_Block* Create(void* pArg);
	virtual void Free() override;
};
END	
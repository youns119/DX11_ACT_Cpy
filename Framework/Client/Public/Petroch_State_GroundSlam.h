#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CEffectObject;
END

BEGIN(Client)

class CPetroch_State_GoundSlam final : public CState
{
	using super = CState;

private:
	CPetroch_State_GoundSlam();
	virtual ~CPetroch_State_GoundSlam() = default;

private:
	virtual HRESULT Initialize(void* pObj) override;

public:
	virtual void Enter(void* pObj) override;
	virtual void Update(_float fTimeDelta, void* pObj) override;
	virtual void Exit(void* pObj) override;

private:
	void Call_Effects();
	void Disable_Effects();

private:
	_int	m_iAnimIndex{ 0 };

private:
	CBoss_Petroch*		m_pPetroch{ nullptr };
	CTransform*			m_pTransform{ nullptr };
	const _float4x4*	m_pParentWorldMatrix{ nullptr };
	const _float4x4*	m_pMidLegFootL{ nullptr };
	const _float4x4*	m_pMidLegFootR{ nullptr };
	const _float4x4*	m_pBackLegFootL{ nullptr };
	const _float4x4*	m_pBackLegFootR{ nullptr };

	vector<CEffectObject*> m_EffectObjects;

public:
	static CPetroch_State_GoundSlam* Create(void* pArg);
	virtual void Free() override;
};
END	
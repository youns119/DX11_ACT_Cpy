#pragma once
#include "Client_Defines.h"
#include "Event.h"

BEGIN(Client)
struct FPlayerStat;
class CEvent_AttackOne : public CEvent
{
	using super = CEvent;

private:
	CEvent_AttackOne();
	virtual ~CEvent_AttackOne() = default;

public:
	HRESULT Init(CGameObject* pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pSocketMatrix);

public:
	virtual HRESULT Execute() override;

private:
	const _float4x4* m_pParentWorldMatrix = { nullptr };
	const _float4x4* m_pSocketMatrix = { nullptr };

private:
	FPlayerStat* m_pPlayerStat{};

public:
	static CEvent_AttackOne* Create(CGameObject* pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pSocketMatrix);
	virtual void Free() override;
};

END
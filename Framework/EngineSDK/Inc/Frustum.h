#pragma once

/* 절두체의 정보를 보관할 수 있을꺼야. */
/* 안에 있다 밖에 있다에 대한 판단을 할 ㅅ 있는 기능. */

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	using super = CBase;

private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Update();

	void Transform_LocalSpace(_fmatrix WorldMatrix);
	_bool Is_InWorldSpace(_fvector vWorldPoint, _float fRange = 0.f) const;
	_bool Is_InLocalSpace(_fvector vLocalPoint, _float fRange = 0.f) const;

	const array<_float4, 8>& Get_OriginalPoints() const { return m_OriginalPoints; }
	const array<_float4, 8>& Get_WorldPoints() const { return m_WorldPoints; }

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	//_float4					m_vOriginal_Points[8] = {};
	//_float4					m_vWorld_Points[8] = {};
	//_float4					m_vWorld_Planes[6] = {};
	//_float4					m_vLocal_Planes[6] = {};

	array<_float4, 8>			m_OriginalPoints {};
	array<_float4, 8>			m_WorldPoints {};
	array<_float4, 6>			m_WorldPlanes {};
	array<_float4, 6>			m_LocalPlanes {};


private:
	void Make_Planes(const _float4* pPoints, _float4* pPlanes);
	void Make_Planes(const array<_float4, 8>& Points, _Out_ array<_float4, 6>& pPlanes);

public:
	static CFrustum* Create();
	virtual void Free() override;
};

END
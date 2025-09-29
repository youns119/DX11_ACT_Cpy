#pragma once

#include "Base.h"

BEGIN(Engine)
class CTransform;
class CNavigation;

class CChaser final : public CBase
{
	using super = CBase;

private:
	CChaser() = default;
	virtual ~CChaser() = default;

public:
	HRESULT Initialize(CTransform* pTransform, CNavigation* pNavigation);

	void Start_Chase(_fvector GoalPosition);

private:
	vector<_uint> m_OpenCells;
	vector<_bool> m_visitedCell; // visited

public:
	static CChaser* Create(CTransform* pTransform, CNavigation* pNavigation);
	virtual void Free() override;

};
END

#pragma once

#include "Base.h"

BEGIN(Engine)
/* 청크는 피직스에 접근해서 해당 피직스 액터의 카운트를 증가 혹은 감소 시킨다. */
/* 청크에 접근하는 방법 ? */
class CPhysics;

class CPhysics_Chunk final : public CBase
{
	using super = CBase;

public:
	CPhysics_Chunk() = default;
	virtual ~CPhysics_Chunk() = default;

public:
	void Add_Physics(CPhysics* pPhysics);
	void Add_Count();
	void Sub_Count();

private:
	vector<CPhysics*> m_Physics;

public:
	static CPhysics_Chunk* Create();
	virtual void Free() override;
};
END


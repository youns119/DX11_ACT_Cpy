#pragma once

#include "Base.h"

BEGIN(Engine)
/* ûũ�� �������� �����ؼ� �ش� ������ ������ ī��Ʈ�� ���� Ȥ�� ���� ��Ų��. */
/* ûũ�� �����ϴ� ��� ? */
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


#pragma once

#include "Base.h"

BEGIN(Engine)
class CPhysics;

class CPhysics_ControllerHitCallback final : public PxUserControllerHitReport, public CBase
{
private:
	CPhysics_ControllerHitCallback() = delete;
	CPhysics_ControllerHitCallback(map<PxActor*, CPhysics*>& m_ActorToDynamicPhysicsCom);
	virtual ~CPhysics_ControllerHitCallback() = default;

public:
	void onShapeHit(const PxControllerShapeHit& hit) override;
    void onControllerHit(const PxControllersHit& hit) override {}
    void onObstacleHit(const PxControllerObstacleHit& hit) override {}

private:
	/* �̰� �Ŵ����� ��� �־�� �ϳ�? */
	/* ��۸� �����͵� ó�� */
	/* �Ŵ����� ��� �ִٰ� ���̳��� ������ �������� �� ã�Ƽ� �ʿ��� ������ �� �ֵ��� ó�� Free �Լ����� ���ָ� ���� �ʳ� */
	map<PxActor*, CPhysics*>& m_ActorToDynamicPhysicsCom;

public:
	static CPhysics_ControllerHitCallback* Create(map<PxActor*, CPhysics*>& m_ActorToDynamicPhysicsCom);
	virtual void Free() override;
};
END

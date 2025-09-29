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
	/* 이거 매니저가 들고 있어야 하나? */
	/* 댕글링 포인터들 처리 */
	/* 매니저가 들고 있다가 다이나믹 피직스 릴리즈할 때 찾아서 맵에서 제거할 수 있도록 처리 Free 함수에서 해주면 되지 않나 */
	map<PxActor*, CPhysics*>& m_ActorToDynamicPhysicsCom;

public:
	static CPhysics_ControllerHitCallback* Create(map<PxActor*, CPhysics*>& m_ActorToDynamicPhysicsCom);
	virtual void Free() override;
};
END

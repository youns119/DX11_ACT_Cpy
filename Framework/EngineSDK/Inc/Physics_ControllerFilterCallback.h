#pragma once

#include "Base.h"

BEGIN(Engine)
class CPhysics_ControllerFilterCallback final : public PxControllerFilterCallback, public CBase
{
private:
	CPhysics_ControllerFilterCallback() = default;
	CPhysics_ControllerFilterCallback(const CPhysics_ControllerFilterCallback& rhs) = default;
	virtual ~CPhysics_ControllerFilterCallback() = default;

public:
	virtual bool filter(const PxController& a, const PxController& b);

public:
	static CPhysics_ControllerFilterCallback* Create();
	virtual void Free() override;
};
END

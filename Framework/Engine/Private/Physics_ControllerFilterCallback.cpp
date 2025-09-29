#include "Physics_ControllerFilterCallback.h"

#include "Physics_Controller.h"

bool CPhysics_ControllerFilterCallback::filter(const PxController& a, const PxController& b)
{
	if (static_cast<CPhysics_Controller::USER_DATA*>(a.getUserData())->isGhost)
		return false;

	if (static_cast<CPhysics_Controller::USER_DATA*>(b.getUserData())->isGhost)
		return false;

	return true;
}

CPhysics_ControllerFilterCallback* CPhysics_ControllerFilterCallback::Create()
{
	return new CPhysics_ControllerFilterCallback();
}

void CPhysics_ControllerFilterCallback::Free()
{
	CBase::Free();
}

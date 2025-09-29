#include "Physics_ControllerHitCallback.h"

#include "Physics.h"

CPhysics_ControllerHitCallback::CPhysics_ControllerHitCallback(map<PxActor*, CPhysics*>& ActorToDynamicPhysicsCom)
	: m_ActorToDynamicPhysicsCom { ActorToDynamicPhysicsCom }
{
}

void CPhysics_ControllerHitCallback::onShapeHit(const PxControllerShapeHit& hit)
{
	if (nullptr == hit.actor)
		return;

	auto iter = m_ActorToDynamicPhysicsCom.find(hit.actor);
	if (m_ActorToDynamicPhysicsCom.end() == iter)
		return;

	if (hit.actor->getConcreteType() == PxConcreteType::eRIGID_DYNAMIC)
	{
		/* 댕글링 포인터 관리 안하고 있음 */
		iter->second->Set_Kinematic(false);
	}
}

CPhysics_ControllerHitCallback* CPhysics_ControllerHitCallback::Create(map<PxActor*, CPhysics*>& ActorToDynamicPhysicsCom)
{
	return new CPhysics_ControllerHitCallback(ActorToDynamicPhysicsCom);
}

void CPhysics_ControllerHitCallback::Free()
{
	CBase::Free();
}

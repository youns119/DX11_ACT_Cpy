#include "Phsics_SimulationCallback.h"

void CPhsics_SimulationCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{

}

void CPhsics_SimulationCallback::onWake(PxActor** ppActor, PxU32 iNumActors)
{
}

void CPhsics_SimulationCallback::onSleep(PxActor** ppActor, PxU32 iNumActors)
{
	/* 안쓰기로 마음 먹음 */
}

CPhsics_SimulationCallback* CPhsics_SimulationCallback::Create()
{
	return new CPhsics_SimulationCallback();
}

void CPhsics_SimulationCallback::Free()
{
	CBase::Free();
}

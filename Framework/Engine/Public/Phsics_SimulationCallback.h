#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CPhsics_SimulationCallback final : public PxSimulationEventCallback, public CBase
{
private:
    CPhsics_SimulationCallback() = default;
    virtual ~CPhsics_SimulationCallback() = default;

public:
    // Called when collision starts
    virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;

    // Called when collision stops
    virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override {}

    // Called when actors start or stop touching
    virtual void onConstraintBreak(PxConstraintInfo*, PxU32) override {}
    virtual void onWake(PxActor**, PxU32) override;
    virtual void onSleep(PxActor**, PxU32) override;
    virtual void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) override {}

public:
    static CPhsics_SimulationCallback* Create();
    virtual void Free() override;
};
END

#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class IInteractive_Bounceable abstract
{
protected:
	virtual ~IInteractive_Bounceable() = default;

public:
	virtual _vector Get_OffsetPosition() PURE;
};

END
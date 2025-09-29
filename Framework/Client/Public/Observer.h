#pragma once

#include "Client_Defines.h"

BEGIN(Client)
class IObserver abstract
{
public:
	virtual void On_Notify() = 0;
};
END

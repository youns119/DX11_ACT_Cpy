#pragma once

#include "Client_Defines.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class IParried abstract
{
protected:
	virtual ~IParried() = default;

public:
	virtual void Parried() PURE;

public :
	virtual CGameObject* Get_Parried() PURE;
};

END
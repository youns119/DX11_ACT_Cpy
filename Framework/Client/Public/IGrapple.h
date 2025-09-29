#pragma once

#include "Client_Defines.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class IGrapple abstract
{
protected:
	virtual ~IGrapple() = default;

public:
	virtual _vec3 Get_GrapplePos() PURE;
	virtual _uint Get_Type() PURE;
};

END
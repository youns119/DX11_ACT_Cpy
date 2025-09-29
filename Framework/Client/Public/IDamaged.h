#pragma once

#include "Client_Defines.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class IDamaged abstract
{
public :
	enum class DAMAGE
	{
		DAMAGE_NORMAL,
		DAMAGE_HEAVY,
		DAMAGE_SUPER,

		DAMAGE_END
	};

protected :
	virtual ~IDamaged() = default;

public :
	virtual void Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager) PURE;
};

END
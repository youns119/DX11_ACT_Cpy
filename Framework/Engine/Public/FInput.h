#pragma once

#include "EInputType.h"

BEGIN(Engine)

struct FInput
{
	INPUTTYPE eInputType{};
	_uint iKey{};

	_bool operator==(const FInput& Other) const
	{
		return eInputType == Other.eInputType && iKey == Other.iKey;
	}
};

END
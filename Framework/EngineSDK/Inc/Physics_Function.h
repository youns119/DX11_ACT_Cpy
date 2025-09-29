#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)
static PxExtendedVec3 ConvertExtended(_vec3 vIn)
{
	return PxExtendedVec3 { vIn.x, vIn.y, vIn.z };
}

static _vec3 ConvertExtended(PxExtendedVec3 vIn)
{
	return _vec3 { vIn.x, vIn.y, vIn.z };
}

static PxVec3 Convert(_vec3 vIn)
{
	return PxVec3 { vIn.x, vIn.y, vIn.z };
}

static _vec3 Convert(PxVec3 vIn)
{
	return _vec3 { vIn.x, vIn.y, vIn.z };
}

static PxQuat Convert(_vec4 vIn)
{
	return PxQuat { vIn.x, vIn.y, vIn.z, vIn.w };
}

static _vec4 Convert(PxQuat vIn)
{
	return _vec4 { vIn.x, vIn.y, vIn.z, vIn.w };
}
END

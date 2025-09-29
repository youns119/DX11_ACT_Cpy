#pragma once

BEGIN(Client)

struct FProjectileStat
{
	_float3 vSrc{};
	_float3 vDst{};

	_float fForce{};
	_float fAccel{};
	_float fDecel{};
	_float fForceFriction{};
	_float fMinSpeed{};
	_float fMaxSpeed{};

	_float fDamage{};
	_float fLifeTime{};
};

END
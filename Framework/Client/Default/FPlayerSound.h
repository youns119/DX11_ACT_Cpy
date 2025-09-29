#pragma once

BEGIN(Client)

struct FPlayerSound
{
	_wstring strSound{};
	_uint iChannel{};
	_float fVolume{};
};

END
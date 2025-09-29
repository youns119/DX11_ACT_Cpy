#pragma once

#include "Base.h"

#include "FInput.h"

BEGIN(Engine)

class ENGINE_DLL CInputGate final
	: public CBase
{
	using super = CBase;

public :
	struct FThreshold
	{
		_float fTimeThreshold{};
		_float fTime{};
	};

private:
	CInputGate();
	virtual ~CInputGate() = default;

public:
	void Update(_float fTimeDelta);

public:
	void Add_Input(INPUTTYPE eInputType, _uint iKey, _float fTimeThreshold);
	_bool Has_Input() const { return !m_listInput .empty(); }

public :
	_bool Tap(INPUTTYPE eInputType, _uint iKey);
	_bool Hold(INPUTTYPE eInputType, _uint iKey);

public :
	void Clear();

private:
	list<pair<FInput, FThreshold>> m_listInput;

public:
	static CInputGate* Create();
	virtual void Free() override;
};

END
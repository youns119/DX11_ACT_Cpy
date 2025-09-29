#pragma once

#include "Base.h"

#include "FInput.h"

BEGIN(Engine)

class ENGINE_DLL CInputBuffer final
	: public CBase
{
	using super = CBase;

private:
	CInputBuffer();
	virtual ~CInputBuffer() = default;

public:
	HRESULT Initialize(_float fBufferDuration);
	void Update(_float fTimeDelta);

public:
	void Add_Input(INPUTTYPE eInputType, _uint iKey);
	_bool Has_Input() const { return !m_queInput.empty(); }
	_bool Is_FirstInput(INPUTTYPE eInputType, _uint iKey);

public:
	void Clear();

private:
	queue<pair<FInput, _float>> m_queInput;

private:
	_float m_fBufferTime{};
	_float m_fBufferDuration{};

public:
	static CInputBuffer* Create(_float fBufferDuration);
	virtual void Free() override;
};

END
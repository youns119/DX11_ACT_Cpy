#pragma once

#include "Base.h"

BEGIN(Engine)

class ICloneable abstract
{
private:
	virtual ICloneable* Clone(void* pArg) = 0;
};

// �������̽��� ����� �;����� �츮�� CBase�� �������̽��� �ƴ϶� �߻�Ŭ�����ϱ� �׷��� ���� ������ ���߷��� 
class ENGINE_DLL CCloneable abstract : public CBase
{
protected:
	CCloneable() = default;
	virtual ~CCloneable() = default;

public:
	virtual CCloneable* Clone(void* pArg) = 0;
};

END

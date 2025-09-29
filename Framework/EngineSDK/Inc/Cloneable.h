#pragma once

#include "Base.h"

BEGIN(Engine)

class ICloneable abstract
{
private:
	virtual ICloneable* Clone(void* pArg) = 0;
};

// 인터페이스로 만들고 싶었지만 우리의 CBase는 인터페이스가 아니라 추상클래스니까 그래도 조금 구색을 갖추려고 
class ENGINE_DLL CCloneable abstract : public CBase
{
protected:
	CCloneable() = default;
	virtual ~CCloneable() = default;

public:
	virtual CCloneable* Clone(void* pArg) = 0;
};

END

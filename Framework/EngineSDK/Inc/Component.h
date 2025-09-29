#pragma once

#include "Cloneable.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CCloneable
{
	using super = CCloneable;

protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& Prototype);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Render() { return S_OK; }

protected:
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pContext{ nullptr };
	
	class CGameInstance* m_pGameInstance{ nullptr };

	_bool m_isCloned { false };

public:
	virtual CCloneable* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

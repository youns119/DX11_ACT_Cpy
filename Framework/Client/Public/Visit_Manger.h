#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CVisit_Manger final : public CGameObject
{
	using super = CGameObject;

private:
	CVisit_Manger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVisit_Manger(const CVisit_Manger& Prototype);
	virtual ~CVisit_Manger() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	/* if visited -> false */
	static _bool Is_Visit_FromStatic(const _wstring& strTag);

	_bool Is_Visit(const _wstring& strTag);

private:
	std::set<_wstring> m_VisitedTags;

public:
	static CVisit_Manger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END

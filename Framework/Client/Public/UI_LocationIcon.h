#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Manager;

class CUI_LocationIcon final : public CUI
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUI::DESC UIDesc{};

		const _float4x4* pNPCWorld;
	};

private:
	CUI_LocationIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LocationIcon(const CUI_LocationIcon& Prototype);
	virtual ~CUI_LocationIcon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	const _float4x4* m_pNPCWorld;

	CUI_Manager* m_pUIManager = {nullptr};

public:
	static CUI_LocationIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END

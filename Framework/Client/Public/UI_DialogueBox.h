#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_DialogueBox final : public CUI
{
public:
	struct DESC
	{
		CUI::DESC UIDesc{};
	};

private:
	CUI_DialogueBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_DialogueBox(const CUI_DialogueBox& Prototype);
	virtual ~CUI_DialogueBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Set_DialogueText(const _tchar* szDialogue);

private:
	vector<_wstring> m_vecText;

public:
	static CUI_DialogueBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END

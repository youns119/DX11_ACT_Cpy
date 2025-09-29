#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

struct FPlayerStat;
struct FShellStat;

class CUI_Manager;
class CUI_Hide;
class CUI_Menu;

class CUI_Player final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;

		FPlayerStat* pPlayerStat;
		FShellStat** ppShellStat;
		CGameObject** ppGrappleTarget;
		CGameObject** ppFinishTarget;
	};

private:
	CUI_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Player(const CUI_Player& Prototype);
	virtual ~CUI_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	FPlayerStat* m_pPlayerStat = { nullptr };
	FShellStat** m_ppShellStat = { nullptr };
	CGameObject** m_ppGrappleTarget = {nullptr};
	CGameObject** m_ppFinishTarget = {nullptr};

	CUI_Manager* m_pUIManager = { nullptr };

private:
	HRESULT Ready_PartObject();

public:
	static CUI_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END

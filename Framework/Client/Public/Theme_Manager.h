#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Theme.h"

BEGIN(Client)
class CTheme_Manager final : public CGameObject
{
	using super = CGameObject;

private:
	CTheme_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTheme_Manager(const CTheme_Manager& Prototype);
	virtual ~CTheme_Manager() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	void Set_PendingThemeFilePath(const _wstring& strPendingThemeFilePath);
	_bool Set_Atmospher_With_Pending();

	static void Set_PendingThemeFilePath_FromStatic(const _wstring& strPendingThemeFilePath);
	static _bool Set_Atmospher_With_Pending_FromStatic();
	static void Set_PendingThemeFilePaht_With_Sheleport_FromStatic(_uint iSheleportID);
	static void Load_Theme_With_Sheleport_FromStatic(_uint iSheleportID);

private:
	_wstring m_strPendingFilePath;
	_bool m_isPending { false };

public:
	static CTheme_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END

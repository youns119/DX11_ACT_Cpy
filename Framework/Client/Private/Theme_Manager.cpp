#include "pch.h"
#include "Theme_Manager.h"
#include "Theme.h"
#include "FileData_Locator.h"
#include "FData.h"

CTheme_Manager::CTheme_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CTheme_Manager::CTheme_Manager(const CTheme_Manager& Prototype)
	: super(Prototype)
{
}

HRESULT CTheme_Manager::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTheme_Manager::Initialize(void* pArg)
{
	if (FAILED(super::Initialize(nullptr)))
		return E_FAIL;

	return S_OK;
}

void CTheme_Manager::Set_PendingThemeFilePath(const _wstring& strPendingThemeFilePath)
{
	m_isPending = true;
	m_strPendingFilePath = strPendingThemeFilePath;
}

_bool CTheme_Manager::Set_Atmospher_With_Pending()
{
	if (true == m_isPending)
	{
		m_isPending = false;
		THEME_DESC* pTheme = nullptr;
		if (SUCCEEDED(THEME_DESC::Load_Themes(&pTheme, { m_strPendingFilePath }, 1)))
		{
			pTheme->Set_Atmosphere();
			Safe_Delete(pTheme);
			return true;
		}
	}

	return false;
}

void CTheme_Manager::Set_PendingThemeFilePath_FromStatic(const _wstring& strPendingThemeFilePath)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CTheme_Manager* pThemeManager = static_cast<CTheme_Manager*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_ThemeManager"));

	/* 없으면 만들어 근데 없을리가 없음 ㅋ */
	if (nullptr == pThemeManager)
	{
		if (FAILED(pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_ThemeManager", LEVEL_STATIC, L"Layer_ThemeManager")))
		{
			MSG_BOX("Failed To Add gameObejct to layer");
			return;
		}
		pThemeManager = static_cast<CTheme_Manager*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_ThemeManager"));
	}
		
	pThemeManager->Set_PendingThemeFilePath(strPendingThemeFilePath);
}

_bool CTheme_Manager::Set_Atmospher_With_Pending_FromStatic()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CTheme_Manager* pThemeManager = static_cast<CTheme_Manager*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_ThemeManager"));

	/* 없으면 만들어 */
	if (nullptr == pThemeManager)
	{
		if (FAILED(pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, L"Prototype_GameObject_ThemeManager", LEVEL_STATIC, L"Layer_ThemeManager")))
		{
			MSG_BOX("Failed To Add gameObejct to layer");
			return false;
		}
		pThemeManager = static_cast<CTheme_Manager*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_ThemeManager"));
	}

	return pThemeManager->Set_Atmospher_With_Pending();
}

void CTheme_Manager::Set_PendingThemeFilePaht_With_Sheleport_FromStatic(_uint iSheleportID)
{
	_wstring strThemeFilePath = CFileData_Locator<FSheleportData>::Get_FileData()->Get_Data(0, iSheleportID).szThemeFilePath;

	Set_PendingThemeFilePath_FromStatic(strThemeFilePath);
}

void CTheme_Manager::Load_Theme_With_Sheleport_FromStatic(_uint iSheleportID)
{
	_wstring strThemeFilePath = CFileData_Locator<FSheleportData>::Get_FileData()->Get_Data(0, iSheleportID).szThemeFilePath;

	THEME_DESC* pThemeDesc { nullptr };
	vector<_wstring> FilePaths { strThemeFilePath };
	THEME_DESC::Load_Themes(&pThemeDesc, FilePaths, 1);
	pThemeDesc->Set_Atmosphere();
	Safe_Delete(pThemeDesc);
}

CTheme_Manager* CTheme_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTheme_Manager* pInstance = new CTheme_Manager(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CTheme_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CTheme_Manager::Clone(void* pArg)
{
	CTheme_Manager* pInstance = new CTheme_Manager(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CTheme_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTheme_Manager::Free()
{
	super::Free();
}

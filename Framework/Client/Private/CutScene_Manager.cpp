#include "pch.h"
#include "CutScene_Manager.h"

#include "GameInstance.h"
#include "UI_Manager.h"

CCutScene_Manager::CCutScene_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CCutScene_Manager::CCutScene_Manager(const CCutScene_Manager& Prototype)
	: super(Prototype)
{
}

HRESULT CCutScene_Manager::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCutScene_Manager::Initialize(void* pArg)
{
	CCutScene_Manager::DESC* pDesc = static_cast<DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}	

void CCutScene_Manager::Begin_CutScene(_wstring strCutSceneKey)
{
	m_mapCutScenes.emplace(strCutSceneKey, false);
	m_strCurPlayingScene = strCutSceneKey;

	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
	pUIManager->Show_CutSceneUI();
}

void CCutScene_Manager::End_CutScene(_wstring strCutSceneKey)
{
	m_mapCutScenes[strCutSceneKey] = true;
	m_strCurPlayingScene = L"";

	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));
	pUIManager->Hide_CutSceneUI();
}

CCutScene_Manager* CCutScene_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCutScene_Manager* pInstance = new CCutScene_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCutScene_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCutScene_Manager::Clone(void* pArg)
{
	CCutScene_Manager* pInstance = new CCutScene_Manager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CCutScene_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCutScene_Manager::Free()
{
	super::Free();
}

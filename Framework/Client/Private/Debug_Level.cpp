#include "pch.h"
#include "Debug_Level.h"

#include "GameInstance.h"
#include "Level_Loading.h"

CDebug_Level::CDebug_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CDebug_Level::CDebug_Level(const CDebug_Level& Prototype)
	: super(Prototype)
{
}

HRESULT CDebug_Level::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDebug_Level::Initialize(void* pArg)
{
	return S_OK;
}

void CDebug_Level::Priority_Update(_float fTimeDelta)
{
}

void CDebug_Level::Update(_float fTimeDelta)
{
	ImGui::Begin("Debug_Level");

	if (ImGui::Button("Village"))
	{
		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_VILLAGE));
	}
	if (ImGui::Button("Field"))
	{
		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIELD));
	}
	if (ImGui::Button("PinBall"))
	{
		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_PINBALL));
	}
	if (ImGui::Button("CrabTomb"))
	{
		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CRABTOMB));
	}
	if (ImGui::Button("Beach"))
	{
		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BEACH));
	}
	if (ImGui::Button("Club"))
	{
		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CLUB));
	}
	ImGui::End();
}

void CDebug_Level::Late_Update(_float fTimeDelta)
{
}

HRESULT CDebug_Level::Render()
{
	return S_OK;
}

CDebug_Level* CDebug_Level::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebug_Level* pInstance = new CDebug_Level(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CDebug_Level");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CDebug_Level::Clone(void* pArg)
{
	CDebug_Level* pInstance = new CDebug_Level(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CDebug_Level");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDebug_Level::Free()
{
	super::Free();
}

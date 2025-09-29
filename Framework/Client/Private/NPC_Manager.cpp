#include "pch.h"
#include "NPC_Manager.h"

#include "GameInstance.h"
#include "UI_Manager.h"

CNPC_Manager::CNPC_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CNPC_Manager::CNPC_Manager(const CNPC_Manager& Prototype)
	: super(Prototype)
{
}

HRESULT CNPC_Manager::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Manager::Initialize(void* pArg)
{
	CNPC_Manager::DESC* pDesc = static_cast<DESC*>(pArg);

	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}
_int CNPC_Manager::Load_State(_wstring strNPCKey)
{
	auto it = m_mapState.find(strNPCKey);
	if (it != m_mapState.end())
		return it->second;

	m_mapState[strNPCKey] = 0;
	return m_mapState[strNPCKey];
}

void CNPC_Manager::Save_State(_wstring strNPCKey, _int iState)
{
	auto it = m_mapState.find(strNPCKey);
	if (it == m_mapState.end()) 
		return;
	
	m_mapState[strNPCKey] = iState;
}


CNPC_Manager* CNPC_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNPC_Manager* pInstance = new CNPC_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CNPC_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CNPC_Manager::Clone(void* pArg)
{
	CNPC_Manager* pInstance = new CNPC_Manager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CNPC_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Manager::Free()
{
	super::Free();

	m_mapState.clear();
}

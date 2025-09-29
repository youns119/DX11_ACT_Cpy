#include "Object_Manager.h"

#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"

CObject_Manager::CObject_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	//m_iNumLevels = iNumLevels;

	//m_pLayers = new map<const _wstring, CLayer*>[m_iNumLevels];

	m_pLayers.resize(iNumLevels);
	m_fLagTime = 0.f;
	return S_OK;
}

void CObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_pLayers.size(); i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			_float fTime = fTimeDelta;

			if (m_fLagTime >= 0.f)
			{
				if (m_LagLayers.find(Pair.first) != m_LagLayers.end())
					fTime *= m_fLagRatio;
			}
			Pair.second->Priority_Update(fTime);
		}
	}
}

void CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_pLayers.size(); i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			_float fTime = fTimeDelta;

			if (m_fLagTime >= 0.f)
			{
				if (m_LagLayers.find(Pair.first) != m_LagLayers.end())
					fTime *= m_fLagRatio;
			}
			Pair.second->Update(fTime);
		}
	}
}

void CObject_Manager::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_pLayers.size(); i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			_float fTime = fTimeDelta;

			if (m_fLagTime >= 0.f)
			{
				if (m_LagLayers.find(Pair.first) != m_LagLayers.end())
					fTime *= m_fLagRatio;
			}
			Pair.second->Late_Update(fTime);
		}
	}
}

void CObject_Manager::Final_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_pLayers.size(); i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			_float fTime = fTimeDelta;

			if (m_fLagTime >= 0.f)
			{
				if (m_LagLayers.find(Pair.first) != m_LagLayers.end())
					fTime *= m_fLagRatio;
			}
			Pair.second->Final_Update(fTime);
		}
	}

	if (m_fLagTime >= 0.f)
	{
		if (!m_bLagLoop)
		{
			m_fLagTime -= fTimeDelta;
			if (m_fLagTime <= 0.f)
				Reset_Lag();
		}
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);
	
	m_LagLayers.clear();
	m_pLayers[iLevelIndex].clear();
}

HRESULT CObject_Manager::Add_GameObject_To_Layer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag, pArg));

	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}

	pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

const std::list<CGameObject*>& CObject_Manager::Get_GameObjects_In_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr != pLayer)
		return pLayer->Get_GameObjects();
	else
		return m_EmptyLayer;
}
//
vector<_wstring> CObject_Manager::Get_LayerTags(_uint iLevelIndex) const
{
	vector<_wstring> LayerTags;

	//if (iLevelIndex >= m_pLayers.size())

	for (auto& Pair : m_pLayers[iLevelIndex])
		LayerTags.push_back(Pair.first);

	return LayerTags;
}

CComponent* CObject_Manager::Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Find_Component(strComponentTag, iIndex);
}

CComponent* CObject_Manager::Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPartTag, const _wstring& strComponentTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Find_Component(strPartTag, strComponentTag, iIndex);
}

CComponent* CObject_Manager::Find_Component_In_PartObject(_uint iLevelIndex, const _wstring& strLayerTag, const vector<_wstring>& strPartTags, const _wstring& strComponentTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Find_Component_In_PartObject(strPartTags, strComponentTag, iIndex);
}

void CObject_Manager::Delete_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	
	if (pLayer != nullptr)
		pLayer->Delete_GameObject(iIndex);
}

void CObject_Manager::Delete_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	
	if(pLayer != nullptr)
		pLayer->Delete_GameObject(pGameObject);
}

void CObject_Manager::Delete_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, const vector<_wstring>& strPartTags, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	
	if (pLayer != nullptr)
		pLayer->Delete_GameObject(strPartTags, iIndex);
}

CGameObject* CObject_Manager::Find_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Find_GameObject(iIndex);
}

CGameObject* CObject_Manager::Find_GameObject_With_ID(_uint iLevelIndex, const _uint iID)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
	{
		for (auto& pGameObject : Pair.second->Get_GameObjects())
		{
			if (iID == pGameObject->Get_ID())
				return pGameObject;
		}
	}

	return nullptr;
}

CGameObject* CObject_Manager::Find_GameObject_With_Name(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strName)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Find_GameObject(strName);
}

HRESULT CObject_Manager::Add_Layer_To_Lag(const _wstring& LayerTag)
{
	if (nullptr == Find_Layer(m_pGameInstance->Get_CurrentLevelIndex(), LayerTag))
		return E_FAIL;

	if(m_LagLayers.find(LayerTag) == m_LagLayers.end())
		m_LagLayers.insert(LayerTag);
	
	return S_OK;
}

//_float3 CObject_Manager::Ray_Cast_To_Mesh_In_Layer(_fvector vRayOrigin, _fvector vRayDirection, _uint iLevelIndex, const _wstring& LayerTag)
//{
//	const list<CGameObject*>& GameObjects = Find_Layer(iLevelIndex, LayerTag)->Get_GameObjects();
//
//	for (auto& pGameObject : GameObjects)
//	{
//		CVIBuffer* pVIBuffer = nullptr;
//		pVIBuffer = static_cast<CVIBuffer*>(pGameObject->Find_Component(L"Com_VI_Buffer"));
//		if (nullptr != pVIBuffer)
//		{
//			const vector<VTXMESH>& Vertices = pVIBuffer->Get_VertexData();
//			const vector<_uint>& Indices = pVIBuffer->Get_IndexData();
//
//			_float fDist = 0;
//
//			for (size_t i = 0; i < Indices.size(); i+=3)
//			{
//				_vector v0 = XMLoadFloat3(&Vertices[Indices[i  ]].vPosition);
//				_vector v1 = XMLoadFloat3(&Vertices[Indices[i+1]].vPosition);
//				_vector v2 = XMLoadFloat3(&Vertices[Indices[i+2]].vPosition);
//
//				TriangleTests::Intersects(vRayOrigin, vRayDirection, v0, v1, v2, fDist);
//			}
//		}
//
//		pVIBuffer = static_cast<CVIBuffer*>(pGameObject->Find_Component(L"Com_Model"));
//		if (nullptr != pVIBuffer)
//		{
//
//		}
//	}
//}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	auto iter = m_pLayers[iLevelIndex].find(strLayerTag);

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CObject_Manager* CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed To Created : CObject_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Manager::Free()
{
	CBase::Free();

	//for (size_t i = 0; i < m_iNumLevels; ++i)
	for (auto& pLayers : m_pLayers)
	{
		for (auto& Pair : pLayers)
			Safe_Release(Pair.second);

		pLayers.clear();
	}
	//Safe_Delete_Array(m_pLayers);

	Safe_Release(m_pGameInstance);
}

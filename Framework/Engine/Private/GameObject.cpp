#include "GameObject.h"

#include "GameInstance.h"
#include "Event.h"

_uint CGameObject::s_iID = 0;

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	
	Safe_AddRef(m_pGameInstance);
}

CGameObject::CGameObject(const CGameObject& Prototype)
	: m_pDevice{ Prototype.m_pDevice }
	, m_pContext{ Prototype.m_pContext }
	, m_pGameInstance{ Prototype.m_pGameInstance }
	, m_CombinedWorldMatrix { Prototype.m_CombinedWorldMatrix }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	Safe_AddRef(m_pGameInstance);
}

HRESULT CGameObject::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg) // Create TransformCom (if nullptr : default tranform value, else fill with pArg)
{
	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
	CTransform::TRANSFORM_DESC* pTransformDesc{ nullptr };
	if (nullptr != pDesc)
	{
		pTransformDesc = &pDesc->TransformDesc;
		m_strName = pDesc->strName; // 이거 왜 주석처리했었지?
		m_pParentWorldMatrix = pDesc->pParentWorldMatrix;
	}

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Initialize(pTransformDesc)))
		return E_FAIL;

	m_Components.emplace(g_strTransformTag, m_pTransformCom);
	Safe_AddRef(m_pTransformCom);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	m_iID = s_iID;
	++s_iID;
	return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
		{
			if (m_bTimeLag && m_pGameInstance->Get_IsLag())
			{
				if (Pair.second->m_bTimeLag)
					Pair.second->Priority_Update(fTimeDelta);
				else Pair.second->Priority_Update(fTimeDelta / m_pGameInstance->Get_LagRatio());
			}
			else Pair.second->Priority_Update(fTimeDelta);
		}
	}
}

void CGameObject::Update(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
		{
			if (m_bTimeLag && m_pGameInstance->Get_IsLag())
			{
				if (Pair.second->m_bTimeLag)
					Pair.second->Update(fTimeDelta);
				else Pair.second->Update(fTimeDelta / m_pGameInstance->Get_LagRatio());
			}
			else Pair.second->Update(fTimeDelta);
		}
	}
}

void CGameObject::Late_Update(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
		{
			if (m_bTimeLag && m_pGameInstance->Get_IsLag())
			{
				if (Pair.second->m_bTimeLag)
					Pair.second->Late_Update(fTimeDelta);
				else Pair.second->Late_Update(fTimeDelta / m_pGameInstance->Get_LagRatio());
			}
			else Pair.second->Late_Update(fTimeDelta);
		}
	}
}

void CGameObject::Final_Update(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
		{
			if (m_bTimeLag && m_pGameInstance->Get_IsLag())
			{
				if (Pair.second->m_bTimeLag)
					Pair.second->Final_Update(fTimeDelta);
				else Pair.second->Final_Update(fTimeDelta / m_pGameInstance->Get_LagRatio());
			}
			else Pair.second->Final_Update(fTimeDelta);
		}
	}
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

CComponent* CGameObject::Find_Component(const _wstring& strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Delete_PartObject(const vector<_wstring>& strPartTags, _uint iDepth)
{
	auto iter = m_PartObjects.find(strPartTags[iDepth]);
	if (iter == m_PartObjects.end())
		return;

	if ((_uint)strPartTags.size() == iDepth + 1) // 진짜 지울 놈의 바로 위
	{
		(iter->second)->Set_IsDead();
		Safe_Release(iter->second);
		m_PartObjects.erase(iter);
	}
	else
		iter->second->Delete_PartObject(strPartTags, iDepth + 1);
}

void CGameObject::Save_GameObject(HANDLE hFile)
{
	// 위치? 정보 말고 저장할게 있을까? 공통적으로 -> 그래서 world matrix만 저장할게
	DWORD dwByte {};
	WriteFile(hFile, &m_pTransformCom->Get_WorldMatrix_Ref(), sizeof(_float4x4), &dwByte, nullptr);
}

CComponent* CGameObject::Find_Component_In_PartObject(const vector<_wstring>& strPartTags, const _wstring& strComponentTag)
{
	CGameObject* pPartObject = Find_PartObject(strPartTags);

	return pPartObject->Find_Component(strComponentTag);
}

vector<pair<_float, CGameObject*>> CGameObject::Ray_Cast_To_Collider(_fvector RayOrigin, _fvector RayDirection, _float fRayLength)
{
	vector<pair<_float, CGameObject*>> Hits;

	CCollider* pCollider = static_cast<CCollider*>(Find_Component(L"Com_Collider"));
	if (nullptr != pCollider)
	{
		_float fDist = 0.f;
		if (pCollider->Ray_Cast(RayOrigin, RayDirection, fRayLength, fDist))
			Hits.push_back({ fDist, this });
	}

	for (auto& Pair : m_PartObjects)
	{
		// C++11 이상에서는 RVO가 적용되어 std::move 하지 않아도 최적화가 된다한다.
		auto PartHits = Pair.second->Ray_Cast_To_Collider(RayOrigin, RayDirection, fRayLength); 

		Hits.insert(Hits.end(), PartHits.begin(), PartHits.end());
		//Hits.insert(Hits.end(), std::make_move_iterator(PartHits.begin()), std::make_move_iterator(PartHits.end()));
		PartHits.clear();
	}

	return Hits;
}

vector<pair<CGameObject*, CCollider*>> CGameObject::Intersect_Collider(CCollider* pOtherCollider)
{
	vector<pair<CGameObject*, CCollider*>> Hits;

	CCollider* pCollider = static_cast<CCollider*>(Find_Component(L"Com_Collider"));

	if (nullptr != pCollider)
	{
		if (pCollider->Intersect(pOtherCollider))
			Hits.push_back({ this, pCollider });
	}

	for (auto& Pair : m_PartObjects)
	{
		// C++11 이상에서는 RVO가 적용되어 std::move 하지 않아도 최적화가 된다한다.
		vector<pair<CGameObject*, CCollider*>> PartHits = Pair.second->Intersect_Collider(pOtherCollider);

		Hits.insert(Hits.end(), PartHits.begin(), PartHits.end());
		//Hits.insert(Hits.end(), std::make_move_iterator(PartHits.begin()), std::make_move_iterator(PartHits.end()));
		PartHits.clear();
	}

	return Hits;

}

_float CGameObject::Calculate_Distance_From(_vector vPosition)
{
	return XMVectorGetX(XMVector3Length(vPosition - m_pTransformCom->Get_State(CTransform::STATE::POSITION)));
}


HRESULT CGameObject::Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
	CCloneable* pClone = (m_pGameInstance->Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag, pArg));
	CComponent* pComponent = dynamic_cast<CComponent*>(pClone);
	if (nullptr == pComponent)
		return E_FAIL;

	if (nullptr != Find_Component(strComponentTag))
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);
	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CGameObject::Delete_Component(const _wstring& strComponentTag, CComponent** ppOut)
{
	CComponent* pComponent = Find_Component(strComponentTag);
	if (pComponent == nullptr)
		return E_FAIL;

	Safe_Release(pComponent);
	m_Components.erase(strComponentTag);

	*ppOut = pComponent;

	return S_OK;
}

HRESULT CGameObject::Bind_ObjectID_To_Shader(CShader* pShader, const _char* pConstantName) const
{
	return pShader->Bind_RawValue(pConstantName, &m_iID, sizeof(m_iID));
}

HRESULT CGameObject::Add_PartObject(const _wstring& strPartTag, _uint iPrototypeLevelID, const _wstring& strPrototypeTag, void* pArg)
{
	auto iter = m_PartObjects.find(strPartTag);
	if (iter != m_PartObjects.end())
		return E_FAIL;

	CGameObject* pPartObject = nullptr;
	pPartObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(iPrototypeLevelID, strPrototypeTag, pArg));
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PartObjects.emplace(strPartTag, pPartObject);

	return S_OK;
}

CGameObject* CGameObject::Find_PartObject(const vector<_wstring>& strPartTags, _uint iDepth)
{
	if ((_uint)strPartTags.size() == iDepth)
		return this;

	auto iter = m_PartObjects.find(strPartTags[iDepth]);
	if (iter == m_PartObjects.end())
		return nullptr;

	return iter->second->Find_PartObject(strPartTags, iDepth + 1);
}

HRESULT CGameObject::Call_Event(const _char* szEventTag)
{
	string strEventTag = string(szEventTag);
	_wstring strEvent = CUtils::ConvertToWideChar(strEventTag);

	CEvent* pEvent = Find_Event(strEvent);
	if (pEvent == nullptr)
		return S_OK;

	if (FAILED(pEvent->Execute()))
		return E_FAIL;

	return S_OK;
}

void CGameObject::Compute_CamDistance()
{
	_vector vCamPos = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());
	_vector vWorldPos = m_pTransformCom->Get_State(CTransform::STATE::POSITION);

	m_fCamDistance = XMVectorGetX(XMVector3Length(vCamPos - vWorldPos));
}

HRESULT CGameObject::Add_Event(const _wstring& strEventTag, CEvent* pEvent)
{
	if (pEvent == nullptr)
		return E_FAIL;

	CEvent* pFindEvent = Find_Event(strEventTag);
	if (pFindEvent != nullptr)
	{
		Safe_Release(pEvent);
		return S_OK;
	}

	m_mapEvent.emplace(strEventTag, pEvent);

	return S_OK;
}

CEvent* CGameObject::Find_Event(const _wstring& _strEventTag)
{
	auto iter = m_mapEvent.find(_strEventTag);
	if (iter == m_mapEvent.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	super::Free();

	for (auto& Pair : m_mapEvent)
		Safe_Release(Pair.second);
	m_mapEvent.clear();

	for (auto& Pair : m_PartObjects)
		Safe_Release(Pair.second);
	m_PartObjects.clear();

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pTransformCom);

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}


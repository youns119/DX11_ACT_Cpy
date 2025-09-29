#include "Effect_Manager.h"
#include "GameInstance.h"
#include "EffectObject.h"

CEffect_Manager::CEffect_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffect_Manager::Initialize()
{
	m_fLagTime = 0.f;
	return S_OK;
}

void CEffect_Manager::Priority_Update(_float fTimeDelta)
{
	_float fTime = fTimeDelta;
	for (auto& Pair : m_Effects)
	{
		for (auto& Effect : Pair.second)
		{
			if (true == Effect->Get_IsActive())
			{
				if (m_fLagTime > 0.f)
					fTime *= m_fLagRatio;
				Effect->Priority_Update(fTime);
			}
		}
	}
}

void CEffect_Manager::Update(_float fTimeDelta)
{
	_float fTime = fTimeDelta;
	for (auto& Pair : m_Effects)
	{
		for (auto& Effect : Pair.second)
		{
			if (true == Effect->Get_IsActive())
			{
				if (m_fLagTime > 0.f)
					fTime *= m_fLagRatio;
				Effect->Update(fTime);
			}
		}
	}
}

void CEffect_Manager::Late_Update(_float fTimeDelta)
{
	_float fTime = fTimeDelta;
	for (auto& Pair : m_Effects)
	{
		for (auto& Effect : Pair.second)
		{
			if (true == Effect->Get_IsActive())
			{
				if (m_fLagTime > 0.f)
				{
					m_fLagTime -= fTimeDelta;
					fTime *= m_fLagRatio;
				}
				Effect->Late_Update(fTime);
			}
		}
	}
}

void CEffect_Manager::Clear()
{
	for (auto& pEffectPair : m_Effects)
	{
		for (auto& Effect : pEffectPair.second)
			Safe_Release(Effect);

		pEffectPair.second.clear();
	}
	m_Effects.clear();
}

CEffectObject* CEffect_Manager::Call_Effect(_uint iEffectType, _fvector vStartPos, _fvector vLookAt, _bool bIsPartObject, const _float4x4* pAdoptedWorldMatrix, _float fLookRotateDegree, _bool bFlip, const _float4x4* pParentWorldMatrix)
{
	if (m_Effects.find(iEffectType) == m_Effects.end())
		return nullptr;

	auto& Effects = m_Effects.find(iEffectType)->second;

	for (auto& Effect : Effects)
	{
		if (false == Effect->Get_IsActive())
		{
			Effect->Set_IsActive(true);
			CTransform* pEffectTransform = static_cast<CTransform*>(Effect->Find_Component(g_strTransformTag));
			pEffectTransform->Locate(_float3(0.f, 0.f, 0.f));
			pEffectTransform->LookAt(vLookAt);

			_float fRotateDegree = fLookRotateDegree;
			if (true == bFlip)
				fRotateDegree += 180.f;
			pEffectTransform->Rotate_By_State(CTransform::STATE::LOOK, XMConvertToRadians(fRotateDegree));

			pEffectTransform->Set_State(CTransform::STATE::POSITION, vStartPos);
			if (true == bIsPartObject)
				Effect->Set_AdoptedMatrix(pAdoptedWorldMatrix);
			if (pParentWorldMatrix != nullptr)
				Effect->Set_ParentWorldMatrix(pParentWorldMatrix);
			return Effect;
		}
	}
	return nullptr;
}

HRESULT CEffect_Manager::Pool_Effect(_uint iEffectType, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iEffectNum, void* pArg)
{
	if (m_Effects.find(iEffectType) != m_Effects.end())
		return E_FAIL;

	vector<CEffectObject*> Effects;
	CEffectObject* pEffectObject = { nullptr };

	Effects.reserve(iEffectNum);
	for (size_t i = 0; i < iEffectNum; i++)
	{
		pEffectObject = static_cast<CEffectObject*>(m_pGameInstance->Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag, pArg));
		if (nullptr == pEffectObject)
			return E_FAIL;
		pEffectObject->Set_IsActive(false);
		Effects.push_back(pEffectObject);
	}

	m_Effects[iEffectType] = Effects;
	return S_OK;
}

CEffect_Manager* CEffect_Manager::Create()
{
	CEffect_Manager* pInstance = new CEffect_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CEffect_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Manager::Free()
{
	super::Free();

	for (auto& pEffectPair : m_Effects)
	{
		for (auto& Effect : pEffectPair.second)
			Safe_Release(Effect);

		pEffectPair.second.clear();
	}
	m_Effects.clear();

	Safe_Release(m_pGameInstance);
}

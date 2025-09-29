#include "pch.h"
#include "Event_SandBurst.h"
#include "GameInstance.h"
#include "Player.h"

CEvent_SandBurst::CEvent_SandBurst()
{
}

HRESULT CEvent_SandBurst::Init(CGameObject* _pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pSocketMatrix)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pParentWorldMatrix = pParentWorldMatrix;
	m_pSocketMatrix = pSocketMatrix;

	return S_OK;
} 

HRESULT CEvent_SandBurst::Execute()
{
	_matrix CombinedMatrix{}, SocketMatrix{};
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	for (size_t i = 0; i < 3; ++i)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	CombinedMatrix = SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PTCL_SANDBURST, CombinedMatrix.r[(_uint)CTransform::STATE::POSITION] + vUp * 0.25f, XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);
	return S_OK;
}

CEvent_SandBurst* CEvent_SandBurst::Create(CGameObject* _pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pSocketMatrix)
{
	CEvent_SandBurst* pInstance = new CEvent_SandBurst;

	if (FAILED(pInstance->Init(_pGameObject, pParentWorldMatrix, pSocketMatrix)))
	{
		MSG_BOX("Failed To Create : CEvent_SandBurst");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_SandBurst::Free()
{
	super::Free();
}
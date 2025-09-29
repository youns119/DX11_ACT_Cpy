#include "pch.h"
#include "Event_SwimBubble.h"
#include "GameInstance.h"
#include "Player.h"

CEvent_SwimBubble::CEvent_SwimBubble()
{
}

HRESULT CEvent_SwimBubble::Init(CGameObject* _pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pSocketMatrix)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pParentWorldMatrix = pParentWorldMatrix;
	m_pSocketMatrix = pSocketMatrix;

	return S_OK;
}

HRESULT CEvent_SwimBubble::Execute()
{
	_matrix CombinedMatrix{}, SocketMatrix{};

	SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	for (size_t i = 0; i < 3; ++i)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	
	CombinedMatrix = SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_SWIMBUBBLE, CombinedMatrix.r[(_uint)CTransform::STATE::POSITION], XMVectorSet(0.f, 0.f, 1.f, 0.f), false, nullptr);
	return S_OK;
}

CEvent_SwimBubble* CEvent_SwimBubble::Create(CGameObject* _pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pSocketMatrix)
{
	CEvent_SwimBubble* pInstance = new CEvent_SwimBubble;

	if (FAILED(pInstance->Init(_pGameObject, pParentWorldMatrix, pSocketMatrix)))
	{
		MSG_BOX("Failed To Create : CEvent_SwimBubble");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_SwimBubble::Free()
{
	super::Free();
}
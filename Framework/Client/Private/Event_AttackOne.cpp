#include "pch.h"
#include "Event_AttackOne.h"
#include "GameInstance.h"
#include "Player.h"

CEvent_AttackOne::CEvent_AttackOne()
{
}

HRESULT CEvent_AttackOne::Init(CGameObject* _pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pSocketMatrix)
{
	if (FAILED(super::Init(_pGameObject)))
		return E_FAIL;

	m_pParentWorldMatrix = pParentWorldMatrix;
	m_pSocketMatrix = pSocketMatrix;

	m_pPlayerStat = static_cast<CPlayer*>(_pGameObject)->Get_PlayerStat();

	return S_OK;
}

HRESULT CEvent_AttackOne::Execute()
{
	_matrix CombinedMatrix{}, SocketMatrix{}, ParentMatrix{};
	_vector vLook{}, vUp{};

	SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	for (size_t i = 0; i < 3; ++i)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	ParentMatrix = XMLoadFloat4x4(m_pParentWorldMatrix);
	CombinedMatrix = SocketMatrix * ParentMatrix;
	vLook = ParentMatrix.r[(_uint)CTransform::STATE::LOOK];
	vUp = ParentMatrix.r[(_uint)CTransform::STATE::UP];
	switch (m_pPlayerStat->iComboCount)
	{
	case 1:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_ATT00, ParentMatrix.r[(_uint)CTransform::STATE::POSITION] + vUp + vLook, -vLook, false, nullptr, 10.f);
		break;
	case 2:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_ATT00, ParentMatrix.r[(_uint)CTransform::STATE::POSITION] + vUp * 0.5f + vLook, -vLook, false, nullptr, 12.5f, true);
		break;
	case 3:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_ATT00, ParentMatrix.r[(_uint)CTransform::STATE::POSITION] + vUp + vLook, -vLook, false, nullptr, 10.f);
		break;
	case 4:
		CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_MESH_ATT00, ParentMatrix.r[(_uint)CTransform::STATE::POSITION] + vUp * 0.5f + vLook, -vLook, false, nullptr, -12.5f, true);
		break;
	default:
		break;
	}
	

	// 스프링암에 타겟이 설정 되었을 시 해당 타겟을 바라보도록 할 것(위치 지정 후 룩앳)
	
	return S_OK;
}

CEvent_AttackOne* CEvent_AttackOne::Create(CGameObject* _pGameObject, const _float4x4* pParentWorldMatrix, const _float4x4* pSocketMatrix)
{
	CEvent_AttackOne* pInstance = new CEvent_AttackOne;

	if (FAILED(pInstance->Init(_pGameObject, pParentWorldMatrix, pSocketMatrix)))
	{
		MSG_BOX("Failed To Create : CEvent_AttackOne");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_AttackOne::Free()
{
	super::Free();
}
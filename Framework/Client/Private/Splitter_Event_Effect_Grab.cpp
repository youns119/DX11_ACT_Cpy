#include "pch.h"
#include "Splitter_Event_Effect_Grab.h"
#include "GameInstance.h"
#include "Enemy_Splitter.h"


CSplitter_Event_Effect_Grab::CSplitter_Event_Effect_Grab(CGameObject* _pGameObject)
{
	m_pModel = static_cast<CModel*>(_pGameObject->Find_Component(TEXT("Com_Model")));
	m_pTransform = static_cast<CTransform*>(_pGameObject->Find_Component(g_strTransformTag));
}


HRESULT CSplitter_Event_Effect_Grab::Execute()
{
	_matrix BoneMatrix = XMLoadFloat4x4(&m_pModel->Get_BoneCombinedMatrix_By_Name("FXP_Grab.90"));
	_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();

	_vector vPos = (BoneMatrix * WorldMatrix).r[(_uint)CTransform::STATE::POSITION];
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	// _vector vUp = m_pTransform->Get_State(CTransform::STATE::UP) * 0.25f;
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::SPLITR_MESH_GRAB, vPos + vLook * 0.5f, vLook, false, nullptr, -7.5f, true);

	return S_OK;
}

CSplitter_Event_Effect_Grab* CSplitter_Event_Effect_Grab::Create(CGameObject* _pGameObject)
{
	CSplitter_Event_Effect_Grab* pInstance = new CSplitter_Event_Effect_Grab(_pGameObject);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CSplitter_Event_Effect_Grab");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSplitter_Event_Effect_Grab::Free()
{
	__super::Free();
}
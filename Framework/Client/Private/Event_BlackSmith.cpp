#include "pch.h"
#include "Event_BlackSmith.h"
#include "GameInstance.h"
#include "GameObject.h"

CEvent_BlackSmith::CEvent_BlackSmith(CGameObject* _pGameObject)
{
	CModel* pModel = static_cast<CModel*>(_pGameObject->Find_Component(L"Com_Model"));
	m_pSocketMatrix = &pModel->Get_BoneCombinedMatrix_By_Name("Knob2.19_end");
}


HRESULT CEvent_BlackSmith::Execute()
{
	CTransform* m_pTransform = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	_vector vEffectLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) }, vPos{};
	vPos = (XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(&m_pTransform->Get_WorldMatrix_Ref())).r[(_uint)CTransform::STATE::POSITION];
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->Call_Effect((_uint)EFFECT_TYPE::PTCL_HITSPARK, vPos, vEffectLook, false, nullptr);
	pGameInstance->Play_Sound(L"BlackSmith.wav", (_uint)SOUND_CHANNEL::NPC, 0.f);

	return S_OK;
}

CEvent_BlackSmith* CEvent_BlackSmith::Create(CGameObject* _pGameObject)
{
	CEvent_BlackSmith* pInstance = new CEvent_BlackSmith(_pGameObject);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEvent_BlackSmith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_BlackSmith::Free()
{
	__super::Free();
}

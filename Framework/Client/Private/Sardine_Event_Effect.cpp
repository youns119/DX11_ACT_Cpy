#include "pch.h"
#include "Sardine_Event_Effect.h"
#include "GameInstance.h"
#include "GameObject.h"

CSardine_Event_Effect::CSardine_Event_Effect(CGameObject* _pGameObject)
{
	CModel* pModelCom = static_cast<CModel*>(_pGameObject->Find_Component(L"Com_Model"));
	m_pGameObject = _pGameObject;
	m_pParentWorldMatrix = &static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag))->Get_WorldMatrix_Ref();
	m_pTailMatrix = &pModelCom->Get_BoneCombinedMatrix_By_Name("Tail_004.18_end");
}

HRESULT CSardine_Event_Effect::Execute()
{
	_vector vPos{}, vUp{}, vLook{}, vCamPos{}, vCamLook{}, vZeroPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) }, vZeroLook{ XMVectorSet(0.f, 0.f, 1.f, 0.f) };
	CTransform* pTransformCom = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));
	vPos = (XMLoadFloat4x4(m_pTailMatrix) * XMLoadFloat4x4(m_pParentWorldMatrix)).r[(_uint)CTransform::STATE::POSITION];
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::PLAYER_PTCL_JUMPBUBBLE, vPos, vZeroLook, false, nullptr);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::BOWMAN_PTCL_CLAW, vPos, vZeroLook, false, nullptr);
	vCamPos = XMLoadFloat4(&CGameInstance::GetInstance()->Get_CamPosition());
	vCamLook = XMVectorSetW(XMVector3Normalize(vCamPos - vPos), 0.f);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::MESH_RADIALDISTORTION, vPos + vCamLook, vCamLook, false, nullptr);
	return S_OK;
}

CSardine_Event_Effect* CSardine_Event_Effect::Create(CGameObject* _pGameObject)
{
	CSardine_Event_Effect* pInstance = new CSardine_Event_Effect(_pGameObject);

	return pInstance;
}

void CSardine_Event_Effect::Free()
{
	__super::Free();
}

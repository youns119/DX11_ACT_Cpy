#include "pch.h"
#include "Inkerton_Event_InkSpurt.h"
#include "GameInstance.h"
#include "Boss_Inkerton.h"
#include "Animation.h"
#include "Inkerton_Gun.h"

CInkerton_Event_InkSpurt::CInkerton_Event_InkSpurt()	
{
}

HRESULT CInkerton_Event_InkSpurt::Execute()
{		
	_vector vPos = static_cast<CTransform*>(m_pGameObject->Find_Component(L"Com_Transform"))->Get_State(CTransform::STATE::POSITION);
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	CGameInstance::GetInstance()->Call_Effect((_uint)EFFECT_TYPE::ENEMY_SPRITE_SQUID, vPos, vLook, false, nullptr);

	return S_OK;
}

CInkerton_Event_InkSpurt* CInkerton_Event_InkSpurt::Create(CGameObject* _pGameObject)
{
	CInkerton_Event_InkSpurt* pInstance = new CInkerton_Event_InkSpurt();

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CInkerton_Event_InkSpurt");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInkerton_Event_InkSpurt::Free()
{
	__super::Free();
}

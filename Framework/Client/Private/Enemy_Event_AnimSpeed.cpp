#include "pch.h"
#include "Enemy_Event_AnimSpeed.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Animation.h"

CEnemy_Event_AnimSpeed::CEnemy_Event_AnimSpeed(_float _fSpeed)
	: m_fSpeed(_fSpeed)	
{
}

HRESULT CEnemy_Event_AnimSpeed::Execute()
{	
	static_cast<CModel*>(m_pGameObject->Find_Component(TEXT("Com_Model")))->Get_CurrAnimation()->Set_BaseAnimSpeed(m_fSpeed);
	return S_OK;
}

CEnemy_Event_AnimSpeed* CEnemy_Event_AnimSpeed::Create(CGameObject* _pGameObject, _float _fSpeed)
{
	CEnemy_Event_AnimSpeed* pInstance = new CEnemy_Event_AnimSpeed(_fSpeed);

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CEnemy_Event_AnimSpeed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_Event_AnimSpeed::Free()
{
	__super::Free();
}

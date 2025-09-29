#include "pch.h"
#include "TestEvent2.h"

#include "GameObject.h"
#include "Transform.h"

CTestEvent2::CTestEvent2()
{
}

HRESULT CTestEvent2::Execute()
{
	CTransform* pTransformCom = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));

	pTransformCom->Set_Scale(_float3(1.f, 1.f, 1.f));

	return S_OK;
}

CTestEvent2* CTestEvent2::Create(CGameObject* _pGameObject)
{
	CTestEvent2* pInstance = new CTestEvent2;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CTestEvent2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestEvent2::Free()
{
	__super::Free();
}
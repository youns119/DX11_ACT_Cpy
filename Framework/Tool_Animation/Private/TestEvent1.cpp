#include "pch.h"
#include "TestEvent1.h"

#include "GameObject.h"
#include "Transform.h"

CTestEvent1::CTestEvent1()
{
}

HRESULT CTestEvent1::Execute()
{
	CTransform* pTransformCom = static_cast<CTransform*>(m_pGameObject->Find_Component(g_strTransformTag));

	pTransformCom->Set_Scale(_float3(2.f, 2.f, 2.f));

	return S_OK;
}

CTestEvent1* CTestEvent1::Create(CGameObject* _pGameObject)
{
	CTestEvent1* pInstance = new CTestEvent1;

	if (FAILED(pInstance->Init(_pGameObject)))
	{
		MSG_BOX("Failed To Create : CTestEvent1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestEvent1::Free()
{
	__super::Free();
}
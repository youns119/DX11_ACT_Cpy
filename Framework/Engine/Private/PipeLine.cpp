#include "PipeLine.h"

CPipeLine::CPipeLine()
{
}

HRESULT CPipeLine::Initialize()
{
	for (_uint i = 0; i < (_uint)TRANSFORM::MAX; ++i)
	{
		XMStoreFloat4x4(&m_TransformationMatrices[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformationInverseMatrices[i], XMMatrixIdentity());
	}

	m_vCamPosition = _float4(0.f, 0.f, 0.f, 1.f);

	return S_OK;
}

void CPipeLine::Update()
{
	for (_uint i = 0; i < (_uint)TRANSFORM::MAX; ++i)
	{
		XMStoreFloat4x4(&m_TransformationInverseMatrices[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformationMatrices[i])));
	}

	//_vector vCamPosition = XMLoadFloat4x4(&m_TransformationInverseMatrices[(_uint)TRANSFORM::VIEW]).r[3];
	//XMStoreFloat4(&m_vCamPosition, vCamPosition);

	//memcpy(&m_vCamPosition, &m_TransformationInverseMatrices[(_uint)TRANSFORM::VIEW], sizeof(_float4));
	XMStoreFloat4(&m_vCamPosition, XMLoadFloat4x4(&m_TransformationInverseMatrices[(_uint)TRANSFORM::VIEW]).r[3]);
}

CPipeLine* CPipeLine::Create()
{
	CPipeLine* pInstance = new CPipeLine();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPipeLine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPipeLine::Free()
{
	super::Free();
}

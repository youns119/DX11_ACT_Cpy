#include "PartObject.h"

CPartObject::CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CPartObject::CPartObject(const CPartObject& Prototype)
	: super(Prototype)
	, m_CombinedWorldMatrix { Prototype.m_CombinedWorldMatrix }
{
}

HRESULT CPartObject::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CPartObject::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	m_pParentWorldMatrix = pDesc->pParentWorldMatrix;

	if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
		return E_FAIL;

	return S_OK;
}

void CPartObject::Priority_Update(_float fTimeDelta)
{
}

void CPartObject::Update(_float fTimeDelta)
{
}

void CPartObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
	return S_OK;
}

void CPartObject::Free()
{
	super::Free();
}

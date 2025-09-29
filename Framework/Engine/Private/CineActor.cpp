#include "CineActor.h"

#include "GameInstance.h"

CCineActor::CCineActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super{ pDevice, pContext }
{
}

CCineActor::CCineActor(const CCineActor& Prototype)
	: super(Prototype)
{
}

HRESULT CCineActor::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCineActor::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	m_tCineActor.ePathType = pDesc->tCineActor.ePathType;
	m_tCineActor.fTime = pDesc->tCineActor.fTime;
	m_tCineActor.fLerp = pDesc->tCineActor.fLerp;

	m_tCineActor.eLerpType = pDesc->tCineActor.eLerpType;

	m_tCineActor.vBazierControl = pDesc->tCineActor.vBazierControl;

	m_tCineActor.vCatmullRomControlStart = pDesc->tCineActor.vCatmullRomControlStart;
	m_tCineActor.vCatmullRomControlEnd = pDesc->tCineActor.vCatmullRomControlEnd;

	if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Locate(pDesc->vPosition);
	m_pTransformCom->Rotate(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);

	return S_OK;
}

void CCineActor::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CCineActor::Update(_float fTimeDelta)
{
	super::Update(fTimeDelta);
}

void CCineActor::Late_Update(_float fTimeDelta)
{
	super::Late_Update(fTimeDelta);
}

void CCineActor::Final_Update(_float fTimeDelta)
{
	super::Final_Update(fTimeDelta);
}

HRESULT CCineActor::Render()
{
	if (FAILED(super::Render()))
		return E_FAIL;

	return S_OK;
}

CCineActor* CCineActor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCineActor* pInstance = new CCineActor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CCineActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CCineActor::Clone(void* pArg)
{
	return nullptr;
}

void CCineActor::Free()
{
	super::Free();
}
#include "pch.h"
#include "UI_Finish.h"

#include "GameInstance.h"
#include "FPlayerStat.h"

#include "Collider.h"

#include "Enemy.h"

CUI_Finish::CUI_Finish(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Finish::CUI_Finish(const CUI_Finish& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Finish::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Finish::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	m_pPlayerStat = pDesc->pPlayerStat;
	m_ppFinishTarget = pDesc->ppFinishTarget;

	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	m_isActive = false;
	m_vCenter = _float2(g_iWindowSizeX * 0.5f, g_iWindowSizeY * 0.5f);

	return S_OK;
}

void CUI_Finish::Priority_Update(_float fTimeDelta)
{
}

void CUI_Finish::Update(_float fTimeDelta)
{
	if (m_pPlayerStat->bFinishAttack)
	{
		m_isActive = false;
		return;
	}

	if (*m_ppFinishTarget)
	{
		CEnemy* pEnemy = static_cast<CEnemy*>(*m_ppFinishTarget);
		if (pEnemy == nullptr || pEnemy->IsFinished())
			return;

		m_isActive = true;
		_vec3 vTargetPos = static_cast<CCollider*>((*m_ppFinishTarget)->Find_Component(L"Com_Collider"))->Get_BoundingCenter();

		_float4x4 matView = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW);
		_float4x4 matProj = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ);
		_float2 vViewSize = _float2
		(
			static_cast<_float>(m_pGameInstance->Get_ViewportSize().x),
			static_cast<_float>(m_pGameInstance->Get_ViewportSize().y)
		);

		_float2 vScreenPos = CUtils::Get_WorldToScreenPos
		(
			vTargetPos._vector(),
			XMLoadFloat4x4(&matView),
			XMLoadFloat4x4(&matProj),
			vViewSize
		);

		_vector vCurPosVec = XMLoadFloat3(&m_vLocate);
		_vector vTargetPosVec = XMVectorSetZ(XMLoadFloat2(&vScreenPos), m_vLocate.z);

		_vector vLerpVec = XMVectorLerp(vCurPosVec, vTargetPosVec, fTimeDelta * m_fLerpSpeed);

		_float3 vNewPos = {};
		XMStoreFloat3(&vNewPos, vLerpVec);

		m_pTransformCom->Locate(vNewPos.x - s_fViewportWidth * 0.5f, -vNewPos.y + s_fViewportHeight * 0.5f, vNewPos.z);
		m_vLocate = vNewPos;
	}
	else
	{
		m_isActive = false;
	}
}

void CUI_Finish::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
		m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
}

HRESULT CUI_Finish::Render()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", s_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", s_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Texture_To_Shader(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UIParams", &m_tParams, sizeof(UI_PARAMS))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bHovered", &m_bHover, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bRightAngle", &m_bRightAngle, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bStretchY", &m_bStretchY, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &m_fRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CUI_Finish* CUI_Finish::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Finish* pInstance = new CUI_Finish(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Finish");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Finish::Clone(void* pArg)
{
	CUI_Finish* pInstance = new CUI_Finish(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Finish");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Finish::Free()
{
	CUI::Free();
}

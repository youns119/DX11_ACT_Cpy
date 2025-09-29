#include "pch.h"
#include "UI.h"

#include "GameInstance.h"

#include <sstream>
#include <locale>

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI::CUI(const CUI& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(Load_Data(*pDesc)))
		return E_FAIL;

	if(FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	pDesc->UIObjectDesc.fSizeX = m_vScale.x;
	pDesc->UIObjectDesc.fSizeY = m_vScale.y;

	pDesc->UIObjectDesc.fX = m_vLocate.x;
	pDesc->UIObjectDesc.fY = m_vLocate.y;
	pDesc->UIObjectDesc.fZ = m_vLocate.z;

	lstrcpy(m_szText, pDesc->szText);

	m_bInWorld = pDesc->bInWorld;
	m_fWorldOffsetY = pDesc->fWorldOffsetY;

	m_isActive = pDesc->isActive;
	m_bRightAngle = pDesc->bRightAngle;
	m_bStretchY = pDesc->bStretchY;
	m_eSorting = pDesc->eSorting;

	if (pDesc->pOwner)
		m_pOwner = pDesc->pOwner;

	return S_OK;
}

void CUI::Priority_Update(_float fTimeDelta)
{
}

void CUI::Update(_float fTimeDelta)
{
}

void CUI::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI::Render()
{
	return S_OK;
}

void CUI::Set_Locate(_float3 vLocate)
{
	m_pTransformCom->Locate(vLocate.x - g_iWindowSizeX * 0.5f, -vLocate.y + g_iWindowSizeY * 0.5f, vLocate.z);
	m_vLocate = vLocate;
}

void CUI::Set_Scale(_float2 vScale)
{
	m_pTransformCom->Set_Scale(vScale.x, vScale.y, 0.f);
	m_vScale = vScale;
}

void CUI::Set_CoverTexture(_wstring strTextureTag, _uint iTextureNum)
{
	CUIObject::Delete_Component(TEXT("Com_CoverTexture"), reinterpret_cast<CComponent**>(&m_pCoverTextureCom));
	Safe_Release(m_pCoverTextureCom);

	CUIObject::Add_Component((_uint)LEVEL_STATIC, strTextureTag, TEXT("Com_CoverTexture"), reinterpret_cast<CComponent**>(&m_pCoverTextureCom));
	m_iCoverTextureNum = iTextureNum;
}

void CUI::Delete_CoverTexture()
{
	CUIObject::Delete_Component(TEXT("Com_CoverTexture"), reinterpret_cast<CComponent**>(&m_pCoverTextureCom));
	Safe_Release(m_pCoverTextureCom);
}

void CUI::Change_Texture(_wstring strTextureTag, _uint iTextureNum)
{
	CUIObject::Delete_Component(TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom));
	Safe_Release(m_pTextureCom);

	CUIObject::Add_Component((_uint)LEVEL_STATIC, strTextureTag, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom));

	m_strTextureTag = strTextureTag;
	m_iTextureNum = iTextureNum;
}

_wstring CUI::Format_With_Comma(_float fValue)
{
	wstringstream ws;
	ws.imbue(locale("en_US.UTF-8"));  // 미국 영어 로케일
	ws << fValue;

	return ws.str();
}

void CUI::Set_Text(_float fValue)
{
	lstrcpy(m_szText, Format_With_Comma(fValue).c_str());
}

void CUI::Shake(_float2 vOffset)
{
	m_pTransformCom->Locate(m_vLocate.x - s_fViewportWidth * 0.5f + vOffset.x, -m_vLocate.y + s_fViewportHeight * 0.5f + vOffset.y, m_vLocate.z);
}

void CUI::Set_Move(_float2 vMoveDir, _float fSpeed, _float fMaxDistance)
{
	m_bMoving = true;
	m_fMoveTime = 0.f;
	m_fMoveDistance = 0.f;

	m_vMoveDir = vMoveDir;
	m_fSpeed = fSpeed;
	m_fMaxDistance = fMaxDistance;
}

void CUI::Move(_float fTimeDelta)
{
	if (m_bMoving)
	{
		m_fMoveTime += m_fSpeed * fTimeDelta;

		m_vLocate.x = m_vLocate.x + m_vMoveDir.x * m_fMoveTime;
		m_vLocate.y = m_vLocate.y + m_vMoveDir.y * m_fMoveTime;

		m_pTransformCom->Locate(m_vLocate.x - s_fViewportWidth * 0.5f, - m_vLocate.y + s_fViewportHeight * 0.5f, m_vLocate.z);

		m_fMoveDistance += m_fMoveTime;

		if (m_fMoveDistance >= m_fMaxDistance)
			m_bMoving = false;
	}
}

void CUI::Start_ScalingY(_float fStartScale, _float fEndScale)
{
	m_fStartScale = fStartScale;
	m_fEndScale = fEndScale;

	m_fScalingRatio = m_fStartScale;

	m_bScaling = true;
}

void CUI::ScalingY(_float fTimeDelta)
{
	// 작아지기
	if (m_fStartScale > m_fEndScale)
	{
		if (m_fScalingRatio <= m_fEndScale)
		{
			m_fScalingRatio = m_fEndScale;
			m_bScaling = false;
		}

		if (m_bScaling)
			m_fScalingRatio -= fTimeDelta;
	}
	else
	{
		if (m_fScalingRatio >= m_fEndScale)
		{
			m_fScalingRatio = m_fEndScale;
			m_bScaling = false;
		}

		if (m_bScaling)
			m_fScalingRatio += fTimeDelta;
	}
	
}

void CUI::Set_ObjectLocate(CGameObject* pGameObject, _float2 vOffset)
{
	_vector vObjectPos = static_cast<CTransform*>(pGameObject->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);

	_float4x4 matView = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW);
	_float4x4 matProj = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ);
	_float2 vViewSize = _float2
	(
		static_cast<_float>(m_pGameInstance->Get_ViewportSize().x),
		static_cast<_float>(m_pGameInstance->Get_ViewportSize().y)
	);

	_float2 vScreenPos = CUtils::Get_WorldToScreenPos
	(
		vObjectPos,
		XMLoadFloat4x4(&matView),
		XMLoadFloat4x4(&matProj),
		vViewSize
	);

	_float2 vNewPos = { vScreenPos.x + vOffset.x, vScreenPos.y + vOffset.y };
	m_pTransformCom->Locate(vNewPos.x - s_fViewportWidth * 0.5f, -vNewPos.y + s_fViewportHeight * 0.5f, m_vLocate.z);
	m_vLocate = { vNewPos.x, vNewPos.y, m_vLocate.z };
}

_float2 CUI::Get_ObjectLocate(CGameObject* pGameObject, _float2 vOffset)
{
	_vector vObjectPos = static_cast<CTransform*>(pGameObject->Find_Component(g_strTransformTag))->Get_State(CTransform::STATE::POSITION);

	_float4x4 matView = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW);
	_float4x4 matProj = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ);
	_float2 vViewSize = _float2
	(
		static_cast<_float>(m_pGameInstance->Get_ViewportSize().x),
		static_cast<_float>(m_pGameInstance->Get_ViewportSize().y)
	);

	_float2 vScreenPos = CUtils::Get_WorldToScreenPos
	(
		vObjectPos,
		XMLoadFloat4x4(&matView),
		XMLoadFloat4x4(&matProj),
		vViewSize
	);

	return { vScreenPos.x + vOffset.x, vScreenPos.y + vOffset.y };	
}

HRESULT CUI::Load_Data(DESC& Desc)
{
	HANDLE hFile = CreateFile(Desc.strFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to open file");
		return E_FAIL;
	}
	_bool bResult = {};

	DWORD dwByte = 0;
	size_t length = 0;

	bResult = ReadFile(hFile, &m_vScale, sizeof(_float2), &dwByte, nullptr);
	bResult = ReadFile(hFile, &m_vLocate, sizeof(_float3), &dwByte, nullptr);

	bResult = ReadFile(hFile, &m_vAxis, sizeof(_float3), &dwByte, nullptr);
	bResult = ReadFile(hFile, &m_fDegrees, sizeof(_float), &dwByte, nullptr);

	bResult = ReadFile(hFile, &m_tParams.vSpriteSize, sizeof(_float2), &dwByte, nullptr);
	bResult = ReadFile(hFile, &m_tParams.vSpriteIndex, sizeof(_float2), &dwByte, nullptr);

	bResult = ReadFile(hFile, &m_tParams.fThreshold, sizeof(_float), &dwByte, nullptr);
	bResult = ReadFile(hFile, &m_fInterval, sizeof(_float), &dwByte, nullptr);

	bResult = ReadFile(hFile, &m_tParams.vStretchRange, sizeof(_float2), &dwByte, nullptr);

	bResult = ReadFile(hFile, &m_vTextOffset, sizeof(_float2), &dwByte, nullptr);
	bResult = ReadFile(hFile, &m_fTextScale, sizeof(_float), &dwByte, nullptr);
	bResult = ReadFile(hFile, &m_vTextColor, sizeof(_float4), &dwByte, nullptr);
	bResult = ReadFile(hFile, &m_vHoverTextColor, sizeof(_float4), &dwByte, nullptr);

	length = m_strTextureTag.length();
	bResult = ReadFile(hFile, &length, sizeof(size_t), &dwByte, nullptr);
	m_strTextureTag.resize(length);
	bResult = ReadFile(hFile, &m_strTextureTag[0], sizeof(_tchar) * (length + 1), &dwByte, nullptr);

	length = m_strFontTag.length();
	bResult = ReadFile(hFile, &length, sizeof(size_t), &dwByte, nullptr);
	m_strFontTag.resize(length);
	bResult = ReadFile(hFile, &m_strFontTag[0], sizeof(_tchar) * (length + 1), &dwByte, nullptr);

	length = m_strMaskTextureTag.length();
	bResult = ReadFile(hFile, &length, sizeof(size_t), &dwByte, nullptr);
	m_strMaskTextureTag.resize(length);
	bResult = ReadFile(hFile, &m_strMaskTextureTag[0], sizeof(_tchar) * (length + 1), &dwByte, nullptr);

	bResult = ReadFile(hFile, &m_iShaderPass, sizeof(_uint), &dwByte, nullptr);
	bResult = ReadFile(hFile, &m_tParams.vSrcColor, sizeof(_float4), &dwByte, nullptr);
	bResult = ReadFile(hFile, &m_tParams.vDstColor, sizeof(_float4), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CUI::Ready_Components(DESC& Desc)
{
	/* Com_Shader */
	if (FAILED(CUIObject::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(CUIObject::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (m_strTextureTag != TEXT(""))
	{
		/* Com_Texture */
		if (FAILED(CUIObject::Add_Component(Desc.eLevelID, m_strTextureTag,
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}

	if (m_strMaskTextureTag != TEXT(""))
	{
		/* Com_Texture */
		if (FAILED(CUIObject::Add_Component(Desc.eLevelID, m_strMaskTextureTag,
			TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
			return E_FAIL;
	}

	return S_OK;
}

void CUI::Change_World(_float fOffsetY)
{
	_float3 vPos = _float3(m_pParentWorldMatrix->_41, m_pParentWorldMatrix->_42, m_pParentWorldMatrix->_43);
	_vector vecPos = XMLoadFloat3(&vPos);

	_matrix ViewMatrix{ m_pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORM::VIEW) };
	_matrix ProjMatrix{ m_pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORM::PROJ) };

	_matrix ViewInverseMatrix{ m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM::VIEW) };

	_vector vCamLook = XMVector3Normalize(ViewInverseMatrix.r[2]);

	//vCamLook = XMVector3Normalize(-vCamLook);

	_float4 CamPos = m_pGameInstance->Get_CamPosition();
	_vector vCamPos = XMLoadFloat4(&CamPos); // 카메라 위치

	_vector vRelativePos = XMVector3Normalize(vecPos - vCamPos);

	vCamLook = XMVector3Normalize(vCamLook);

	_float fDot = XMVector3Dot(vRelativePos, vCamLook).m128_f32[0];

	if (fDot < 0.0f)
		return;

	_vector vOffset = XMVectorSet(0.f, fOffsetY, 0.f, 0.f);
	_vector vWolrdPos = vecPos + vOffset;

	_vector vViewPos = XMVector3TransformCoord(vWolrdPos, ViewMatrix);

	_vector vProjPos = XMVector3TransformCoord(vViewPos, ProjMatrix);

	vProjPos = vProjPos / XMVectorGetW(vProjPos);

	_uint2 vViewportSize = m_pGameInstance->Get_ViewportSize();

	// 화면 좌표 변환
	_float screenX = XMVectorGetX(vProjPos) * vViewportSize.x * 0.5f;
	_float screenY = XMVectorGetY(vProjPos) * vViewportSize.y * 0.5f;

	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMVectorSet(screenX, screenY, 0.99f, 1.f));
}

CUI* CUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI* pInstance = new CUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI::Clone(void* pArg)
{
	CUI* pInstance = new CUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI::Free()
{
	CUIObject::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pCoverTextureCom);
	Safe_Release(m_pVIBufferCom);
}
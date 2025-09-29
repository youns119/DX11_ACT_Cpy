#include "pch.h"
#include "UI_Debug.h"

#include "GameInstance.h"

CUI_Debug::CUI_Debug(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Debug::CUI_Debug(const CUI_Debug& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_Debug::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Debug::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	
	// Data Load, Ready_Components
	if (FAILED(CUI::Initialize(&pDesc->UIDesc)))
		return E_FAIL;

	if (FAILED(CUIObject::Initialize(&pDesc->UIDesc.UIObjectDesc)))
		return E_FAIL;

	m_pTransformCom->Rotate(m_vAxis, XMConvertToRadians(m_fDegrees));

	return S_OK;
}

void CUI_Debug::Priority_Update(_float fTimeDelta)
{
}

void CUI_Debug::Update(_float fTimeDelta)
{
	// 부모 행렬 로드
	_matrix parentMatrix = XMLoadFloat4x4(m_pParentWorldMatrix);

	// 부모 행렬에서 회전 & 위치만 가져오기 (스케일 제거)
	//XMVECTOR parentRight = XMVector3Normalize(parentMatrix.r[0]); // X축 (오른쪽)
	//XMVECTOR parentUp = XMVector3Normalize(parentMatrix.r[1]);    // Y축 (위쪽)
	//XMVECTOR parentLook = XMVector3Normalize(parentMatrix.r[2]);  // Z축 (앞쪽)
	XMVECTOR parentPos = parentMatrix.r[3];                      // 위치 (변환값 그대로 사용)

	// 자식의 월드 행렬
	_matrix myMatrix = m_pTransformCom->Get_WorldMatrix();

	// 부모 회전 및 위치 반영, BUT 크기 영향은 제거
	//myMatrix.r[0] = parentRight;
	//myMatrix.r[1] = parentUp;
	//myMatrix.r[2] = parentLook;
	myMatrix.r[3] = XMVectorAdd(myMatrix.r[3], parentPos); // 위치만 추가

	//m_pTransformCom->Set_State(CTransform::STATE::RIGHT, myMatrix.r[0]);
	//m_pTransformCom->Set_State(CTransform::STATE::UP, myMatrix.r[1]);
	//m_pTransformCom->Set_State(CTransform::STATE::LOOK, myMatrix.r[2]);
	m_pTransformCom->Set_State(CTransform::STATE::POSITION, parentMatrix.r[3]);
}

void CUI_Debug::Late_Update(_float fTimeDelta)
{
	if (m_isActive)
	{
		if (m_pOwner)
			m_pGameInstance->Add_RenderGroup(m_pOwner->Get_RenderGroup(), this);
		else
			m_pGameInstance->Add_RenderGroup(m_eRenderGroup, this);
	}
}

HRESULT CUI_Debug::Render()
{
	if (!m_pTextureCom)
		return S_OK;

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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bHovered", &m_bHovered, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bRightAngle", &m_bRightAngle, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bStretchY", &m_bStretchY, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_InputAssembler()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	_float2 vTextPos = { m_vLocate.x + m_vTextOffset.x, m_vLocate.y + m_vTextOffset.y };
	
	m_pGameInstance->Render_Font(m_strFontTag, m_szText, vTextPos, XMLoadFloat4(&m_vTextColor), 0.f, {0.f, 0.f}, m_fTextScale, SORTING::RIGHT);

	return S_OK;
}


CUI_Debug* CUI_Debug::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Debug* pInstance = new CUI_Debug(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Debug");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Debug::Clone(void* pArg)
{
	CUI_Debug* pInstance = new CUI_Debug(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Debug");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Debug::Free()
{
	CUI::Free();
}
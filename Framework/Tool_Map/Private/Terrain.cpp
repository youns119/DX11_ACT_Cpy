#include "pch.h"
#include "Terrain.h"

#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTerrain::CTerrain(const CTerrain& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	CGameObject::DESC* pDesc = static_cast<CGameObject::DESC*>(pArg);
	pDesc->TransformDesc.fSpeedPerSec = 5.f;

	if (FAILED(super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{
	//m_pNavigationCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CTerrain::Update(_float fTimeDelta)
{
}

void CTerrain::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugRenderComponent(CRenderer::DEBUGRENDERGROUP::BLEND, m_pNavigationCom);
#endif
}

HRESULT CTerrain::Render()
{
	//이거 넣으니까 IMGUI 가 출력이 안됨 다녀와서 보기
	if (FAILED(Bind_ObjectID_To_Shader(m_pShaderCom, "g_iObjectID"))) // 즁요 ㅋㅋ
		return E_FAIL;

	HRESULT hr{};

	hr = Bind_ShaderResources();
	if (FAILED(hr))
		return E_FAIL;

	hr = m_pShaderCom->Begin(0); // 0 : default , 1 : brush
	if (FAILED(hr))
		return E_FAIL;

	hr = m_pVIBufferCom->Bind_InputAssembler();
	if (FAILED(hr))
		return E_FAIL;

	hr = m_pVIBufferCom->Render();
	if (FAILED(hr))
		return E_FAIL;



	return S_OK;
}

_int CTerrain::Add_Point(_float3& vOutPointPos)
{
	_int		iPointIdx = { -1 };
	_float3		vMousePos = {};

	if (true == m_pGameInstance->Compute_PixelPickPos(vMousePos))
	{
		iPointIdx = m_pNavigationCom->Add_Point(vMousePos);
		vOutPointPos = vMousePos;
	}
	else
	{
		MSG_BOX("피킹 범위 밖이다.");
		return -1; // 피킹 실패 -1 리턴ㅋ
	}

	return iPointIdx; // PickingCnt 인덱스 리턴하게 ㅇㅇㅇ
}

HRESULT CTerrain::Delete_Point(_uint iIndex)
{
	m_pNavigationCom->Erase_Point(iIndex);
	return S_OK;
}

HRESULT CTerrain::Add_Cell(const vector<_int>& Point)
{
	m_pNavigationCom->Add_Cell(Point);
	return S_OK;
}

HRESULT CTerrain::Delete_PrevCell()
{
	_uint iSize = m_pNavigationCom->Get_CellSize();
	if (iSize == 0)
	{
		MSG_BOX("셀없어여");
		return S_OK;
	}
	m_pNavigationCom->Erase_Cell(iSize -1);
	return S_OK;
}

HRESULT CTerrain::Save_Navigation(const _wstring& strFilePath)
{
	m_pNavigationCom->Save_Navigation(strFilePath);
	return S_OK;
}

HRESULT CTerrain::Load_Navigation(const _wstring& strFilePath)
{
	m_pNavigationCom->Load_Navigation(strFilePath);
	return S_OK;
}

HRESULT CTerrain::Generate_HeightMap(_float4 vLocalPickPos, _float fRadius, _float fHeightScale, _float fSigma, _bool IsUp)
{
	_vector vLocal = XMVector3TransformCoord(XMLoadFloat4(&vLocalPickPos), m_pTransformCom->Get_WorldMatrix_Inversed());
	_float4 vLocalPos{};
	XMStoreFloat4(&vLocalPos, vLocal);
	return m_pVIBufferCom->Generate_HeightMap(vLocalPos, fRadius, fHeightScale, fSigma, IsUp);
}

HRESULT CTerrain::Save_HeightMap(const _tchar* pHeightMapFile)
{
	return m_pVIBufferCom->Save_HeightMap(pHeightMapFile);
}

HRESULT CTerrain::Load_HeightMap(const _tchar* pHeightMapFile)
{
	return m_pVIBufferCom->Load_HeightMap(pHeightMapFile);
}

HRESULT CTerrain::Ready_Components()
{
	/* Com_Texture_Diffuse */
	if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture_Diffuse"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::DIFFUSE]))))
		return E_FAIL;

	/* Com_Texture_Normal */
	if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Normal"), // 노이즈
		TEXT("Com_Texture_Normal"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::NORMAL]))))
		return E_FAIL;

	/* Com_Texture_Normal_Second */
	if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Normal_Wavy"), // 약한 웨이브ㅏ
		TEXT("Com_Texture_Normal_Second"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::SECOND_NORMAL]))))
		return E_FAIL;

	/* Com_Texture_Normal_Third */
	if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Normal_Sand"), // 흰빨쎈거
		TEXT("Com_Texture_Normal_Third"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::THIRD_NORMAL]))))
		return E_FAIL;

	/* Com_Texture_Mask*/
	if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		TEXT("Com_Texture_Mask"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::MASK]))))
		return E_FAIL;

	///* Com_Texture_Brush*/
	//if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Texture_Brush"),
	//	TEXT("Com_Texture_Brush"), reinterpret_cast<CComponent**>(&m_TextureComs[(_uint)TEXTURE_TYPE::BRUSH]))))
	//	return E_FAIL;

	/* Com_Shader */
	if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Navigation */
	//if (FAILED(super::Add_Component(LEVELID::LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
	////if (FAILED(super::Add_Component(LEVELID::LEVEL_VILLAGE, TEXT("Prototype_Component_Navigation_TriangluationTest"),
	//	TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{

	if (FAILED(m_pTransformCom->Bind_WorldMatrix_To_Shader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",
		m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
		m_pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORM::PROJ))))
		return E_FAIL;
	 
	if (FAILED(m_TextureComs[(_uint)TEXTURE_TYPE::DIFFUSE]->Bind_Textures_To_Shader(m_pShaderCom, "g_DiffuseTexture"))) // Textures
		return E_FAIL;
	if (FAILED(m_TextureComs[(_uint)TEXTURE_TYPE::NORMAL]->Bind_Texture_To_Shader(m_pShaderCom, "g_NormalTexture"))) // Texture
		return E_FAIL;
	if (FAILED(m_TextureComs[(_uint)TEXTURE_TYPE::SECOND_NORMAL]->Bind_Texture_To_Shader(m_pShaderCom, "g_SecondNormalTexture"))) // Texture
		return E_FAIL;
	if (FAILED(m_TextureComs[(_uint)TEXTURE_TYPE::THIRD_NORMAL]->Bind_Texture_To_Shader(m_pShaderCom, "g_ThirdNormalTexture"))) // Texture
		return E_FAIL;
	if (FAILED(m_TextureComs[(_uint)TEXTURE_TYPE::MASK]->Bind_Texture_To_Shader(m_pShaderCom, "g_MaskTexture"))) //Texture
		return E_FAIL;

	//if (FAILED(m_TextureComs[(_uint)TEXTURE_TYPE::BRUSH]->Bind_Texture_To_Shader(m_pShaderCom, "g_BrushTexture")))
	//	return E_FAIL;

	//// forward shading
	//const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//	return E_FAIL;

	////_float4 vCamPosition = m_pGameInstance->Get_CamPosition();
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;



	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CTerrain::Free()
{
	super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pNavigationCom);

	for (auto& pTextureCom : m_TextureComs)
		Safe_Release(pTextureCom);

}

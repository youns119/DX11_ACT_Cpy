#include "pch.h"
#include "Light_Player.h"
#include "GameInstance.h"

CLight_Player::CLight_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CLight_Player::CLight_Player(const CLight_Player& Prototype)
	: super(Prototype)
{
}

HRESULT CLight_Player::Initialize_Prototype()
{
	if (FAILED(super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLight_Player::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);
	if (nullptr == pDesc)
		return E_FAIL;

	if (FAILED(super::Initialize(&pDesc->GameObjectDesc)))
		return E_FAIL;

	m_pPlayer = pDesc->pPlayer;


	LIGHT_DESC LightDesc = {};
	LightDesc.eType = LIGHT_TYPE::POINT;
	LightDesc.fRange = 50.f;
	LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 0.f);
	LightDesc.vDiffuse = _float4(5.f, 5.f, 5.f, 1.f);
	LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.vAttenuation = _float3(1.f, 0.1f, 1.f);

	m_pGameInstance->Add_Light(m_strLightTag, LightDesc);

	return S_OK;
}

void CLight_Player::Update(_float fTimeDelta)
{
	_mat PlayerWorldMatrix = m_pPlayer->Get_CombinedMatrix();
	_vec3 vPlayerPos = PlayerWorldMatrix.position;
	_vec3 vCamPos = _vec3(m_pGameInstance->Get_CamPosition());

	LIGHT_DESC LightDesc = m_pGameInstance->Get_LightDesc(m_strLightTag);

#ifdef _DEBUG
	//static _float fLightDistance = 0.4f;
	//ImGui::Begin("player light");

	//ImGui::DragFloat3("Diffuse", (_float*)&LightDesc.vDiffuse, 0.005f, 0.f);
	//ImGui::DragFloat3("attenuation", (_float*)&LightDesc.vAttenuation, 0.005f, 0.f);
	//ImGui::DragFloat("Light Distance", &fLightDistance, 0.0005f, 0.f);

	//ImGui::End();
#endif // _DEBUG


	_vec3 vPlayerToCam = (vCamPos - vPlayerPos).Normalized();

	_vec3 lightPos = vPlayerPos
		+ XMVectorSet(0.f, 1.5f, 0.f, 0.f)
		+ vPlayerToCam * 0.871f;

	XMStoreFloat4(&LightDesc.vPosition, lightPos._vector());
	m_pGameInstance->Set_LightDesc(L"Light_Player", LightDesc);
}

CLight_Player* CLight_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLight_Player* pInstance = new CLight_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CLight_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCloneable* CLight_Player::Clone(void* pArg)
{
	CLight_Player* pInstance = new CLight_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Create : CLight_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLight_Player::Free()
{
	super::Free();

	m_pGameInstance->Remove_Light(m_strLightTag);
}

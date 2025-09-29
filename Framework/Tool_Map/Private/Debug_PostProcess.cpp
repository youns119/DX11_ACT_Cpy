#include "pch.h"
#include "Debug_PostProcess.h"

#ifdef _DEBUG
#include "GameInstance.h"
#include "Dialog.h"


CDebug_PostProcess::CDebug_PostProcess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: super(pDevice, pContext)
{
}

CDebug_PostProcess::CDebug_PostProcess(const CDebug_PostProcess& Prototype)
	: super(Prototype)
{
}

HRESULT CDebug_PostProcess::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDebug_PostProcess::Initialize(void* pArg)
{
	return S_OK;
}

void CDebug_PostProcess::Priority_Update(_float fTimeDelta)
{
	m_iImGuiID = 0;
}

void CDebug_PostProcess::Update(_float fTimeDelta)
{
	ImGui::Begin("Post process render");

	Debug_Fog();
	Debug_Bloom();
	Debug_ColorCorrection();
	Debug_GlobalLight();
	Debug_SSAO();

	Handle_FileIO();

	ImGui::End();
}

void CDebug_PostProcess::Late_Update(_float fTimeDelta)
{
}

HRESULT CDebug_PostProcess::Render()
{
	return S_OK;
}

void CDebug_PostProcess::Debug_Fog()
{
	FOG_PARAMS FogParams = m_pGameInstance->Get_FogParams();
	_bool isManipulated = false;

	ImGui::PushID(m_iImGuiID++);
	ImGui::SeparatorText("Fog");

	/* Fog */
	if (ImGui::DragFloat2("Fog range", reinterpret_cast<_float*>(&FogParams.vRange), 1.f, 0.f, 1000.f))
	{
		if (FogParams.vRange.x >= FogParams.vRange.y)
			swap(FogParams.vRange.x, FogParams.vRange.y);

		isManipulated = true;
	}
	if (ImGui::ColorEdit4("Fog color", reinterpret_cast<_float*>(&FogParams.vFogColor)))
		isManipulated = true;

	ImGui::Spacing();

	/* Abyssal */
	if (ImGui::DragFloat2("Abyssal range", reinterpret_cast<_float*>(&FogParams.vAbyssalRange), 1.f, -1000.f, 100.f))
	{
		if (FogParams.vAbyssalRange.x >= FogParams.vAbyssalRange.y)
			swap(FogParams.vAbyssalRange.x, FogParams.vAbyssalRange.y);

		isManipulated = true;
	}
	if (ImGui::ColorEdit4("Abyssal color", reinterpret_cast<_float*>(&FogParams.vAbyssalColor)))
		isManipulated = true;

	ImGui::PopID();

	if (true == isManipulated)
		m_pGameInstance->Set_FogParams(FogParams);

	ImGui::Spacing();
}

void CDebug_PostProcess::Debug_Bloom()
{
	BLOOM_PARAMS BloomParams = m_pGameInstance->Get_BloomParams();

	ImGui::PushID(m_iImGuiID++);
	ImGui::SeparatorText("Bloom");
	if (ImGui::SliderFloat("threshold", &BloomParams.fThreshold, 0.f, 4.f))
	{
		m_pGameInstance->Set_BloomParams(BloomParams);
	}
	ImGui::PopID();

	ImGui::Spacing();
}

void CDebug_PostProcess::Debug_ColorCorrection()
{
	COLORCORRECTION_PARAMS ColorCorrectionParams = m_pGameInstance->Get_ColorCorrectionParams();

	ImGui::PushID(m_iImGuiID++);
	ImGui::SeparatorText("Tone mapping");
	if (ImGui::SliderFloat("contrast", &ColorCorrectionParams.fContrast, -3.f, 3.f))
	{
		m_pGameInstance->Set_ColorCorrectionParams(ColorCorrectionParams);
	}
	if (ImGui::SliderFloat("brightness", &ColorCorrectionParams.fBrightness, -1.f, 1.f))
	{
		m_pGameInstance->Set_ColorCorrectionParams(ColorCorrectionParams);
	}
	if (ImGui::SliderFloat("saturation", &ColorCorrectionParams.fSaturation, 0.f, 2.f))
	{
		m_pGameInstance->Set_ColorCorrectionParams(ColorCorrectionParams);
	}
	if (ImGui::DragFloat("Temperature", &ColorCorrectionParams.fTemperature, 1.f, 100.f, 40000.f))
	{
		m_pGameInstance->Set_ColorCorrectionParams(ColorCorrectionParams);
	}
	if (ImGui::DragFloat("Tint", &ColorCorrectionParams.fTint, 0.005f, -1.f, 1.f))
	{
		m_pGameInstance->Set_ColorCorrectionParams(ColorCorrectionParams);
	}
	if (ImGui::SliderFloat("Exposure", &ColorCorrectionParams.fExposure, 0.f, 2.f))
	{
		m_pGameInstance->Set_ColorCorrectionParams(ColorCorrectionParams);
	}
	if (ImGui::DragFloat("Gamma", &ColorCorrectionParams.fGamma, 0.005f, 0.f, 10.f))
	{
		m_pGameInstance->Set_ColorCorrectionParams(ColorCorrectionParams);
	}
	if (ImGui::DragFloat("On/Off ToneMapping", &ColorCorrectionParams.IsToneMapping, 0.005f, 0.f, 1.f))
	{
		m_pGameInstance->Set_ColorCorrectionParams(ColorCorrectionParams);
	}
	ImGui::PopID();

	ImGui::Spacing();
}

void CDebug_PostProcess::Debug_GlobalLight()
{
	LIGHT_DESC LightDesc = m_pGameInstance->Get_LightDesc(L"Light_Global");

	ImGui::PushID(m_iImGuiID++);
	ImGui::SeparatorText("Global Light");

	if (ImGui::DragFloat3("Direction", reinterpret_cast<_float*>(&LightDesc.vDirection), 0.005f, -4.f, 4.f))
	{
		m_pGameInstance->Set_LightDesc(L"Light_Global", LightDesc);
		m_pGameInstance->SetUp_ShadowLight(_vec3(LightDesc.vDirection));
	}
	if (ImGui::DragFloat3("Diffuse", reinterpret_cast<_float*>(&LightDesc.vDiffuse), 0.005f, 0.f, 5.f))
		m_pGameInstance->Set_LightDesc(L"Light_Global", LightDesc);
	if (ImGui::DragFloat3("Ambient", reinterpret_cast<_float*>(&LightDesc.vAmbient), 0.005f, 0.f, 1.f))
		m_pGameInstance->Set_LightDesc(L"Light_Global", LightDesc);

	ImGui::PopID();
	ImGui::Spacing();
}

void CDebug_PostProcess::Debug_SSAO()
{
	SSAO_PARAMS SSAOParams = m_pGameInstance->Get_SSAOParams();

	ImGui::PushID(m_iImGuiID++);
	ImGui::SeparatorText("SSAO");

	if (ImGui::DragFloat("Radius", &SSAOParams.fRadius, 0.005f, 0.f, 1.f))
	{
		m_pGameInstance->Set_SSAOParams(SSAOParams);
	}
	if (ImGui::DragFloat("Bias", &SSAOParams.fBias, 0.0005f, 0.f, 1.f))
	{
		m_pGameInstance->Set_SSAOParams(SSAOParams);
	}


	ImGui::PopID();
	ImGui::Spacing();
}

void CDebug_PostProcess::Handle_FileIO()
{
	ImGui::PushID(m_iImGuiID++);
	ImGui::Separator;

	if (ImGui::Button("Save"))
		Save_Data();

	ImGui::SameLine();

	if (ImGui::Button("Load"))
		Load_Data();

	ImGui::PopID();
	ImGui::Spacing();
}

void CDebug_PostProcess::Save_Data()
{
	CDialog* pDialog = CDialog::Create(CDialog::TYPE::SAVE);

	CDialog::FILE_TYPE eFileType = CDialog::FILE_TYPE::ATMOSPHERE;
	pDialog->Set_FileType(eFileType);
	pDialog->Set_DefaultExt(L".atmosphere");
	//pDialog->Set_DefaultExt(CWindowFileDialog::s_FileTypes[static_cast<_uint>(eFileType)].pszSpec);

	if (SUCCEEDED(pDialog->Show_Dialog()))
	{
		_wstring strFilePath = pDialog->Get_FilePath();

		HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (INVALID_HANDLE_VALUE != hFile)
		{
			DWORD dwByte{};

			const LIGHT_DESC& LightDesc = m_pGameInstance->Get_LightDesc(L"Light_Global");
			const COLORCORRECTION_PARAMS& ColorCorrectionParams = m_pGameInstance->Get_ColorCorrectionParams();
			const BLOOM_PARAMS& BloomParams = m_pGameInstance->Get_BloomParams();
			const FOG_PARAMS& FogParams = m_pGameInstance->Get_FogParams();

			WriteFile(hFile, &LightDesc, sizeof(LIGHT_DESC), &dwByte, nullptr);
			WriteFile(hFile, &ColorCorrectionParams, sizeof(COLORCORRECTION_PARAMS), &dwByte, nullptr);
			WriteFile(hFile, &BloomParams, sizeof(BLOOM_PARAMS), &dwByte, nullptr);
			WriteFile(hFile, &FogParams, sizeof(FOG_PARAMS), &dwByte, nullptr);
		}
		CloseHandle(hFile);

	}
	Safe_Release(pDialog);
}

void CDebug_PostProcess::Load_Data()
{
	CDialog* pDialog = CDialog::Create(CDialog::TYPE::LOAD);

	CDialog::FILE_TYPE eFileType = CDialog::FILE_TYPE::ATMOSPHERE;
	pDialog->Set_FileType(eFileType);
	pDialog->Set_DefaultExt(L".atmosphere");
	//pDialog->Set_DefaultExt(CWindowFileDialog::s_FileTypes[static_cast<_uint>(eFileType)].pszSpec);

	if (SUCCEEDED(pDialog->Show_Dialog()))
	{
		_wstring strFilePath = pDialog->Get_FilePath();

		HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (INVALID_HANDLE_VALUE != hFile)
		{
			DWORD dwByte{};
			_bool bResult{};

			LIGHT_DESC LightDesc{};
			COLORCORRECTION_PARAMS ColorCorrectionParams{};
			BLOOM_PARAMS BloomParams{};
			FOG_PARAMS FogParams{};

			bResult = ReadFile(hFile, &LightDesc, sizeof(LIGHT_DESC), &dwByte, nullptr);
			bResult = ReadFile(hFile, &ColorCorrectionParams, sizeof(COLORCORRECTION_PARAMS), &dwByte, nullptr);
			bResult = ReadFile(hFile, &BloomParams, sizeof(BLOOM_PARAMS), &dwByte, nullptr);
			bResult = ReadFile(hFile, &FogParams, sizeof(FOG_PARAMS), &dwByte, nullptr);

			m_pGameInstance->Set_LightDesc(L"Light_Global", LightDesc);
			m_pGameInstance->Set_ColorCorrectionParams(ColorCorrectionParams);
			m_pGameInstance->Set_BloomParams(BloomParams);
			m_pGameInstance->Set_FogParams(FogParams);
		}
		CloseHandle(hFile);

	}
	Safe_Release(pDialog);
}

CDebug_PostProcess* CDebug_PostProcess::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDebug_PostProcess* pInstance = new CDebug_PostProcess(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CDebug_PostProcess");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CDebug_PostProcess::Clone(void* pArg)
{
	CDebug_PostProcess* pInstance = new CDebug_PostProcess(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CDebug_PostProcess");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDebug_PostProcess::Free()
{
	super::Free();
}

#endif // _DEBUG
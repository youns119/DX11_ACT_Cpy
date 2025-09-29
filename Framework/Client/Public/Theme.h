#pragma once

#include "GameInstance.h"

namespace Client
{
	struct THEME_DESC
	{
		LIGHT_DESC				LightDesc {};
		COLORCORRECTION_PARAMS	ColorCorrectionParams {};
		BLOOM_PARAMS			BloomParams {};
		FOG_PARAMS				FogParams {};

		static THEME_DESC Lerp(const THEME_DESC& Src, const THEME_DESC& Dst, _float fFactor)
		{
			THEME_DESC Desc {};
			Desc.LightDesc = LIGHT_DESC::Lerp(Src.LightDesc, Dst.LightDesc, fFactor);
			Desc.ColorCorrectionParams = COLORCORRECTION_PARAMS::Lerp(Src.ColorCorrectionParams, Dst.ColorCorrectionParams, fFactor);
			Desc.BloomParams = BLOOM_PARAMS::Lerp(Src.BloomParams, Dst.BloomParams, fFactor);
			Desc.FogParams = FOG_PARAMS::Lerp(Src.FogParams, Dst.FogParams, fFactor);

			return Desc;
		}

		void Set_Atmosphere() const
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			pGameInstance->Set_LightDesc(L"Light_Global", LightDesc);
			pGameInstance->SetUp_ShadowLight(_vec3(LightDesc.vDirection));
			pGameInstance->Set_ColorCorrectionParams(ColorCorrectionParams);
			pGameInstance->Set_BloomParams(BloomParams);
			pGameInstance->Set_FogParams(FogParams);
		}

		static THEME_DESC Get_CurrentAtmospehre()
		{
			THEME_DESC Desc {};
			
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Desc.LightDesc =pGameInstance->Get_LightDesc(L"Light_Global");
			Desc.ColorCorrectionParams = pGameInstance->Get_ColorCorrectionParams();
			Desc.BloomParams = pGameInstance->Get_BloomParams();
			Desc.FogParams = pGameInstance->Get_FogParams();

			return Desc;
		}

		static HRESULT Load_Themes(THEME_DESC** ppThemes, const vector<_wstring>& strFilePaths, _uint iNumThemes)
		{
			vector<THEME_DESC> Themes(iNumThemes);

			for (_uint i = 0; i < iNumThemes; ++i)
			{
				const _wstring& strFilePath = strFilePaths[i];

				HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
				if (INVALID_HANDLE_VALUE == hFile)
					return E_FAIL;

				DWORD dwByte {};
				_bool bResult {};

				bResult = ReadFile(hFile, &Themes[i], sizeof(THEME_DESC), &dwByte, nullptr);
				CloseHandle(hFile);

				if (false == bResult)
					return E_FAIL;
			}

			for (_uint i = 0; i < iNumThemes; ++i)
			{
				ppThemes[i] = new THEME_DESC(Themes[i]);
			}

			return S_OK;
		}
	};
}
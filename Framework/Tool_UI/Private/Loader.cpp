#include "pch.h"
#include "Loader.h"

#include "GameInstance.h"

/* for.Level Static*/
#include "Camera_Free.h"

/* for.Level UITool*/
#include "UI_Dummy.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

//_uint APIENTRY Thread_Main(void* pArg)
_uint WINAPI Thread_Main(void* pArg)
{
	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Start_Loading()))
	{
		MSG_BOX("Failed to Loading");
	}

	return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);
	// m_eNextLevelID가 필요로하는 자원을 로드해주기위한 스레드를 만들어낸다. 
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Start_Loading()
{
	HRESULT hr{};

	// chat gpt 가 CoInitializeEx 코드는 임계영역 전에 작성하는게 좋다고 했음
	hr = CoInitializeEx(nullptr, 0);
	if (FAILED(hr)) // 내가 작성함
		return hr;

	EnterCriticalSection(&m_CriticalSection);

	switch (m_eNextLevelID)
	{
	case LEVELID::LEVEL_UITOOL:
		hr = Loading_For_Level_UITool();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

_float CLoader::Get_CurrnetProgress() const
{
	_float fRatio = m_iCurrentLine / static_cast<_float>(m_iNumTotalLine);

	fRatio = min(1.f, fRatio);
	fRatio = max(0.f, fRatio);

	return fRatio;
}

#ifdef _DEBUG
void CLoader::SetUp_WindowText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}
#endif

HRESULT CLoader::Loading_For_Level_UITool()
{
	/* Load resoruce for Static */
	lstrcpy(m_szLoadingText, TEXT("Load resoruce for Static"));

	/* for Static: GameObject =================================================================== */
	lstrcpy(m_szLoadingText, TEXT("Load Resources for Static: GameObject"));

	/*For. Prototype_GameObject_Camera_Free*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* for UITool: Texture =================================================================== */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐 원형을 생성한다."));

	/* For.Prototype_Component_Texture_Background */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Background"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Background.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Loading */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Locked */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Locked"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Locked.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Mask.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Round */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Round"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Round.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Triangle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Triangle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Triangle.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Shell_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, L"Prototype_Component_Texture_Shell_Logo",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Shell.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Square */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, L"Prototype_Component_Texture_Square",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Square.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Clock */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, L"Prototype_Component_Texture_Clock",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Clock.png", 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_X */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, L"Prototype_Component_Texture_X",
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/X.png", 1))))
		return E_FAIL;

	/* Button ================================================================================ */

	/* For.Prototype_Component_Texture_Button_Arrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Button_Arrow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Button/Arrow.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BlurryBox */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_BlurryBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Button/BlurryBox.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BlurryRect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_BlurryRect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Button/BlurryRect.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Button */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Button"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Button/Button.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Rect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Button/Rect.png"), 1))))
		return E_FAIL;

	/* Compass ================================================================================= */

	/* For.Prototype_Component_Texture_Compass */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Compass"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Compass/Compass.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CompassArrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_CompassArrow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Compass/CompassArrow.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CompassArrow2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_CompassArrow2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Compass/CompassArrow2.dds"), 1))))
		return E_FAIL;

	/* Cursor ================================================================================= */

	/* For.Prototype_Component_Texture_Cursor */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Cursor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Cursor/Cursor_%d.dds"), 2))))
		return E_FAIL;

	/* Debuff ================================================================================= */

	/* For.Prototype_Component_Texture_Fear */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Fear"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Debuff/Fear.png"), 1))))
		return E_FAIL;

	/* Dialogue ================================================================================== */

	/* For.Prototype_Component_Texture_Dialogue_Box */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Dialogue_Box"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/Dialogue Box.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Kril_dialogue */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Kril_dialogue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/Kril/Kril_dialogue_%d.png"), 23))))
		return E_FAIL;

	///* For.Prototype_Component_Texture_NPC_AppleKing_dialogue */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_NPC_AppleKing_dialogue"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_AppleKing/NPC_AppleKing_dialogue.png"), 1))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_NPC_Chitan_dialogue */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_NPC_Chitan_dialogue"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Chitan/NPC_Chitan_dialogue_%d.png"), 4))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_NPC_Konche_dialogue */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_NPC_Konche_dialogue"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Konche/NPC_Konche_dialogue_%d.png"), 6))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_NPC_Tortellini_dialogue */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_NPC_Tortellini_dialogue"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialogue/NPC_Tortellini/NPC_Tortellini_dialogue.png"), 1))))
	//	return E_FAIL;

	/* Grapple ================================================================================== */

	/* For.Prototype_Component_Texture_Grapple */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Grapple"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Grapple/Grapple_%d.png"), 2))))
		return E_FAIL;

	/* Icon ================================================================================== */

	/* For.Prototype_Component_Texture_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon_%d.png"), 12))))
		return E_FAIL;

	/* Logo ================================================================================== */

	/* For.Prototype_Component_Texture_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo/Logo.dds"), 1))))
		return E_FAIL;

	/* Mask ================================================================================== */

	/* For.Prototype_Component_Texture_Dissolve */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Mask/Dissolve.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_DitherBlueNoise */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_DitherBlueNoise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Mask/DitherBlueNoise.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Noise */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Noise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Mask/Noise.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SandNoise */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_SandNoise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Mask/SandNoise.dds"), 1))))
		return E_FAIL;

	/* Menu ================================================================================== */

	/* For.Prototype_Component_Texture_MenuButton */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_MenuButton"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuButton.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MenuDot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_MenuDot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuDot.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MenuGrass */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_MenuGrass"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuGrass.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MenuHeader */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_MenuHeader"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuHeader.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MenuStar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_MenuStar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuStar.png"), 1))))
		return E_FAIL;

	/* MoonSnail ================================================================================== */

	/* For.Prototype_Component_Texture_Arrow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Arrow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/Arrow.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Decoration */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Decoration"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/Decoration.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MoonsnailFilled */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_MoonsnailFilled"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/MoonsnailFilled.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MoonsnailOutline */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_MoonsnailOutline"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/MoonsnailOutline.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_PrevBorder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_PrevBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/PrevBorder.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_PrevFilled */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_PrevFilled"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/PrevFilled.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_RectBorder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_RectBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/RectBorder.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Star */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/Star.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Star_8Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Star_8Point"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/Star_8Point.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_WindowBorder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_WindowBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/WindowBorder.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_WindowFilled */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_WindowFilled"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MoonSnail/WindowFilled.png"), 1))))
		return E_FAIL;

	/* Notice ================================================================================== */

	/* For.Prototype_Component_Texture_Notice */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Notice"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Notice/NoticeBorder.dds"), 1))))
		return E_FAIL;

	/* NPC ================================================================================== */

	/* For.Prototype_Component_Texture_ChatBubble */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_ChatBubble"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/NPC/ChatBubble.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_LocationIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_LocationIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/NPC/LocationIcon_%d.png"), 6))))
		return E_FAIL;

	/* Passenger ================================================================================== */

	/* For.Prototype_Component_Texture_StowawayStar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_StowawayStar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Passenger/StowawayStar.dds"), 1))))
		return E_FAIL;

	/* Peril ================================================================================== */

	/* For.Prototype_Component_Texture_Peril */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Peril"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Peril/Peril_%d.png"), 2))))
		return E_FAIL;

	/* PlayerUI ================================================================================== */

	/* For.Prototype_Component_Texture_Crystal */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Crystal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/Crystal.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CurrencyBorder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_CurrencyBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/CurrencyBorder.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Ghost */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Ghost"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/Ghost.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_HeartKelpCount */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_HeartKelpCount"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/HeartKelpCount.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_HookCount */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_HookCount"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/HookCount.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_HurtBorder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_HurtBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/HurtBorder.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_IconBorder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_IconBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/IconBorder.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_IconBorder2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_IconBorder2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/IconBorder2.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Microplastic */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Microplastic"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/Microplastic.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Ring */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/Ring.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_TechniqueBorder */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_TechniqueBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/TechniqueBorder.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Umami */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Umami"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/Umami_%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UmamiWhite */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_UmamiWhite"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/UmamiWhite.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_RemainLocationIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_RemainLocationIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PlayerUI/RemainLocationIcon.dds"), 1))))
		return E_FAIL;
	
	/* Recycle ================================================================================== */

	/* For.Prototype_Component_Texture_Recycle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Recycle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Recycle/Recycle_%d.dds"), 10))))
		return E_FAIL;

	/* Shell ================================================================================== */

	/* For.Prototype_Component_Texture_Shell_Def */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Shell_Def"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shell/Def.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Shell_Weight */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Shell_Weight"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shell/weight.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Shell */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Shell"), 
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shell/Shell_%d.png"), 3))))
		return E_FAIL;

	/* Skill ================================================================================== */

	/* For.Prototype_Component_Texture_Skill */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Skill"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Skill/Skill_%d.png"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SkillChoice */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_SkillChoice"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Skill/SkillChoice.png"), 1))))
		return E_FAIL;

	/* Sprite ================================================================================== */

	/* For.Prototype_Component_Texture_Keyboard */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Keyboard"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Sprite/Keyboard.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CuteKeyboard */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_CuteKeyboard"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Sprite/CuteKeyboard.dds"), 1))))
		return E_FAIL;

	/* Stat ================================================================================== */

	/* For.Prototype_Component_Texture_StatIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_StatIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stat/Icon_%d.png"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ExtraIcons */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_ExtraIcons"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stat/ExtraIcons.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Outline_Def */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Outline_Def"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stat/Outline_DEF.png"), 1))))
		return E_FAIL;

	/* Stretch ================================================================================== */

	/* For.Prototype_Component_Texture_BarBorder_Armor */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_BarBorder_Armor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stretch/BarBorder_Armor.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BarBorder_Health */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_BarBorder_Health"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stretch/BarBorder_Health.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BarBoss */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_BarBoss"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stretch/BarBoss.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_RampHp */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_RampHp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stretch/RampHp.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_RampShell */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_RampShell"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stretch/RampShell.dds"), 1))))
		return E_FAIL;
	
	/* Technique ================================================================================== */

	/* For.Prototype_Component_Texture_Technique */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_Component_Texture_Technique"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Technique/Tech_%d.png"), 3))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("객체 원형을 생성"));

	/*For. Prototype_GameObject_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVELID::LEVEL_UITOOL, TEXT("Prototype_GameObject_UI"), CUI_Dummy::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	m_iCurrentLine = m_iNumTotalLine;

	lstrcpyW(m_szLoadingText, TEXT("로딩끝."));
	m_bIsFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Create : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	Engine::CBase::Free();

	WaitForSingleObject(m_hThread, INFINITE);
	//DeleteObject(m_hThread); // 이거 결국 사용 안하는듯, 리턴값도 false 임
	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}

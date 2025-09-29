#include "pch.h"
#include "UI_Game.h"

#include "GameInstance.h"
#include "UI_FixStretch.h"
#include "UI_FixStretchBar.h"
#include "UI_Texture.h"
#include "UI_DynamicTexture.h"
#include "UI_Effect.h"

CUI_Game::CUI_Game(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Game::CUI_Game(const CUI_Game& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_Game::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_Game::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	pDesc->UIObjectDesc.fSizeX = g_iWindowSizeX;
	pDesc->UIObjectDesc.fSizeY = g_iWindowSizeY;
	pDesc->UIObjectDesc.fX = g_iWindowSizeX * 0.5f;
	pDesc->UIObjectDesc.fY = g_iWindowSizeY * 0.5f;
	pDesc->UIObjectDesc.fZ = 0.01f;

	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(CUIObject::Initialize(&pDesc->UIObjectDesc)))
		return E_FAIL;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	m_pInputable = pDesc->pInputable;

	return S_OK;
}

void CUI_Game::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_Game::Update(_float fTimeDelta)
{
	if (*m_pInputable)
	{
		m_fCurTime -= fTimeDelta;

		// 0
		if (m_pGameInstance->Key_Down(KEY::UP))
		{
			if (m_vecDirection[m_iCurIndex] == 0)
			{
				m_pGameInstance->Play_Sound(L"Game.wav", (_uint)SOUND_CHANNEL::UI, 0.5f);
				m_pUIKey[m_vecIndex[m_iCurIndex]]->Change_SrcColor({ 1.f, 1.f, 0.f, 1.f });
			}
			else
			{
				m_pGameInstance->Play_Sound(L"GameFail.wav", (_uint)SOUND_CHANNEL::UI, 0.5f);
				m_pUIKey[m_vecIndex[m_iCurIndex]]->Change_SrcColor({ 1.f, 1.f, 0.f, 1.f });
				m_bFail = true;
				*m_pInputable = false;
			}
			m_pUIKeyEffect[m_vecIndex[m_iCurIndex]]->Show();
			m_iCurIndex++;
		}
		else if (m_pGameInstance->Key_Down(KEY::DOWN))
		{
			if (m_vecDirection[m_iCurIndex] == 1)
			{
				m_pGameInstance->Play_Sound(L"Game.wav", (_uint)SOUND_CHANNEL::UI, 0.5f);
				m_pUIKey[m_vecIndex[m_iCurIndex]]->Change_SrcColor({ 1.f, 1.f, 0.f, 1.f });
			}
			else
			{
				m_pGameInstance->Play_Sound(L"GameFail.wav", (_uint)SOUND_CHANNEL::UI, 0.5f);
				m_pUIKey[m_vecIndex[m_iCurIndex]]->Change_SrcColor({ 1.f, 1.f, 0.f, 1.f });
				m_bFail = true;
				*m_pInputable = false;
			}
			m_pUIKeyEffect[m_vecIndex[m_iCurIndex]]->Show();
			m_iCurIndex++;
		}
		else if (m_pGameInstance->Key_Down(KEY::LEFT))
		{
			if (m_vecDirection[m_iCurIndex] == 2)
			{
				m_pGameInstance->Play_Sound(L"Game.wav", (_uint)SOUND_CHANNEL::UI, 0.5f);
				m_pUIKey[m_vecIndex[m_iCurIndex]]->Change_SrcColor({ 1.f, 1.f, 0.f, 1.f });
			}
			else
			{
				m_pGameInstance->Play_Sound(L"GameFail.wav", (_uint)SOUND_CHANNEL::UI, 0.5f);
				m_pUIKey[m_vecIndex[m_iCurIndex]]->Change_SrcColor({ 1.f, 1.f, 0.f, 1.f });
				m_bFail = true;
				*m_pInputable = false;
			}
			m_pUIKeyEffect[m_vecIndex[m_iCurIndex]]->Show();
			m_iCurIndex++;
		}
		else if (m_pGameInstance->Key_Down(KEY::RIGHT))
		{
			if (m_vecDirection[m_iCurIndex] == 3)
			{
				m_pGameInstance->Play_Sound(L"Game.wav", (_uint)SOUND_CHANNEL::UI, 1.f);
				m_pUIKey[m_vecIndex[m_iCurIndex]]->Change_SrcColor({ 1.f, 1.f, 0.f, 1.f });
			}
			else
			{
				m_pGameInstance->Play_Sound(L"GameFail.wav", (_uint)SOUND_CHANNEL::UI, 1.f);
				m_pUIKey[m_vecIndex[m_iCurIndex]]->Change_SrcColor({ 1.f, 1.f, 0.f, 1.f });
				m_bFail = true;
				*m_pInputable = false;
			}
			m_pUIKeyEffect[m_vecIndex[m_iCurIndex]]->Show();
			m_iCurIndex++;
		}

		// 안틀리고 끝
		if (m_iCurIndex == m_vecDirection.size())
		{
			m_bSuccess = true;
			*m_pInputable = false;
		}

		if (!m_bSuccess && m_fCurTime <= 0.f)
		{
			m_bFail = true;
			*m_pInputable = false;
		}
	}
	super::Update(fTimeDelta);
}

void CUI_Game::Late_Update(_float fTimeDelta)
{
	if (*m_pInputable)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
		super::Late_Update(fTimeDelta);
	}
}

HRESULT CUI_Game::Render()
{
	return S_OK;
}

void CUI_Game::Set_Direction(_int iLevel)
{
	m_iCurLevel = iLevel;

	m_fCurTime = m_fMaxTime;

	m_iCurIndex = 0;
	m_vecDirection.clear();
	m_vecIndex.clear();

	m_bSuccess = false;
	m_bFail = false;

	for (_int i = 0; i < 2 * m_iCurLevel + 3; i++)
	{
		_int iRandomDirection = CUtils::Compute_Random(0, 3);
		m_vecDirection.push_back(iRandomDirection);
	}

	m_pClock->Set_Locate({ 825.f + 69.f * m_iCurLevel, 50.f, 0.2f });
	
	_int iEndFactor = 10 + 27 * m_iCurLevel;

	m_pBorder->Change_EndFactor(iEndFactor);
	m_pBar->Change_EndFactor(iEndFactor * 1.1f);

	_int iIndex = 0;

	for (_int i = 0; i < 13; i++)
	{
		if (i >= 5 - m_iCurLevel && i <= 7 + m_iCurLevel)
		{
			m_pUIKey[i]->Set_IsActive(true);
			m_pUIKey[i]->Change_SrcColor({ 1.f, 1.f, 1.f, 1.f });
			m_pUIKey[i]->Set_SpriteXY(m_vecDirection[iIndex], 3);
			m_pUIKeyEffect[i]->Set_SpriteXY(m_vecDirection[iIndex++], 3);

			// 키보드 인덱스를 담는다
			m_vecIndex.push_back(i);
		}
		else
		{
			m_pUIKey[i]->Set_IsActive(false);
			m_pUIKeyEffect[i]->Set_IsActive(false);
		}
	}
}

HRESULT CUI_Game::Ready_PartObject()
{
	/* Bar */
	CUI_FixStretch::DESC FixStretchDesc{};
	{
		FixStretchDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Club/UI_Bar.dat";

		FixStretchDesc.fChangeRatio = 1.f;
		FixStretchDesc.bMiddle = true;

		FixStretchDesc.fStartFactor = 1.f;
		FixStretchDesc.fEndFactor = 10.f;
		FixStretchDesc.fStretchSpeed = 5.f;

		FixStretchDesc.pCur = &m_fCurTime;
		FixStretchDesc.pMax = &m_fMaxTime;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Bar", LEVEL_STATIC, L"Prototype_GameObject_UI_FixStretch", &FixStretchDesc)))
		return E_FAIL;

	m_pBar = static_cast<CUI_FixStretch*>(Find_PartObject({ L"Part_UI_Bar" }));

	/* Border */
	CUI_FixStretchBar::DESC FixStretchBarDesc{};
	{
		FixStretchBarDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Club/UI_Border.dat";

		FixStretchBarDesc.fChangeRatio = 1.f;
		FixStretchBarDesc.bMiddle = true;
		
		FixStretchBarDesc.fStartFactor = 1.f;
		FixStretchBarDesc.fEndFactor = 10.f;
		FixStretchBarDesc.fStretchSpeed = 5.f;

		FixStretchBarDesc.pCur = &m_fCurTime;
		FixStretchBarDesc.pMax = &m_fMaxTime;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Border", LEVEL_STATIC, L"Prototype_GameObject_UI_FixStretchBar", &FixStretchBarDesc)))
		return E_FAIL;

	m_pBorder = static_cast<CUI_FixStretchBar*>(Find_PartObject({ L"Part_UI_Border" }));

	CUI_Texture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Club/UI_Clock.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Clock", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pClock = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_Clock"}));

	CUI_DynamicTexture::DESC KeyboardDesc{};
	CUI_Effect::DESC EffectDesc{};

	_float2 vStart = { 220.f, 49.f };
	_float fOffsetX = 70.f;

	KeyboardDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Club/UI_Keyboard.dat";
	EffectDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/Club/UI_Keyboard.dat";

	for (_int i = 0; i < 13; i++)
	{
		_wstring strTag = L"Part_UI_Keyboard" + to_wstring(i);
		_wstring strEffectTag = L"Part_UI_KeyboardEffect" + to_wstring(i);
		_float2 vLocate = { vStart.x + fOffsetX * i, vStart.y };

		KeyboardDesc.vLocate = vLocate;
		EffectDesc.vLocate = vLocate;

		if (FAILED(Add_PartObject(strTag, LEVEL_STATIC, L"Prototype_GameObject_UI_DynamicTexture", &KeyboardDesc)))
			return E_FAIL;

		if (FAILED(Add_PartObject(strEffectTag, LEVEL_STATIC, L"Prototype_GameObject_UI_Effect", &EffectDesc)))
			return E_FAIL;

		m_pUIKey[i] = static_cast<CUI_DynamicTexture*>(Find_PartObject({ strTag }));
		m_pUIKeyEffect[i] = static_cast<CUI_Effect*>(Find_PartObject({ strEffectTag }));
	}
	return S_OK;
}

CUI_Game* CUI_Game::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Game* pInstance = new CUI_Game(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_Game");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_Game::Clone(void* pArg)
{
	CUI_Game* pInstance = new CUI_Game(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_Game");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Game::Free()
{
    CUIObject::Free();
}


#include "pch.h"
#include "UI_PlayerRight.h"

#include "GameInstance.h"
#include "Player.h"
#include "Skill_Database.h"
#include "UI_Manager.h"

#include "UI_Value.h"
#include "UI_Texture.h"
#include "UI_SkillEffect.h"


CUI_PlayerRight::CUI_PlayerRight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_PlayerRight::CUI_PlayerRight(const CUI_PlayerRight& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUI_PlayerRight::Initialize_Prototype()
{
	if (FAILED(CUIObject::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUI_PlayerRight::Initialize(void* pArg)
{
	DESC* pDesc = static_cast<DESC*>(pArg);

	pDesc->UIObjectDesc.fSizeX = g_iWindowSizeX;
	pDesc->UIObjectDesc.fSizeY = g_iWindowSizeY;
	pDesc->UIObjectDesc.fX = g_iWindowSizeX * 0.5f;
	pDesc->UIObjectDesc.fY = g_iWindowSizeY * 0.5f;
	pDesc->UIObjectDesc.fZ = 0.01f;

	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	pDesc->UIObjectDesc.GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	m_pIsTalking = pDesc->pIsTalking;

	if (FAILED(CUIObject::Initialize(&pDesc->UIObjectDesc)))
		return E_FAIL;

	m_pPlayerStat = pDesc->pPlayerStat;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	m_vInitLocate[0] = m_pUISkill[0]->Get_Locate();
	m_vInitLocate[1] = m_pUISkill[1]->Get_Locate();

	m_vCenter = (((_vec3)m_vInitLocate[0] + (_vec3)m_vInitLocate[1]) / 2.f)._float3();

	m_vInitScale[0] = m_pUISkill[0]->Get_Scale();
	m_vInitScale[1] = m_pUISkill[1]->Get_Scale();

	_float fX = m_vInitLocate[0].x - m_vInitLocate[1].x;
	_float fY = m_vInitLocate[0].y - m_vInitLocate[1].y;
	m_fRadius = sqrtf(fX * fX + fY * fY) * 0.5f;

	if (m_pPlayerStat->ePrimaryActiveSkill != ACTIVESKILL::ACTIVESKILL_END)
		m_pUISkill[0]->Change_TextureNum((_uint)m_pPlayerStat->ePrimaryActiveSkill);
	else
		m_pUISkill[0]->Set_IsActive(false);

	if (m_pPlayerStat->eSecondaryActiveSkill != ACTIVESKILL::ACTIVESKILL_END)
		m_pUISkill[1]->Change_TextureNum((_uint)m_pPlayerStat->eSecondaryActiveSkill);
	else
		m_pUISkill[1]->Set_IsActive(false);

	return S_OK;
}

void CUI_PlayerRight::Priority_Update(_float fTimeDelta)
{
	super::Priority_Update(fTimeDelta);
}

void CUI_PlayerRight::Update(_float fTimeDelta)
{
	if (m_pPlayerStat->ePrimaryActiveSkill == ACTIVESKILL::ACTIVESKILL_END)
	{
		if (m_pUISkill[0]->Get_Scale().x > m_pUISkill[1]->Get_Scale().x)
			m_pUISkill[0]->Set_IsActive(false);
		else 
			m_pUISkill[1]->Set_IsActive(false);
	}
	else
	{
		if (!m_bSwapping)
		{
			if (m_pUISkill[0]->Get_Scale().x > m_pUISkill[1]->Get_Scale().x)
			{
				m_pUISkill[0]->Change_TextureNum((_uint)m_pPlayerStat->ePrimaryActiveSkill);
				m_pUISkill[0]->Set_IsActive(true);
			}
			else
			{
				m_pUISkill[1]->Change_TextureNum((_uint)m_pPlayerStat->ePrimaryActiveSkill);
				m_pUISkill[1]->Set_IsActive(true);
			}
		}
	}

	if (m_pPlayerStat->eSecondaryActiveSkill == ACTIVESKILL::ACTIVESKILL_END)
	{
		if (m_pUISkill[0]->Get_Scale().x > m_pUISkill[1]->Get_Scale().x)
			m_pUISkill[1]->Set_IsActive(false);
		else
			m_pUISkill[0]->Set_IsActive(false);
	}
	else
	{
		if (!m_bSwapping)
		{
			if (m_pUISkill[0]->Get_Scale().x > m_pUISkill[1]->Get_Scale().x)
			{
				m_pUISkill[1]->Change_TextureNum((_uint)m_pPlayerStat->eSecondaryActiveSkill);
				m_pUISkill[1]->Set_IsActive(true);
			}
			else
			{
				m_pUISkill[0]->Change_TextureNum((_uint)m_pPlayerStat->eSecondaryActiveSkill);
				m_pUISkill[0]->Set_IsActive(true);
			}
		}
	}

	if (m_pUISkill[0]->Get_IsActive() || m_pUISkill[1]->Get_IsActive())
		Setting_SkillColor();

	if (m_pUISkill[0]->Get_IsActive() && m_pUISkill[1]->Get_IsActive())
		Swap_Skill(fTimeDelta);

	super::Update(fTimeDelta);
}

void CUI_PlayerRight::Late_Update(_float fTimeDelta)
{
	//if (!m_pPlayerStat->bTalking)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);

		super::Late_Update(fTimeDelta);
	}
}

HRESULT CUI_PlayerRight::Render()
{
    return S_OK;
}

HRESULT CUI_PlayerRight::Ready_PartObject()
{
	/* Currency */
	CUI_Texture::DESC TextureDesc{};
	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerRight/UI_Currency.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Currency", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Layer_UIManager", 0));

	/* Microplastic */
	CUI_Value::DESC ValueDesc{};
	{
		ValueDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerRight/UI_Microplastic.dat";

		ValueDesc.pCount = &pUIManager->Get_Micro_Ref();
	}
	if (FAILED(Add_PartObject(L"Part_UI_Microplastic", LEVEL_STATIC, L"Prototype_GameObject_UI_Value", &ValueDesc)))
		return E_FAIL;

	/* TechniqueBorder */
	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerRight/UI_TechniqueBorder.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_TechniqueBorder", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	/* Skill */
	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerRight/UI_BigSkill.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_BigSkill", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUISkill[0] = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_BigSkill" }));

	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerRight/UI_SmallSkill.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_SmallSkill", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	m_pUISkill[1] = static_cast<CUI_Texture*>(Find_PartObject({ L"Part_UI_SmallSkill" }));

	CUI_SkillEffect::DESC EffectDesc{};
	{
		EffectDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerRight/UI_SkillEffect.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_SkillEffect", LEVEL_STATIC, L"Prototype_GameObject_UI_SkillEffect", &EffectDesc)))
		return E_FAIL;

	m_pUIEffect = static_cast<CUI_SkillEffect*>(Find_PartObject({ L"Part_UI_SkillEffect" }));

	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerRight/UI_Tab.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Tab", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerRight/UI_Q.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_Q", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	/* HookBorder */
	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerRight/UI_HookBorder.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_HookBorder", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	/* Hook */
	{
		ValueDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerRight/UI_Hook.dat";

		ValueDesc.pCount = &m_pPlayerStat->iHookCount;
	}
	if (FAILED(Add_PartObject(L"Part_UI_Hook", LEVEL_STATIC, L"Prototype_GameObject_UI_Value", &ValueDesc)))
		return E_FAIL;

	/* HeartKelpBorder */
	{
		TextureDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerRight/UI_HeartKelpBorder.dat";
	}
	if (FAILED(Add_PartObject(L"Part_UI_HeartKelpBorder", LEVEL_STATIC, L"Prototype_GameObject_UI_Texture", &TextureDesc)))
		return E_FAIL;

	/* HeartKelp */
	{
		ValueDesc.UIDesc.strFileName = L"../Bin/DataFiles/UI_Data/PlayerRight/UI_HeartKelp.dat";

		ValueDesc.pCount = &m_pPlayerStat->iHeartKelpCount;
	}
	if (FAILED(Add_PartObject(L"Part_UI_HeartKelp", LEVEL_STATIC, L"Prototype_GameObject_UI_Value", &ValueDesc)))
		return E_FAIL;

	return S_OK;
}

void CUI_PlayerRight::Init()
{
	m_vInitLocate[0] = m_pUISkill[0]->Get_Locate();
	m_vInitLocate[1] = m_pUISkill[1]->Get_Locate();

	m_vInitScale[0] = m_pUISkill[0]->Get_Scale();
	m_vInitScale[1] = m_pUISkill[1]->Get_Scale();
}

void CUI_PlayerRight::Swap_Skill(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(KEY::TAB) && !m_bSwapping)
	{
		m_pGameInstance->Play_Sound(L"Tab.wav", (_uint)SOUND_CHANNEL::UI, 0.8f);
		Init();
		m_bSwapping = true;
		m_fSwapLerp = 0.f;
		m_bClockwise = !m_bClockwise;
	}

	if (m_bSwapping)
	{
		const _float fSpeed = 3.f;
		m_fSwapLerp += fTimeDelta * fSpeed;

		if (m_fSwapLerp >= 1.f)
		{
			m_fSwapLerp = 1.f;
			m_bSwapping = false;
			
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObjects_In_Layer(m_pGameInstance->Get_CurrentLevelIndex(), L"Layer_Player").front());
			pPlayer->Swap_ActiveSkill();
		}

		// 원형 회전용 각도 구하기
		_float fAngle0 = atan2f(m_vInitLocate[0].y - m_vCenter.y, m_vInitLocate[0].x - m_vCenter.x);
		_float fAngle1 = fAngle0 + (m_bClockwise ? XM_PI : -XM_PI);

		// 보간된 각도 계산
		_float fNewAngle0 = fAngle0 + (fAngle1 - fAngle0) * m_fSwapLerp;
		_float fNewAngle1 = fNewAngle0 + XM_PI;

		// 보간된 위치 계산
		_float3 vNewPos0 = {};
		XMStoreFloat3(&vNewPos0,
			{
			m_vCenter.x + cosf(fNewAngle0) * m_fRadius,
			m_vCenter.y + sinf(fNewAngle0) * m_fRadius,
			0.1f
			});
		_float3 vNewPos1 = {};
		XMStoreFloat3(&vNewPos1,
			{
			m_vCenter.x + cosf(fNewAngle1) * m_fRadius,
			m_vCenter.y + sinf(fNewAngle1) * m_fRadius,
			0.1f
			})
			;

		// 위치 적용
		m_pUISkill[0]->Set_Locate(vNewPos0);
		m_pUISkill[1]->Set_Locate(vNewPos1);

		// 스케일 보간
		_float2 vNewScale0 = {};
		XMStoreFloat2(&vNewScale0, ((_vec2)m_vInitScale[0] + ((_vec2)m_vInitScale[1] - (_vec2)m_vInitScale[0]) * m_fSwapLerp)._vector());
		_float2 vNewScale1 = {};
		XMStoreFloat2(&vNewScale1, ((_vec2)m_vInitScale[1] + ((_vec2)m_vInitScale[0] - (_vec2)m_vInitScale[1]) * m_fSwapLerp)._vector());

		m_pUISkill[0]->Set_Scale(vNewScale0);
		m_pUISkill[1]->Set_Scale(vNewScale1);
	}
}

void CUI_PlayerRight::Setting_SkillColor()
{
	for (_int i = 0; i < 2; i++)
	{
		_uint iTextureNum = m_pUISkill[i]->Get_TextureNum();
		_float fSkillPoint = CSkill_DataBase::Get().Get_ActiveSkill((ACTIVESKILL)iTextureNum).fSkillPoint;

		if (m_pPlayerStat->fCurrSkillPoint < fSkillPoint)
			m_pUISkill[i]->Change_SrcColor({ 0.5f, 0.5f, 0.5f, 0.5f });
		else
		{
			switch (iTextureNum)
			{
			case 0:
				m_pUISkill[i]->Change_SrcColor({ 0.8f, 0.f, 1.f, 1.f });
				break;
			case 1:
				m_pUISkill[i]->Change_SrcColor({ 0.f, 0.f, 1.f, 1.f });
				break;
			case 2:
				break;
			}
		}
	}
}

CUI_PlayerRight* CUI_PlayerRight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PlayerRight* pInstance = new CUI_PlayerRight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_PlayerRight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCloneable* CUI_PlayerRight::Clone(void* pArg)
{
	CUI_PlayerRight* pInstance = new CUI_PlayerRight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_PlayerRight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PlayerRight::Free()
{
    CUIObject::Free();
}


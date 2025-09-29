#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

struct FPlayerStat;

class CUI_Texture;
class CUI_SkillEffect;

class CUI_PlayerRight final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;

		FPlayerStat* pPlayerStat;
		_bool* pIsTalking = { nullptr };
	};

private:
	CUI_PlayerRight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerRight(const CUI_PlayerRight& Prototype);
	virtual ~CUI_PlayerRight() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	FPlayerStat* m_pPlayerStat = { nullptr };
	_bool* m_pIsTalking = { nullptr };

	CUI_Texture* m_pUISkill[2] = { nullptr };
	CUI_SkillEffect* m_pUIEffect = { nullptr };

	_float3		m_vInitLocate[2] = {};
	_float2		m_vInitScale[2] = {};
	_float3		m_vCenter = {};
	_float		m_fRadius = {};

	_bool  m_bSwapping = { false };
	_bool m_bClockwise = { false };

	_float m_fSwapLerp = { 0.f };
private:
	HRESULT Ready_PartObject();
	
	void Init();
	void Swap_Skill(_float fTimeDelta);
	void Setting_SkillColor();

public:
	static CUI_PlayerRight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END

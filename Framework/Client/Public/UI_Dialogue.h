#pragma once

#include "Client_Defines.h"
#include "UIObject.h"
#include "FData.h"

BEGIN(Engine)

class CTexture;
class CShader;
class CVIBuffer_Rect;

END

BEGIN(Client)

struct FPlayerStat;

class CPlayer;
class CUI_Speaker;
class CUI_DialogueBox;

class CUI_Dialogue final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;
	};

private:
	CUI_Dialogue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Dialogue(const CUI_Dialogue& Prototype);
	virtual ~CUI_Dialogue() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

	void Set_Dialogue(_int iFileNum, _int iCurState, std::function<void()> Callback = {});

	_int Get_CurDialogueIndex() { return m_iCurIndex; }

private:
	FDialogueData m_tData = {};

	_int m_iCurFileNum = {};
	_int m_iCurState = {};

	_int m_iCurIndex = {-1};
	_int m_vecSize = {};

	CUI_Speaker* m_pUISpeakerKril = { nullptr };
	CUI_Speaker* m_pUISpeakerOther = { nullptr };

	CUI_DialogueBox* m_pUIBoxLeft = {nullptr};
	CUI_DialogueBox* m_pUIBoxRight = {nullptr};

	map<_wstring, _wstring> m_mapTextureTag;
	vector<FDialogueData> m_vecCurDialogueData;

	std::function<void()> m_Callback = {};

private:
	HRESULT Ready_PartObject();

	void Set_TextureTag();
	void Proceed_Dialogue();

public:
	static CUI_Dialogue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END

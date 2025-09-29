#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

#include "Enemy.h"

BEGIN(Client)

struct EnemyStats;

class CUI_FixStretch;
class CUI_FixStretchBar;
class CUI_BallanceBar;
class CUI_BallanceBorder;

class CUI_Boss final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;
		const _float4x4* pCenterMatrixPtr{ nullptr };

		EnemyStats* pEnemyStats = { nullptr };
		_bool* pIsAggroed = { nullptr };
		_bool* pIsDead = { nullptr };
		_tchar szName[MAX_PATH] = {};

		_bool* pIsGrab = { nullptr };
		_bool* pIsUnblockable = { nullptr };
	};

private:
	CUI_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Boss(const CUI_Boss& Prototype);
	virtual ~CUI_Boss() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	EnemyStats* m_pEnemyStats = { nullptr };

	CUI_FixStretch* m_pHpBorder = { nullptr };
	CUI_FixStretchBar* m_pHpBar = { nullptr };

	CUI_BallanceBar* m_pBallanceBar = { nullptr };
	CUI_BallanceBorder* m_pBallanceBorder = { nullptr };
	
	_bool*			m_pIsAggroed = { nullptr };
	_bool			m_bIsAggroed = { false };
	_bool*			m_pIsDead = { nullptr };

	_bool* m_pIsGrab = { nullptr };
	_bool* m_pIs = { nullptr };

	_tchar			m_szName[MAX_PATH] = {};

private:
	const _float4x4* m_pCenterMatrixPtr{ nullptr };

private:
	HRESULT Ready_PartObject();

public:
	static CUI_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END

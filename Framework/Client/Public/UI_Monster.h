#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

#include "Enemy.h"

BEGIN(Engine)

class CTexture;
class CShader;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CUI_BallanceBar;
class CUI_BallanceBorder;

class CUI_Monster final : public CUIObject
{
	using super = CUIObject;

public:
	struct DESC
	{
		CUIObject::UIOBJECT_DESC UIObjectDesc;

		EnemyStats* pEnemyStats = { nullptr };
		const _float4x4* pMonsterWorld;

		_bool* pIsAggroed = { nullptr };
		_bool* pIsDead = { nullptr };

		_bool* pIsGrab = { nullptr };
		_bool* pIsUnblockable = { nullptr };
	};

private:
	CUI_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Monster(const CUI_Monster& Prototype);
	virtual ~CUI_Monster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	EnemyStats* m_pEnemyStats = { nullptr };
	const _float4x4* m_pMonsterWorld;

	_bool* m_pIsAggroed = { nullptr };
	_bool* m_pIsDead = { nullptr };
	CUI_BallanceBar* m_pBallanceBar = { nullptr };
	CUI_BallanceBorder* m_pBallanceBorder = { nullptr };

	_bool* m_pIsGrab = { nullptr };
	_bool* m_pIsUnblockable = { nullptr };

private:
	HRESULT Ready_PartObject();

public:
	static CUI_Monster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;

};

END

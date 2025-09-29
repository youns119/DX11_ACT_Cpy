#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

class CLight_Player final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc {};
		CGameObject* pPlayer { nullptr };
	};

private:
	CLight_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLight_Player(const CLight_Player& Prototype);
	virtual ~CLight_Player() = default;	

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;

private:
	CGameObject* m_pPlayer { nullptr };
	const _wstring m_strLightTag { L"Light_Player" };

public:
	static CLight_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};


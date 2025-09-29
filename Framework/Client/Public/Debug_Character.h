#pragma once

#include "Client_Defines.h"

#include "GameObject.h"

BEGIN(Client)

class CPlayer;

class CDebug_Character final
	: public CGameObject
{
	using super = CGameObject;

public:
	CDebug_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDebug_Character(const CDebug_Character& Prototype);
	virtual ~CDebug_Character() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Debug_Player();
	void Debug_Character();

private:
	void Debug_PlayerStat();
	void Debug_MovementComponent(CGameObject* pGameObject);

private:
	CPlayer* m_pPlayer{};

private:
	_uint m_iPickedID{ 0 };

public:
	static CDebug_Character* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
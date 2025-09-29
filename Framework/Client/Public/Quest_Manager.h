#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CQuest_Manager final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
	};

private:
	CQuest_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuest_Manager(const CQuest_Manager& Prototype);
	virtual ~CQuest_Manager() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	static CQuest_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END

#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CDebug_Level final : public CGameObject
{
	using super = CGameObject;

private:
	CDebug_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDebug_Level(const CDebug_Level& Prototype);
	virtual ~CDebug_Level() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	/* private member variable */

private:
	/* private member function */

public:
	static CDebug_Level* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END

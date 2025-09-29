#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CDebug_ObjectInfo final : public CGameObject
{
	using super = CGameObject;

private:
	enum class MODE
	{
		PICK_PIXEL,
		FIND_NAME,

		LAST,
	};
	enum
	{
		MODE_END = MODE::LAST,
	};

private:
	CDebug_ObjectInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDebug_ObjectInfo(const CDebug_ObjectInfo& Prototype);
	virtual ~CDebug_ObjectInfo() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

private:
	CGameObject* m_pPickedGameObject { nullptr };
	MODE m_eMode { MODE::PICK_PIXEL };

private:
	void Find_GameObject_With_ID();
	void Find_GameObject_With_Name();

	void Debug_Transform(CGameObject* pGameObject);
	void Debug_Activation(CGameObject* pGameObject);

public:
	static CDebug_ObjectInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END

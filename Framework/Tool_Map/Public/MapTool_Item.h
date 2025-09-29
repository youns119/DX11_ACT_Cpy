#pragma once

#include "Tool_Define.h"
#include "GameObject.h"

BEGIN(Engine)
//class CShader;
//class CCollider;
END

BEGIN(MapTool)
class CMapTool_Item final : public CGameObject
{
	using super = CGameObject;
public:
	enum class SHELL
	{
		SHELL_HOME,

		SHELL_SODACAN,
		SHELL_MIRRORBALL,

		SHELL_DUCK,
		SHELL_TENNISBALL,

		SHELL_END
	};
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
		SHELL eType{};
	};

private:
	CMapTool_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapTool_Item(const CMapTool_Item& Prototype);
	virtual ~CMapTool_Item() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_int Get_Type() { return m_iItemType; }
private:

	_int	m_iItemType = { -1 };
public:
	static CMapTool_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
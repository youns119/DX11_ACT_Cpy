#pragma once

#include "Tool_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(MapTool)

class CMapTool_NPC final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};
		_wstring strModelTag;
	};

private:
	CMapTool_NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapTool_NPC(const CMapTool_NPC& Prototype);
	virtual ~CMapTool_NPC() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


	HRESULT Ready_Components(const _wstring& strModelTag);

private:
	CModel* m_pModelCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };

public:
	static CMapTool_NPC* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
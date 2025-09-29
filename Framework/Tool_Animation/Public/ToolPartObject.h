#pragma once

#include "ToolAnimation_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CModel;
class CShader;
class CCollider;

END

BEGIN(ToolAnimation)

class CToolPartObject final
	: public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::GAMEOBJECT_DESC PartObjectDesc{};

		const _float4x4* pSocketMatrix{ nullptr };
	};

private:
	CToolPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolPartObject(const CToolPartObject& Prototype);
	virtual ~CToolPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Show(_bool _bShow) { m_bShow = _bShow; }
	void Set_SocketBone(const _float4x4* _pSocketMatrix);

public:
	HRESULT Change_Model(const _wstring& _strModelTag, LEVELID _eLevelID);

private:
	CModel* m_pModelCom{};
	CShader* m_pShaderCom{};
	CCollider* m_pColliderCom{};

private:
	const _float4x4* m_pSocketMatrix{};

private:
	_bool m_bShow{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CToolPartObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
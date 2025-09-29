#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CUI_Manager;

class CNPC_NonInteractive final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		const _tchar* szFileName = L"";
		_wstring strModelTag = L"";

		_uint	iIdleAnimNumber = {};
	};

private:
	CNPC_NonInteractive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_NonInteractive(const CNPC_NonInteractive& Prototype);
	virtual ~CNPC_NonInteractive() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	CModel* m_pModelCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };

	_bool m_bInFrustum = { false };

private:
	void Check_InFrustum();

private:
	HRESULT Load_Data(DESC& Desc);
	HRESULT Ready_Components(DESC& Desc);

public:
	static CNPC_NonInteractive* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
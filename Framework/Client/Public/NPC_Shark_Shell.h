#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CNPC_Shark_Shell final : public CGameObject
{
	using super = CGameObject;

public:
	struct DESC
	{
		CGameObject::DESC GameObjectDesc{};

		const _float4x4* pSocketMatrix{ nullptr };
		const _char* szFilePath{};
		CModel* pModel{ nullptr };
		CGameObject* pOwner{ nullptr };
	};

private:
	CNPC_Shark_Shell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Shark_Shell(const CNPC_Shark_Shell& Prototype);
	virtual ~CNPC_Shark_Shell() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	HRESULT Load_From_File(const _char* szFilePath, CModel* pModel);
	HRESULT Ready_Components();

private:
	CShader*				m_pShaderCom { nullptr };
	CModel*					m_pModelCom { nullptr };

	const _float4x4* m_pSocketMatrix{ nullptr };
	
public:
	static CNPC_Shark_Shell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END

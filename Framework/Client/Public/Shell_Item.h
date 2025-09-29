#pragma once

#include "Client_Defines.h"

#include "Item.h"

#include "Shell_DataBase.h"

BEGIN(Engine)

class CModel;
class CShader;

END

BEGIN(Client)

class CShellComponent;

class CShell_Item abstract
	: public CItem
{
	using super = CItem;

public:
	struct DESC
	{
		CItem::DESC ItemDesc{};

		SHELL eShell{};
		_float fShellCurrHP{};
	};

protected:
	CShell_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShell_Item(const CShell_Item& Prototype);
	virtual ~CShell_Item() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta);
	virtual HRESULT Render() override;

public:
	FShellStat* Get_ShellStat() { return &m_tShellStat; }
	const FShellStat* Get_ShellStat_Const() const { return &m_tShellStat; }

protected:
	CModel* m_pModelCom{};
	CShader* m_pShaderCom{};

protected:
	FShellStat m_tShellStat{};
	FShellData m_tShellData{};

protected:
	virtual HRESULT Ready_Components();

public:
	virtual CCloneable* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END
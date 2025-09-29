#pragma once

#include "Shell_Item.h"

#include "IDamaged.h"

BEGIN(Client)

class CShellSkill;

class CShell_Part final
	: public CShell_Item
	, public IDamaged
{
	using super = CShell_Item;

public :
	struct DESC
	{
		CShell_Item::DESC ShellItemDesc{};

		CGameObject* pOwner{};
	};

protected:
	CShell_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShell_Part(const CShell_Part& Prototype);
	virtual ~CShell_Part() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta);
	virtual HRESULT Render() override;

public :
	_float Get_SkillPoint();

	CShellSkill* Get_ShellSkill() { return m_pSkillCom; }

	CGameObject* Get_Owner() { return m_pOwner; }

public :
	virtual void Damaged(_float fDamage, DAMAGE eDamage, CGameObject* pDamager) override;
	void Broken();

public :
	virtual void Use_Skill();

public :
	virtual void Swap_Model(_bool bSwap);

private :
	CModel* m_pRollModelCom{};

private :
	CShellSkill* m_pSkillCom{};

private:
	const _float4x4* m_pSocketMatrix{};
	const _float4x4* m_pRollSocketMatrix{};

private :
	_float3 m_vOriginScale{}, m_vOriginRotation{}, m_vOriginPosition{};
	_float3 m_vRollScale{}, m_vRollRotation{}, m_vRollPosition{};

private :
	CGameObject* m_pOwner{};

private :
	_bool m_bSwap{};

private:
	HRESULT Load_From_File(const _char* szFilePath);

private:
	HRESULT Ready_Components() override;

public:
	static CShell_Part* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
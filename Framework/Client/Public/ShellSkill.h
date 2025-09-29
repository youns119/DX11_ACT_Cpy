#pragma once

#include "Client_Defines.h"

#include "Component.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CShell_Part;

class CShellSkill abstract
	: public CComponent
{
	using super = CComponent;

public :
	struct DESC
	{
		CShell_Part* pOwner{};

		_float fSkillPoint{};
	};

protected :
	CShellSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShellSkill(const CShellSkill& Prototype);
	virtual ~CShellSkill() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual void Final_Update(_float fTimeDelta);
	HRESULT Render() override;

public :
	_float Get_SkillPoint() const { return m_fSkillPoint; }
	_float Get_SkillDamage() const { return m_fSkillDamage; }

	CGameObject* Get_ShellOwner();

public :
	virtual void Use_Skill();

protected :
	CShell_Part* m_pOwner{};

protected:
	_float m_fSkillPoint{};
	_float m_fSkillDamage{};

protected :
	_float Calculate_Damage();

public:
	virtual CCloneable* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END
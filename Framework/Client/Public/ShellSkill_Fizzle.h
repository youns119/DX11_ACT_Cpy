#pragma once

#include "ShellSkill.h"

BEGIN(Client)

class CProjectile_Fizzle;

class CShellSkill_Fizzle final
	: public CShellSkill
{
	using super = CShellSkill;

private:
	CShellSkill_Fizzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShellSkill_Fizzle(const CShellSkill_Fizzle& Prototype);
	virtual ~CShellSkill_Fizzle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta);
	HRESULT Render() override;

public:
	virtual void Use_Skill() override;

public:
	static CShellSkill_Fizzle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once

#include "ShellSkill.h"

BEGIN(Engine)

class CMovement;
class CEffectObject;

END

BEGIN(Client)

class CPlayer;

class CShellSkill_RollOut final
	: public CShellSkill
{
	using super = CShellSkill;

private:
	CShellSkill_RollOut(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShellSkill_RollOut(const CShellSkill_RollOut& Prototype);
	virtual ~CShellSkill_RollOut() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta);

public :
	virtual void Use_Skill() override;
	void Cancel_Skill();

private:
	void Call_RollingEffects();
	void Call_NonLoopEffects();
	void Control_RollingEffects();
	void Disable_RollingEffects();

private :
	CPlayer* m_pPlayer{};

private :
	CTransform* m_pTransform{};

private :
	_bool m_bRoll{};
	_float m_fSkillTimeAcc{};
	_float m_fEffectTimeAcc{};
	const _float m_fSkillDuration{ 5.f };
	const _float m_fEffectDuration{ 0.15f };
	const _float m_fRotationSpeed{ 40.f };

private:
	vector<CEffectObject*>	m_EffectObjects;
	CEffectObject*			m_pSparkEffect{ nullptr };

public:
	static CShellSkill_RollOut* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
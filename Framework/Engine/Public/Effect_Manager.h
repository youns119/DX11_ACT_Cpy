#pragma once
#include "Base.h"

BEGIN(Engine)

class CGameInstance;
class CEffectObject;

class CEffect_Manager final : public CBase
{
	using super = CBase;

private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	HRESULT Initialize();
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

	void Clear();

	CEffectObject* Call_Effect(_uint iEffectType, _fvector vStartPos, _fvector vLookAt, _bool bIsPartObject, const _float4x4* pAdoptedWorldMatrix, _float fLookRotateDegree, _bool bFlip, const _float4x4* pParentWorldMatrix);
	HRESULT Pool_Effect(_uint iEffectType, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iEffectNum, void* pArg);
	void	Call_Lag(_float fLagRatio, _float fLagTime) 
	{
		m_fLagRatio = fLagRatio * 0.5f;
		m_fLagTime = fLagTime * 0.25f; 
	}
private:
	CGameInstance*						m_pGameInstance{ nullptr };
	map<_uint, vector<CEffectObject*>>	m_Effects;
	_float								m_fLagTime{};
	_float								m_fLagRatio{};

public:
	static CEffect_Manager* Create();
	virtual void Free() override;

};

END
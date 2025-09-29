#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameInstance;
class CLayer;
class CGameObject;
class CComponent;

/*1. 원형객체를 복제하여 == CPrototype_Manager에 추가해놓은 원형객체를 클론하여 오브젝트 매니져에 보관하낟. */
/*2. 보관시에 CLayer클래스에 분류하여 객체들을 저장해놓자. */
/*3. 보관하고 있는 객체들을 반복적으로 Update를 호출해준다. */
/*4. 모아놓은 객체들의 렌더함수를 호출한다.(x) */

class CObject_Manager final : public CBase
{
	using super = CBase;
	using LAYERS = std::map<const _wstring, CLayer*>;

private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	void Final_Update(_float fTimeDelta);

	void Clear(_uint iLevelIndex);

	HRESULT Add_GameObject_To_Layer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	const std::list<CGameObject*>& Get_GameObjects_In_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	//_float3 Ray_Cast_To_Mesh_In_Layer(_fvector vRayOrigin, _fvector vRayDirection, _uint iLevelIndex, const _wstring& LayerTag);
	vector<_wstring> Get_LayerTags(_uint iLevelIndex) const;
	CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex);
	CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPartTag, const _wstring& strComponentTag, _uint iIndex);
	CComponent* Find_Component_In_PartObject(_uint iLevelIndex, const _wstring& strLayerTag, const vector<_wstring>& strPartTags, const _wstring& strComponentTag, _uint iIndex);

	void Delete_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex);
	void Delete_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject);
	void Delete_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, const vector<_wstring>& strPartTags, _uint iIndex);
	CGameObject* Find_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex);
	CGameObject* Find_GameObject_With_ID(_uint iLevelIndex, const _uint iID);
	CGameObject* Find_GameObject_With_Name(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strName);

	/* For Hit Lag or Anything Needs Lag */
	HRESULT Add_Layer_To_Lag(const _wstring& LayerTag);
	void Call_Lag(_float fLagRatio, _float fLagTime, _bool bLoop = false)
	{
		m_fLagRatio = fLagRatio;
		m_fLagTime = fLagTime;
		m_bLagLoop = bLoop;
		m_bIsLag = true;
	}
	void Reset_Lag()
	{
		m_fLagRatio = 1.f;
		m_fLagTime = 0.f;
		m_bLagLoop = false;
		m_bIsLag = false;
	}

	_float Get_LagRatio() const { return m_fLagRatio; }
	_float Get_LagTime() const { return m_fLagTime; }
	_bool Get_IsLag() const { return m_bIsLag; }

private:
	std::list<CGameObject*>		m_EmptyLayer;
	std::vector<LAYERS>			m_pLayers;

	std::set<_wstring>			m_LagLayers;
	_float						m_fLagTime{};
	_float						m_fLagRatio{};
	_bool						m_bLagLoop{};
	_bool						m_bIsLag{};

	CGameInstance*				m_pGameInstance{ nullptr };

private:
	class CLayer* Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);

public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END

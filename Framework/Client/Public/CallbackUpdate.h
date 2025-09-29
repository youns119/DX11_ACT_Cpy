#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CCallbackUpdate final : public CGameObject
{
	using super = CGameObject;
private:
	CCallbackUpdate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCallbackUpdate(const CCallbackUpdate& Prototype);
	virtual ~CCallbackUpdate() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Add_PriorityUpdate_Callback(std::function<_bool(_float fElapsed, _float fTimeDelta)> Callback);
	void Add_Update_Callback(std::function<_bool(_float fElapsed, _float fTimeDelta)> Callback);
	void Add_LateUpdate_Callback(std::function<_bool(_float fElapsed, _float fTimeDelta)> Callback);

	static CCallbackUpdate* Find_CallbackUpdate();

private:
	list<pair<_float, std::function<_bool(_float, _float)>>> m_PriorityUpdateCallbacks;
	list<pair<_float, std::function<_bool(_float, _float)>>> m_UpdateCallbacks;
	list<pair<_float, std::function<_bool(_float, _float)>>> m_LateUpdateCallbacks;

public:
	static CCallbackUpdate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END

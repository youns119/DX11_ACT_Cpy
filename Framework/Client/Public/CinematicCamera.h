#pragma once

#include "Client_Defines.h"

#include "GameObject.h"

BEGIN(Engine)

class CCineCamera;

END

BEGIN(Client)

class CCinematicCamera final
	: public CGameObject
{
	using super = CGameObject;

private:
	CCinematicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCinematicCamera(const CCinematicCamera& Prototype);
	virtual ~CCinematicCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public :
	CCineCamera* Get_CineCamera() { return m_pCineCamera; }

public :
	void Play_Cinematic(const _wstring& strCinematic, const _wstring& strReturnName, _uint iLerp, _float fReturnLerp = 0.1f);

private :
	CCineCamera* m_pCineCamera{};

private:
	HRESULT Ready_PartObject();
	HRESULT Ready_Event();

public:
	static CCinematicCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
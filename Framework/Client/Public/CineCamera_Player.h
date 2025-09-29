#pragma once

#include "Client_Defines.h"

#include "CineCamera.h"

BEGIN(Client)

class CCineCamera_Player
	: public CCineCamera
{
	using super = CCineCamera;

private :
	CCineCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCineCamera_Player(const CCineCamera_Player& Prototype);
	virtual ~CCineCamera_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual void Final_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public :
	_bool Get_InputEnable() { return m_bInputEnable; }

public :
	void Play_Cinematic(const _wstring& strCinematic);
	void Stop_Cinematic();

public :
	void Toggle_Input(_bool bToggle) { m_bInputEnable = bToggle; }

	void Reset();

private :
	_wstring m_strCurr{};

	_bool m_bInputEnable{};

public:
	static CCineCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};

END
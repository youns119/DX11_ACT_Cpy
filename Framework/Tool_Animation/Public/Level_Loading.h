#pragma once

#include "ToolAnimation_Defines.h"
#include "Level.h"

BEGIN(ToolAnimation)

class CLoader;

class CLevel_Loading final 
	: public CLevel
{
private :
	CLevel_Loading(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Loading() = default;

public :
	virtual HRESULT Init(LEVELID _eNextLevelID);
	virtual void Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private :
	CLoader* m_pLoader{};

private :
	LEVELID m_eNextLevelID{ LEVELID::LEVEL_END };

	_float m_fCurrentProgress{};

private :
	HRESULT Ready_Layer_BackGround(const _tchar* _pLayerTag);

public :
	static CLevel_Loading* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVELID _eNextLevelID);
	virtual void Free() override;
};

END
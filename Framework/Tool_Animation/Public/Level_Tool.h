#pragma once

#include "ToolAnimation_Defines.h"
#include "Level.h"

BEGIN(ToolAnimation)

class CLevel_Tool final
	: public CLevel
{
private:
	CLevel_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Tool() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _wstring& _strLayerTag);
	HRESULT Ready_Environment(const _wstring& _strLayerTag);
	HRESULT Ready_Layer_AnimObject(const _wstring& _strLayerTag);

private:
	void SetUp_CollisionGroup();

public:
	static CLevel_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END
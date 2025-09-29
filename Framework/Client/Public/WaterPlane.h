#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer;
END

BEGIN(Client)
class CWaterPlane final : public CGameObject
{
	using super = CGameObject;
public:

	struct DESC
	{
		GAMEOBJECT_DESC GameObjectDesc {};
	};



private:
	CWaterPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWaterPlane(const CWaterPlane& Prototype);
	virtual ~CWaterPlane() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CVIBuffer* m_pVIBufferCom { nullptr };
	CShader* m_pShaderCom { nullptr };
	CTexture* m_pTextureCom { nullptr };
	CTexture* m_pTextureCom_Sky { nullptr };
	CTexture* m_pTextureCom_Distortion { nullptr };
	CTexture* m_pTextureCom_Normal { nullptr };

	_float m_fRotation { 90.f };
	_float m_fTimeAcc { 0 };

	struct DEBUG_PARAMS
	{
		_vec2 vTileSize { 6.7f, 6.9f };
		_vec2 vPanndingSpeed { 0.01f, 0.017f };
		_float fDistortionIntensity { 0.305f };
		_float fLightPower { 50.f };
		_float fLightIntensity { 1.f };
	} m_DebugParams;

private:
	HRESULT Ready_Component();

public:
	static CWaterPlane* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END

#pragma once
#ifdef _DEBUG
#include "Tool_Define.h"
#include "GameObject.h"

BEGIN(MapTool)
class CDebug_PostProcess final : public CGameObject
{
	using super = CGameObject;

private:
	CDebug_PostProcess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDebug_PostProcess(const CDebug_PostProcess& Prototype);
	virtual ~CDebug_PostProcess() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint m_iImGuiID{ 0 };

private:
	void Debug_Fog();
	void Debug_Bloom();
	void Debug_ColorCorrection();
	void Debug_GlobalLight();
	void Debug_SSAO();

	void Handle_FileIO();
	void Save_Data();
	void Load_Data();

public:
	static CDebug_PostProcess* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCloneable* Clone(void* pArg) override;
	virtual void Free() override;
};
END
#endif
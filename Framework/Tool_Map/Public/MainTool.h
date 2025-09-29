#pragma once

#include "Tool_Define.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(MapTool)

class CMainTool final : public CBase
{
	using super = CBase;
private:
	CMainTool();
	virtual ~CMainTool() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

private: /*엔진의 기능을 이용하기 위해 게임인스터스 클래스를 객체화 */
	Engine::CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

#ifdef _DEBUG
private:
	_float m_fTimerAcc = {};
	_uint m_iDrawCount = {};
	_tchar m_szFPS[MAX_PATH] = {};
#endif

private:
	HRESULT Start_Level(LEVELID eStartLevel);
	HRESULT Ready_Prototype_For_Static();
	HRESULT Gara_Function(); // todo : Make Texture and Save To png, dds file.

public:
	static CMainTool* Create();
	virtual void Free() override;

};

END